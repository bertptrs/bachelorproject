#!/usr/bin/python2

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
    colors = itertools.cycle(['b', 'g', 'r', 'c', 'm', 'y', 'k'])

    maxCores = max(timings[1].keys())
    numImplementations = len(timings)
    i = 0
    index = numpy.arange(1, maxCores + 1)

    barWidth = 0.8 / numImplementations

    for implementation in timings:
        averages = []
        for nCores in timings[implementation]:
            averages.append(sum(timings[implementation][nCores]) / len(timings[implementation][nCores]))

        plt.bar(index + i * barWidth - 0.4, averages, barWidth,
                label="Implementation " + str(implementation),
                color=colors.next())
        i += 1

    plt.xlim(1 - 1, maxCores + 1)
    plt.xlabel('MPI worker count')
    plt.ylabel('Runtime (s)')
    plt.xticks(index, index)
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
