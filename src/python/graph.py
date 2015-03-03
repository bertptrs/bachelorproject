import random
from collections import namedtuple

Edge = namedtuple('Edge', 'u v')

class Algorithm:

    def __init__(self):
        # Capacities for edges
        self.C = dict()
        # Set of nodes
        self.V = set()
        # List of edges
        self.E = list()
        # Current flow
        self.F = dict()

        self.sink = None
    
    def run(self, source, sink):
        self.sink = sink
        self.init(source, sink)
        self.iterations = 0
        while not self.done():
            self.iter()
            self.print_status()
            self.iterations += 1

        return self.flow(sink)

    def test(self):
        self.addEdge('s', 'u', 10)
        self.addEdge('s', 'v', 1)
        self.addEdge('u', 't', 5)
        self.addEdge('u', 'v', 15)
        self.addEdge('v', 't', 20)

        flow = self.run('s', 't')

        assert flow == 11
        print "Completed after " + str(self.iterations) + " iterations."

    def print_status(self):
        for edge in self.E:
            print str(edge) + " (" + str(self.C[edge]) + "): " + str(self.F[edge])
        print "Current flow at sink: " + str(self.flow(self.sink))
 
    def addEdge(self, u, v, c):
        assert c > 0, "Capacities for edges should be positive."
        edge = Edge(u, v)

        if edge in self.C:
            self.C[edge] += c
        else:
            self.C[edge] = c

        self.V.add(u)
        self.V.add(v)
        self.E.append(edge)
        self.F[edge] = 0

    def flow(self, v):
        flow = 0
        for edge in self.F:
            if edge.v == v:
                flow += self.F[edge]

        return flow
