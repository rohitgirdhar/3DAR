#!/usr/bin/python

import cv2
import numpy

fname = "00000011"

def hashNMatch(sfm_kpts, act_kpts, img_shape):
    bin_size = 2 # 4x4 blocks
    mp = numpy.zeros([img_shape[1]/bin_size + 10, img_shape[0]/bin_size + 10])
    for kpt in sfm_kpts:
        mp[kpt.pt[0]/bin_size][kpt.pt[1]/bin_size] = 1
    res = []
    i = 0
    for kpt in act_kpts:
        if mp[kpt.pt[0]/bin_size][kpt.pt[1]/bin_size] == 1:
            res.append(i)
        i += 1
    print "Got", len(res), "overlapping keypoints"
    return res


def visKpts(fname, pts2d):
    fname = "../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/" + fname + ".jpg"
    img = cv2.imread(fname)
    img_bw = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    detector = cv2.SIFT(2000)
#    detector = cv2.FastFeatureDetector(20)
    act_kpts = detector.detect(img_bw)
    kpts = [cv2.KeyPoint(
        img.shape[1]/2 + pt[0], 
        img.shape[0]/2 + pt[1], 
        1) for pt in pts2d]
    out = cv2.drawKeypoints(img, kpts, color=(0,0,0))
    act_kpts_indices = hashNMatch(kpts, act_kpts, img.shape)
    act_kpts = [act_kpts[i] for i in act_kpts_indices]
    out = cv2.drawKeypoints(out, act_kpts, color=(0,0,255))
    cv2.imshow("SFM kpts", out)
    cv2.waitKey(0);
    return act_kpts_indices


def cameraCalib(pts2d, pts3d):
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
    return [cameraMat, rvecs, tvecs]


f = open('ptcorrs/' + fname + '.ptcorr')
pts3d = []
pts2d = []
for line in f.readlines():
    line = line.split()
    p3 = [float(line[0]), float(line[1]), float(line[2])]
    p2 = [float(line[3]), float(line[4])]
    pts3d.append(p3)
    pts2d.append(p2)

idxs = visKpts(fname, pts2d)
pts2d = [pts2d[i] for i in idxs]
pts3d = [pts3d[i] for i in idxs]
cameraMat, rvecs, tvecs = cameraCalib(pts2d, pts3d)
print cameraMat
res = cv2.Rodrigues(numpy.array(rvecs).reshape(1,3).astype('float32'))
print "rvec", rvecs
print "center", -res[0].T.dot(tvecs)
print res[0]
