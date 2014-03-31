#!/usr/bin/python 

import sys
import cv2
import numpy as np

def help():
    print 'Usage ./merge.py <source image path> <augmented image> <fovy scale ratio>'

def merge(overlay, orig):
    rows,cols,channels = overlay.shape
    roi = orig[0:rows, 0:cols]

    tmp = cv2.cvtColor(overlay, cv2.COLOR_BGR2GRAY);
    retval, mask = cv2.threshold(tmp,10,255, cv2.THRESH_BINARY);
    mask_inv = cv2.bitwise_not(mask)

    orig_bg = cv2.bitwise_and(roi,roi,mask = mask_inv)
    overlay_fg = cv2.bitwise_and(overlay,overlay,mask = mask)

    dst = cv2.add(orig_bg,overlay_fg)
    orig[0:rows, 0:cols ] = dst

    return orig

if len(sys.argv) < 4:
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
#cv2.imshow("test", res)
#cv2.waitKey(0)
