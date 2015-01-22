#include "FFTuple.h"
#include "util.h"
#include <limits>

void FFTuple::addEdge(int source, int destination, double capacity) {
	edges.emplace_back(source, destination, capacity, 0);
	edges.emplace_back(destination, source, 0, 0);
}

vector<int> FFTuple::getPath(const vector<int> curPath, const int source, const int dest) const {
	if (source == dest) {
		return curPath;
	}

	auto outgoing = getEdges([source] (Edge e) { return get<0>(e) == source;});
	for (auto edgeNo : outgoing) {
		auto& edge = edges[edgeNo];
		int remaining = get<2>(edge) - get<3>(edge);
		if (remaining > 0 && !contains(curPath.begin(), curPath.end(), edgeNo)) {
			vector<int> newPath(curPath);
			newPath.push_back(edgeNo);
			newPath = getPath(newPath, get<1>(edge), dest);
			if (!newPath.empty()) {
				return newPath;
			}
		}
	}

	return {};
}

double FFTuple::minResiduals(const vector<int>& pathEdges) const {
	double minResidual = numeric_limits<double>::infinity();
	for (const int i : pathEdges) {
		auto& edge = edges[i];
		minResidual = min(minResidual, get<2>(edge) - get<3>(edge));
	}
	return minResidual;
}

constexpr int FFTuple::backEdge(int i) {
	return i ^ 1;
}

double FFTuple::maxFlow(const int source, const int sink) {
	init();
	auto path = getPath({}, source, sink);
	while (!path.empty()) {
		double flow = minResiduals(path);
		for (int i : path) {
			get<3>(edges[i]) += flow;
			get<3>(edges[backEdge(i)]) -= flow;
		}
		path = getPath({}, source, sink);
	}

	double flow = 0;
	for (int i : getEdges([source] (Edge e) { return get<0>(e) == source;})) {
		flow += get<3>(edges[i]);
	}
	return flow;
}

void FFTuple::clear() {
	edges.clear();
}

void FFTuple::init() {
	for (Edge& edge : edges) {
		get<3>(edge) = 0;
	}
}
