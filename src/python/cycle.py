#!/usr/bin/python2
# encoding = utf-8
import argparse
from scipy.io import mminfo, mmread
import scipy.sparse as sparse
import sys
import random

permanentMarks = None
temporaryMarks = None

def visit(matrix, n):
    if n in temporaryMarks:
        return True

    if n not in permanentMarks:
        temporaryMarks.add(n)
        row = matrix.getrow(n).nonzero()[1]
        for i in row:
            if i != n:
                if visit(matrix, i):
                    return True

        permanentMarks.add(n)
        temporaryMarks.remove(n)

    return False

# Check whether a matrix represents a cyclic graph.
#
# This is done by attempting a topological sort. If it fails, there are cycles.
def check_cycle(matrix):
    global permanentMarks, temporaryMarks
    matrix = sparse.csr_matrix(matrix)
    matrix.sum_duplicates()

    temporaryMarks = set()
    permanentMarks = set()

    result = visit(matrix, 0)

    temporaryMarks = None
    permanentMarks = None

    return result

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', type=argparse.FileType('r'), help="The data file to check.")

    opts = parser.parse_args()
    graph = mmread(opts.file)
    opts.file.close()

    if check_cycle(graph):
        print "This graph contains cycles."
    else:
        print "This graph contains no cycles."

if __name__ == "__main__":
    import timing
    main()
