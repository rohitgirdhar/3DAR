#!/usr/bin/python2.7

import numpy as np
import sys
import argparse

inf = 99999999

def selectGreedily(E, K, useMax):
    N = np.shape(E)[0]
    selected = np.zeros([1,N])
    err = np.empty([1, N])
    err[:] = inf
    match = np.empty([1,N])
    while K:
        maxdiff = 0
        maxdiff_i = 0
        for i in range(N):
            if selected[0][i]:
                continue
            diff = 0
            for j in range(N):
                if (err[0][j] > E[i][j]).all():
                    if useMax:
                        diff = max(diff, err[0][j] - E[i][j])
                    else:
                        diff += err[0][j] - E[i][j]
            if diff > maxdiff:
                maxdiff = diff
                maxdiff_i = i
        selected[0][maxdiff_i] = 1
        for i in range(N):
            if err[0][i] > E[maxdiff_i][i]:
                err[0][i] = E[maxdiff_i][i]
                match[0][i] = maxdiff_i
        K -= 1
    return selected,match,sum(err[0][:])

def main():

    parser = argparse.ArgumentParser(description='Greedy aprox to top-K optimization')
    parser.add_argument('-f', '--fname', nargs=1, required=True, help='Input Error Filename')
    parser.add_argument('-o', '--output-fname', nargs=1, required=True, help='Output Filename')
    parser.add_argument('-K', nargs=1, type=int, required=True, help='Size of K set')
    parser.add_argument('--max', action='store_const', const=True, help='Use the max instead of sum')

    args = parser.parse_args()
    
    error_file = args.fname[0]
    K = args.K[0]
    output_fname = args.output_fname[0]

    E = np.genfromtxt(error_file, dtype=float, delimiter=' ')
    sel,match,tot_err = selectGreedily(E, K, args.max)
    f = open(output_fname, 'w')
    for i in range(np.shape(E)[0]):
        if sel[0][i]:
            print >> f, 's'
        else:
            print >> f, 'h',
            print >> f, int(match[0][i]) + 1 # to make it 1 indexed
    f.close()
    print 'Total Error', tot_err

if __name__ == '__main__':
    main()
