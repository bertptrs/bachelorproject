#include <cassert>
#include <iterator>
#include <limits>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <stdexcept>
#include <cstdint>

#include "MPI.h"
#include "channels.h"

#include "PushLift.h"
#include "FileReader.h"

#include "utility.h"

static const weight_t EPSILON = 1e-20;

using namespace MPI;

/**
 * Get a specific node from the datastructure.
 *
 * @param [const] edges An optionally const adjecency list
 * @param from origin node number
 * @param to destination node number
 * @returns A [const] edge reference.
 * @throws out_out_range if the node could not be found.
 */
template<typename VectorType>
typename enable_if<is_same<vector<vector<pair<int, weight_t>>>,
		 typename remove_const<VectorType>::type
	>::value
, decltype(*declval<VectorType>().begin()->begin())
	>::type
	edgeGet(VectorType& edges, int from, int to) {
		auto& edgeList = edges[from];
		const pair<int, weight_t> reference = make_pair(to, -numeric_limits<weight_t>::infinity());
		auto it = lower_bound(edgeList.begin(), edgeList.end(), reference);

		if (it == edges[from].end() || it->first != to) {
			throw out_of_range("Back edge not found");
		} else {
			return *it;
		}
	}

PushLift::PushLift(const Argstate& args) :
	args(args)
{
	FileReader reader(args.getFilename());
	graph = reader.read();

	init();
}

PushLift::PushLift(const Argstate& args, const Graph& graph) :
	args(args),
	graph(graph)
{
	init();
}

void PushLift::init() {
	random_device rd;
	random.seed(rd());

	numNodes = graph.getNodes().size();
}

int PushLift::randomNode() {
	set<int> nodes = graph.getNodes();
	set<int>::iterator iter = nodes.begin();
	uniform_int_distribution<int> dist(0, numNodes - 1);
	advance(iter, dist(random));

	return *iter;
}

weight_t PushLift::flow() {
	int locations[2];
	int& source = locations[0];
	int& sink = locations[1];
	if (communicator.isMaster()) {
		// Determine flow route.
		source = args.getSource();
		sink = args.getSink();

		if (source == Graph::NO_NODE) {
			source = randomNode();
		}

		if (sink == Graph::NO_NODE) {
			sink = randomNode();
		}
	}
	// Synchronize flow directions between workers
	COMM_WORLD.Bcast(locations, 2, INTEGER, 0);

	return flow(source, sink);
}

weight_t PushLift::flow(int sourceArg, int sinkArg) {
	source = sourceArg;
	sink = sinkArg;

	assert(source >= 0);
	assert(sink >= 0);
	assert(source != sink);

	initAlgo();
	run();

	return D[sink];
}

void PushLift::initAlgo() {
	// Allocated too large, so that we do not need to worry about 1 indexing.
	H = vector<int>(graph.getMaxNode() + 1, 0);
	D = vector<weight_t>(graph.getMaxNode() + 1, 0);
	adjecentWorkers = vector<set<int>>(graph.getMaxNode() + 1);

	// Set height of the source
	H[source] = graph.getMaxNode();

	// Initialize edges.
	edges = vector<vector<pair<int, weight_t>>>(graph.getMaxNode() + 1);
	const auto graphEdges = graph.getEdges();
	const int size = graphEdges.size();

	for (int i = 0; i < size; i++) {
		const auto& edge = graphEdges[i];
		// Remove self-loops
		if (edge.first.first != edge.first.second) {
			addEdge(edge.first, edge.second);
			addEdge(reverse(edge.first), 0);
		}
	}

	// Sort arrays edges, so that binary search is possible.
	for (auto& edgeList : edges) {
		sort(edgeList.begin(), edgeList.end());
	}

	// Determine adjecent workers for each node.
	// This is used in the communication step.
	for (int i = 0; i < (int) edges.size(); i ++) {
		if (!communicator.mine(i)) {
			continue;
		}
		for (const auto& edge : edges[i]) {
			if (!communicator.mine(edge.first)) {
				adjecentWorkers[i].insert(communicator.owner(edge.first));
			}
		}
	}

	if (communicator.mine(source)) {
		// Perform saturating push
		if (shouldDebug()) {
			communicator.getDebugStream() << "Performing initial push" << endl;
		}
		for (pair<int, weight_t>& edge : edges[source]) {
			performPush(source, edge.first, edge.second);
		}
	} else if (shouldDebug()) {
		communicator.getDebugStream() << "Listening for initial push." << endl;
	}

	communicator.getDebugStream() << "Calculating flow over " << graphEdges.size() << " edges." << endl;

}

void PushLift::addEdge(const pair<int, int>& conn, weight_t weight) {
	assert(weight >= 0);

	auto& edgeList = edges[conn.first];
	for (pair<int, weight_t>& existingEdge : edgeList) {
		if (existingEdge.first == conn.second) {
			existingEdge.second += weight;
			return;
		}
	}

	edgeList.push_back(make_pair(conn.second, weight));
}

