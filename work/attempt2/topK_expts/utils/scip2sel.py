#!/usr/bin/python2.7

import sys
import re

def help():
    print 'Use: ./prog <scip res file> <total_num_imgs>'

if len(sys.argv) < 3:
    help()
    sys.exit(-1)

inp = sys.argv[1]
N = int(sys.argv[2])

a = {}
b = {}
c = {}
def scipParser(inp):
    f = open(inp)
    for line in f.readlines():
        elts = line.split()
        temp = re.findall(r'a_(\d+)', elts[0])
        if temp:
            a[temp[0]] = 1
        temp = re.findall(r'b_(\d+)', elts[0])
        if temp:
            b[temp[0]] = [1,]
        temp = re.findall(r'c_(\d+)', elts[0])
        if temp:
            c[temp[0]] = [1,]
        temp = re.findall(r'Z1_(\d+)_(\d+)', elts[0])
        if temp:
            if temp[0][0] in c:
                c[temp[0][0]].append(temp[0][1])
        temp = re.findall(r'Z2_(\d+)_(\d+)', elts[0])
        if temp:
            if temp[0][0] in b: 
                b[temp[0][0]].append(temp[0][1])
    f.close()

def display():
    for i in range(1, N+1):
        if str(i) in a:
            print 's'
        elif str(i) in b:
            print 'r', b[str(i)][1]
        elif str(i) in c:
            print 'h', c[str(i)][1]
        else:
            print ''

scipParser(inp)
display()
