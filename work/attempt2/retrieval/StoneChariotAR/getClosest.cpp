#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<cmath>
#include<stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include <opencv2/flann/flann.hpp>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace cv;

typedef struct Tree{
  int centers[10][128];
  struct Tree* sub[10];
  int nodes[10];
  int n_centers;
  int n_sub;
  int depth;
}Tree;

typedef struct RImage{
	int index;
	double r_score;
	double m_score;
	vector< Point2f > corners;
}RImage;

Tree* ParseTree(ifstream &t_file, int d){
  if( !t_file.good() ){
    return NULL;
  }
  int centers, subs;
  t_file >> centers >> subs;
  Tree *t = (Tree*)malloc(sizeof(Tree)); 
  t->n_centers = centers;
  t->n_sub = subs;
  t->depth = d;
  for(int i = 0 ; i < centers ; i++ ){
    for(int j = 0 ; j < 128 ; j++ ){
      t_file >> t->centers[i][j]; 
    }
  }
  for(int i = 0 ; i < subs ; i++ ){
    t->sub[i] = ParseTree(t_file,d+1);
  }
  for(int i = subs ; i < 10 ; i++){
    t->sub[i] = NULL;
  }
  return t;
}

void GetPath(Tree* tree , int* center , int* path , int depth){
  if(tree == NULL){
    path[depth] = -1;
    return;
  }
  double minD = 100000;
  double tempD;
  int minC = 2;  
  for(int i = 0 ; i < tree->n_centers ; i++ ){
    tempD = 0;
    for(int j = 0 ; j < 128 ; j++ ){
       tempD += pow((double)(tree->centers[i][j] - center[j]),2.0);
    }
    tempD = sqrt(tempD);
    if(tempD < minD){
      minD = tempD;
      minC = i;
    }
  }
  path[depth] = minC+1;
  GetPath(tree->sub[minC], center, path, depth+1);
}

int GetWord( int *path ){
	int w = 0;
	for( int i = 0 ; i < 4 ; i++ ){
//		cout << path[i] << " ";
		w = (w*10) + path[i] - 1;
	}
	w += 1;
//	cout << w << endl;
	return w;
}

int compare ( const void *p1 , const void *p2 ){
	double p1_c = (* (RImage *)p1).r_score;
	double p2_c = (* (RImage *)p2).r_score;
	return p1_c < p2_c ? 1 : -1; 
}

int compare2 ( const void *p1 , const void *p2 ){
	double p1_c = (* (RImage *)p1).m_score;
	double p2_c = (* (RImage *)p2).m_score;
	return p1_c < p2_c ? 1 : -1; 
}

