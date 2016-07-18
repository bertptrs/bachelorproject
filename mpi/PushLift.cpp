#include <cassert>
#include <iterator>
#include <limits>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <stdexcept>
#include <cstdint>

#include "MPI.hpp"
#include "channels.hpp"

#include "PushLift.hpp"
#include "FileReader.hpp"

#include "utility.hpp"

static const weight_t EPSILON = 1e-20;

using namespace MPI;

ostream& operator << (ostream& output, const Edge& edge) {
	output << "Edge ((" << edge.first.first << " -> "
		<< edge.first.second << "), " << edge.second
		<< ")";

	return output;
}

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
	uniform_int_distribution<int> dist(1, numNodes);

	return dist(random);
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

	if (args.isVerbose() && communicator.isMaster()) {
		communicator.getOutputStream() << "Calculating max flow from "
			<< source << " to " << sink << endl;
	}

	initAlgo();
	run();

	weight_t finalFlow = getExcess(sink);

	COMM_WORLD.Bcast(&finalFlow, 1, DOUBLE_PRECISION, communicator.owner(sink));

	return finalFlow;
}

void PushLift::initAlgo() {
	// Allocated too large, so that we do not need to worry about 1 indexing.
	adjecentWorkers = vector<set<int>>(graph.getMaxNode() + 1);

	initDataStructure(graph.getMaxNode(), graph.getEdges().size());

	// Set height of the source
	setHeight(source, graph.getMaxNode());

	// Initialize edges.
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

	// Determine adjecent workers for each node.
	// This is used in the communication step.
	for (int i = 1; i <= graph.getMaxNode(); i++) {
		if (!communicator.mine(i)) {
			continue;
		}
		for (const auto edge : getNeighbours(i)) {
			if (!communicator.mine(edge)) {
				adjecentWorkers[i].insert(communicator.owner(edge));
			}
		}
	}

	if (communicator.mine(source)) {
		// Perform saturating push
		if (shouldDebug()) {
			communicator.getDebugStream() << "Performing initial push" << endl;
		}
		for (int nodeNo : getNeighbours(source)) {
			performPush(source, nodeNo, getCapacity(source, nodeNo));
		}
	} else if (shouldDebug()) {
		communicator.getDebugStream() << "Listening for initial push." << endl;
	}

}

void PushLift::run() {
	uint64_t iter = 0;

	while (!todo.empty() || !communicator.canShutdown()) { // Stopping condition for world size 1

		if (hasQueuedNode()) {
			int node = source;
			while ((node == source || node == sink || getExcess(node) == 0) && hasQueuedNode()) {
				node = getQueuedNode();
			}

			if (node != source && node != sink && getExcess(node) > 0) {
				work(node);
			}
		}

		// Communication should happen
		receivePush();
		receiveLift();

		if (shouldDebug() && iter % (1 << 18) == 0) {
			// Print status update, is nice.
			communicator.getDebugStream() << "Iteration " << iter << ": flow at sink: " << getExcess(sink)
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
	int height = getHeight(node);
	weight_t excess = getExcess(node);
	if (excess < EPSILON) {
		setExcess(node, 0);
		return;
	}

	int goodNeighbour = -1;
	int goodHeight = height;

	for (int neighbour : getNeighbours(node)) {
		if (getCapacity(node, neighbour) > 0) {
			const int edgeHeight = getHeight(neighbour);

			if (edgeHeight < goodHeight) {
				goodNeighbour = neighbour;
			} else {
				// Candidate for a lift operation
				minHeight = min(minHeight, edgeHeight);
			}
		}
	}

	// No push available, must lift.
	if (goodNeighbour != -1) {
		// Can perform a push operation.
		weight_t delta = min(getCapacity(node, goodNeighbour), excess);
		performPush(node, goodNeighbour, delta);
		if (getExcess(node) > 0) {
			queueNode(node);
		}
	} else {
		assert(minHeight != numeric_limits<int>::max());
		int newHeight = minHeight + 1;
		int delta = newHeight - height;
		performLift(node, delta);
	}
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
void PushLift::writeFlow(ostream&) const {
	assert(false && "Currently not supported.");
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

void PushLift::performPush(int from, int to, weight_t delta)
{
	weight_t capacity = getCapacity(from, to);
	weight_t backCapacity = getCapacity(to, from);

	setExcess(to, getExcess(to) + delta);
	setExcess(from, getExcess(from) - delta);

	setCapacity(from, to, capacity - delta);
	setCapacity(to, from, backCapacity + delta);

	if (communicator.mine(to)) {
		queueNode(to);
	} else {
		communicator.sendPush(from, to, delta);
	}
}

void PushLift::performLift(int node, int delta) {
	setHeight(node, getHeight(node) + delta);
	if (communicator.mine(node)) {
		communicator.sendLift(node, delta, adjecentWorkers[node]);

		if (getExcess(node) > 0) {
			queueNode(node);
		}
	}
}
