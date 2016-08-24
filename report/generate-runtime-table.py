#!/usr/bin/env python3

import argparse
import collections
import graphhelper
import os
import re
import statistics
import sys
import tarfile

def getArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument('files', type=argparse.FileType('rb'), help="The data tarbals to use.", nargs="+")
    parser.add_argument('--output', '-o', type=argparse.FileType('w'), help="The file to write the output to")
    parser.add_argument('--cores', '-c', type=int, required=True, nargs="+", help="The cores to include in the table.")

    return parser.parse_args()

def getResults(tarbal):
    results = collections.defaultdict(list)

    for resultFile in tarbal:
        with tarbal.extractfile(resultFile) as resultData:
            workers, _, algoTime, _ = graphhelper.getData(resultData)
            results[workers].append(algoTime)

    return results

def parseTarbal(fileHandle):
    name, implementation = re.findall(r"(\w+)-impl(\d)+", os.path.basename(fileHandle.name))[0]

    with tarfile.open(fileobj=fileHandle) as tarbal:
        results = getResults(tarbal)

    mean = {k: statistics.mean(v) for k, v in results.items()}
    stdev = {k: statistics.stdev(v) for k, v in results.items()}

    return name, implementation, mean, stdev

def getDataRows(fileList):
    results = collections.defaultdict(dict)
    for fileHandle in fileList:
        name, implementation, mean, stdev = parseTarbal(fileHandle)
        results[name][implementation] = {cores: (mean[cores], stdev[cores]) for cores in mean.keys()}

    return results

def filterRows(indices, rows):
    return {name: {implementation: {core: data for core, data in cores.items() if core in indices} for implementation, cores in implementations.items()} for name, implementations in rows.items()}

def printHeader(output, implementations):
    columns = 'l|r|' + ''.join(['|r'] * 2 * len(implementations))
    output.write('\\begin{tabular}{' + columns + '} \n')
    output.write('\\multicolumn{2}{c||}{}')

    # Write implementation names
    for index, implementation in enumerate(implementations):
        if index < len(implementations) - 1:
            output.write(' & \\multicolumn{2}{c|}{Implementation '+ implementation + '}')
        else:
            output.write(' & \\multicolumn{2}{c}{Implementation' + implementation + '}')

    output.write(' \\\\ \n')

    # Write mu/sigma row
    output.write('Graph & \# workers')
    output.write(''.join([' & $\mu$ & $\sigma$'] * len(implementations)))
    output.write(' \\\\ \n')

    output.write('\\hline \n')
    output.write('\\hline \n')

def printDataSection(output, name, section, cores, implementations):
    output.write('\\multirow{' + str(len(cores)) + '}{*}{' + name + '}')
    for core in sorted(cores):
        output.write(' & ' + str(core))
        for implementation in implementations:
            if implementation not in section or core not in section[implementation]:
                output.write(' & & ')
            else:
                mean, stdev = section[implementation][core]
                output.write(' & {0:.3f} & {1:.3f}'.format(mean, stdev))
        output.write('\\\\ \n')

    output.write('\\hline\n')

def getImplementations(rows):
    implementations = set()
    for row in rows.values():
        implementations |= row.keys()

    return sorted(implementations)

def main():
    args = getArgs()

    rows = getDataRows(args.files)
    cores = sorted(args.cores)

    print(cores)

    filtered = filterRows(cores, rows)

    implementations = getImplementations(filtered)

    if args.output is not None:
        output = args.output
    else:
        output = sys.stdout

    printHeader(output, implementations)

    for name in sorted(filtered.keys()):
        printDataSection(output, name, filtered[name], cores, implementations)

    # finally, write the footer
    output.write('\\end{tabular}\n')

if __name__ == "__main__":
    main()
