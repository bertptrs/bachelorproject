#!/usr/bin/env python3
import matplotlib.pyplot as plt
import graphhelper
import collections
import math

def getTimings(filelist):
    timings = {}

    for data in filelist:
        ncores, initialization, _, _ = graphhelper.getData(data)

        if ncores not in timings:
            timings[ncores] = []

        timings[ncores].append(initialization)

    return {k: sum(v) / len(v) for (k, v) in sorted(timings.items())}

def plot(timings):
    xvalues = sorted(timings.keys())
    yvalues = [timings[v] for v in xvalues]

    plt.xlim(min(xvalues), max(xvalues))
    plt.ylim(0, math.ceil(max(yvalues)))
    plt.xlabel('MPI worker count')
    plt.ylabel('Initialization time (s)')
    plt.plot(xvalues, yvalues)

def main():
    parser = graphhelper.getArgParser()

    args = parser.parse_args();

    timings = getTimings(graphhelper.getArchiveIterator(args.files))

    plot(timings)

    if args.output:
        plt.savefig(args.output.name)
    else:
        plt.show()

if __name__ == "__main__":
	main()
