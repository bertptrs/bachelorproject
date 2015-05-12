#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <utility>
#include <vector>
#include <set>

using namespace std;

typedef pair<pair<int, int>, float> Edge;

class Graph {
	private:
		map<pair<int, int>, float> edges;

	public:
		static const int NO_NODE = -1; 

		void clear();
		void addEdge(int source, int sink, float weight);
		void addEdge(pair<int, int> edge, float weight);

		vector<Edge> getEdges() const;
		set<int> getNodes() const;
};

#endif
