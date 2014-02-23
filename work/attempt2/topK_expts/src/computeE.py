#!/usr/bin/python2.7

import cv2
import numpy as np
import pickle

IMAGES_DIR = 'Images_sc/'
NVM_FILE = 'dense_sc.nvm'
N = 285
START_LINE_NO = 290
WIDTH = 1024.0
HEIGHT = 768.0
OUTPUT_FILE = 'E.txt'

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
    E = np.zeros([N,N])
    for i in range(N):
        for j in range(i+1, N):
            E[i][j] = computeError(i,j)
            E[j][i] = E[i][j]
            print 'setting', i, j, '->', E[i][j]
    return E

def filter_matches(kp1, kp2, matches, ratio = 0.75):
    mkp1, mkp2 = [], []
    for m in matches:
        if len(m) == 2 and m[0].distance < m[1].distance * ratio:
            m = m[0]
            mkp1.append( kp1[m.queryIdx] )
            mkp2.append( kp2[m.trainIdx] )
    p1 = np.float32([kp.pt for kp in mkp1])
    p2 = np.float32([kp.pt for kp in mkp2])
    kp_pairs = zip(mkp1, mkp2)
    return p1, p2, kp_pairs

flann_params = dict(algorithm = 1, trees = 5)
matcher = cv2.FlannBasedMatcher(flann_params, {})  # bug : need to pass empty dict (#1329)
def computeHomography(i, j):
    I = cv2.imread(IMAGES_DIR + str(i).zfill(8) + '.jpg', 
            cv2.IMREAD_GRAYSCALE)
    P = cv2.imread(IMAGES_DIR + str(j).zfill(8) + '.jpg', 
            cv2.IMREAD_GRAYSCALE)
    s = cv2.SIFT()
    [f1,d1] = s.detectAndCompute(I, None)
    [f2,d2] = s.detectAndCompute(P, None)

    raw_matches = matcher.knnMatch(d1, d2, k = 2) #2
    p1, p2, kp_pairs = filter_matches(f1, f2, raw_matches)
    if len(p1) >= 4:
        H, status = cv2.findHomography(p1, p2, cv2.RANSAC, 5.0)
    else:
        H, status = None, None
    return H

def intersect(a, b):
    return list(set(a) & set(b))

def computeError(i, j):
    ## reconstructing i with j
    H = computeHomography(j, i) # j -> i
    if H == None:
        return float('inf')
    commonPts = intersect(kpts[i].keys(), kpts[j].keys())
    if len(commonPts) == 0:
        return float('inf')
    all_pts = []
    for pt in commonPts:
        all_pts.append(kpts[j][pt])
    all_pts = cv2.perspectiveTransform(
            np.array(all_pts, dtype='float32').reshape(1, -1, 2),
            H)
    d = 0.0
    idx = 0
    for pt in all_pts:
        d += cv2.norm(pt - kpts[i][commonPts[idx]])
        idx += 1
    return d / len(commonPts)

def writeToFile(E, output_fname):
    f = open(output_fname, "w")
    for row in E:
        for elt in i:
            f.write('%f ', elt)
        f.write('\n')
    f.close()

if (1):
    f = open('savekpts', 'rb')
    kpts = pickle.load(f)
    f.close()
else:
    readNVMFile()
    f = open('savekpts', 'wb')
    pickle.dump(kpts, f)
    f.close()
print 'read NVM file'
E = computeE()
print E
f = open('saveE', 'wb')
pickle.dump(E, f)
f.close()
writeToFile(E, OUTPUT_FILE)
#print kpts
