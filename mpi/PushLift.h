#ifndef PUSHLIFT_H
#define PUSHLIFT_H

#include <random>
#include <vector>
#include <utility>
#include <list>

#include "Graph.h"
#include "Argstate.h"

using namespace std;

class PushLift {
	private:
		const Argstate args;
		Graph graph;
		mt19937 random;
		int numNodes;

		// Flow information
		int source, sink;
		vector<int> H; // Height of node
		vector<weight_t> D; // Excess of node
		vector<vector<pair<int, weight_t>>> edges; // Edges with remaining capacity.
										// Stored as an adjecency list with weights.

		// MPI information
		int rank, worldSize;

		int randomNode();

		// General initializer. Called by all contructors.
		void init();
		// Initialize the data for the algorithm.
		void initAlgo();

		// Add an edge to the internal datastructure.
		void addEdge(const pair<int, int>& conn, weight_t weight);

		// Find the back edge for a given edge
		pair<int, weight_t>& getEdge(int, int);

		void work(int node);
		void run();

		// Number of active nodes.
		int activeNodes() const;

		bool shouldDebug() const;

	public:
		PushLift(const Argstate& args);
		PushLift(const Argstate& args, const Graph& graph);

		weight_t flow();
		weight_t flow(int source, int sink);
};

#endif
