#!/usr/bin/python

# split the nvm file point correspondence for  each image, the point correspondence with the point cloud

# .ptcorr format:
# <3 components of 3D point> <2 components of 2D point>

import os
import shutil 

class NVMParser:
    def storePtCorr(self, inp_fname, outp_dname):
        """
        @param self
        @param inp_fname : input NVM filename
        @param outp_dname : output directory name
        """
        f = open(inp_fname)
        f.next(); f.next() # ignore header
        num_cam = int(f.next())
        
        # ignore next num_cam lines
        for temp in range(num_cam):
            f.next()
        f.next() # blank

        shutil.rmtree(outp_dname)
        os.mkdir(outp_dname)

        num_pts = int(f.next())
        for pt in range(num_pts):
            line = f.next().split()
            pt3d = [line[0], line[1], line[2]]
            num_imgs = int(line[6])
            for pos in range(7, len(line), 4):
                img = line[pos]
                pt2d = [line[pos + 2], line[pos + 3]]
                f2 = open(os.path.join(
                    outp_dname, img.zfill(8) + '.ptcorr'), "a")
                print >> f2, " ".join(pt3d), " ".join(pt2d)
                f2.close()

ob = NVMParser()
ob.storePtCorr('dense.nvm', 'ptcorrs')

