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
    Mat H = computeHomography(im_m, im_q);
    
    vector<Point3f> pts3d;
    vector<Point2f> pts2d;
    string path_im_ptcorr = string(ptcorr_root) + source_fname + ".ptcorr";
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

        Point2f pt2d_new;
        pt2d_new.x = H.at<double>(0, 0) * pt2d.x + H.at<double>(0, 1) * pt2d.y + H.at<double>(0, 2);
        pt2d_new.y = H.at<double>(1, 0) * pt2d.x + H.at<double>(1, 1) * pt2d.y + H.at<double>(1, 2);
        float w = H.at<double>(2, 0) * pt2d.x + H.at<double>(2, 1) * pt2d.y + H.at<double>(2, 2);
        pt2d_new.x = pt2d_new.x / w;
        pt2d_new.y = pt2d_new.y / w;

        pts3d.push_back(pt3d);
        pts2d.push_back(pt2d_new);
    }
//    cout << "pts3d: " << pts3d.size() << endl;
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

Mat CameraTransformer::computeHomography(Mat img_object, Mat img_scene) {
    if (!img_object.data || !img_scene.data) {
        cerr << "Unable to read the image files" << endl;
        exit(-1);
    }
    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( img_object, keypoints_object );
    detector.detect( img_scene, keypoints_scene );

//    cout << "kp : " << keypoints_object.size() << endl;
    
    //-- Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;

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
   
    /*
    Mat img_matches;
    drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
            good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
            vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    */
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

    //return Mat::eye(3, 3, CV_64F); // for now
}
