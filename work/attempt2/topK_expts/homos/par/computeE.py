#!/usr/bin/python2.7

import cv2
import numpy as np
import pickle
import gc
import multiprocessing as mp
import os, sys

IMAGES_DIR = 'Images_dir/Images_bob/'
HOMOGRAPHIES_DIR = 'Homographies/Homographies_bob/'
NVM_FILE = 'NVM_Files/dense_bob.nvm'
N = 96
START_LINE_NO = 101 # line with #points (and then the points)
WIDTH = 1024.0
HEIGHT = 768.0
OUTPUT_FILE = 'E_bob_norm.txt'
method = 'norm'     # can be 'avg' or 'norm'. avg means the error value is the average distance
                    # and 'norm' is #of 3D points within radius error. norm will actually give a similarity matrix (not error)
RADIUS = 20.0 # in this pixel radius; used only if method == norm

Homos = {}
def readHomos():
    for i in range(N):
        Homos[i] = {}
        f = open(HOMOGRAPHIES_DIR + 'homo_' + str(i) + '.txt')
        lines = f.readlines()
        f.close()
        for j in range(N):
            temp = lines[ 3*j : 3*j+3 ]
            H = np.zeros([3, 3], dtype='float32')
            for k in range(3):
                elts = temp[k].split()
                H[k][0] = float(elts[0])
                H[k][1] = float(elts[1])
                H[k][2] = float(elts[2])
            Homos[i][j] = H

# stores the kpts as :
# { img_id : { 3d_pt_id : [x,y] .. } .. }
kpts = {}
def addToKpts(Wpt_idx, img_idx, x, y):
    if img_idx not in kpts.keys():
        kpts[img_idx] = {}
    if Wpt_idx not in kpts[img_idx].keys():
        kpts[img_idx][Wpt_idx] = np.array([x, y], dtype='float32')

def readNVMFile():
    f = open(NVM_FILE)
    lines = f.readlines()[START_LINE_NO-1:]
    n_3dpts = int(lines[0])
    for i in range(n_3dpts):
        pts = lines[i + 1].split()
        n_2dpts = int(pts[6])
        for j in range(n_2dpts):
            params = pts[7 + j * 4 : 7 + j * 4 + 4]
            img_idx = params[0]
            kp_idx = params[1]
            x = float(params[2]) + WIDTH/2
            y = float(params[3]) + HEIGHT/2
            addToKpts(int(i), int(img_idx), float(x), float(y))
    f.close()
 
def computeE():
    l = []
    for i in range(N):
        for j in range(i, N):
            l.append( (i,j) )
    pool = mp.Pool(4)
    pool.map(E_computer, l)

lock = mp.Lock()
def E_computer(tup):
    i = tup[0]
    j = tup[1]
    err = computeError(i,j)
    lock.acquire()
    fback = open('process.txt', 'a')
    fback.write(str(i) + " " + str(j) + " " + str(err) + "\n")
    sys.stdout.write(str(i) + " " + str(j) + " " + str(err) + "\n")
    fback.close()
    lock.release()
    gc.collect()

def computeHomography(i, j):
    # read the file i, and lines 3*j to 3*j+2
    return Homos[i][j];

def intersect(a, b):
    return list(set(a) & set(b))

def computeError(i, j):
    if i == j:
        if method == 'avg':
            return 0.0
        elif method == 'norm':
            return 1.0
        else:
            return float('inf')

    ## reconstructing i with j
    H = computeHomography(j, i) # j -> i
    if H == None:
        return float('inf')
    commonPts = intersect(kpts[i].keys(), kpts[j].keys())
    n_commonPts = len(commonPts)
    if n_commonPts == 0:
        return float('inf')
    all_pts = []
    for pt in commonPts:
        all_pts.append(kpts[j][pt])
    all_pts = cv2.perspectiveTransform(
            np.array([all_pts], dtype='float32'),
            H)
    d = 0.0
    for idx in range(n_commonPts):
        dist = cv2.norm(all_pts[0][idx][:] - kpts[i][commonPts[idx]])
        if method == 'norm':
            if dist < RADIUS:
                d += 1.0
        else:
            d += dist
    return d / n_commonPts

def readToE(fname):
    f = open(fname)
    E = np.zeros([N, N])
    for line in f.readlines():
        elts = line.split()
        E[int(elts[0])][int(elts[1])] = float(elts[2])
        E[int(elts[1])][int(elts[0])] = float(elts[2])
    f.close()
    return E

def writeToFile(E, output_fname):
    f = open(output_fname, "w")
    for row in E:
        for elt in row:
            f.write('%f ' % elt)
        f.write('\n')
    f.close()



readHomos()
print 'Read Homographies'
if (0):
    f = open('../savekpts', 'rb')
    kpts = pickle.load(f)
    f.close()
else:
    readNVMFile()
    f = open('savekpts', 'wb')
    pickle.dump(kpts, f)
    f.close()
print 'read NVM file'
if (0):
    f = open('saveE', 'rb')
    E = pickle.load(f)
    f.close()
else:
    computeE()
#    f = open('saveE', 'wb')
#    pickle.dump(E, f)
#    f.close()
    E = readToE('process.txt')
writeToFile(E, OUTPUT_FILE)
#print kpts
