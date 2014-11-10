Code to generate Snapshots for a 3D model
=========================================

Steps
-----
1. Click a large number of pictures of the model you wish to augment. Make sure to get all the viewpoints you
will need while augmenting.
2. Use [VisualSFM](http://ccwu.me/vsfm/) to generate a SfM model of the 3D structure using the images clicked above.
This should give you a model with the camera parameters for each of the images (in `cameras_v2.txt` file - output of the SfM procedure).
3. Now you will need:
    - CAD 3D model of the augmetation
    - CAD 3D model of the current state of the model (optional): This is required to handle occlusions due to the model itself.
                                                                 For eg, in Stone Chariot if we augment using just the snapshot of the whole super-structure,
                                                                 it will and hide the top parts of Chariot itself. We want it to be behind the structure.
                                                                 Hence, we need a snapshot that clips out the part that should not be overlayed. For this,
                                                                 you can pass the current state of the 3D model to the code and it will generate 2 snapshots:
                                                                 one of the augmentation and other of the current state, and clip off the augmentation where 
                                                                 there is something in the current structure.
                                                                 [Here](https://www.dropbox.com/sh/w5g5rw6rx2mi7gj/AACDfBnzy8IweuQGEMQbnUDTa?dl=0) is the link to snapshots I used.
                                                                 I'm talking about the difference in the `head_*` and `final_*` snapshot images.
                                                                 Else, just set  it to an empty 3D model, and it will give the whole augmentation itself (I
                                                                 used that for generating the snapshots for Charminar)
4. Compile the above code with `make`
5. Another thing to understand: `FOVY_MULTIPLY_FACTOR`
    I introduced this to increase the field of view of the camera when taking the snapshot. This is
    required specially when the augmentation is towards an edge of the image, and usually gets clipped in all augmentation snapshots
    (since it goes out of view for that snapshot). I used the value 2 for this, but remember this will give snapshots that see twice the
    world than what camera parameters file says, and hence need to take the center crop of this image (after transforming using homography
    at test time) to augment the final image.
6. Set the paths to 3D model obj files in `overlay.cpp`
7. Use runAll.py to generate the snapshots. Change `FOVY_MULTIPLY_FACTOR` if required (hard coded to 2)


