#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;

/**
 * @return img2pts: <imgID : <3dptID: <x,y> > >
 */
void
readKptsFromNVM(string fpath,
        map<int, map<int, pair<float,float>>> &img2pts) {
    ifstream fin(fpath.c_str());
    string line;
    getline(fin, line); getline(fin, line); // ignore
    getline(fin, line);
    int nimgs = stoi(line);
    while(nimgs--) getline(fin, line);
    getline(fin, line);
    
    getline(fin, line);
    int npts = stoi(line);
    cerr << npts << " total feature points" << endl;
    float temp;
    for (int ftrID = 0; ftrID < npts; ftrID++) {
        fin >> temp >> temp >> temp;
        fin >> temp >> temp >> temp;
        int num_imgs;
        fin >> num_imgs;
        while (num_imgs--) {
            int img_idx;
            float x, y;
            fin >> img_idx >> temp >> x >> y;
            if (img2pts.count(img_idx) <= 0) {
                img2pts[img_idx] = map<int, pair<float,float>>();
            }
            img2pts[img_idx][ftrID] = make_pair(x,y);
        }
    }
    fin.close();
}

