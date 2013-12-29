#include <jni.h>
#include <vector>

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <fstream>
#include <android/log.h>


using namespace std;
using namespace cv;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

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

Mat QueryAug;
int AugFlag = 0;

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
	return p1_c - p2_c < 0 ? 1 : -1;
}

int compare2 ( const void *p1 , const void *p2 ){
	double p1_c = (* (RImage *)p1).m_score;
	double p2_c = (* (RImage *)p2).m_score;
	return p1_c - p2_c < 0 ? 1 : -1;
}


int isInside ( int x , int y , int x1 , int y1  , int x3 , int y3 )
{
	if ( x >= x1 && x <= x3 ){
		if ( y >= y1 && y <= y3 ){
			return 1 ;
		}
	}
	return 0;
}

int findInt ( string c)
{
	int a = 0 ;
	int i = 0 ;
	while ( c[5+i] != '.' )
	{
		a = a*10 + (int)(c[5 + i] - '0') ;
		i++;
	}
	return a ;
}


/*For augmentation */
vector<DMatch> GetMatches( int n, vector<int>QWords, RImage *ImagesRetrieved,
		vector<string>& ImageList, vector<KeyPoint>& Rkeypoints, vector<KeyPoint>& keypoints
		){

	vector< int > RWords;
	vector < Mat > RMatches ;
	string WordKeyDir = "/sdcard/VittalaN/Augment/wordkeys/wk_";

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

	vector<DMatch> imatches;
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

	return  imatches;

}

void mergeImgs(Mat& orig, Mat snap) {
    for (int i = 0; i < snap.rows; i++) {
        for (int j = 0; j < snap.cols; j++) {
            if (norm(snap.at<Vec3b>(i,j)) > 10) {
                orig.at<Vec3b>(i,j) = snap.at<Vec3b>(i,j);
            }
        }
    }
}


