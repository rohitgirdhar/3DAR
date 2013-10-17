/**
 * TODO
 * 1. Change how query img is read
 */

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "CameraTransformer.hpp"

using namespace std;
using namespace glm;
using namespace cv;

void CameraTransformer::recalibrateCamera(
        string source_fname,
        string match_fname,
        vec3& center,
        vec3& look_dir,
        vec3& up_dir) {
    char *cameras_file = "cameras_v2.txt";
    
    char *images_root = "../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/";
    char *ptcorr_root = "ptcorrs/";
    
    string path_im_q = string(images_root) + source_fname + ".jpg";
    string path_im_m = string(images_root) + match_fname + ".jpg";
    Mat im_q = imread(path_im_q.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    Mat im_m = imread(path_im_m.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    
    vector<Point3f> pts3d;
    vector<Point2f> pts2d;
    vector<Point2f> pts2d_old; // only for visualization
    string path_im_ptcorr = string(ptcorr_root) + match_fname + ".ptcorr";
    ifstream f(path_im_ptcorr.c_str());
    if (!f) {
        cerr << "Unable to open file: " << path_im_ptcorr << endl;
        exit(-1);
    }
    string line;
    while (getline(f, line)) {
        istringstream iss(line);
        Point3f pt3d;
        Point2f pt2d;
        iss >> pt3d.x >> pt3d.y >> pt3d.z >> pt2d.x >> pt2d.y;
        pts3d.push_back(pt3d);
        pts2d_old.push_back(pt2d);
    }

    Mat H = computeHomography(im_m, im_q);

    perspectiveTransform(pts2d_old, pts2d, H);
    visualizeMatching(im_q, im_m, pts2d, pts2d_old);
    Mat cameraMat = Mat::eye(3, 3, CV_64F), distCoeff = Mat::zeros(8, 1, CV_64F);
    vector<Mat> rvecs, tvecs;
    vector< vector<Point3f> > objs;
    objs.push_back(pts3d);
    vector< vector<Point2f> > imgs;
    imgs.push_back(pts2d);
    objs.resize(imgs.size(), objs[0]);
    cameraMat = initCameraMatrix2D(objs, imgs, Size(1024, 768));
    calibrateCamera(
            objs,
            imgs, 
            Size(1024, 768),
            cameraMat,
            distCoeff,
            rvecs,
            tvecs,
            CV_CALIB_USE_INTRINSIC_GUESS);
    Mat rotMat;
    Rodrigues(rvecs[0], rotMat);
//    cout << "Rot mat: " << rotMat << endl;
    cout << "Camera Mat : " << cameraMat << endl;
    look_dir[0] = rotMat.at<double>(2,0);
    look_dir[1] = rotMat.at<double>(2,1);
    look_dir[2] = rotMat.at<double>(2,2);
    up_dir[0] = -rotMat.at<double>(1,0);
    up_dir[1] = -rotMat.at<double>(1,1);
    up_dir[2] = -rotMat.at<double>(1,2);

    Mat C = rotMat.t() * tvecs[0];
//    cout << "C : " << C << endl;
    center[0] = -C.at<double>(0);
    center[1] = -C.at<double>(1);
    center[2] = -C.at<double>(2);
}

Mat CameraTransformer::computeHomography(
        Mat img_object, 
        Mat img_scene) {
    if (!img_object.data || !img_scene.data) {
        cerr << "Unable to read the image files" << endl;
        exit(-1);
    }
    int minHessian = 400;

    SiftFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( img_object, keypoints_object );
    detector.detect( img_scene, keypoints_scene );

//    cout << "kp : " << keypoints_object.size() << endl;
    
    //-- Step 2: Calculate descriptors (feature vectors)
    SiftDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute( img_object, keypoints_object, descriptors_object );
    extractor.compute( img_scene, keypoints_scene, descriptors_scene );

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );

//    cout << "matches : " << matches.size() << endl;

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    std::vector< DMatch > good_matches;

    cout << "max/min dist : " << max_dist << " " << min_dist << endl;

    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( matches[i].distance <= 3*min_dist )
        { good_matches.push_back( matches[i]); }
    }
   
    
    Mat img_matches;
    drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
            good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
            vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    
    imwrite("good_matches.jpg", img_matches);
    
    cout << "Good matches : " << good_matches.size() << endl;
    
    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    return findHomography( obj, scene, CV_RANSAC );
}

void CameraTransformer::visualizeMatching(
        Mat new_img, 
        Mat old_img, 
        vector<Point2f> new_pts, 
        vector<Point2f> old_pts) {
    vector<KeyPoint> new_kpts, old_kpts;
    vector<DMatch> matches;
    int i;
    float ppx = 512, ppy = 384;
    for (i = 0; i < new_pts.size(); i++) {
        new_kpts.push_back(KeyPoint(new_pts[i].x + ppx, new_pts[i].y + ppy, 1));
        old_kpts.push_back(KeyPoint(old_pts[i].x + ppx, old_pts[i].y + ppy, 1));
//        matches.push_back(DMatch(i, i, 0));
    }
    Mat img_matches;
    drawMatches(old_img, old_kpts, new_img, new_kpts, matches, img_matches);
    imwrite("matches.jpg", img_matches);
}
