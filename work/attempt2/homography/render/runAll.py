#!/usr/bin/python 

import sys
import os

def help():
    print './runAll.py <cameras_v2.txt file path>'

if len(sys.argv) < 2:
    help()
    sys.exit(-1)

f = open(sys.argv[1])
lines = f.readlines()

for i in range(1, 1000):
    l1 = 2 * (7 * i + 4)
    if l1 > len(lines):
        break
    idx = lines[l1-1][:8]
    focal = lines[l1+1]
    center = lines[l1+4]
    rot1 = lines[l1+7]
    rot2 = lines[l1+8]
    rot3 = lines[l1+9]
    
    ftemp = open('tempcam.txt', 'w')
    print >> ftemp, focal,
    print >> ftemp, center,
    print >> ftemp, rot1,
    print >> ftemp, rot2,
    print >> ftemp, rot3,
    ftemp.close()

    os.system('make')
    cmd = './a.out ' + idx + ' tempcam.txt 2 snapshots/' + idx + ".jpg"
    print 'Calling:', cmd
    os.system(cmd)

f.close()
