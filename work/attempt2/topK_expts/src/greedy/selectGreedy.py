#!/usr/bin/python2.7

import numpy as np
import sys

if len(sys.argv) < 2:
    print 'usage : ./a.out error_fpath K'
    sys.exit(0)

error_file = sys.argv[1]
K = int(sys.argv[2])
inf = 99999999

def selectGreedily(E):
    N = np.shape(E)[0]
    selected = np.zeros([1,N])
    err = np.empty([1, N])
    err[:] = inf
    match = np.empty([1,N])
    global K
    while K:
        maxdiff = 0
        maxdiff_i = 0
        for i in range(N):
            if selected[0][i]:
                continue
            diff = 0
            for j in range(N):
                if (err[0][j] > E[i][j]).all():
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
    return selected,match

def main():
    E = np.genfromtxt(error_file, dtype=float, delimiter=' ')
    sel,match = selectGreedily(E)
    for i in range(np.shape(E)[0]):
        if sel[0][i]:
            print 's'
        else:
            print 'h',
            print int(match[0][i])

if __name__ == '__main__':
    main()
