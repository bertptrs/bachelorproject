import argparse
import fordfulkerson
import pushrelabel
import pushlift
import sys
from collections import namedtuple

Edge = namedtuple('Edge', 'u v')
WeightedEdge = namedtuple('WeightedEdge', 'u v c')

def getGraphs():
    return [
			pushlift.PushLift(),
            pushrelabel.PushRelabel(),
            fordfulkerson.FordFulkerson()
            ]

def readFile(input, default=1.0):
    edges = list()
    for line in input:
        line = line.strip()
        components = line.split()
        if len(components) == 3:
            edges.append(WeightedEdge(components[0], components[1], float(components[2])))
        else:
            if len(components) == 2:
                edges.append(WeightedEdge(components[0], components[1], default))
            else:
                print "Invalid tuple provided:", components
                sys.exit(1)

    return edges

def initGraph(graph, edges):
    for edge in edges:
        graph.addEdge(edge.u, edge.v, edge.c)
