#include "PushRelabel.h"
#include <cassert>
#include "util.h"
#include <limits>
#include <iostream>

PushRelabel::Edge::Edge(int u, int v, double c) :
	origin(u),
	destination(v),
	capacity(c),
	flow(0)
{
}

template<class Predicate>
PushRelabel::eset PushRelabel::forPredicate(Predicate P) const {
	eset result;
	for (eptr edge : edges) {
		if (P(edge)) {
			result.insert(edge);
		}
	}
	return result;
}

PushRelabel::iset PushRelabel::getActive(const int source) const {
	iset activeNodes;
	for (int i : nodes()) {
		// The source is never an active node,
		// and also has a negative flow.
		if (i == source) continue;
		double flow = getExcess(i);

		// Sanity check
		assert(flow >= 0 && "Flow can never be negative!");
		
		if (flow > 0) {
			activeNodes.insert(i);
		}
	}
	return activeNodes;
}

double PushRelabel::getExcess(int i) const {
	// Since there is negative incoming flow for back edges, counting
	// the incoming flow is sufficient.
	eset incoming = getIncoming(i);
	return accumulate(incoming.begin(), incoming.end(), 0.0, [](double flow, const eptr& e) {
			return flow + e->flow;
	});
}

PushRelabel::eset PushRelabel::getOutgoing(int u) const {
	return forPredicate([u](eptr e) -> bool { return e->origin == u; });
}

PushRelabel::eset PushRelabel::getIncoming(int v) const {
	return forPredicate([v](const eptr& e) -> bool { return e->destination == v; });
}

void PushRelabel::addEdge(int u, int v, double c) {
	edges.insert(make_shared<Edge>(u, v, c));
	edges.insert(make_shared<Edge>(v, u, 0));
}

void PushRelabel::clear() {
	edges.clear();
}

void PushRelabel::init() {
	for (eptr edge : edges) {
		edge->flow = 0;
	}
	heights.clear();
}

double PushRelabel::maxFlow(const int s, const int t) {
	init();
	// Saturate outgoing edges of source
	for (eptr edge : getOutgoing(s)) {
		push(edge, numeric_limits<double>::infinity());
	}
	heights[s] = numNodes();

	iset active = getActive(s);
	while (!active.empty()) {
		const int node = randomElement(active);
		if (node == t) {
			active.erase(node);
			continue;
		}
		double excess = getExcess(node);
		assert(excess > 0);
		bool pushed = false;
		for (eptr edge : getOutgoing(node)) {
			if (canPush(edge) && excess) {
				excess -= push(edge, excess);
				pushed = true;

				if (excess == 0) {
					break;
				}
			}
		}

		if (!pushed) {
			// If we did not push, we should attempt a relabel.
			relabel(node);
		}

		active = getActive(s);
	}
	
	return getFlow(t);
}

PushRelabel::eptr PushRelabel::getBackEdge(const eptr& edge) const {
	eset candidates = forPredicate([edge](const eptr other) -> bool { return edge->origin == other->destination && edge->destination == other->origin; });
	assert(candidates.size() >= 1);

	return *candidates.begin();
}

bool PushRelabel::canPush(eptr edge) {
	int originHeight = heights[edge->origin];
	int destinationHeight = heights[edge->destination];

	return originHeight == destinationHeight + 1 && edge->capacity > edge->flow;
}

double PushRelabel::push(eptr edge, double amount) {
	double delta = min(amount, edge->capacity - edge->flow);
	eptr inverse = getBackEdge(edge);
	edge->flow += delta;
	inverse->flow -= delta;
	excess[edge->origin] -= delta;
	excess[edge->destination] += delta;

	return delta;
}

void PushRelabel::relabel(int node) {
	int newHeight = numeric_limits<int>::max();
	for (const eptr& edge : getOutgoing(node)) {
		if (edge->capacity > edge->flow) {
			newHeight = min(newHeight, heights[edge->destination]);
		}
	}

	heights[node] = newHeight + 1;
}

double PushRelabel::getFlow(const int t) const {
	double flow = 0;
	for (const eptr& edge : getIncoming(t)) {
		flow += edge->flow;
	}

	return flow;
}

int PushRelabel::numNodes() const {
	return (int) nodes().size();
}

set<int> PushRelabel::nodes() const {
	set<int> nodes;
	for (eptr e : edges) {
		nodes.insert(e->origin);
		nodes.insert(e->destination);
	}
	return nodes;
}
