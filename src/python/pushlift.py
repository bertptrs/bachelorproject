# coding=utf-8
from graph import Edge, Algorithm
import random

class PushLift(Algorithm):

    def __init__(self):
        Algorithm.__init__(self)
        # Capacity function
        self.cf = dict()
        # Height
        self.h = dict()
        # Excess function
        self.e = dict()
        # Source and sink variables
        self.source = None
        self.sink = None
        # Nodes
        self.nodes = set()

    def init(self, source, sink):
        self.source = source
        self.sink = sink

        self.cf.clear()
        self.h.clear()
        self.e.clear()
        self.nodes.clear()

        # Ensure reflectivity of edges
        for edge in self.E:
            self.F[edge] = 0
            backEdge = self.getBackEdge(edge)

            # Create the backedge if it does not exist.
            if backEdge not in self.C:
                self.C[backEdge] = 0
                self.F[backEdge] = 0
                self.E.append(backEdge)

        # Initialize capacities
        for edge in self.E:
            backEdge = self.getBackEdge(edge)
            
            self.cf[edge] = self.C[edge]
            self.cf[backEdge] = self.C[backEdge]

            self.nodes.add(edge.u)
            self.nodes.add(edge.v)

        # Saturate all outgoing edges.
        for edge in self.outbound(source):
            backEdge = self.getBackEdge(edge)

            self.cf[edge] = 0
            self.cf[backEdge] = self.C[edge] + self.C[backEdge]
            self.e[edge.v] = self.C[edge]

        self.h[source] = len(self.nodes)
        for node in self.nodes:
            if node != source:
                self.h[node] = 0

            if not node in self.e:
                self.e[node] = 0

    def done(self):
        # the algorithm is done if no node is active.
        return not self.activeNodes()

    def iter(self):
        # Select a random active node
        active = list(self.activeNodes())
        random.shuffle(active)

        # Perform a single iteration of the "thread" loop, described in Hong's algorithm, line 4 - 22
        u = active[0]		# The current u being considered.
        e = self.e[u]		# e' in the original paper
        v = None			# v caret in the original paper
        h = float("inf")	# ĥ in the original paper

        # Find a pushable edge
        for edge in self.outbound(u):
            if self.cf[edge] <= 0:
                continue

            newH = self.h[edge.v] # h' in the paper
            if v is None or newH < self.h[v]:
                v = edge.v
                h = newH

        if self.h[u] > h:
            # We can apply a push
            edge = Edge(u, v)
            d = min(e, self.cf[edge])
            self.cf[edge] -= d
            self.cf[self.getBackEdge(edge)] += d

            self.e[u] -= d
            self.e[v] += d
        else:
            # We must lift this node.
            self.h[u] = h + 1

    def outbound(self, u):
        # Calculate outbound edges for node u
        edges = set()

        for edge in self.E:
            if edge.u is u:
                edges.add(edge)

        return edges

    def activeNodes(self):
        nodes = set()

        for node in self.e:
            if node == self.sink or node == self.source:
                continue

            if self.e[node] > 0:
                nodes.add(node)

        return nodes

    def print_status(self):
        for node in self.nodes:
            print "Node", node, "excess:", self.e[node], "height", self.h[node]

        for edge in self.E:
            print edge, "capacity:", self.C[edge], "remaining", self.cf[edge]

    def flow(self, node):
        if node != self.sink:
            print "flow is defined only at the sink"
            raise Exception()

        return self.e[node]
