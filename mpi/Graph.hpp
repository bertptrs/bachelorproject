#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <utility>
#include <vector>
#include <set>

using namespace std;

typedef double weight_t;
typedef pair<pair<int, int>, weight_t> Edge;

class Graph {
	private:
		map<pair<int, int>, weight_t> edges;
		int maxNode = 0;

	public:
		static const int NO_NODE = -1;

		void clear();
		void addEdge(int source, int sink, weight_t weight);
		void addEdge(pair<int, int> edge, weight_t weight);

		vector<Edge> getEdges() const;
		set<int> getNodes() const;
		int getMaxNode() const;
};

#endif