void PushLift::run() {
	uint64_t iter = 0;

	while (!todo.empty() || !communicator.canShutdown()) { // Stopping condition for world size 1

		if (hasQueuedNode()) {
			int node = source;
			while ((node == source || node == sink || D[node] == 0) && hasQueuedNode()) {
				node = getQueuedNode();
			}

			if (node != source && node != sink && D[node] > 0) {
				work(node);
			}
		}

		// Communication should happen
		receivePush();
		receiveLift();

		if (shouldDebug() && iter % (1 << 18) == 0) {
			// Print status update, is nice.
			communicator.getDebugStream() << "Iteration " << iter << ": flow at sink: " << D[sink]
				<< " (" << activeNodes() << " active)" << endl;
		}
		iter++;
	}

	if (shouldDebug()) {
		communicator.getDebugStream() << "Shutting down." << endl;
	}
}

// Perform an action on this specific node.
void PushLift::work(int node) {
	int minHeight = numeric_limits<int>::max();
	int& height = H[node];
	weight_t& excess = D[node];
	if (excess < EPSILON) {
		excess = 0;
		return;
	}

	for (auto& edge : edges[node]) {
		if (edge.second > 0) {
			const int edgeHeight = H[edge.first];

			if (edgeHeight < height) {
				// Can perform a push operation.
				weight_t delta = min(edge.second, excess);
				performPush(node, edge.first, delta);
				if (D[node] > 0) {
					queueNode(node);
				}
				return;
			} else {
				// Candidate for a lift operation
				minHeight = min(minHeight, edgeHeight);
			}
		}
	}

	// No push available, must lift.
	if (minHeight != numeric_limits<int>::max()) {
		int newHeight = minHeight + 1;
		int delta = newHeight - height;
		performLift(node, delta);
	} else {
		cerr << "ERROR STATE Node: " << node << " has no valid outbound edges!" << endl
			<< D[node] << " " << H[node] << endl;
		assert(false && "No operation applicable. Invalid state?");
	}
}

// Find  an edge from "from" to "to". This operation is O(N).
pair<int, weight_t>& PushLift::getEdge(int from, int to) {
	return edgeGet(edges, from, to);
}

pair<int, weight_t>& PushLift::getEdge(const pair<int, int>& conn) {
	return getEdge(conn.first, conn.second);
}

const pair<int, weight_t>& PushLift::getEdge(int from, int to) const {
	return edgeGet(edges, from, to);
}

const pair<int, weight_t>& PushLift::getEdge(const pair<int, int>& conn) const {
	return getEdge(conn.first, conn.second);
}

int PushLift::activeNodes() const {
	return todo.size();
}

bool PushLift::shouldDebug() const {
	return args.isVerbose();
}


/*
 * Write a Matrix Market file with the final flow.
 * Only edges with actual flow are displayed.
 */
void PushLift::writeFlow(ostream& target) const {
	set<pair<pair<int, int>, weight_t>> flowingEdges;

	// Determine edges with flow
	for (const auto& edge : graph.getEdges()) {
		if (edge.first.first == edge.first.second) {
			continue;
		}
		if (getEdge(edge.first).second < edge.second) {
			// This edge has less capacity, so it has flow.
			flowingEdges.insert(edge);
		}
	}

	// Add the MTX header
	target << "%%MatrixMarket matrix coordinate real general" << endl
		<< "%-------------------------------------------------" << endl
		<< "% Original file: " << args.getFilename() << endl
		<< "% Max flow calculated from " << source << " to " << sink << endl
		<< "%-------------------------------------------------" << endl;

	// Write dimensions
	target << graph.getMaxNode() << " " << graph.getMaxNode() << " " << flowingEdges.size() << endl;

	for (const auto& edge : flowingEdges) {
		weight_t localFlow = edge.second - getEdge(edge.first).second;
		target << edge.first.first << " " << edge.first.second << " " << localFlow << endl;
	}

}

void PushLift::receivePush() {
	while (communicator.hasPush()) {
		PushType push = communicator.getPush();
		performPush(push.from, push.to, push.amount);
	}
}

void PushLift::receiveLift() {
	while (communicator.hasLift()) {
		LiftType lift = communicator.getLift();
		performLift(lift.node, lift.delta);
	}
}

void PushLift::queueNode(int node) {
	todo.push(node);
}

bool PushLift::hasQueuedNode() const {
	return !todo.empty();
}

int PushLift::getQueuedNode() {
	int node = todo.front();
	todo.pop();
	return node;
}

void PushLift::performPush(int from, int to, weight_t delta) {
	auto& edge = getEdge(from, to);
	auto& backEdge = getEdge(to, from);

	D[to] += delta;
	D[from] -= delta;

	edge.second -= delta;
	backEdge.second += delta;

	if (communicator.mine(to)) {
		queueNode(to);
	} else {
		communicator.sendPush(from, to, delta);
	}
}

void PushLift::performLift(int node, int delta) {
	H[node] += delta;
	if (communicator.mine(node)) {
		communicator.sendLift(node, delta, adjecentWorkers[node]);

		if (D[node] > 0) {
			queueNode(node);
		}
	}
}