void Augment(Mat& source, Mat& sourceRGB){
	int N = 285;
	ifstream InvertedIndexFile("/sdcard/VittalaN/Augment/InvertedIndexF.txt",ios::in);

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

	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Augment: Inverted index search");

	ifstream t_file("/sdcard/VittalaN/Augment/HKMeans_10000.Tree",ios::in);
	Tree* tree = ParseTree(t_file,0);
	t_file.close();
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Augment: Tree read");


	string imageListFileName = "/sdcard/VittalaN/Augment/Annotations.txt";
	vector < string > ImageList;
	vector < string > Annotations;
	string temp, tempLine;
	ifstream imageListFile;
	imageListFile.open(imageListFileName.c_str(),ios::in);
	string TF_filename = "/sdcard/VittalaN/Augment/DCount.txt";
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

	Mat img(source);
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

	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Augment: Hist ready");

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

	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","InvInd search done");

/*	for(int n = 1 ;  n <= 10 ; n++){
		cout << Annotations[ ImagesRetrieved[ n ].index - 1 ]  << " " << ImagesRetrieved[ n ].r_score ;
		cout << endl;
	}
*/

	std::vector<KeyPoint> Rkeypoints;
	vector<Point2f> obj;
	vector<Point2f> scene;
	vector<DMatch> imatches;
	for( int n = 1 ; n <= 10 ; n++ ){

	   obj.clear();
	   scene.clear();
	   imatches.clear();
	   imatches = GetMatches ( n, QWords, ImagesRetrieved, ImageList, Rkeypoints, keypoints);
	   char str[200];
	   sprintf(str, "imatches: %d", imatches.size());
		__android_log_write(ANDROID_LOG_VERBOSE,"Progress",str);
	   //cout << imatches.size() << endl;
	   for( int i = 0; i < imatches.size(); i++ )
	   {
		   //-- Get the keypoints from the good matches
		   obj.push_back( Rkeypoints[ imatches[i].queryIdx ].pt );
		   scene.push_back( keypoints[ imatches[i].trainIdx ].pt );
	   }


	   std::vector<uchar> inliers(obj.size(),0);
	   if(obj.size() == 0 ) continue;
	   Mat F = findFundamentalMat( obj, scene, inliers , CV_FM_RANSAC, 2, 0.99 );
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
	}
	qsort( ImagesRetrieved + 1, N , sizeof(ImagesRetrieved[ 1 ]), compare2 );
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Augment: Geom Verification done");



/*	string resOutFName = "output_All/" + imageName + "_out.txt";
/	ofstream resOutF( resOutFName.c_str() );
	for(int n = 1 ;  n <= 100 ; n++){
//		cout << ImageList[ImagesRetrieved[n].index - 1] << " " << Annotations[ ImagesRetrieved[ n ].index - 1 ]  << " " << ImagesRetrieved[ n ].m_score ;
		if( ImagesRetrieved[n].r_score == 0 ) break;
		cout << ImageList[ImagesRetrieved[n].index - 1];
		cout << endl;
		resOutF << ImageList[ImagesRetrieved[n].index - 1] << endl;
	}
	resOutF.close();

*/
	string bestMatch = ImageList[ ImagesRetrieved[2].index - 1 ];
//	cout<< bestMatch << endl;
	string snapsDir = "/sdcard/VittalaN/Augment/snapshots/";
	Mat homoSource = imread( snapsDir + "final_" + bestMatch + ".jpg" );
	Mat final;
	   obj.clear();
	   scene.clear();
	   imatches.clear();
	   string templog = "Augment:best match" + bestMatch;
		__android_log_write(ANDROID_LOG_VERBOSE,"Progress",templog.c_str());

	imatches = GetMatches ( 2, QWords, ImagesRetrieved, ImageList, Rkeypoints,keypoints);
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","after getmatches");

	for( int i = 0; i < imatches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( Rkeypoints[ imatches[i].queryIdx ].pt );
		scene.push_back( keypoints[ imatches[i].trainIdx ].pt );
	}


//	cout << obj.size() << " " << scene.size() << " " << imatches.size() << endl;


	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","before    !!!N homography");
	if( obj.size() == 0){
		__android_log_write(ANDROID_LOG_VERBOSE,"Progress","no matching points :(");
		return;
	}
	Mat H = findHomography( obj, scene, CV_RANSAC );
	warpPerspective( homoSource, final, H, Size( homoSource.cols, homoSource.rows ));
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Augment Homography");

	QueryAug = sourceRGB.clone();
	mergeImgs( QueryAug, final);
	AugFlag = 1;
	imwrite( "/sdcard/aug.jpg",QueryAug);


}

int N = 4729;
map< int, map< int, int > > InvertedFile;
Tree *tree;
vector < string > ImageList;
vector < string > Annotations;
vector < int > TF;




