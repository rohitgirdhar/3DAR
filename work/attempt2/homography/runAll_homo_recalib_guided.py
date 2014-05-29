#!/usr/bin/python2.7 

N = 96
#imgs_root = '~/work/project/07_3DAR/work/attempt2/Images_sc/'
imgs_root = '~/work/project/07_3DAR/work/attempt2/Images_bob/'
#snaps_root = '~/work/project/07_3DAR/work/attempt2/homography/render/snapshots_sc/'
snaps_root = '~/work/project/07_3DAR/work/attempt2/homography/render/snapshots_bob/'
ptcorr_dir_path = '../../BobReconst/ptcorrs/'
model_3d = '../../BobReconst/3DModels/cap_aligned.obj'
resultsdir = '~/work/project/07_3DAR/work/attempt2/homography/results/'
#guide_file = '../topK_expts/src/opt/results/res_bob_96_60_1000.sel'
guide_file = '../topK_expts/src/greedy/results/res_50.txt'
query_imgs_file = '../topK_expts/src/greedy/test_set' # optional

## GUIDE FILE FORMAT
# [s/r/h] <id>
# id is only required with r/h
# id must be *1* indexed!

import sys
import os
import subprocess
import time

f = open(guide_file)
lines = f.readlines()

try:
    f = open(query_imgs_file)
    qimgs = map(int, f.readlines())
except:
    qimgs = range(N)

for i in qimgs:

    match = 0
    line = lines[i].split()
    method = line[0];
    if method == 'h' or method == 'r':
        match = int(line[1]) - 1
    elif method == 's':
        match = i

    print 'Reconstructing ', i, 'by', method, 'from', match
    
    img = str(i).zfill(8)
    match = str(match).zfill(8)

#    os.chdir('../retrieval/StoneChariotAR/')
#    print '>>>>>>>>running for', img, '-------'
#    start = time.time()
#    match = subprocess.check_output('./getClosest Images/ ' + img, shell=True)
#    end = time.time()
#    print '>>>>>>found closest match:', match, '------- in', end-start, ' secs'
#    os.chdir('../../homography/')

    if method == 'h' or method == 's':
        os.chdir('txHomo/')
        start = time.time()
        subprocess.call('./matcher ' + imgs_root + match + '.jpg ' + 
                imgs_root + img + '.jpg ' + ' m.txt', shell=True)
        subprocess.call('./augmenter m.txt ' +
                snaps_root + match + '.jpg 2 ', shell=True)
        end = time.time()
        print '>>>>>>>>Transformed snap in', end-start, ' secs'
        os.chdir('..')

        start = time.time()
        subprocess.call('python merge.py ' + imgs_root + img + '.jpg txHomo/snapmod.jpg 2', shell=True)
        subprocess.call('cp result.jpg ' + resultsdir + img + '.jpg', shell=True)
        end = time.time()
        print '>>>>>>>Merged and saved in', end-start, ' secs'
    
    elif method == 'r':
        os.chdir('../recalib2/')
        start = time.time()
        
        subprocess.call('bash run.sh ' + imgs_root + ' ' + img + ' ' + match + ' ' +
                ptcorr_dir_path + ' ' + model_3d, shell=True)
        subprocess.call('cp result.jpg ' + resultsdir + img + '.jpg', shell=True)

        end = time.time()
        print '>>>>>>>>>Recalibrated in: ', end-start, ' secs'
        os.chdir('../homography/')

