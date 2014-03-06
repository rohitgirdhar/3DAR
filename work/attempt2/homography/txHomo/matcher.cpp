
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/opencv.hpp"

#define GEO_VERIFY 1

using namespace cv;
using namespace std;

void readme();

/**
 * @function main
 * @brief Main function
 */

int main( int argc, char** argv )
{
  if( argc < 4 )
  { readme(); return -1; }

  Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE);
  Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE);
  char* output_fname = argv[3];
  
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
  cout << "Doing GEOMETRIC VERIFICATION" << endl;
  good_matches.clear();
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

  ofstream fout;
  fout.open(output_fname, ios::out);
  Point2f pt1, pt2;

  for( int i = 0; i < good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    pt1 = keypoints_1[ good_matches[i].queryIdx ].pt;
    pt2 = keypoints_2[ good_matches[i].trainIdx ].pt;
    fout << pt1.x << " " << pt1.y << " " << pt2.x << " " << pt2.y << endl;
  }
  fout.close();
  return 0;
}

/**
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./a.out <closest_img_path> <query_img_path> <output_fname>" << std::endl; }