extern "C" {
JNIEXPORT void JNICALL Java_com_example_vittalatemple_LaunchActivity_LoadData(JNIEnv* env, jobject thiz)
{

	//=======================================
	//	Loading Inverted Index File
	//=======================================
	ifstream InvertedIndexFile("/sdcard/VittalaN/InvertedIndex_10K.txt",ios::in);

	LOGI("ProgressCheck: InvertedFile open status: %d", InvertedIndexFile.is_open());
	int vword, vcount, vimage, vnum;
	while(InvertedIndexFile.good()){
		InvertedIndexFile >> vword >> vcount;
		for(int i = 0 ; i < vcount ; i++ ){
			InvertedIndexFile >> vimage >> vnum ;
			InvertedFile[vword][vimage] = vnum;
		}
	}
	InvertedIndexFile.close();
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","InvertedIndexFile Loaded");
	LOGI("ProgressCheck: InvertedFile size= %d",(int)InvertedFile.size());

	//=======================================
	//	Loading the HKMeans Tree
	//=======================================
	ifstream t_file("/sdcard/VittalaN/HKMeans_10000.Tree",ios::in);
	LOGI("ProgressCheck: Tree file open status: %d", t_file.is_open());
	tree = ParseTree(t_file,0);
	t_file.close();
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","HKMeans tree Loaded");

	//======================================================
	//	Loading Image file names and Term Frequency Count
	//======================================================
	string imageListFileName = "/sdcard/VittalaN/Annotations.txt";
	string temp, tempLine;
	ifstream imageListFile;
	imageListFile.open(imageListFileName.c_str(),ios::in);
	string TF_filename = "/sdcard/VittalaN/DCount.txt";
	int tempTF;
	ifstream TF_file;
	TF_file.open(TF_filename.c_str(), ios::in );

	for(int i = 1 ; i <= N ; i++ ){
		imageListFile >> temp;
		getline( imageListFile, tempLine);
		ImageList.push_back( temp );
		Annotations.push_back( tempLine );
		TF_file >> tempTF;
		TF.push_back(tempTF);

	}
	//=======================================
			//	Loading Image file names
			//=======================================
	/*         ifstream an1_file("/sdcard/Golkonda_5500/AnnInfo.txt",ios::in);
         ifstream an2_file("/sdcard/Golkonda_5500/AnnText.txt",ios::in);
	     ifstream an3_file("/sdcard/Golkonda_5500/AnnBoundary.txt",ios::in);
	     int TotalAnn;
	     an1_file>>TotalAnn;
	     string TempImageId;
	     for( int i = 0 ; i < TotalAnn ; i++ ){
	            Annotation TempAnn;
	            an1_file >> TempImageId >> TempAnn.type ;
	            getline( an2_file , TempAnn.text );
	            if( TempAnn.type.compare("OBJECT") == 0 ){
	                   an3_file >> TempAnn.boundary;
	            }
	            ImageAnns[ TempImageId ].push_back(TempAnn);
	     }
	     an1_file.close();
	     an2_file.close();
	     an3_file.close();

	 */
}

}

