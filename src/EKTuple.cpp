#include "EKTuple.h"
#include <map>

vector<int> EKTuple::getPath(const vector<int>, const int source, const int dest) const {
	set<int> visited;
	queue<int> todo;
	map<int, int> prev;

	// Add any useful edges to the todo list
	addTodo(todo, [source](Edge e) { return get<0>(e) == source && get<2>(e) > get<3>(e); });
	while (!todo.empty()) {
		int v;
		int index = todo.front();
		todo.pop();
		const Edge& edge = edges[index];
		if (visited.count(index)) {
			continue;
		}

		visited.insert(index);
		v = get<1>(edge);
		if (v == dest) {
			// Reconstruct the path. The order does not matter.
			vector<int> path = {index};
			while (prev.find(index) != prev.end()) {
				index = prev[index];
				path.push_back(index);
			}

			return path;
		} else {
			for (int edge : getEdges([v](Edge e) { return get<0>(e) == v && get<2>(e) > get<3>(e); })) {
				if (!visited.count(edge) && prev.find(edge) == prev.end()) {
					prev[edge] = index;
					todo.push(edge);
				}
			}
		}
	}

	return {};
}

