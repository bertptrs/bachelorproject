#include "Graph.hpp"
#include <cassert>
#include <algorithm>

void Graph::clear() {
	edges.clear();
}

void Graph::addEdge(pair<int, int> edge, weight_t weight) {
	assert(weight >= 0);
	edges[edge] += weight;

	maxNode = max({maxNode, edge.first, edge.second});
}

void Graph::addEdge(int source, int sink, weight_t weight) {
	addEdge(make_pair(source, sink), weight);
}

vector<Edge> Graph::getEdges() const {
	vector<Edge> result(edges.size());
	for (const Edge& edge : edges) {
		result.push_back(edge);
	}

	return result;
}

set<int> Graph::getNodes() const {
	set<int> result;

	for (const Edge& edge : edges) {
		result.insert(edge.first.first);
		result.insert(edge.first.second);
	}

	return result;
}

int Graph::getMaxNode() const {
	return maxNode;
}
