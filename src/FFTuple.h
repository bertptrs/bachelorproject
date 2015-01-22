#ifndef FFTUPLE_H
#define FFTUPLE_H

#include <iostream>
#include <tuple>
#include <set>
#include <vector>
#include <algorithm>
#include <limits>
#include "AbstractGraph.h"

using namespace std;

/**
 * Ford-Fulkerson implementation using tuples
 */
class FFTuple : public AbstractGraph {
	protected:
		// from, to, cap, flow
		typedef tuple<int, int, double, double> Edge;

		// List of edges. Back-edges are at odd-indices.
		vector<Edge> edges;

		template<class Predicate>
			set<int> getEdges(Predicate p) const {
				set<int> result;
				for (unsigned int i = 0; i < edges.size(); i++) {
					if (p(edges[i])) {
						result.insert(i);
					}
				}
				return result;
			}

		virtual vector<int> getPath(const vector<int> curPath, const int source, const int dest) const;
		double minResiduals(const vector<int>& pathEdges) const;
		void init();
		static constexpr int backEdge(int i);

	public:
		virtual double maxFlow(const int source, const int sink);
		virtual void addEdge(int u, int v, double c);
		virtual void clear();

};

#endif
