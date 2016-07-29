import argparse
import re
import tarfile

def getArgParser():
    parser = argparse.ArgumentParser()
    parser.add_argument('files', type=argparse.FileType('rb'), help="The data file to use.", nargs="+")
    parser.add_argument('--output', '-o', type=argparse.FileType('wb'), help="Optional: A data file to store the graph in.")

    return parser

def getData(currentFile):
    contents = currentFile.read().decode('utf-8')
    ncores = int(re.findall("Running maxflow on (\d+) MPI", contents)[0])
    algoTime = float(re.findall("Algorithm finished after (\d+\.?\d*)s", contents)[0])
    initTime = float(re.findall("Initialization finshed after (\d+\.?\d*)s", contents)[0])

    implementations = re.findall("on implementation (\d+)\.", contents)

    if len(implementations) > 0:
        implementation = int(implementations[0])
    else:
        implementation = 1

    currentFile.close()

    return (ncores, initTime, algoTime, implementation)

def getArchiveIterator(fileList):
    for current in fileList:
        archive = tarfile.open(fileobj=current)

        for tarinfo in archive:
            yield archive.extractfile(tarinfo)

        archive.close()

        current.close()
