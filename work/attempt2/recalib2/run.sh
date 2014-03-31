#!/bin/bash

if [ $# -lt 6 ]
then
    echo 'Use: ' $0 ' <IMGS_DIR_PATH> <imgID_toAug> <imgID2_closestMatch> <ptcorr_dir_path> <3D model path> <result_img_path>'
    echo 'eg ./prog ../Images_bob 00000000 00000001 ../../BobReconst/ptcorrs/ ../../BobReconst/3DModels/cap_aligned.obj res.jpg'
    exit
fi

bash runCalib.sh $1 $2 $3 $4
render/render cam.txt 1 snap.jpg $5
composite -blend 60 snap.jpg ${1}/${2}.jpg ${6}
