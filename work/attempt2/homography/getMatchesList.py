#!/usr/bin/python2.7 

N = 285
imgs_root = '~/work/project/07_3DAR/work/attempt2/Images_sc/'
snaps_root = '~/work/project/07_3DAR/work/attempt2/homography/render/snapshots_sc/'
resultsdir = 'results/'

import sys
import os
import subprocess
import time

os.chdir('../retrieval/StoneChariotAR/')
f = open('matches.txt', 'w')

for i in range(N):
    img = str(i).zfill(8)
    start = time.time()
    match = subprocess.check_output('./getClosest Images/ ' + img, shell=True)
    end = time.time()

    print >>f, img, match

f.close()
