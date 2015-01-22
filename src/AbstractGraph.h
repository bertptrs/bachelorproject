#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

class AbstractGraph {
	public:
		virtual ~AbstractGraph() = default;
		virtual void addEdge(int u, int v, double c) = 0;
		virtual void clear() = 0;
		virtual double maxFlow(int s, int t) = 0;
};

#endif
