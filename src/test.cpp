#include <iostream>
#include <cassert>
#include "AbstractGraph.h"
#include "FFTuple.h"
#include "EKTuple.h"

using namespace std;

void testWorstCase(AbstractGraph& g) {
	g.clear();

	g.addEdge('s', 'a', 1000);
	g.addEdge('s', 'b', 1000);
	g.addEdge('a', 't', 1000);
	g.addEdge('b', 't', 1000);
	g.addEdge('a', 'a', 1);

	cout << g.maxFlow('s', 't') << endl;
	assert(g.maxFlow('s', 't') == 2000);
}

void testNormal(AbstractGraph& g) {
	g.clear();

	g.addEdge('s', 'a', 15);
	g.addEdge('s', 'c', 4);
	g.addEdge('a', 'b', 12);
	g.addEdge('b', 'c', 3);
	g.addEdge('b', 't', 7);
	g.addEdge('c', 'd', 10);
	g.addEdge('d', 'b', 5);
	g.addEdge('d', 't', 10);

	cout << g.maxFlow('s', 't') << endl;
	assert(g.maxFlow('s', 't') == 14);
}

int main() {
	EKTuple graph;
	testWorstCase(graph);
	testNormal(graph);
	cout << "OK" << endl;
	return 0;
}
