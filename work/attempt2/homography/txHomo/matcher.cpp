/**
 * @file SURF_FlannMatcher
 * @brief SURF detector + descriptor + FLANN Matcher
 * @author A. Huaman
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/opencv.hpp"

#define GEO_VERIFY 0

using namespace cv;
using namespace std;

void readme();

/**
 * @function main
 * @brief Main function
 */

Mat addBorder(Mat src, int scale) {
    int orig_rows = src.rows, orig_cols = src.cols;
    resize(src, src, Size(0,0), (1.0/scale), (1.0/scale));
    int pad_sides = (orig_cols - src.cols) / 2.0;
    int pad_top = (orig_rows - src.rows) / 2.0;
    copyMakeBorder(src, src,
            pad_top, pad_top, 
            pad_sides, pad_sides,
            BORDER_CONSTANT);
    return src;
}

int main( int argc, char** argv )
{
  if( argc < 5 )
  { readme(); return -1; }

  Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE);
  Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE);
  int scale = atoi(argv[4]);
  
  img_1 = addBorder(img_1, scale);
  img_2 = addBorder(img_2, scale);
//  imshow("test", img_1);
//  waitKey(0);

  if( !img_1.data || !img_2.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 800;

//  SurfFeatureDetector detector( minHessian );
//  OrbFeatureDetector detector(1000);
//  FastFeatureDetector detector( 10 );
//  SiftFeatureDetector detector;
  SIFT  detector(4000);

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( img_1, keypoints_1 );
  detector.detect( img_2, keypoints_2 );

  //-- Step 2: Calculate descriptors (feature vectors)
  SiftDescriptorExtractor extractor;
//  SurfDescriptorExtractor extractor;
//    OrbDescriptorExtractor  extractor;

  Mat descriptors_1, descriptors_2;

  extractor.compute( img_1, keypoints_1, descriptors_1 );
  extractor.compute( img_2, keypoints_2, descriptors_2 );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
//    BFMatcher matcher(NORM_HAMMING);  
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  vector< DMatch > close_matches;
  for (int i = 0; i < matches.size(); i++) {
      if (matches[i].distance <= 8 * min_dist) {
        close_matches.push_back(matches[i]);
      }
  }
  matches = close_matches;
    
  vector< DMatch > good_matches = matches;
//  printf("-- Max dist : %f \n", max_dist );
//  printf("-- Min dist : %f \n", min_dist );
#if GEO_VERIFY 
  vector<uchar> inliers;
  vector<Point2f> pts1, pts2;
  for (int i = 0; i < matches.size(); i++) {
      pts1.push_back(keypoints_1[matches[i].queryIdx].pt);
      pts2.push_back(keypoints_2[matches[i].trainIdx].pt);
  }
  Mat F = findFundamentalMat(Mat(pts1), Mat(pts2),
          FM_RANSAC, 3, 0.99, inliers);
  for (int i = 0; i < inliers.size(); i++) {
      if ( (int)inliers[i] ) {
          good_matches.push_back(matches[i]);
      }
  }
#endif

     //-- Localize the object
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_1[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_2[ good_matches[i].trainIdx ].pt );
  }

  Mat H = findHomography( obj, scene, RANSAC );

  Mat K = imread(argv[3]), Ktx;
  warpPerspective(K, Ktx, H, Size(K.cols, K.rows));
  imwrite("snapmod.jpg", Ktx);
  cout << "Written mod image to snapmod.jpg" << endl;

  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( img_1, keypoints_1, img_2, keypoints_2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imwrite("matches.jpg", img_matches);
  cout << "Written matches to matches.jpg" << endl;
//  imshow( "Good Matches", img_matches );
//  waitKey(0);

  return 0;
}

/**
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./a.out <closest_img_path> <query_img_path> <image to tx> <scale>" << std::endl; }

