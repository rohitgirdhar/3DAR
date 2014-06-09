#!/usr/bin/python2.7
# Eg usage: python selectGreedy.py -f E_bob.txt -o results/res_40_max.txt -K 40 --max -n E_bob_norm.txt

import numpy as np
import sys
import argparse
import random

inf = 99999999

def selectGreedily(E, K, useMax, test_ids):
    N = np.shape(E)[0]
    selected = np.zeros([1,N])
    err = np.empty([1, N])
    err[:] = inf
    match = np.empty([1,N])
    while K:
        maxdiff = 0
        maxdiff_i = 0
        for i in range(N):
            if i in test_ids:
                continue
            if selected[0][i]:
                continue
            diff = 0
            for j in range(N):
                if j in test_ids:
                    continue
                if (err[0][j] > E[j][i]).all(): # E(j,i) gives the error in approx j by i
                    if useMax:
                        diff = max(diff, err[0][j] - E[j][i])
                    else:
                        diff += err[0][j] - E[j][i]
            if diff > maxdiff:
                maxdiff = diff
                maxdiff_i = i
        selected[0][maxdiff_i] = 1
        for i in range(N):
            if err[0][i] > E[maxdiff_i][i]:
                err[0][i] = E[maxdiff_i][i]
                match[0][i] = maxdiff_i
        K -= 1
    return selected,match,err[0][test_ids]

def selectRandom(E, K, test_ids):
    N = np.shape(E)[0]
    selected = np.zeros([1,N])
    err = np.empty([1, N])
    err[:] = inf
    match = np.empty([1,N])
    while K:
        maxdiff_i = 0
        while True:
            i = random.randint(0, N-1)
            if i in test_ids:
                continue
            if selected[0][i]:
                continue
            maxdiff_i = i
            break
        selected[0][maxdiff_i] = 1
        for i in range(N):
            if err[0][i] > E[maxdiff_i][i]:
                err[0][i] = E[maxdiff_i][i]
                match[0][i] = maxdiff_i
        K -= 1
    return selected,match,err[0][test_ids]

# norms_file: input file, like E[][]
# match : the match vector obtained above
def findNormedError(norms_fpath, match, test_ids):
    norms = np.genfromtxt(norms_fpath, dtype=float, delimiter=' ')
    res = []
    for i in test_ids:
        res.append(norms[i][match[0][i]])
    return (sum(res) / len(test_ids)),res

def main():

    parser = argparse.ArgumentParser(description='Greedy aprox to top-K optimization')
    parser.add_argument('-f', '--fname', nargs=1, required=True, help='Input Error Filename')
    parser.add_argument('-o', '--output-fname', nargs=1, required=True, help='Output Filename')
    parser.add_argument('-n', nargs=1, required=False, 
            help="""
            norms file path. Array file, similar to -f, only with norms of similarity 
            i.e., E[i][j] = #of 3D points that match in i & j (within a given radius) / total # of points
            """)
    parser.add_argument('-t', nargs=1, required=True, type=str, help='File with Test image IDs')
    parser.add_argument('-K', nargs=1, type=int, required=True, help='Size of K set')
    parser.add_argument('--max', action='store_const', const=True, help='Use the max instead of sum')
    parser.add_argument('--select-random', 
            action='store_const', 
            const=True, 
            help='Randomly select elements instead of by greedy')
    parser.add_argument('-v', 
            '--verbose', 
            action='store_const',
            const=True,
            help='Print individual errors, similarities')

    args = parser.parse_args()
    
    error_file = args.fname[0]
    norms_file = args.n[0] if args.n else None
    K = args.K[0]
    output_fname = args.output_fname[0]

    test_ids_file = open(args.t[0])
    test_ids = map(int, test_ids_file.readlines())

    E = np.genfromtxt(error_file, dtype=float, delimiter=' ')
    if args.select_random:
        print 'selecting randomly'
        sel,match,ind_errs = selectRandom(E, K, test_ids)
    else:
        sel,match,ind_errs = selectGreedily(E, K, args.max, test_ids)
    tot_err = sum(ind_errs)

    f = open(output_fname, 'w')
    for i in range(np.shape(E)[0]):
        if sel[0][i]:
            print >> f, 's'
        else:
            print >> f, 'h',
            print >> f, int(match[0][i]) + 1 # to make it 1 indexed
    f.close()
#    print 'Avg Reconst Error', 
    print tot_err / len(ind_errs), # Print avg reconst error

    if norms_file:
#        print 'Similarity Norm', 
        sim,individual_sims = findNormedError(norms_file, match, test_ids)
        print sim
        if args.verbose:
            print 'Individual Sim Values'
            print '\n'.join(map(str, individual_sims))
    if args.verbose:
        print 'Individual Reproj error values'
        print '\n'.join(map(str,ind_errs))
if __name__ == '__main__':
    main()
