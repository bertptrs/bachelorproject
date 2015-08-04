#ifndef PUSHLIFT_H
#define PUSHLIFT_H

#include <random>
#include <vector>
#include <utility>
#include <list>
#include <iostream>
#include <queue>

#include "Graph.hpp"
#include "Argstate.hpp"
#include "MPICommunicator.hpp"
#include <set>

using namespace std;

ostream& operator << (ostream&, const Edge&);

class PushLift {
	protected:


		const Argstate args;
		Graph graph;
		mt19937 random;
		int numNodes;

		// Flow information
		int source, sink;
		queue<int> todo;
		// MPI information
		vector<set<int>> adjecentWorkers;
		MPICommunicator communicator;
		// MPI helpers
		void receivePush();
		void receiveLift();
		int randomNode();

		// General initializer. Called by all contructors.
		void init();
		// Initialize the data for the algorithm.
		void initAlgo();

		// Methods to be implemented by instantiations, to use the current datastructure.
		virtual void addEdge(const pair<int, int>& conn, weight_t weight) = 0;
		virtual weight_t getExcess(const int& nodeNo) const = 0;
		virtual void setExcess(const int& nodeNo, const weight_t& newWeight) = 0;
		virtual int getHeight(const int& nodeNo) const = 0;
		virtual void setHeight(const int& nodeNo, const int& newHeight) = 0;
		virtual weight_t getCapacity(const int& from, const int& to) const = 0;
		virtual void setCapacity(const int& from, const int& to, const weight_t& newCapacity) = 0;
		virtual vector<int> getNeighbours(int node) const = 0;
		virtual void initDataStructure(const int& maxNode, const int& numEdges) = 0; // Initialize the datastructure for given parameters.

		// Queue an edge for the todo list.
		void queueNode(int node);
		bool hasQueuedNode() const;
		int getQueuedNode();

		void work(int node);
		void run();
		void performPush(int from, int to, weight_t delta);
		void performLift(int node, int delta);

		// Number of active nodes.
		int activeNodes() const;

		bool shouldDebug() const;

	public:
		PushLift(const Argstate& args);
		PushLift(const Argstate& args, const Graph& graph);
		virtual ~PushLift() = default;

		weight_t flow();
		weight_t flow(int source, int sink);

		void writeFlow(ostream&) const;
};

#endif