extern "C" {
JNIEXPORT jstring JNICALL Java_com_example_vittalatemple_Sample3View_Search(JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray yuv, jintArray bgra)
//JNIEXPORT jstring JNICALL Java_com_example_vittalatemple_QueryActivity_Search(JNIEnv* env, jobject thiz)
{




	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Start...");

	//=======================================
	//	Loading the frame
	//=======================================
	//		Mat* pMatGr=(Mat*)addrGray;
	//	    Mat* pMatRgb=(Mat*)addrRgba;

	//		Mat img_temp = *pMatGr;
	//		Mat img_temp = imread("/data/data/com.example.heritagecam/files/TestImage.jpg",CV_LOAD_IMAGE_GRAYSCALE);

    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    jint*  _bgra = env->GetIntArrayElements(bgra, 0);

    Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
    Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
    Mat img_temp(height, width, CV_8UC1, (unsigned char *)_yuv);

    cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);


/*	Mat img_temp = imread("/sdcard/TestImage.jpg",CV_LOAD_IMAGE_GRAYSCALE);

	int height = img_temp.rows;
	int width = img_temp.cols;
*/
	Mat img;
	if( height > 500 || width > 500 ){
		int new_h = 360;
		int new_w = (new_h*width)/height;
		img.create(new_h,new_w,CV_8UC3);
		resize(img_temp,img,img.size(),0,0, INTER_LINEAR);
	}else{
		img = img_temp.clone();
	}

	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Frame Loaded");

	//#if 0
	//============================================
			//	Feature Extraction and Histogram Building
	//============================================
	SiftFeatureDetector detector;
	std::vector<KeyPoint> keypoints;

	SiftDescriptorExtractor extractor;
	Mat descriptors;

	detector.detect(img,keypoints);
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","SIFT detection");

	extractor.compute(img,keypoints,descriptors);
	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","SIFT descriptor extraction");


	map< int, int > QueryHist;
	vector < int > QWords;
	int des[128];
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
	RImage ImagesRetrieved[ 4735 ];
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

	__android_log_write(ANDROID_LOG_VERBOSE,"Progress","Inverted index search");

	for( int n = 1 ; n <= 10 ; n++ ){
		__android_log_write( ANDROID_LOG_VERBOSE,"Annotation",ImageList[ ImagesRetrieved[n].index - 1 ].c_str() );
	}
#if 0
	std::vector<KeyPoint> Rkeypoints;
	vector< int > RWords;
	vector < Mat > RMatches ;
	string WordKeyDir = "/sdcard/VittalaN/wordkeys/uf_10_";
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
		Mat F = findFundamentalMat( obj, scene, inliers , CV_FM_RANSAC, 2, 0.99 );
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
	qsort( ImagesRetrieved + 1, 10 , sizeof(ImagesRetrieved[ 1 ]), compare2 );
#endif


	string ref = ImageList[ ImagesRetrieved[1].index - 1 ];
	string check = ref.substr( ref.find("_") + 1, ref.find_last_of("_") - ref.find("_") - 1 );
	int countCheck = 0;
	for( int n = 2 ; n <= 10 ; n++ ){
		__android_log_print( ANDROID_LOG_DEBUG,"Annotation","%s: %s: %lf : %s", check.c_str(),
				ImageList[ ImagesRetrieved[n].index - 1 ].c_str(), ImagesRetrieved[n].r_score,
				Annotations[ImagesRetrieved[n].index -1].c_str() );
		ref = ImageList[ ImagesRetrieved[n].index - 1 ];
		if( check.compare( ref.substr(ref.find("_")+1, ref.find_last_of("_") - ref.find("_") - 1) )  == 0 )
			countCheck++;

	}

	string result;
#if 0
	if( ImagesRetrieved[1].m_score > 60 )
		result = Annotations[ImagesRetrieved[ 1 ].index - 1];
	else if( ImagesRetrieved[ 1 ].m_score > 12 ){
		result = Annotations[ImagesRetrieved[ 1 ].index - 1];
		if( countCheck < 4 ){
			result = "This MAY BE " + result;
		}
	}
	else
		result = "No Annotation Found";
#endif
	if(countCheck > 2){
		result = Annotations[ImagesRetrieved[ 1 ].index - 1];
		if(check.compare("StoneChariot") == 0 ){
			Augment(img, mbgra);
		}else{
			AugFlag = 0;
		}
	}
	else{
		result = "No Annotation Found";
	}


    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);
	__android_log_print( ANDROID_LOG_DEBUG,"AnnotationFinal","%s",result.c_str() );
	return env->NewStringUTF(result.c_str());

	//#endif
	//	   return env->NewStringUTF("hello.. debugging!");


	/*
       if( Aflag == 0 ){
    	   __android_log_write(ANDROID_LOG_VERBOSE,"Result","No Annotation Found");
//   		   putText(img, "No Annotation Found" , Point(20,100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0,255), 1,8);
   		   char ret[100] = "No Annotation Found";

   		   return env->NewStringUTF(ret);
//           circle(*pMatRgb, Point(20,20), 10, Scalar(255,0,0,255));

       }
	 */
	//       imwrite("/sdcard/test_output.jpg",img);


}



}


extern "C" {
JNIEXPORT void JNICALL Java_com_example_vittalatemple_Sample3View_FindFeatures(JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray yuv, jintArray bgra)
{
    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    jint*  _bgra = env->GetIntArrayElements(bgra, 0);

    Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
    Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
    Mat mgray(height, width, CV_8UC1, (unsigned char *)_yuv);

    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);


    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);
}

}

extern "C" {
JNIEXPORT jint JNICALL Java_com_example_vittalatemple_Sample3View_CheckAugmented(JNIEnv* env, jobject thiz)
{
	if( AugFlag == 1){
		return 1;
	}
	else{
		return 0;
	}

}
}
