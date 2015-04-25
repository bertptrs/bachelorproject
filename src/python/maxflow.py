import argparse
import fordfulkerson
import pushrelabel
import pushlift
import sys
import scipy.sparse
import scipy.io
from collections import namedtuple

Edge = namedtuple('Edge', 'u v')
WeightedEdge = namedtuple('WeightedEdge', 'u v c')

def getGraphs():
    return [
			pushlift.PushLift(),
            pushrelabel.PushRelabel(),
            fordfulkerson.FordFulkerson(),
            fordfulkerson.EdmondsKarp()
            ]

def readFile(input, default=1.0):
    if input.name.endswith(".edges"):
        return readFileEdges(input, default)

    if input.name.endswith(".mtx"):
        return readFileMatrixMarket(input)
    
    print "Unsupported file format:", input.name
    return list()
    
def readFileEdges(input, default):
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

def readFileMatrixMarket(input):
    matrix = scipy.io.mmread(input)
    edges = list()

    entries = scipy.sparse.find(matrix)
    for i in range(0, len(entries[0])):
        if entries[0][i] != entries[1][i]:
            edges.append(WeightedEdge(str(entries[0][i]), str(entries[1][i]), entries[2][i]))
    
    return edges

def initGraph(graph, edges):
    for edge in edges:
        graph.addEdge(edge.u, edge.v, edge.c)
