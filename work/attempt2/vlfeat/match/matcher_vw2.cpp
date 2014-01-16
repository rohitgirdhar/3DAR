//////////////////////////////////////////////////////////
// Image matching, using Visual Words from Jay's system //
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/opencv.hpp"
#include "Sift2Word.h"

#define OUTPUT_FILE "../matches.txt"
#define MATCHES_IMG_FILE "../matches.jpg"
#define WORDKEYS_DIR "../../retrieval/StoneChariotAR/wordkeys/" 
//#define HTREE_FILE "../../retrieval/StoneChariotAR/HKMeans_10000.Tree"
#define HTREE_FILE "HKMeans_50000.Tree"
#define scaleRows (768.0/360.0)
#define scaleCols (1024.0/480.0)

using namespace cv;
using namespace std;

void readme();

void readKeypoints(char *img_idx, 
        vector<KeyPoint>& kpts, 
        vector<int>& vws) {
    string fname = string(WORDKEYS_DIR) +
        "wk_" + img_idx + ".txt";
    ifstream wordsFile;
    wordsFile.open(fname.c_str());
    if (!wordsFile.is_open()) {
        cerr << "Unable to open file: " << fname << endl;
        return;
    }
    while (true) {
        int vw;
        float x, y, scale, orient;
        wordsFile >> vw >> x >> y >> scale >> orient;
        if (wordsFile.eof()) break;
        kpts.push_back(KeyPoint(x * scaleCols, y * scaleRows, scale, orient));
        vws.push_back(vw);
    }
    wordsFile.close();
}

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
    Mat img_2 = imread( root + argv[2] + ".jpg", IMREAD_GRAYSCALE );

    if( !img_1.data || !img_2.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

    SIFT  detector(4000);

    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector.detect( img_1, keypoints_1 );
    detector.detect( img_2, keypoints_2 );

    //-- Step 2: Calculate descriptors (feature vectors)
    SiftDescriptorExtractor extractor;

    Mat descriptors_1, descriptors_2;

    extractor.compute( img_1, keypoints_1, descriptors_1 );
    extractor.compute( img_2, keypoints_2, descriptors_2 );

    vector<int> VW_1, VW_2;
    cout << "Read " << keypoints_2.size() << " keypoints for db img" << endl;
    Sift2Word converter(HTREE_FILE);
    VW_1 = converter.getVWs(descriptors_1);
    VW_2 = converter.getVWs(descriptors_2);

    vector<DMatch> matches = converter.match(VW_1, VW_2);
    cout << "Found " << matches.size() << " matches" << endl;

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

    cout << "Found " << good_matches.size() << " good matches" << endl;
    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
            good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
            vector<char>(), DrawMatchesFlags::DEFAULT );

    //-- Show detected matches
    imwrite(MATCHES_IMG_FILE, img_matches);
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

