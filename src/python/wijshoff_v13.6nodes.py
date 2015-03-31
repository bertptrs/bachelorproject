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
    global Old_F, F, Remainder_C, Unsat, Initial

    # Initialize
    Old_F = dict()
    F = dict()
    Remainder_C = dict()
    Unsat = dict()
    Initial = dict()
    tmp = list()
    for edge in G:
        if edge.u == 's':
            Initial[edge] = 'T'
            Old_F[edge] = 0
            F[edge] = C[edge]
            Remainder_C[edge] = 0
            Unsat[edge] = 0
        else:
	    Initial[edge] = 'T'
            Old_F[edge] = 0
            F[edge] = 0
            Remainder_C[edge] = C[edge]
            Unsat[edge] = 0

    #G.extend(tmp)

def pick_positive_remainder(v):
    global G, Remainder_C

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.u == v and Remainder_C[edge] > 0 and F[edge] >= Old_F[edge]:
            return edge

    return None

def pick_positive_flow(u):
    global G, F

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.v == u and F[edge] > 0:
            return edge

    return None

def pick_positive_unsat(v):
    global G, Remainder_C

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.u == v and Unsat[edge] > 0:
            return edge

    return None

def pick_positive_deltaflowforward(v):
    global G, F, Old_F

    # Shuffle edges such that we always visit these in different order.
    # To disable: comment out line with "random.shuffle()".
    tmp = list(G)
    random.shuffle(tmp)
    for edge in tmp:
        if edge.u == v and F[edge] - Old_F[edge] > 0 and Old_F[edge] > 0:
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

        if iteration > 100:
            exit()

        # Shuffle edges such that we always visit these in different order.
        # To disable: comment out line with "random.shuffle()".
        tmp = list(G)
        random.shuffle(tmp)
        for edge in tmp:
            if Old_F[edge] != F[edge]:

                if (edge.u == 's' and Old_F[edge] > F[edge]) or (edge.v == 't' and F[edge] > Old_F[edge]):
                    print "FLOWING OUT FOR EDGE <", edge.u, edge.v, ">"
                    Old_F[edge] = F[edge]

                    changed = True
                    print_state()
                else:
                    if F[edge] > Old_F[edge]:
                        # Pick <v, w> such that Unsat(v, w) > 0
                        tmpedge = pick_positive_unsat(edge.v)

                        if tmpedge and Unsat[edge] == 0:
                            print "FORWARDING FLOW FROM <", edge.u, edge.v, "> TO UNSAT FOR EDGE <", tmpedge.u, tmpedge.v, ">"
                            delta_change = min(Unsat[tmpedge], F[edge] - Old_F[edge])
                            Old_F[edge] += delta_change 
                            Unsat[tmpedge] -= delta_change

                            Initial[edge] = 'F'
                            changed = True
                            print_state()
                        else:
                            if F[edge] > Old_F[edge]:
                                # Pick <v, w> such that Remainder_C(v, w) > 0 and F(v,w) > Old_F(v,w)
                                tmpedge = pick_positive_remainder(edge.v)

                                if tmpedge:
                                    # such edge exists
                                    if F[edge] >= Old_F[edge]:
                                        print "FOR EDGE <", edge.u, edge.v, "> FOUND EDGE <", tmpedge.u, tmpedge.v, ">"
                                        print "PUSHING DELTA_CHANGE ONTO EDGE <", tmpedge.u, tmpedge.v, ">"

                                        delta_change = min(Remainder_C[tmpedge], F[edge] - Old_F[edge])

                                        Old_F[edge] += delta_change

                                        F[tmpedge] += delta_change
                                        Remainder_C[tmpedge] -= delta_change
    
                                        Initial[edge] = 'F'
                                        changed = True
                                        print_state()
                                else:
                                    # such edge does not exist
                                    if (Unsat[edge] == 0):
                                        print "FOR EDGE <", edge.u, edge.v, "> EDGE NOT FOUND"

                                        delta_change = F[edge] - Old_F[edge]

                                        Old_F[edge] += delta_change
                                        F[edge] -= delta_change
                                        Remainder_C[edge] += delta_change

                                        Initial[edge] = 'F'
                                        changed = True
                                        print_state()
                                    else:
                                        print "FOR EDGE <", edge.u, edge.v, "> EDGE NOT FOUND AND REDUCING UNSAT"

                                        delta_change = min(F[edge] - Old_F[edge], Unsat[edge])

                                        Unsat[edge] -= delta_change
                                        F[edge] -= delta_change
                                        Remainder_C[edge] += delta_change

                                        Initial[edge] = 'F'
                                        changed = True
                                        print_state()

                    else:
                        if  F[edge] < Old_F[edge]:
                            if Unsat[edge] == 0:
                                # Pick <w, u> such that F(w, u) > 0
                                tmpedge = pick_positive_flow(edge.u)

                                if tmpedge:
                                    # such edge exists
                                    print "FOR EDGE <", edge.u, edge.v, "> FOUND EDGE <", tmpedge.u, tmpedge.v, ">"
                                    print "PUSHING DELTA_CHANGE ONTO EDGE <", tmpedge.u, tmpedge.v, ">"

                                    delta_change = min(Old_F[tmpedge], Old_F[edge] - F[edge])

                                    Old_F[edge] -= delta_change

                                    Old_F[tmpedge] -= delta_change

                                    Initial[edge] = 'F'
                                    changed = True
                                    print_state()
                                else:
                                    print "SOMETHING WENT WRONG WITH EDGE <", edge.u, edge.v, ">"
                                    print_state()

                            else:
                                # Unsat[edge] != 0
                                print "ABSORBING LOCALLY FOR EDGE <", edge.u, edge.v, ">"
                                delta_change = min(Unsat[edge], Old_F[edge] - F[edge])

                                Old_F[edge] -= delta_change
                                Unsat[edge] -= delta_change

                                Initial[edge] = 'F'
                                changed = True
                                print_state()


            if Unsat[edge] != 0 and Old_F[edge] >= Unsat[edge]:
                if (edge.v == 't'):
                    print "FLOWING UNSAT OUT FOR EDGE <", edge.u, edge.v, ">"
                    # Note that for any edge <x, t>: F(x, t) >= Old_F(x, t) at all times
                    F[edge] -= Unsat[edge]
                    Old_F[edge] -= Unsat[edge]
                    Remainder_C[edge] += Unsat[edge]
                    Unsat[edge] = 0

                    Initial[edge] = 'F'
                    changed = True
                    print_state()
                else:
                    # Pick <v, w> such that F(v,w) - Old_F(v,w) > 0 and Old_F(v,w) > 0
                    tmpedge = pick_positive_deltaflowforward(edge.v)

                    if tmpedge:
                        print "FORWARDING UNSAT FOR EDGE <", edge.u, edge.v, "> TO <", tmpedge.u, tmpedge.v, ">"
                        delta_change = min(Unsat[edge], F[tmpedge] - Unsat[tmpedge])
                        F[edge] -= delta_change
                        Remainder_C[edge] += delta_change
                        Unsat[edge] -= delta_change
                        Unsat[tmpedge] += delta_change

                        Initial[edge] = 'F'
                        changed = True
                        print_state()
                    else:
                        if F[edge] > 0:
                            print "UNLOADING UNSAT FOR EDGE <", edge.u, edge.v, ">"
                            delta_change = min(Unsat[edge], F[edge])
                            F[edge] -= delta_change
                            Remainder_C[edge] += delta_change
                            Unsat[edge] -= delta_change

                            Initial[edge] = 'F'
                            changed = True
                            print_state()

            if edge.u != 's' and Unsat[edge] == 0 and F[edge] == 0 and Old_F[edge] == 0 and Remainder_C[edge] > 0 and Initial[edge] == 'T':
                print "LOADING EDGE <", edge.u, edge.v, ">"
                F[edge] += Remainder_C[edge]
                Unsat[edge] += Remainder_C[edge]
                Remainder_C[edge] = 0

                Initial[edge] = 'F'
                changed = True
                print_state()

            #if edge.u == 's' and F[edge] >= Old_F[edge] and Remainder_C[edge] > 0 and Unsat[edge] == 0:
            #    print "RELOADING EDGE <", edge.u, edge.v, ">"
            #    F[edge] += Remainder_C[edge]
            #    Remainder_C[edge] = 0
            #
            #    print_state()
            #    changed = True

