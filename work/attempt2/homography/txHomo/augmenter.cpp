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

int main( int argc, char** argv )
{
  if( argc < 4 )
  { readme(); return -1; }

  char *m_file = argv[1];
  char *targetIm = argv[2];
  int scale = atoi(argv[3]);

  Mat K = imread(targetIm, IMREAD_GRAYSCALE);
  
  if( !K.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  ifstream fin;
  fin.open(m_file);

  float x, y;
  float ADD_X = K.rows * scale / 2.0f;
  float ADD_Y = K.cols * scale / 2.0f;
  while( fin >> x >> y )
  {
    obj.push_back(Point2f(x + ADD_X, y + ADD_Y));
    fin >> x >> y; scene.push_back(Point2f(x + ADD_X, y + ADD_Y));
  }

  Mat H = findHomography( obj, scene, RANSAC );

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

