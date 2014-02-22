#!/usr/bin/python2.7 

N = 285
imgs_root = '~/work/project/07_3DAR/work/attempt2/Images_sc/'
snaps_root = '~/work/project/07_3DAR/work/attempt2/homography/render/snapshots_sc/'
resultsdir = 'results/'

import sys
import os
import subprocess
import time

for i in range(N):
    img = str(i).zfill(8)
    os.chdir('../retrieval/StoneChariotAR/')
    print '>>>>>>>>running for', img, '-------'
    start = time.time()
    match = subprocess.check_output('./getClosest Images/ ' + img, shell=True)
    end = time.time()
    print '>>>>>>found closest match:', match, '------- in', end-start, ' secs'
    os.chdir('../../homography/')

    os.chdir('../matching_experiments/exp1_analogMatch/src')
    start = time.time()
    cmd = "bash run_matlab.sh runMatching  \\'" + match + "\\',\\'" + img + "\\'"
#    print cmd
    subprocess.call(cmd, shell=True)
    subprocess.call( " cp m.txt ../../../homography/txHomo/ ", shell=True)
    os.chdir('../../../homography')
    os.chdir('txHomo')
    subprocess.call('./augmenter m.txt ' +
            snaps_root + match + '.jpg 2 ', shell=True)
    end = time.time()
    os.chdir('..')
    print '>>>>>>>>Transformed snap in', end-start, ' secs'

    start = time.time()
    subprocess.call('python merge.py ' + imgs_root + img + '.jpg txHomo/snapmod.jpg 2', shell=True)
    subprocess.call('cp result.jpg ' + resultsdir + img + '.jpg', shell=True)
    end = time.time()
    print '>>>>>>>Merged and saved in', end-start, ' secs'

