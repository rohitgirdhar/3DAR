#!/bin/bash

if [ $# -lt 5 ]
then
    echo 'Use: ' $0 ' <IMGS_DIR_PATH> <imgID_toAug> <imgID2_closestMatch> <ptcorr_dir_path> <3D model path>'
    echo 'eg ./prog ../Images_bob 00000000 00000001 ../../BobReconst/ptcorrs/ ../../BobReconst/3DModels/cap_aligned.obj'
    exit
fi

bash runCalib.sh $1 $2 $3 $4
render/render cam.txt 1 snap.jpg $5
python merge.py ${1}/${2}.jpg snap.jpg 1
