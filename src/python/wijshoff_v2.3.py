# ADDED EDGE TYPES
import sys
import random
from collections import namedtuple

Edge = namedtuple('Edge', 'u v')


def check_assertions():
    # TODO
    pass

def init():
    global G, C
    global Type, Delta_F, F, Remainder_C

    # Initialize for edges and back-edges
    Type = dict()
    Delta_F = dict()
    F = dict()
    Remainder_C = dict()
    tmp = list()
    for edge in G:
        if edge.u == 's':

            Type[Edge(edge.u, edge.v)] = 'F'
            Delta_F[Edge(edge.u, edge.v)] = C[Edge(edge.u, edge.v)]
            F[Edge(edge.u, edge.v)] = 0
            Remainder_C[Edge(edge.u, edge.v)] = 0

            # back-edge
            tmp.append(Edge(edge.v, edge.u))
            Type[Edge(edge.v, edge.u)] = 'B'
            Delta_F[Edge(edge.v, edge.u)] = 0
            F[Edge(edge.v, edge.u)] = 0
            Remainder_C[Edge(edge.v, edge.u)] = C[Edge(edge.u, edge.v)]

        else:
            Type[Edge(edge.u, edge.v)] = 'F'
            Delta_F[Edge(edge.u, edge.v)] = 0
            F[Edge(edge.u, edge.v)] = 0
            Remainder_C[Edge(edge.u, edge.v)] = C[Edge(edge.u, edge.v)]

            # back-edge
            tmp.append(Edge(edge.v, edge.u))
            Type[Edge(edge.v, edge.u)] = 'B'
            Delta_F[Edge(edge.v, edge.u)] = 0
            F[Edge(edge.v, edge.u)] = 0
            Remainder_C[Edge(edge.v, edge.u)] = 0

    G.extend(tmp)

def pick_positive_remainder(v,u,type):
    global G, Remainder_C, Type

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.u == v and edge.v != u and Type[Edge(edge.u,edge.v)] == type and Remainder_C[edge] > 0:
            return edge

    return None