def load_graph():
    ''' Returns a graph G that is a list of Edges (u, v) and a capacity hash table indexed by edge.'''
    G = list()

    # Graph taken from wikipedia:
    #   http://en.wikipedia.org/wiki/Maximum_flow_problem#mediaviewer/File:MFP1.jpg
    G.append(Edge('s', 'o'))
    G.append(Edge('s', 'p'))
    G.append(Edge('o', 'q'))
    G.append(Edge('o', 'p'))
    G.append(Edge('p', 'r'))
    G.append(Edge('q', 't'))
    G.append(Edge('q', 'r'))
    G.append(Edge('r', 't'))

    C = dict()
    C[Edge('s', 'o')] = 3
    C[Edge('s', 'p')] = 3
    C[Edge('o', 'q')] = 3
    C[Edge('o', 'p')] = 2
    C[Edge('p', 'r')] = 2
    C[Edge('q', 't')] = 2
    C[Edge('q', 'r')] = 4
    C[Edge('r', 't')] = 3

    return G, C

def print_state():
    print "Edge\tF\t\tOld_F\t\tRemainder_C\tUnsat"
    for edge in G:
        print "<%s, %s>\t%f\t%f\t%f\t%f" % (edge.u, edge.v, F[edge], Old_F[edge], Remainder_C[edge], Unsat[edge])

    print

def print_solution():
    for edge in origG:
        print edge, " capacity ", C[edge], " flow ", F[edge]

    print

if __name__ == '__main__':
    G, C = load_graph()
    origG = list(G)

    init()
    algo()

    print "Completed."
    print_solution()
