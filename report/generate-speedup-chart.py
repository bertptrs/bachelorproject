#!/usr/bin/env python3

import matplotlib.pyplot as plt
import graphhelper
import tarfile
import os.path as path

def plot(timings, label):
    coreList = sorted(timings.keys())

    avgs = [sum(timings[coreNo]) / len(timings[coreNo]) for coreNo in coreList]

    speedups = [avgs[0] / avg for avg in avgs]

    plt.semilogx(coreList, speedups, label=label, basex=2)
    plt.xlabel("Number of MPI workers")
    plt.ylabel("Relative speedup")
    plt.legend(loc = "best", fancybox = True)
    plt.grid()

def main():
    parser = graphhelper.getArgParser()
    args = parser.parse_args()


    for current in args.files:
        timings = {}
        name = path.basename(current.name)
        label = name[:name.find(".")]

        archive = tarfile.open(fileobj=current);

        for tarinfo in archive:
            datafile = archive.extractfile(tarinfo)

            ncores, initTime, algoTime, implementation = graphhelper.getData(datafile)
            if ncores not in timings:
                timings[ncores] = []

            timings[ncores].append(algoTime)

        plot(timings, label)


    if args.output:
        plt.savefig(args.output.name)
    else:
        plt.show()

if __name__ == "__main__":
    main()
