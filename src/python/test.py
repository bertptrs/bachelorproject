#!/usr/bin/python2 -u
import maxflow
import sys
import random
import argparse
import timing

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--source', help="Source to use in the graph. Default is random.")
    parser.add_argument('-t', '--sink', help="Sink to use in the graph. Default is random.")
    parser.add_argument('-v', '--verbose', action="store_true", help="Increase verbosity")
    parser.add_argument('files', type=argparse.FileType('r'), help="The data file to use.", nargs="+")
    parser.add_argument('-a', '--algorithm', help="The algorithm to use. Default is all of them")

    opts = parser.parse_args()

    for file in opts.files:
        print "Processing ", file.name
        edges = maxflow.readFile(file)

        source = opts.source
        sink = opts.sink

        if source == None:
            random.shuffle(edges)
            source = edges[0].u
        else:
            source = str(source)

        if sink == None:
            random.shuffle(edges)
            sink = edges[0].v
        else:
            sink = str(sink)

        print "Calculating flow over", maxflow.Edge(source, sink)

        graphs = maxflow.getGraphs()
        algos = 0
        for graph in graphs:
            maxflow.initGraph(graph, edges)
            if opts.algorithm is None or str(opts.algorithm) == str(graph.__class__.__name__):
                print graph.__class__.__name__, ":", graph.run(source, sink), "(" + str(graph.iterations) + ")"
                algos = algos + 1

        if algos == 0:
            print "No known algorithm selected."
            sys.exit(2)

if __name__ == "__main__":
    main()
