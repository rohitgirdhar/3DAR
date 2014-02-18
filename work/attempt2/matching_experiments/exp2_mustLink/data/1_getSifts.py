#!/usr/bin/python2.7

import cv2

IMAGES_DIR = 'Images_sc/'
NVM_FILE = 'dense_sc.nvm'
START_LINE_NO = 290
WIDTH = 1024.0
HEIGHT = 768.0

# stores the reqd kpts as :
# { img_id : { kpt_id : [x,y] .. } .. }
reqd_kpts = {}
def addToReqdKpts(img_idx, kp_idx, x, y):
    if img_idx not in reqd_kpts:
        reqd_kpts[img_idx] = {}
    if kp_idx not in reqd_kpts[img_idx]:
        reqd_kpts[img_idx][kp_idx] = [x, y]


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
            addToReqdKpts(img_idx, kp_idx, float(x), float(y))
    f.close()
            
def writeSifts(fname):
    f = open(fname, "w")
    for img in reqd_kpts.keys():
        print 'Writing for img', img, '. num sifts=', len(reqd_kpts[img].keys())
        kpts = []
        for kpt in reqd_kpts[img].keys():
            kpts.append(cv2.KeyPoint(reqd_kpts[img][kpt][0],
                reqd_kpts[img][kpt][1], 1))
        I = cv2.imread('Images_sc/' + '%08d' % int(img) + '.jpg', cv2.IMREAD_GRAYSCALE)
        detector = cv2.SIFT()
        [kpts, descs] = detector.compute(I, kpts)
        for i in range(descs.shape[0]):
            f.write(','.join(map(str, descs[i, :].tolist())) + '\n')
    f.close()

readNVMFile()
writeSifts('allsifts.txt')
