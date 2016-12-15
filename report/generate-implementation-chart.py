#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy
import graphhelper
import itertools

def getTimings(fileList):
    timings = {}

    for current in fileList:
        ncores, initTime, algoTime, implementation = graphhelper.getData(current)
        if implementation not in timings:
            timings[implementation] = {}

        if ncores not in timings[implementation]:
            timings[implementation][ncores] = []

        timings[implementation][ncores].append(algoTime)

    return timings

def plot(timings):
    colors = itertools.cycle(['k', 'w', 'r', 'c', 'm', 'y', 'k'])

    maxCores = max(timings[1].keys())
    numImplementations = len(timings)
    i = 0
    index = numpy.arange(1, maxCores + 1)

    barWidth = 0.8 / numImplementations

    for implementation in timings:
        relevant = timings[implementation]
        averages = {nCores: numpy.mean(relevant[nCores]) for nCores in relevant}
        barHeights = [ averages[cores - 1] if cores - 1 in averages else 0 for cores in range(maxCores)]

        plt.bar(index + i * barWidth - 0.4, barHeights, barWidth,
                label="Implementation " + str(implementation),
                color=next(colors))
        i += 1

    plt.xlim(1 - 1, maxCores + 1)
    plt.xlabel('MPI worker count')
    plt.ylabel('Runtime (s)')
    plt.legend()

def main():
    parser = graphhelper.getArgParser()

    args = parser.parse_args()

    timings = getTimings(graphhelper.getArchiveIterator(args.files))
    plot(timings)

    if args.output:
        plt.savefig(args.output.name)
    else:
        plt.show()

if __name__ == "__main__":
	main()
