#!/usr/bin/python

import argparse
import re
import matplotlib.pyplot as plt


def readValues(currentFile):
	contents = currentFile.read()
	ncores = int(re.findall("Running maxflow on (\d+) MPI", contents)[0])
	algoTime = float(re.findall("Algorithm finished after (\d+\.?\d*)s", contents)[0])
	initTime = float(re.findall("Initialization finshed after (\d+\.?\d*)s", contents)[0])

	currentFile.close()

	return (ncores, initTime, algoTime)

def plot(timings):
	avgs = []
	coreList = timings.keys()
	coreList.sort()
	for coreNo in coreList:
		avgs.append(sum(timings[coreNo]) / len(timings[coreNo]))

	speedups = []
	for i in avgs:
		speedups.append(avgs[0] / i)

	plt.plot(coreList, speedups, label="Measured speedup")
	plt.plot(coreList, coreList, label="Ideal speedup") # Plot the reference line
	plt.xlabel("Number of MPI workers")
	plt.ylabel("Relative speedup")
	plt.legend(loc = "best", fancybox = True)
	plt.grid()

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('files', type=argparse.FileType('r'), help="The data file to use.", nargs="+")
	parser.add_argument('--output', '-o', type=argparse.FileType('w'), help="Optional: A data file to store the graph in.")
	args = parser.parse_args()

	timings = dict()

	for current in args.files:
		ncores, initTime, algoTime = readValues(current)
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
