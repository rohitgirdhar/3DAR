#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <stdio.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/opencv.hpp>
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
		w = (w*10) + path[i] - 1;
	}
	w += 1;
	return w;
}

class Sift2Word {
private:
    Tree* tree;
    int tempDes[128];

public:
    Sift2Word(const char* kmeans_fname) {
        ifstream t_file(kmeans_fname, ios::in);
        tree = ParseTree(t_file, 0);
        t_file.close();
    }

    vector<int> getVWs(Mat descriptors) {
        vector<int> res;
        for (int i = 0; i < descriptors.rows; i++) {
            for (int j = 0; j < 128; j++) {
                tempDes[j] = descriptors.at<float>(i,j);
            }
            int path[12];
            GetPath(tree, tempDes, path, 0);
	    	res.push_back(GetWord(path));
        }
        return res;
    }

    vector<DMatch> match(vector<int> vw1, vector<int> vw2) {
        vector<DMatch> matches;
        for (int i = 0; i < vw1.size(); i++) {
            for (int j = 0; j < vw2.size(); j++) {
                if (vw1[i] == vw2[j]) {
                    DMatch match;
                    match.queryIdx = i;
                    match.trainIdx = j;
                    match.distance = 0;
                    matches.push_back(match);
                }
            }
        }
        return matches;
    }
};

