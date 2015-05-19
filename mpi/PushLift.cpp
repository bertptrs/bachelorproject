#include <cassert>
#include <iterator>
#include <limits>
#include <algorithm>
#include <exception>
#include <iomanip>

#include "MPI.h"

#include "PushLift.h"
#include "FileReader.h"

#include "utility.h"

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
	worldSize = MPI::COMM_WORLD.Get_size();
	rank = MPI::COMM_WORLD.Get_rank();
}

int PushLift::randomNode() {
	set<int> nodes = graph.getNodes();
	set<int>::iterator iter = nodes.begin();
	uniform_int_distribution<int> dist(0, numNodes - 1);
	advance(iter, dist(random));

	return *iter;
}

weight_t PushLift::flow() {
	int source = args.getSource();
	int sink = args.getSink();

	if (source == Graph::NO_NODE) {
		source = randomNode();
	}

	if (sink == Graph::NO_NODE) {
		sink = randomNode();
	}

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

	if (shouldDebug()) {
		cerr << "MPI master calculating max from from " << source << " to " << sink << endl;
		cerr << "MPI World size: " << worldSize << endl;
	}
	// Allocated too large, so that we do not need to worry about 1 indexing.
	H = vector<int>(graph.getMaxNode() + 1, 0);
	D = vector<weight_t>(graph.getMaxNode() + 1, 0);

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

	// Perform saturating push
	for (pair<int, weight_t>& edge : edges[source]) {
		// Find reverse edge. This is annoying and O(N)
		pair<int, weight_t>& backEdge = getEdge(edge.first, source);
		backEdge.second = edge.second;
		D[edge.first] += edge.second;
		edge.second = 0;
	}

	if (shouldDebug()) {
		cerr << "Performed initial push." << endl;
		cerr << "Calculating flow over " << graphEdges.size() << " edges." << endl;
	}
	
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
	const int startNode = rank;
	const int increment = worldSize;
	int iter = 0;

	bool found = true;
	while (found) {
		found = false;

		// TODO: add communication here
		
		for (int i = startNode; i < (int) D.size(); i += increment) {
			if (i == source || i == sink) {
				// Never work on those
				continue;
			}

			if (D[i] > 0) {
				work(i);
				found = true;
				break;
			}
		}

		if (shouldDebug() && iter % (1 << 12) == 0) {
			// Print status update, is nice.
			cerr << "Iteration " << iter << ": flow at sink: " << D[sink]
				<< " (" << activeNodes() << " active)" << endl;
		}
		iter++;
	}

	if (args.isVerbose()) {
		cerr << endl << "Stopping thread " << rank << " after " << iter << " iterations." << endl;
	}
}

// Perform an action on this specific node.
void PushLift::work(int node) {
	assert(worldSize == 1 && "Parallel implementation not yet supported.");

	int minHeight = numeric_limits<int>::max();
	int& height = H[node];
	weight_t& excess = D[node];

	for (auto& edge : edges[node]) {
		if (edge.second > 0) {
			const int edgeHeight = H[edge.first];

			if (edgeHeight < height) {
				// Can perform a push operation.
				pair<int, weight_t>& backEdge = getEdge(edge.first, node);
				weight_t delta = min(edge.second, excess);
				edge.second -= delta;
				backEdge.second += delta;
				D[edge.first] += delta;
				D[node] -= delta;

				// TODO: communicate.
				return;
			} else {
				// Candidate for a lift operation
				minHeight = min(minHeight, edgeHeight);
			}
		}
	}

	// No push available, must lift.
	if (minHeight != numeric_limits<int>::max()) {
		// Can perform a lift operation.
		height = minHeight + 1;

		// TODO: communicate.
	} else {
		assert(false && "No operation applicable. Invalid state?");
	}
}

// Find  an edge from "from" to "to". This operation is O(N).
pair<int, weight_t>& PushLift::getEdge(int from, int to) {
	for (auto& edge : edges[from]) {
		if (edge.first == to) {
			return edge;
		}
	}

	assert(false && "Back edge not found");
	throw runtime_error("Back edge not found");
}

int PushLift::activeNodes() const {
	int active = 0;
	for (const weight_t& delta : D) {
		if (delta > 0) active++;
	}

	return active;
}

bool PushLift::shouldDebug() const {
	return args.isVerbose() && rank == 0;
}
