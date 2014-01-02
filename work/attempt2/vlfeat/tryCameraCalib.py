#!/usr/bin/python

import cv2
import numpy

fname = "00000016" # name of the closest matched image
bin_size = 1 # 1x1 blocks
img_shape = (1024, 768)

def cameraCalib(pts2d, pts3d):
    obj_pts = [numpy.array(pts3d).reshape(-1,3).astype('float32')]
    img_pts = [numpy.array(pts2d).reshape(-1,2).astype('float32')]
    initCamera = cv2.initCameraMatrix2D(obj_pts, img_pts, img_shape)
    ret, cameraMat, distCoeff, rvecs, tvecs = cv2.calibrateCamera(
            obj_pts, 
            img_pts, 
            img_shape, 
            initCamera,
            numpy.zeros(4, 'float32'),
            flags = cv2.CALIB_USE_INTRINSIC_GUESS)
    return [cameraMat, rvecs, tvecs]

f = open('../../GL/temp/ptcorrs/' + fname + '.ptcorr')
pts3d = []
pts2d = []
for line in f.readlines():
    line = line.split()
    p3 = [float(line[0]), float(line[1]), float(line[2])]
    p2 = [float(line[3]), float(line[4])]
    pts3d.append(p3)
    pts2d.append(p2)

f.close()
f = open('matches.txt')
matches = []
for line in f.readlines():
    line = line.split()
    # I is the query, P is the closest match
    m = [float(line[0]), float(line[1]), float(line[2]), float(line[3])]
    matches.append(m)
f.close()

pts2d = [ [pt[0] + img_shape[0]/2, pt[1] + img_shape[1]/2] for pt in pts2d ]

mp = numpy.zeros([img_shape[0]/bin_size + 10, img_shape[1]/bin_size + 10])
i = 0
for pt in pts2d:
    mp[int(pt[0]/bin_size)][int(pt[1]/bin_size)] = i
    i += 1

pts2d_final = []
pts3d_final = []
for match in matches:
    idx = mp[int(match[2]/bin_size)][int(match[3]/bin_size)]
    idx = int(idx)
    if idx != 0:
        pts2d_final.append([match[0], match[1]])
        pts3d_final.append(pts3d[idx])

pts2d_final = [ [pt[0] - img_shape[0]/2, pt[1] - img_shape[1]/2] for pt in pts2d_final ]

print 'Finally using', len(pts2d_final), 'correspondences for camera calib'

cameraMat, rvecs, tvecs = cameraCalib(pts2d_final, pts3d_final)
print cameraMat
res = cv2.Rodrigues(numpy.array(rvecs).reshape(1,3).astype('float32'))
print "rvec", rvecs
print "tvecs", tvecs
center = -numpy.matrix(res[0].T) * numpy.matrix(tvecs[0])

print "center", center 
print res[0]
f = open('cam.txt', 'w')
print >> f, cameraMat[0][0]
print >> f, center.item(0), center.item(1), center.item(2)
print >> f, '\n'.join(' '.join(str(cell) for cell in row) for row in res[0])
f.close()
