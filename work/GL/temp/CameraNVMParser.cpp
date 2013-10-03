/**
 * Class to parse NVM camera output file
 */
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "CameraNVMParser.hpp"
#include <cstring>

using namespace std;
using namespace glm;

void CameraNVMParser::getCameraMatrix(
        string fname, 
        mat3x3 &intr, 
        mat3x4 &extr,
        vec3 &camC,
        double& focal,
        vec4 &qRot) {

    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) intr[i][j] = 0;

    string line;
    mat3x4 res;
    ifstream infile("cameras_v2.txt");
    if (! infile) {
        cerr << "Unable to load Camera output file" << endl;
        return ;
    }
    while(getline(infile, line)) {
        if (line[0] == '#') continue;
        else if (line.size() == 0) continue;
        if (line.rfind(".jpg") != string::npos) {
            if (line.compare(fname) != 0) {
                // do not match, ignore the input
                for (int i=0; i < 12; i++) getline(infile, line);
                continue;
            }
            // this is the first line of structure, filename
            getline(infile, line); // real file name
            getline(infile, line); // focal len
            istringstream iss_1(line);
            float focal_len;
            iss_1 >> focal_len;
            focal = focal_len;
            intr[0][0] = 20*focal_len;
            intr[1][1] = -20*focal_len;

            getline(infile, line); // principal point
            istringstream iss0(line);
            iss0 >> intr[0][2] >> intr[1][2];
            intr[2][2] = 1;

//            float near = 0, far = 100;
 //           intr[2][2] = near + far;
//            intr[2][3] = near * far;

            getline(infile, line); // translation
            istringstream iss(line);
            iss >> extr[0][3] >> extr[1][3] >> extr[2][3]; 
  //          extr[3][3] = 1;

            getline(infile, line); // camera posn
            istringstream iss_c(line);
            iss_c >> camC[0] >> camC[1] >> camC[2];
            getline(infile, line); // R
            getline(infile, line); // R quat
            istringstream iss_2(line);
            iss_2 >> qRot[0] >> qRot[1] >> qRot[2] >> qRot[3];

            getline(infile, line); 
            istringstream iss2(line);
            iss2 >> extr[0][0] >> extr[0][1] >> extr[0][2]; 
            getline(infile, line);
            istringstream iss3(line);
            iss3 >> extr[1][0] >> extr[1][1] >> extr[1][2]; 
            getline(infile, line); 
            istringstream iss4(line);
            iss4 >> extr[2][0] >> extr[2][1] >> extr[2][2];

//            extr[3][0] = extr[3][1] = extr[3][2] = 0;

            getline(infile, line); 
            getline(infile, line); 
            break;
        }
    }
}

/*
void CameraNVMParser::getCameraMatrix(
        string fname, 
        mat4x4 &intr, 
        mat4x4 &extr,
        vec3 &camC,
        double& focal) {

    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) intr[i][j] = 0;

    string line;
    mat3x4 res;
    ifstream infile("cameras_v2.txt");
    if (! infile) {
        cerr << "Unable to load Camera output file" << endl;
        return ;
    }
    while(getline(infile, line)) {
        if (line[0] == '#') continue;
        else if (line.size() == 0) continue;
        if (line.rfind(".jpg") != string::npos) {
            if (line.compare(fname) != 0) {
                // do not match, ignore the input
                for (int i=0; i < 12; i++) getline(infile, line);
                continue;
            }
            // this is the first line of structure, filename
            getline(infile, line); // real file name
            getline(infile, line); // focal len
            istringstream iss_1(line);
            float focal_len;
            iss_1 >> focal_len;
            focal = focal_len;
            intr[0][0] = intr[1][1] = focal_len;

            getline(infile, line); // principal point
            istringstream iss0(line);
            iss0 >> intr[0][2] >> intr[1][2];
            intr[3][2] = -1;

            float near = 0, far = 100;
            intr[2][2] = near + far;
            intr[2][3] = near * far;

            getline(infile, line); // translation
            istringstream iss(line);
            iss >> extr[0][3] >> extr[1][3] >> extr[2][3]; 
            extr[3][3] = 1;

            getline(infile, line); // camera posn
            istringstream iss_c(line);
            iss_c >> camC[0] >> camC[1] >> camC[2];
            getline(infile, line); // R
            getline(infile, line); // R quat
            getline(infile, line); 
            istringstream iss2(line);
            iss2 >> extr[0][0] >> extr[0][1] >> extr[0][2]; 
            getline(infile, line);
            istringstream iss3(line);
            iss3 >> extr[1][0] >> extr[1][1] >> extr[1][2]; 
            getline(infile, line); 
            istringstream iss4(line);
            iss4 >> extr[2][0] >> extr[2][1] >> extr[2][2];

            extr[3][0] = extr[3][1] = extr[3][2] = 0;

            getline(infile, line); 
            getline(infile, line); 
            break;
        }
    }
}
*/
