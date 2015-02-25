#ifndef PUSHRELABEL_H
#define PUSHRELABEL_H
#include <set>
#include <utility>
#include <memory>
#include <map>
#include "AbstractGraph.h"

using namespace std;

class PushRelabel : public AbstractGraph {
	private:
		struct Edge {
			Edge(int u, int v, double c);
			int origin;
			int destination;
			double capacity;
			double flow;
		};
		typedef shared_ptr<Edge> eptr;
		typedef multiset<eptr> eset;
		typedef set<int> iset;
		map<int, int> heights;
		map<int, double> excess;

		eset edges;

		// Return a set of edges satisfying the predicate.
		// TODO: move this to utils at some point.
		template<class Predicate>
			inline eset forPredicate(Predicate P) const;
		// Get all active edges
		iset getActive(const int source) const;
		// Get all edges going out of u
		eset getOutgoing(int u) const;
		// Get all edges leading up to v
		eset getIncoming(int v) const;
		// Get all edges connected to u
		eset getConnected(int u) const;
		// Initialize the edges for running the algorithm and initialize the heights map.
		void init();
		// Nodes in this graph
		set<int> nodes() const;
		// Number of nodes
		inline int numNodes() const;
		// Calculate the current flow, given it is valid.
		double getFlow(const int t) const;
		// Check if a push is allowed. This method checks heights and capacities.
		bool canPush(eptr edge);
		// Perform a push operation
		double push(eptr edge, double amount);
		// Perform a relabel operation
		void relabel(int node);
		// Get the excess of flow at a node.
		double getExcess(int node) const;
		eptr getBackEdge(const eptr& edge) const;

	public:
		virtual void addEdge(int u, int v, double c);
		virtual void clear();
		virtual double maxFlow(const int s, const int t);
};

#endif
