from graph import Edge, Algorithm
import random
from collections import deque

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

    def findPath(self, path, source, sink):
        if source == sink:
            return path

        edges = self.outbound(source)
        random.shuffle(edges)

        for edge in edges:
            if self.C[edge] > self.F[edge] and not edge in path:
                newPath = list(path)
                newPath.append(edge)
                newPath = self.findPath(newPath, edge.v, sink)
                if newPath != None:
                    return newPath
        
        return None

    def outbound(self, node):
        edges = list()
        for edge in self.E:
            if edge.u == node:
                edges.append(edge)

        return edges

    def done(self):
        path = self.findPath(list(), self.source, self.sink)
        return path == None or len(path) == 0

    def iter(self):
        path = self.findPath(list(), self.source, self.sink)
        assert path != None
        minFlow = float("inf")
        for edge in path:
            minFlow = min(minFlow, self.C[edge] - self.F[edge])

        for edge in path:
            self.F[edge] += minFlow
            self.F[self.getBackEdge(edge)] -= minFlow

class EdmondsKarp(FordFulkerson):

    def __init__(self):
        FordFulkerson.__init__(self)

    def findPath(self, path, source, sink):

        visited = set()
        prev = dict()

        if source == sink:
            return path

        todo = deque([source])
        while todo:
            cur = todo.popleft()
            if cur in visited:
                continue

            visited.add(cur)
            edges = self.outbound(cur)
            random.shuffle(edges)
            for edge in edges:
                if self.C[edge] <= self.F[edge]:
                    continue

                node = edge.v
                if node not in prev and node not in visited:
                    prev[node] = cur
                    todo.append(node)

                if node == sink:
                    # Reconstruct path
                    path = list()
                    while node != source:
                        prevNode = prev[node]
                        path.append(Edge(prevNode, node))
                        node = prevNode

                    return path

        return None

if __name__ == '__main__':
    G = FordFulkerson()
    G.test()

    print "Completed."

