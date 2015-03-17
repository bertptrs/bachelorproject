#!/usr/bin/python2
import maxflow
import sys
import random
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--source', help="Source to use in the graph. Default is random.")
    parser.add_argument('-t', '--sink', help="Sink to use in the graph. Default is random.")
    parser.add_argument('-v', '--verbose', action="store_true", help="Increase verbosity")
    parser.add_argument('files', type=argparse.FileType('r'), help="The data file to use.", nargs="+")

    opts = parser.parse_args()

    for file in opts.files:
        print "Processing ", file.name
        edges = maxflow.readFile(file)

        source = opts.source
        sink = opts.sink

        if source == None:
            random.shuffle(edges)
            source = edges[0].u

        if sink == None:
            random.shuffle(edges)
            sink = edges[0].v

        print "Calculating flow over", maxflow.Edge(source, sink)

        graphs = maxflow.getGraphs()
        for graph in graphs:
            maxflow.initGraph(graph, edges)
            print graph.__class__.__name__, ":", graph.run(source, sink)

if __name__ == "__main__":
    main()
