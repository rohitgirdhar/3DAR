/**
 * @file SURF_FlannMatcher
 * @brief SURF detector + descriptor + FLANN Matcher
 * @author A. Huaman
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/opencv.hpp"

#define OUTPUT_FILE "../matches.txt"

using namespace cv;
using namespace std;

void readme();

/**
 * @function main
 * @brief Main function
 */
int main( int argc, char** argv )
{
  if( argc != 3 )
  { readme(); return -1; }

  string root = "../../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/";
  Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE);
  Mat img_2 = imread( root + argv[2], IMREAD_GRAYSCALE );

  if( !img_1.data || !img_2.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

//  SurfFeatureDetector detector( minHessian );
//  FastFeatureDetector detector( 10 );
    SiftFeatureDetector detector;

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( img_1, keypoints_1 );
  detector.detect( img_2, keypoints_2 );

  //-- Step 2: Calculate descriptors (feature vectors)
  SiftDescriptorExtractor extractor;

  Mat descriptors_1, descriptors_2;

  extractor.compute( img_1, keypoints_1, descriptors_1 );
  extractor.compute( img_2, keypoints_2, descriptors_2 );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
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
      if (matches[i].distance < 8 * min_dist) {
        close_matches.push_back(matches[i]);
      }
  }
  matches = close_matches;

//  printf("-- Max dist : %f \n", max_dist );
//  printf("-- Min dist : %f \n", min_dist );

  vector<uchar> inliers;
  vector<Point2f> pts1, pts2;
  vector< DMatch > good_matches;
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
  
  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( img_1, keypoints_1, img_2, keypoints_2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imshow( "Good Matches", img_matches );
  waitKey(0);

  ofstream output;
  Point2f pt1, pt2;
  output.open(OUTPUT_FILE);
  for( int i = 0; i < (int)good_matches.size(); i++ ) { 
      pt1 = keypoints_1[good_matches[i].queryIdx].pt;
      pt2 = keypoints_2[good_matches[i].trainIdx].pt;
      output << pt1.x << " " << pt1.y << " "
             << pt2.x << " " << pt2.y << endl;
  }
  output.close();

  return 0;
}

/**
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./a.out <query_img_path> <closest_match_img_idx>" << std::endl; }

