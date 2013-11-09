
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <opencv2/nonfree/nonfree.hpp>
#include "CameraTransformer.hpp"

using namespace std;
using namespace glm;
using namespace cv;

void tempFun(Mat H) {
    Mat I = imread("snap.jpg");
    Mat T;
    warpPerspective(I, T, H, Size(1024, 768));
    imwrite("snap-mod.jpg", T);
}

void CameraTransformer::recalibrateCamera(
        string source_fname,
        string match_fname,
        vec3& center,
        vec3& look_dir,
        vec3& up_dir) {
    char *cameras_file = "cameras_v2.txt";
    
    char *images_root = "../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/";
    char *ptcorr_root = "ptcorrs/";
    
    string path_im_q = source_fname;
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

    cout << "Got Homography : " << endl;
    cout << H << endl;

    tempFun(H);

    perspectiveTransform(pts2d_old, pts2d, H);

    // correct the matches using F
    bool correctOnF = false;
    if (correctOnF) {
        vector<Point2f> pts2d_old_c, pts2d_c;
        cout << this->F << endl;
        correctMatches(this->F, pts2d_old, pts2d, pts2d_old_c, pts2d_c); 
        pts2d_old = pts2d_old_c;
        pts2d = pts2d_c;
    }

    bool refineOnFExp = false;
    if (refineOnFExp) {
        refineOnF(pts3d, pts2d, pts2d_old);
        cout << "refined to " << pts3d.size() << " " << pts2d.size() << endl;
    }

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
    this->intCameraMat = cameraMat;
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
    for( int i = 0; i < descriptors_object.rows; i++ ) { 
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    std::vector< DMatch > good_matches;

    cout << "max/min dist : " << max_dist << " " << min_dist << endl;

    for( int i = 0; i < descriptors_object.rows; i++ ) { 
        if( matches[i].distance <= 3*min_dist )
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

    this->H = findHomography( obj, scene, CV_RANSAC );
//    vector<char> status(obj.size(), 0);
    vector<uchar> status;
    this->F = findFundamentalMat(obj, scene, FM_RANSAC, 3, 0.99, status);
//    cout << status.type() << endl;
    for (int i = 0; i < obj.size(); i++) {
        if ((int)status[i]) {
            this->seedPts.push_back(keypoints_object[i]);
            this->qPts.push_back(keypoints_scene[i]);
        }
    }
//    cout << " GOT : " << this->seedPts.size() << " " << obj.size() << endl;

    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );

    vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

        line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );

    //-- Show detected matches
    imwrite( "ObjectDetection.jpg", img_matches );

    return this->H;
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

    for (int pt = 0; pt < old_pts.size(); pt++) {
        old_pts[pt].x += ppx; old_pts[pt].y += ppy;
    }
    perspectiveTransform(old_pts, new_pts, this->H);
    for (i = 0; i < new_pts.size(); i++) {
        new_kpts.push_back(KeyPoint(new_pts[i].x , new_pts[i].y , 1));
        old_kpts.push_back(KeyPoint(old_pts[i].x , old_pts[i].y , 1));
//        matches.push_back(DMatch(i, i, 0));
    }
    Mat img_matches;
    drawMatches(old_img, old_kpts, new_img, new_kpts, matches, img_matches);
    imwrite("kpmatches.jpg", img_matches);
}

double get_dist(Point2f a, Point2f b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void CameraTransformer::refineOnF(
        vector<Point3f>& pts3d,
        vector<Point2f>& pts2d_output,
        vector<Point2f>& pts2d) {
    // the matching points are present in seedPts, qPts
    vector<Point3f> pts3d_f;
    vector<Point2f> pts2d_f;
    double THRESH = 20;

    // modify the seed / q points to take care of principal point 
    vector<Point2f> seedPts2, qPts2;
    for (int i = 0; i < seedPts.size(); i++) {
        seedPts2.push_back(Point2f(
                    this->seedPts[i].pt.x - 512,
                    this->seedPts[i].pt.x - 384));
        qPts2.push_back(Point2f(
                    this->qPts[i].pt.x - 512,
                    this->qPts[i].pt.x - 384));
    }

    for (int i = 0; i < seedPts2.size(); i++) {
        double min_dist = 99999;
        int min_dist_idx = 0;
        for (int j = 0; j < pts2d.size(); j++) {
            double d = get_dist(pts2d[j], seedPts2[i]);
            if (d < min_dist) {
                min_dist = d;
                min_dist_idx = j;
            }
        }
        cout << min_dist << endl;
        if (min_dist < THRESH) {
            pts3d_f.push_back(pts3d[min_dist_idx]);
            pts2d_f.push_back(qPts2[i]);
        }
    }
    pts2d_output = pts2d_f;
    pts3d = pts3d_f;
}

