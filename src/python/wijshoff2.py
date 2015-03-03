import random
from graph import Algorithm, Edge

class Wijshoff2(Algorithm):

    def __init__(self):
        Algorithm.__init__(self)
        self.changed = False
        self.source = None

    def init(self, source, sink):
        self.source = source
        self.changed = True
        self.Delta_F = dict()
        self.Remainder_C = dict()
        self.Type = dict()

        tmp = list()

        for edge in self.E:
            self.Type[edge] = 'F'
            self.F[edge] = 0

            backEdge = self.getBackEdge(edge)
            tmp.append(backEdge)
            self.Type[backEdge] = 'B'
            self.F[backEdge] = 0
            self.C[backEdge] = 0
            self.Delta_F[backEdge] = 0

            if edge.u == source:
                self.Delta_F[edge] = self.C[edge]
                self.Remainder_C[edge] = 0
                self.Remainder_C[backEdge] = self.C[edge]
            else:
                self.Delta_F[edge] = 0
                self.Remainder_C[edge] = self.C[edge]
                self.Remainder_C[backEdge] = 0

        self.E.extend(tmp)

    def iter(self):
        assert self.changed
        self.changed = False

        tmp = list(self.E)
        random.shuffle(tmp)

        for edge in tmp:
            if self.Delta_F[edge] > 0:
                if edge.v == self.source or edge.v == self.sink:
                    # flow out at source s or sink t
                    print "FLOW OUT AT", edge.v
                    self.F[Edge(edge.u,edge.v)] += self.Delta_F[edge]
                    self.Delta_F[Edge(edge.u,edge.v)] -= self.Delta_F[edge]
                    changed = True
                    return
                else:
                    tmpedge = self.pick_positive_remainder(edge, self.Type[edge])
                    
                    if tmpedge:
                        print "For ", edge, " found ", tmpedge
                        delta_change = min(self.Remainder_C[tmpedge], self.Delta_F[edge])
                        print "Pushing ", delta_change, " onto edge", tmpedge
                        
                        self.Delta_F[tmpedge] += delta_change
                        self.Remainder_C[self.getBackEdge(tmpedge)] += delta_change
                        
                        self.F[edge] += delta_change
                        self.Delta_F[edge] -= delta_change

                        self.Remainder_C[tmpedge] -= delta_change
                        
                        self.changed = True
                        return

                    else:
                        print "For ", edge , " no edge found"
                        backEdge = self.getBackEdge(edge)
                        if self.Remainder_C[backEdge] >= self.Delta_F[edge]:
                            # Move flow to back-edge
                            self.Delta_F[backEdge] += self.Delta_F[edge]
                            self.Remainder_C[backEdge] -= self.Delta_F[edge]

                            Delta_F[edge] = 0
                            self.changed = True
                        else:
                            print "Could not do anything for ", edge


    def pick_positive_remainder(self, edge, type):
        tmp = list(self.E)
        random.shuffle(tmp)
        for other in tmp:
            if edge.v == other.u and edge.u != other.v and self.Type[other] == type and self.Remainder_C[other] > 0:
                return edge

        return None

    def done(self):
        return not self.changed

    def print_status(self):
        Algorithm.print_status(self)
        for edge in self.Remainder_C:
            print "Remainder: ", edge, self.Remainder_C[edge], self.Type[edge]

if __name__ == '__main__':
    G = Wijshoff2()
    
    G.test()
    
    print "Completed."
