#!/usr/bin/env python3

import matplotlib.pyplot as plt
import graphhelper

def plot(timings):
    coreList = sorted(timings.keys())

    avgs = [sum(timings[coreNo]) / len(timings[coreNo]) for coreNo in coreList]

    speedups = [avgs[0] / avg for avg in avgs]

    plt.plot(coreList, speedups, label="Measured speedup")
    plt.plot(coreList, coreList, label="Ideal speedup") # Plot the reference line
    plt.xlabel("Number of MPI workers")
    plt.ylabel("Relative speedup")
    plt.legend(loc = "best", fancybox = True)
    plt.grid()

def main():
    parser = graphhelper.getArgParser()
    args = parser.parse_args()

    timings = dict()

    for current in graphhelper.getArchiveIterator(args.files):
        ncores, initTime, algoTime, implementation = graphhelper.getData(current)
        if ncores not in timings:
            timings[ncores] = []

        timings[ncores].append(algoTime)

    plot(timings)

    if args.output:
        plt.savefig(args.output.name)
    else:
        plt.show()

if __name__ == "__main__":
    main()
