#!/usr/bin/python2.7

import sys
import numpy as np

if len(sys.argv) < 4:
    print 'usage: ./a.out <data file> <select elts file, one num per row> <output_fname>'
    sys.exit(0)

mat = np.loadtxt(sys.argv[1], 'string')
sel = np.loadtxt(sys.argv[2], 'int')
mat = mat[:, sel]
mat = mat[sel, :]
np.savetxt(sys.argv[3], mat, "%s")

