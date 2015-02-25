import sys
import random
from collections import namedtuple

Edge = namedtuple('Edge', 'u v')


def check_assertions():
    # TODO
    pass

def init():
    global G, C
    global Delta_F, F, Remainder_C

    # Initialize for edges and back-edges
    Delta_F = dict()
    F = dict()
    Remainder_C = dict()
    tmp = list()
    for edge in G:
        Delta_F[Edge(edge.u, edge.v)] = 0
        F[Edge(edge.u, edge.v)] = 0
        Remainder_C[Edge(edge.u, edge.v)] = C[Edge(edge.u, edge.v)]

        # back-edge
        tmp.append(Edge(edge.v, edge.u))
        Delta_F[Edge(edge.v, edge.u)] = 0
        F[Edge(edge.v, edge.u)] = 0
        Remainder_C[Edge(edge.v, edge.u)] = 0

    G.extend(tmp)

    # Initialize for additional node S.
    G.append(Edge('S', 's'))
    G.append(Edge('s', 'S'))
    F[Edge('S', 's')] = 0
    F[Edge('s', 'S')] = 0
    Delta_F[Edge('S', 's')] = sys.maxint
    Delta_F[Edge('s', 'S')] = 0
    Remainder_C[Edge('S', 's')] = 0
    Remainder_C[Edge('s', 'S')] = sys.maxint

    # Initialize for additional node T.
    G.append(Edge('T', 't'))
    G.append(Edge('t', 'T'))
    F[Edge('t', 'T')] = 0
    F[Edge('T', 't')] = 0
    Delta_F[Edge('t', 'T')] = 0
    Delta_F[Edge('T', 't')] = 0
    Remainder_C[Edge('t', 'T')] = sys.maxint
    Remainder_C[Edge('T', 't')] = 0


def pick_positive_remainder(v,u):
    global G, Remainder_C

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.u == v and edge.v != u and Remainder_C[edge] > 0:
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
            if Delta_F[edge] != 0 and (edge.u != 't' or edge.v != 'T') and (edge.u != 'T' or edge.v != 't') and (edge.u != 's' or edge.v != 'S'):
                # Pick <v, w> such that Remainder_C(v, w) > 0
                tmpedge = pick_positive_remainder(edge.v,edge.u)

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

		    if edge.v == 's' and edge.u != 'S':
		        Delta_F[Edge('S','s')] += delta_change
		    print_state()
                else:
                    # such edge does not exist
		    print "FOR EDGE ", edge.u, edge.v, "EDGE NOT FOUND"
                    Delta_F[Edge(edge.v, edge.u)] += Delta_F[edge]
                    if Delta_F[edge] == sys.maxint:
                        Remainder_C[Edge(edge.v, edge.u)] = Delta_F[edge]
                    else:
                        Remainder_C[Edge(edge.v, edge.u)] -= Delta_F[edge]


                    Delta_F[edge] = 0
                    if (edge.v == 's' and edge.u == 'u') or (edge.v == 's' and edge.u == 'v') or (edge.u == 'v' and edge.v == 'u') or (edge.v == 'u' and edge.u == 't') or (edge.v == 'v' and edge.u == 't'):
                        if F[Edge(edge.v,edge.u)] + Delta_F[Edge(edge.v,edge.u)] - F[Edge(edge.u,edge.v)] - Delta_F[Edge(edge.u,edge.v)] > C[Edge(edge.v, edge.u)]:
                            print "WARNING!!!!!!!!!!! TOO MUCH FLOW ON <", edge.v, edge.u, "> REMAINDER EQUALS", Remainder_C[Edge(edge.v, edge.u)]
                            print " "
		    print_state()



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
    print "Edge\tF\t\tDelta_F\t\tRemainder_C"
    for edge in G:
        print "<%s, %s>\t%f\t%f\t%f" % (edge.u, edge.v, F[edge], Delta_F[edge], Remainder_C[edge])

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
