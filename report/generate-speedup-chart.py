#!/usr/bin/python

import matplotlib.pyplot as plt
import graphhelper

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
	parser = graphhelper.getArgParser()
	args = parser.parse_args()

	timings = dict()

	for current in args.files:
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
