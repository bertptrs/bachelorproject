from graph import Edge, Algorithm
import random

class FordFulkerson(Algorithm):

    def __init__(self):
        Algorithm.__init__(self)

    def init(self, source, sink):
        self.source = source
        # Ensure reflectivity of edges
        for edge in self.E:
            self.F[edge] = 0
            backEdge = self.getBackEdge(edge)
            
            # Create the backedge if it does not exist.
            if not backEdge in self.C:
                self.C[backEdge] = 0
                self.F[backEdge] = 0
                self.E.append(backEdge)
        
    def getBackEdge(self, edge):
        return Edge(edge.v, edge.u)

    def findPath(self, path, source, sink):
        if source == sink:
            return path

        edges = list(self.E)
        random.shuffle(edges)

        for edge in edges:
            if edge.u == source and self.C[edge] > self.F[edge] and not edge in path:
                newPath = list(path)
                newPath.append(edge)
                newPath = self.findPath(newPath, edge.v, sink)
                if newPath != None:
                    return newPath
        
        return None

    def done(self):
        return self.findPath(list(), self.source, self.sink) == None

    def iter(self):
        path = self.findPath(list(), self.source, self.sink)
        assert path != None
        minFlow = float("inf")
        for edge in path:
            minFlow = min(minFlow, self.C[edge] - self.F[edge])

        for edge in path:
            self.F[edge] += minFlow
            self.F[self.getBackEdge(edge)] -= minFlow

if __name__ == '__main__':
    G = FordFulkerson()
    G.test()

    print "Completed."

