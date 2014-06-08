#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>
#include "NVMUtils.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

#define MAX_N 1510
#define HOMO_FILE_PREFIX "homo_"

void readAllHomos(int N, 
        string homos_dir, 
        cv::Matx33d homos[][MAX_N]) {
    for (int i = 0; i < N; i++) {
        string fname = homos_dir + "/" + HOMO_FILE_PREFIX + to_string(i) + ".txt";
        ifstream fin(fname.c_str());
        string line;
        int lineno = 0;
        while (getline(fin, line)) {
            int j = lineno / 3;
            int row = lineno % 3;
            istringstream iss(line);
            iss >>  homos[i][j](row, 0)
                >>  homos[i][j](row, 1)
                >>  homos[i][j](row, 2);
            lineno++;
        }
        fin.close();
    }
}

double getErr(const map<int, cv::Point2f> &im1_pts,
        map<int, cv::Point2f> &im2_pts,
        const cv::Matx33d &H) {
    vector<cv::Point2f> im1_allpts;
    for (auto iter = im1_pts.begin(); iter != im1_pts.end(); ++iter) {
        im1_allpts.push_back(iter->second);
    }
    vector<cv::Point2f> im1_allpts_tx;
    cv::perspectiveTransform(im1_allpts, im1_allpts_tx, H);
    int i = 0;
    double res = 0;
    int n_common = 0;
    for (auto iter = im1_pts.begin(); iter != im1_pts.end(); ++iter, ++i) {
        cv::Point2f im1_pt_tx = im1_allpts_tx[i];
        if (im2_pts.count(iter->first) > 0) {
            cv::Point2f im2_pt = im2_pts[iter->first];
            res += cv::norm(im1_pt_tx - im2_pt);
            n_common++;
        }
    }
    if (n_common == 0) return -1;
    else return res / n_common;
}

void computeE(map<int, map<int, cv::Point2f>> &img2pts,
        cv::Matx33d homos[][MAX_N],
        int N,
        double E[][MAX_N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            E[i][j] = getErr(img2pts[i], img2pts[j], homos[i][j]);
        }
        cout << "done " << i << endl;
    }
}

void writeToFile(double E[][MAX_N], int N, string ofile) {
    ofstream fout(ofile.c_str(), ios::out);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (E[i][j] == -1) {
                fout << "inf";
            } else {
                fout << E[i][j];
            }
            fout << " ";
        }
        fout << endl;
    }
    fout.close();
}

map<int, map<int, cv::Point2f>> img2pts;
cv::Matx33d homos[MAX_N][MAX_N];
double E[MAX_N][MAX_N];

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "Show this help message")
        ("nvm-file,f", po::value<string>()->required(), "input NVM fpath")
        ("output-fname,o", po::value<string>()->required(), "output fpath")
        ("homos-dir,h", po::value<string>()->required(), "input homographies dir path")
        ("num-imgs,n", po::value<int>()->required(), "total number of images")
        ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &e) {
        cerr << e.what() << endl;
        return -1;
    }
    
    if (vm["num-imgs"].as<int>() > MAX_N) {
        cerr << "Change MAX_N value in code" << endl;
        return -1;
    }

    readKptsFromNVM(vm["nvm-file"].as<string>(), img2pts);
    cout << "Read kpts" << endl;
    int N = vm["num-imgs"].as<int>();
    readAllHomos(N,
            vm["homos-dir"].as<string>(),
            homos);
    cout << "Read homos" << endl;

    computeE(img2pts, homos, N, E);
    cout << "computed E" << endl;
    writeToFile(E, N, vm["output-fname"].as<string>());
    return 0;
}
