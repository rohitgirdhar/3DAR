#!/usr/bin/python2.7

import numpy as np

### Final values:
## between 0 and 2*scale. 2*scale for INF
## rest scaled between 0 and scale

N = 96
scale = 1
recalibs_dir = 'Recalibs/Recalibs_bob/'
recalib_prefix = 'recalib_'

# Rij = error when ith image is recalib using jth image
R = np.zeros([N,N], dtype='float32')
maxVal = -1
for i in range(N):
    f = open(recalibs_dir + recalib_prefix + str(i) + '.txt')
    lines = f.readlines()
    for j in range(N):
        if len(lines) != N:  # to handle cases where recalib error calculation failed, and didn't return for all imgs
            val = -1
        else:
            val = lines[j]
            if val.strip() == 'NaN' or val.strip() == 'Inf':
                val = -1
        
            try:
                val = float(val)
            except:
                val = -1

        R[i][j] = val
        maxVal = max(val, maxVal)
    f.close()

print 'maxval', maxVal
f = open('R.txt', 'w')
for i in range(N):
    for j in range(N):
        val = R[i][j]
        if val == -1:
            val = maxVal * 2
        f.write(str(val * scale) + ' ')
    f.write('\n')


        

