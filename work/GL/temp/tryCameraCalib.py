#!/usr/bin/python

import cv2
import numpy

def visKpts(fname, pts2d):
    fname = "../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/" + fname + ".jpg"
    img = cv2.imread(fname)
    kpts = [cv2.KeyPoint(
        img.shape[1]/2 + pt[0], 
        img.shape[0]/2 + pt[1], 
        1) for pt in pts2d]
    out = cv2.drawKeypoints(img, kpts)
    cv2.imshow("kpts", out)
    cv2.waitKey(0);


def cameraCalib(fname):
    f = open('ptcorrs/' + fname + '.ptcorr')
    pts3d = []
    pts2d = []

    for line in f.readlines():
        line = line.split()
        p3 = [float(line[0]), float(line[1]), float(line[2])]
        p2 = [float(line[3]), float(line[4])]
        pts3d.append(p3)
        pts2d.append(p2)


    obj_pts = [numpy.array(pts3d).reshape(-1,3).astype('float32')]
    img_pts = [numpy.array(pts2d).reshape(-1,2).astype('float32')]
    initCamera = cv2.initCameraMatrix2D(obj_pts, img_pts, (1024, 768))
    ret, cameraMat, distCoeff, rvecs, tvecs = cv2.calibrateCamera(
            obj_pts, 
            img_pts, 
            (1024, 768), 
            initCamera,
            numpy.zeros(4, 'float32'),
            flags = cv2.CALIB_USE_INTRINSIC_GUESS)
    return [pts3d, pts2d, cameraMat, rvecs, tvecs]

fname = "00000001"
pts3d, pts2d, cameraMat, rvecs, tvecs = cameraCalib(fname)
visKpts(fname, pts2d)
print cameraMat
res = cv2.Rodrigues(numpy.array(rvecs).reshape(1,3).astype('float32'))
print "rvec", rvecs
print "center", -res[0].T.dot(tvecs)
print res[0]
