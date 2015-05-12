#ifndef PUSHLIFT_H
#define PUSHLIFT_H

#include <random>
#include <vector>
#include "mpi.h"
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
		vector<int> D; // Excess of node
		vector<list<pair<int, float>>> edges; // Edges with remaining capacity.
										// Stored as an adjecency list with weights.

		// MPI information
		int rank, worldSize;

		int randomNode();
		int maxNode() const;

		// General initializer. Called by all contructors.
		void init();
		// Initialize the data for the algorithm.
		void initAlgo();

		// Add an edge to the internal datastructure.
		void addEdge(const pair<int, int>& conn, float weight);

	public:
		PushLift(const Argstate& args);
		PushLift(const Argstate& args, const Graph& graph);

		float flow();
		float flow(int source, int sink);
};

#endif
