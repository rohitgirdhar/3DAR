#!/bin/bash

if [ $# -lt 4 ]
then
    echo 'Use: ' $0 ' IMGS_DIR_PATH imgID_toAug imgID2_closestMatch ptcorr_dir_path; eg ./prog ../Images_bob 00000000 00000001 ../../BobReconst/ptcorrs/'
    exit
fi

../utils/matlab_batcher.bash vl_runAll \'${1}/${2}.jpg\',\'${1}/${3}.jpg\'
python tryCameraCalib.py ${3} ${4} 
