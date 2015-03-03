from graph import Edge, Algorithm
import random

class PushRelabel(Algorithm):

    def __init__(self):
        Algorithm.__init__(self)

    def init(self, source, sink):
        self.D = dict()
        self.H = dict()

        # Initialze Height and Excess
        for node in self.V:
            self.H[node] = 0
            self.D[node] = 0

        self.H[source] = len(self.V)

        # Ensure reflectivity of edges
        for edge in self.E:
            self.F[edge] = 0
            backEdge = self.getBackEdge(edge)
            
            # Create the backedge if it does not exist.
            if not backEdge in self.C:
                self.C[backEdge] = 0
                self.F[backEdge] = 0
                self.E.append(backEdge)

        # Perform the saturating initial push
        for edge in self.E:
            if edge.u == source:
                self.push(edge, float("inf")) # Should be a better solution

    def push(self, edge, amount):
        delta = max(0, min(amount, self.C[edge] - self.F[edge]))
        backEdge = self.getBackEdge(edge)
        self.F[edge] += delta
        self.F[backEdge] -= delta
        self.D[edge.u] -= delta
        self.D[edge.v] += delta

    def relabel(self, node):
        minHeight = float("inf")
        for edge in self.E:
            if edge.u == node and self.C[edge] > self.F[edge]:
                minHeight = min(minHeight, self.H[edge.v] + 1)

        self.H[node] = minHeight

    def done(self):
        for node in self.D:
            if self.D[node] > 0 and node != self.sink:
                return False

        return True

    def iter(self):
        node = self.getActiveNode()
        edge = self.getPushableEdge(node)

        if edge == None:
            print "Relabeling " + str(node)
            self.relabel(node)
        else:
            print "Pushing edge " + str(edge)
            self.push(edge, self.D[node])

    def getActiveNode(self):
        nodes = list(self.V)
        random.shuffle(nodes)

        for node in nodes:
            if self.D[node] > 0 and node != self.sink:
                return node

        assert False

    def getPushableEdge(self, node):
        edges = list(self.E)
        random.shuffle(edges)
        for edge in edges:
            if edge.u == node and self.H[edge.u] == self.H[edge.v] + 1 and self.C[edge] > self.F[edge]:
                return edge

        return None

    def print_status(self):
        Algorithm.print_status(self)
        for node in self.V:
            print "Node " + str(node) + " has height " + str(self.H[node])


if __name__ == '__main__':
    G = PushRelabel()
    G.test()

    print "Completed."