def algo():
    iteration = 0
    delta_change = 0
    print_state()
    changed = True
    while changed:
        changed = False

        print "ITERATION ", iteration
        #print_state()
        iteration += 1

        # Shuffle edges such that we always visit these in different order.
        # To disable: comment out line with "random.shuffle()".
        tmp = list(G)
        random.shuffle(tmp)
        for edge in tmp:
            if Delta_F[edge] != 0:
                if edge.v == 's' or edge.v == 't':
                    # flow out at source s or sink t
                    print "FLOW OUT AT", edge.v
                    F[Edge(edge.u,edge.v)] += Delta_F[edge]
                    Delta_F[Edge(edge.u,edge.v)] -= Delta_F[edge]
		    print_state()
                else:
                    # Pick <v, w> such that Remainder_C(v, w) > 0
                    tmpedge = pick_positive_remainder(edge.v,edge.u,Type[Edge(edge.u,edge.v)])

                    changed = True
                    if tmpedge:
                        # such edge exists
		        print "FOR EDGE <", edge.u, edge.v, "> FOUND EDGE <", tmpedge.u, tmpedge.v, ">"
		        delta_change = min(Remainder_C[tmpedge], Delta_F[edge])
                        print "PUSHING ", delta_change, "ONTO EDGE <", tmpedge.u, tmpedge.v, ">"
                        Delta_F[tmpedge] += delta_change
                        Remainder_C[Edge(tmpedge.v,tmpedge.u)] += delta_change

                        F[Edge(edge.u,edge.v)] += delta_change
                        Delta_F[Edge(edge.u,edge.v)] -= delta_change
                    
                        Remainder_C[tmpedge] -= delta_change
                        print "FLOW FOR EDGE <", edge.u, edge.v, "> BECOMES", F[Edge(edge.u,edge.v)]
                        print "DELTA FOR EDGE <", edge.u, edge.v, "> BECOMES", Delta_F[Edge(edge.u,edge.v)]
                        print "NEW DELTA FOR EDGE <", tmpedge.u, tmpedge.v, "> BECOMES", Delta_F[tmpedge]
                        print "NEW REMAINDER FOR EDGE <", tmpedge.u, tmpedge.v, "> BECOMES", Remainder_C[tmpedge]
                        print "REMAINDER FOR EDGE <", tmpedge.v, tmpedge.u, "> IS INCREASED TO", Remainder_C[Edge(tmpedge.v,tmpedge.u)]
                        print " "
                        if (edge.u == 's' and edge.v == 'u') or (edge.u == 's' and edge.v == 'v') or (edge.u == 'u' and edge.v == 'v') or (edge.u == 'u' and edge.v == 't') or (edge.u == 'v' and edge.v == 't'):
                            if F[Edge(edge.u,edge.v)] + Delta_F[Edge(edge.u,edge.v)] - F[Edge(edge.v,edge.u)] - Delta_F[Edge(edge.v,edge.u)] > C[Edge(edge.u, edge.v)]:
                                print "WARNING!!!!!!!!!!! TOO MUCH FLOW ON <", edge.u, edge.v, "> REMAINDER EQUALS", Remainder_C[Edge(edge.u,edge.v)]
                                print " "

		        print_state()
                    else:
                        # such edge does not exist
		        print "FOR EDGE ", edge.u, edge.v, "EDGE NOT FOUND"
                        if Remainder_C[Edge(edge.v, edge.u)] >= Delta_F[edge]:
                            Delta_F[Edge(edge.v, edge.u)] += Delta_F[edge]
                            Remainder_C[Edge(edge.v, edge.u)] -= Delta_F[edge]

                            Delta_F[edge] = 0
                            if (edge.v == 's' and edge.u == 'u') or (edge.v == 's' and edge.u == 'v') or (edge.u == 'v' and edge.v == 'u') or (edge.v == 'u' and edge.u == 't') or (edge.v == 'v' and edge.u == 't'):
                                if F[Edge(edge.v,edge.u)] + Delta_F[Edge(edge.v,edge.u)] - F[Edge(edge.u,edge.v)] - Delta_F[Edge(edge.u,edge.v)] > C[Edge(edge.v, edge.u)]:
                                    print "WARNING!!!!!!!!!!! TOO MUCH FLOW ON <", edge.v, edge.u, "> REMAINDER EQUALS", Remainder_C[Edge(edge.v, edge.u)]
                                    print " "
		            print_state()
                        else:
                            # do nothing
                            print "FOR EDGE ", edge.u, edge.v, "DO NOTHING"



def load_graph():
    ''' Returns a graph G that is a list of Edges (u, v) and a capacity hash table indexed by edge.'''
    G = list()

    # Graph taken from wikipedia:
    #   http://en.wikipedia.org/wiki/Maximum_flow_problem#mediaviewer/File:MFP1.jpg
    G.append(Edge('s', 'u'))
    G.append(Edge('s', 'v'))
    G.append(Edge('u', 't'))
    G.append(Edge('u', 'v'))
    G.append(Edge('v', 't'))

    C = dict()
    C[Edge('s', 'u')] = 10
    C[Edge('s', 'v')] = 1
    C[Edge('u', 't')] = 5
    C[Edge('u', 'v')] = 15
    C[Edge('v', 't')] = 20

    return G, C

def print_state():
    print "Edge\tF\t\tDelta_F\t\tRemainder_C\tType"
    for edge in G:
        print "<%s, %s>\t%f\t%f\t%f\t\%s" % (edge.u, edge.v, F[edge], Delta_F[edge], Remainder_C[edge], Type[edge])

    print

def print_solution():
    for edge in origG:
        print edge, " capacity ", C[edge], " flow ", F[edge] - F[Edge(edge.v,edge.u)]

    print

if __name__ == '__main__':
    G, C = load_graph()
    origG = list(G)

    init()
    algo()

    print "Completed."
    print_solution()
