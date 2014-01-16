#!/usr/bin/python 

import sys
import cv2
import numpy as np

def help():
    print 'Usage ./merge.py <source image path> <augmented image> <fovy scale ratio>'

def merge(overlay, orig):
    res = []
    for i in range(orig.shape[0]):
        for j in range(orig.shape[1]):
            if np.linalg.norm(overlay[i][j]) > 10:
                orig[i][j] = overlay[i][j]
    return orig
    
if len(sys.argv) < 3:
    help()
    sys.exit(0)

I = cv2.imread(sys.argv[1])
O = cv2.imread(sys.argv[2])
s = int(sys.argv[3])

cols = O.shape[1]
rows = O.shape[0]

crop_O = O[rows / 2 - rows / (2 * s) : rows / 2 + rows / (2 * s),
        cols / 2 - cols / (2 * s) : cols / 2 + cols / (2 * s)]
resized_I = cv2.resize(I, (crop_O.shape[1], crop_O.shape[0]))

res = merge(crop_O, resized_I)
cv2.imwrite("result.jpg", res)
cv2.imshow("test", res)
cv2.waitKey(0)
