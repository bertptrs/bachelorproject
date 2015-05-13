#include <cassert>
#include <iterator>

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

float PushLift::flow() {
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

float PushLift::flow(int sourceArg, int sinkArg) {
	source = sourceArg;
	sink = sinkArg;

	assert(source >= 0);
	assert(sink >= 0);
	assert(source != sink);

	initAlgo();

	return 1 / 0.0;
}

int PushLift::maxNode() const {
	set<int> nodes = graph.getNodes();
	if (nodes.empty()) {
		return Graph::NO_NODE;
	} else {
		return *nodes.rbegin();
	}
}

void PushLift::initAlgo() {
	// Allocated too large, so that we do not need to worry about 1 indexing.
	H = vector<int>(maxNode() + 1, 0);
	D = vector<int>(maxNode() + 1, 0);

	// Set height of the source
	H[source] = maxNode();

	// Initialize edges.
	edges = vector<list<pair<int, float>>>(maxNode() + 1);
	for (const Edge& edge : graph.getEdges()) {
		addEdge(edge.first, edge.second);
		addEdge(reverse(edge.first), 0);
	}

	// Perform saturating push
	for (pair<int, float>& edge : edges[source]) {
		// Find reverse edge. This is annoying and O(N)
		for (pair<int, float>& backEdge : edges[edge.first]) {
			if (backEdge.first == source) {
				backEdge.second = edge.second;
				break;
			}
		}

		D[edge.first] += edge.second;
		edge.second = 0;
	}

	if (args.isVerbose()) {
		// Print debug output
		if (rank == 0) {
			cerr << "MPI master" << endl;
			cerr << "MPI World size: " << worldSize << endl;
		} else {
			cerr << "MPI slave " << rank << endl;
		}
	}
}

void PushLift::addEdge(const pair<int, int>& conn, float weight) {
	assert(weight >= 0);

	auto& edgeList = edges[conn.first];
	for (pair<int, float>& existingEdge : edgeList) {
		if (existingEdge.first == conn.second) {
			existingEdge.second += weight;
			return;
		}
	}

	edgeList.push_back(make_pair(conn.second, weight));
}