int main(int argc, char **argv){
	int N = 285;
	ifstream InvertedIndexFile("InvertedIndexF.txt",ios::in);

	map< int, map< int, int > > InvertedFile;
	int vword, vcount, vimage, vnum;
	while(InvertedIndexFile.good()){
		InvertedIndexFile >> vword >> vcount;
		for(int i = 0 ; i < vcount ; i++ ){
			InvertedIndexFile >> vimage >> vnum ;
			InvertedFile[vword][vimage] = vnum;
		}
	}
	InvertedIndexFile.close();
	ifstream t_file("HKMeans_10000.Tree",ios::in);
	Tree* tree = ParseTree(t_file,0);
	t_file.close();

	string imageListFileName = "Annotations.txt";
	vector < string > ImageList;
	vector < string > Annotations;
	string temp, tempLine;
	ifstream imageListFile;
	imageListFile.open(imageListFileName.c_str(),ios::in);
	string TF_filename = "DCount.txt";
	vector < int > TF;
	int tempTF;
	ifstream TF_file;
	TF_file.open(TF_filename.c_str(), ios::in );
	for(int i = 1 ; i <= N ; i++ ){
		imageListFile >> temp;
		getline(imageListFile, tempLine);
		ImageList.push_back( temp );
		Annotations.push_back( tempLine );
		TF_file >> tempTF;
		TF.push_back(tempTF);
	}

	SiftFeatureDetector detector;
	std::vector<KeyPoint> keypoints;

	SiftDescriptorExtractor extractor;
	Mat descriptors;
	
//	string imageName(argv[1]);
	string Dir;
	if( argc < 2 ) {
		Dir = "";
	}
	else{
		Dir = argv[1];
	}
	string imageName = string(argv[2]);

	Mat img_temp = imread(Dir + imageName + ".jpg", IMREAD_GRAYSCALE);
	int height = img_temp.rows;
	int width = img_temp.cols;
	Mat img;
	if( height > 500 || width > 500 ){
		int new_h = 360;
		int new_w = (new_h*width)/height;
		img.create(new_h,new_w,CV_8UC3);
		resize(img_temp,img,img.size(),0,0, INTER_LINEAR);
	}else{
		img = img_temp.clone();
	}
//	ORB orb;
//	orb(img, Mat(), keypoints, descriptors);

	detector.detect(img,keypoints);
	extractor.compute(img,keypoints,descriptors);
	

//	cout << descriptors.rows << endl;
	map< int, int > QueryHist;
	vector < int > QWords;
	int des[128];
//	cout << height << " " << width << ": " << keypoints.size() << " " << descriptors.rows << endl;
	for(int i = 0 ; i < descriptors.rows ; i++ ){
		for(int j = 0 ; j < 128 ; j++){
			des[j] = descriptors.at<float>(i,j);
		}
		int path[12];
		GetPath( tree, des, path, 0 );
		int hist = GetWord( path );
//		cout << hist << " ";
		if(QueryHist.count( hist ) > 0){
			QueryHist[ hist ] += 1;
		}
		else{
			QueryHist[ hist ] = 1;
		}
		QWords.push_back( hist );

	}
//	cout << endl << QWords.size() << endl;

	double idf;
	RImage ImagesRetrieved[ 290 ];
	for(int i = 1 ; i <= N ; i++ ) {
		ImagesRetrieved[ i ].index  = i;
		ImagesRetrieved[ i ].r_score  = 0;
		ImagesRetrieved[ i ].m_score  = 0;
	}

	int word, count;
	for( map< int, int >::iterator it = QueryHist.begin() ; it != QueryHist.end() ; ++it){
		word = (*it).first;
		count = (*it).second;
		if( InvertedFile[ word ].size() == 0 )
			continue;
		idf = log10( N/InvertedFile[word].size() );
		for( map< int , int >::iterator it2 = InvertedFile[word].begin() ; it2 != InvertedFile[word].end() ; ++it2 ){
			int terms = TF[ (*it2).first-1 ] > QWords.size() ? TF[ (*it2).first-1 ] : QWords.size() ;
			ImagesRetrieved[(*it2).first].r_score += ( ( ( count > (*it2).second ? (*it2).second : count ) * 1.0 )/terms)  * idf; 
//			ImagesRetrieved[(*it2).first].r_score += count * idf; 
		}
	}
	
	qsort( ImagesRetrieved + 1, N , sizeof(ImagesRetrieved[ 1 ]), compare );

/*	for(int n = 1 ;  n <= 10 ; n++){
		cout << Annotations[ ImagesRetrieved[ n ].index - 1 ]  << " " << ImagesRetrieved[ n ].r_score ;
		cout << endl;
	}
*/

	std::vector<KeyPoint> Rkeypoints;
	vector< int > RWords;
	vector < Mat > RMatches ;
	string WordKeyDir = "wordkeys/wk_";
	for( int n = 1 ; n <= 10 ; n++ ){
	  string RImgFilePath = WordKeyDir + ImageList[ ImagesRetrieved[n].index - 1] + ".txt";
	  ifstream RImgFile( RImgFilePath.c_str(),ios::in );
	  int countWords, Rword;
	  Point2f RPoint;
	  while( RImgFile.good() && !RImgFile.eof() ){
            RImgFile >> Rword >> RPoint.x >> RPoint.y;
	    RWords.push_back( Rword );
	    KeyPoint temp(RPoint,0);
	    Rkeypoints.push_back(temp);
	  }
	  RImgFile.close();

	  std::vector< DMatch > imatches;
	  for(int i = 0 ; i < RWords.size() ; i++ ){
	    for(int j = 0 ; j < QWords.size() ; j++ ){
		if( RWords[i] == QWords[j] ){
//			if(matches[i] == minc[j] && fabs(distMatchQ[i] - distMatchR[i]) < 10000){
 		  DMatch tempDMatch;
		  tempDMatch.queryIdx = i;
		  tempDMatch.trainIdx = j;
		  tempDMatch.distance = 0;
		  imatches.push_back(tempDMatch);
		}
	     }
	   }
	   std::vector<Point2f> obj;
	   std::vector<Point2f> scene;

   	   for( int i = 0; i < imatches.size(); i++ )
	   {
		//-- Get the keypoints from the good matches
	     obj.push_back( Rkeypoints[ imatches[i].queryIdx ].pt );
	     scene.push_back( keypoints[ imatches[i].trainIdx ].pt ); 
	   }

	   std::vector<uchar> inliers(obj.size(),0);
	   if(obj.size() == 0 ) continue;
	   Mat F = findFundamentalMat( obj, scene, inliers , FM_RANSAC, 2, 0.99 );
	   std::vector<cv::Point2f>::const_iterator itPts=obj.begin();
	   std::vector<uchar>::const_iterator itIn= inliers.begin();
	   std::vector<Point2f> obj_ransac;
	   std::vector<Point2f> scene_ransac;

	   int it = 0, total = 0;
	   while (itPts!=obj.end()) {
	     if (*itIn){
	       obj_ransac.push_back( Rkeypoints[ imatches[ total ].queryIdx ].pt );
	       scene_ransac.push_back( keypoints[ imatches[ total ].trainIdx ].pt );
	       ++it;
	     }
	     ++itPts;
	     ++itIn;
	     total++;
	   }
	   ImagesRetrieved[ n ].m_score = 1.0 * it ; 
	   Rkeypoints.clear();
	   RWords.clear();
	}
	qsort( ImagesRetrieved + 1, N , sizeof(ImagesRetrieved[ 1 ]), compare2 );


		cout << ImageList[ImagesRetrieved[2].index - 1]; 


       return 0;
       
}


