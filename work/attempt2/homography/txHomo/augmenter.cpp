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
float scale;
float numRows, numCols;

Point2f txPoint(Point2f a) {
    Point2f sc = (1.0f / scale) * a;
    return Point2f( sc.x + (1.0f - 1.0f / scale) * numCols / 2.0f,
            sc.y + (1.0f - 1.0f / scale) * numRows / 2.0f);
}

int main( int argc, char** argv )
{
  if( argc < 4 )
  { readme(); return -1; }

  char *m_file = argv[1];
  char *targetIm = argv[2];
  scale = (float) atoi(argv[3]);

  Mat K = imread(targetIm);
  numRows = K.rows; numCols = K.cols;
  
  if( !K.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  ifstream fin;
  fin.open(m_file);

  float x, y;
  while( fin >> x >> y )
  {
    obj.push_back( txPoint(Point2f(x, y)) );
    fin >> x >> y; 
    scene.push_back( txPoint(Point2f(x, y)) );
  }

//  Mat H = findHomography( obj, scene, 0 );
//  cout << "CAUTION!!: Homography without RANSAC" << endl;
  Mat H = findHomography( obj, scene, RANSAC );
  cout << "Using ransac in homography" << endl;

  Mat Ktx;
  warpPerspective(K, Ktx, H, Size(K.cols, K.rows));
  imwrite("snapmod.jpg", Ktx);
  cout << "Written mod image to snapmod.jpg" << endl;

  return 0;
}

/**
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./a.out <matches_file> <target_img> <scale>" << std::endl; }

