#ifndef PUSHLIFT_H
#define PUSHLIFT_H

#include <random>
#include <vector>
#include <utility>
#include <list>
#include <iostream>
#include <queue>

#include "Graph.h"
#include "ActiveState.h"
#include "Argstate.h"
#include "MPICommunicator.h"
#include <set>

using namespace std;

class PushLift {
	private:
		typedef pair<int, weight_t> EdgeWeight;

		const Argstate args;
		Graph graph;
		mt19937 random;
		int numNodes;

		// Flow information
		int source, sink;
		vector<int> H; // Height of node
		vector<weight_t> D; // Excess of node
		vector<vector<EdgeWeight>> edges; // Edges with remaining capacity.
										// Stored as an adjecency list with weights.

		queue<int> todo; // Height and node number.


		// MPI information
		const int rank;
		const int worldSize;
		vector<set<int>> adjecentWorkers;
		MPICommunicator communicator;

		// State checker: Whether or nor we are finished yet.
		ActiveState activeState;

		// MPI helpers
		void receivePush();
		void receiveLift();

		int randomNode();

		// General initializer. Called by all contructors.
		void init();
		// Initialize the data for the algorithm.
		void initAlgo();

		// Add an edge to the internal datastructure.
		void addEdge(const pair<int, int>& conn, weight_t weight);

		// Queue an edge for the todo list.
		void queueNode(int node);
		bool hasQueuedNode() const;
		int getQueuedNode();

		// Find the back edge for a given edge
		pair<int, weight_t>& getEdge(int, int);
		pair<int, weight_t>& getEdge(const pair<int, int>&);
		const pair<int, weight_t>& getEdge(int, int) const;
		const pair<int, weight_t>& getEdge(const pair<int, int>&) const;

		void work(int node);
		void run();
		void performPush(int from, int to, weight_t delta);
		void performLift(int node, int delta);

		// Number of active nodes.
		int activeNodes() const;

		bool shouldDebug() const;
		bool isMaster() const;

	public:
		PushLift(const Argstate& args);
		PushLift(const Argstate& args, const Graph& graph);

		weight_t flow();
		weight_t flow(int source, int sink);

		void writeFlow(ostream&) const;
};

#endif
