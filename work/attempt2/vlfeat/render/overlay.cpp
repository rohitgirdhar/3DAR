#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "glm/glm.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#define CAM_FILE_NAME "../cam.txt"
#define OBJ_FILE "3dmodel/chariot_top_final.obj"
#define OBJ_FILE_CUR "3dmodel/chariot_body_final.obj"
//#define OBJ_FILE "3dmodel/dense.0.mesh.obj"

using namespace std;
using namespace cv;

Mat snapshot(char*);
void mergeImgs(Mat&, Mat, Mat);
void mergeImgs2(Mat&, Mat, Mat);
void mouseControl(int button, int state, int x, int  y);

string src_img;
GLMmodel *pmodel, *curModel;
float focalLen;
float center[3];
float rotMat[3][3];
GLdouble scale = 3;
int curW = 0, curH = 0;
bool clickedCur = false; // this is just state flag
Mat snap, cur;
double i = 0.31, j = 1.05, k = 0.03, r= 270, s = 29;
void handleKeypress(unsigned char key, int x, int y);

//Initializes 3D rendering
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glShadeModel(GL_SMOOTH); //Enable smooth shading
}

void drawScene() {
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cout << "center " << center[0] << " " << center[1] << " " << center[2] << endl;
    gluLookAt(center[0], center[1], center[2],
            center[0] + rotMat[2][0],
            center[1] + rotMat[2][1],
            center[2] + rotMat[2][2],
            -rotMat[1][0],
            -rotMat[1][1],
            -rotMat[1][2]);

/* DEBUG
    gluLookAt(2, 2, 2,
            0,
            0,
            0,
            0,
            1,
            0);
*/

    //Add ambient light
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//Add positioned light
	GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//Add directed light
	GLfloat lightColor1[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	//GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    GLfloat lightPos1[] = {center[0], center[1], center[2], 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	
	glColor3f(0.97f, 0.91f, 0.78f);

    int mode = 0;    

    mode = mode | GLM_SMOOTH;

    mode = mode | GLM_2_SIDED;

    //mode = mode | GLM_MATERIAL;

    //mode = mode | GLM_TEXTURE;



    glPushMatrix();
//    glmDraw(pmodel, mode);
    
    glRotatef(180, 1,0,0);
    glRotatef(60, 0,1,0);
    glTranslatef(0, 0, 2.9);
    glTranslatef(7.4, 0, 0);
    if (clickedCur) {
        glTranslatef(0,1.2,0);  // inc to go up
    } else {
        glTranslatef(0,-2.3,0);  // for the body model
    }
    glScalef(3.4, 3.4, 3.4);
    cout << "tx by " << i << " " << j << " " << k << endl;
    glTranslatef(i,j,k);
    cout << "rot  by : " << r << endl;
    glRotatef(r,0,1,0);
    glScalef(s,s,s);
    cout << "scale by : " << s << endl;
    
    glPushMatrix();
    if (!clickedCur) {
        glmDraw(curModel, mode);
        cur = snapshot("cur.jpg");
    } else {
        glmDraw(pmodel, mode);
        snap = snapshot("snap.jpg");
    }
    glPopMatrix();

    glPopMatrix();
    
    if (clickedCur) {
        // i.e. at the last state
        string f = "../../../StoneChariotReconst/dense.nvm.cmvs/00/visualize/" +
            src_img + ".jpg";
        Mat I = imread(f);
        mergeImgs(I, snap, cur);
        imwrite("result.jpg", I);
    }

    if (!clickedCur) {
        glutPostRedisplay();
        clickedCur = true;
    }
	glutSwapBuffers();
}

//Called when the window is resized
void handleResize(int w, int h) {
    curW = w; curH = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    GLdouble fovy = 2 * atan(h/(2.0 * focalLen)) * 180/3.14;
//    fovy = 50;
    cout << "focal len: " << focalLen << endl;
    cout << "using fovy = " << fovy << " degrees" << endl;
	gluPerspective(fovy, (double)w / (double)h, 1.0, 2000.0);
}

Mat snapshot(char* filename) {
    Mat img(curH, curW, CV_8UC3);
    glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
    glReadPixels((GLint) 0, (GLint) 0,
            (GLint) curW - 1, (GLint) curH - 1,
            GL_BGR, GL_UNSIGNED_BYTE, img.data);
    Mat flipped;
    flip(img, flipped, 0);
    if (filename != NULL) {
        imwrite(filename, flipped);
    }
    return flipped;
}

void mergeImgs2(Mat& orig, Mat snap, Mat snap_match) {
    for (int i = 0; i < snap.rows; i++) {
        for (int j = 0; j < snap.cols; j++) {
            orig.at<Vec3b>(i,j) = Vec3b(0,0,0);
            if (norm(snap.at<Vec3b>(i,j)) > 10) {
                if (norm(snap_match.at<Vec3b>(i,j)) < 2) {
                    orig.at<Vec3b>(i,j) = snap.at<Vec3b>(i,j);
                }
            }
        }
    }

}

void mergeImgs(Mat& orig, Mat snap, Mat snap_match) {
    for (int i = 0; i < snap.rows; i++) {
        for (int j = 0; j < snap.cols; j++) {
            if (norm(snap.at<Vec3b>(i,j)) > 10) {
                if (norm(snap_match.at<Vec3b>(i,j)) < 2) {
                    orig.at<Vec3b>(i,j) = snap.at<Vec3b>(i,j);
                }
            }
        }
    }

}

void readCamFile() {
    char *cam_file_name = CAM_FILE_NAME;
    FILE *f = fopen(cam_file_name, "r");
    fscanf(f, "%f", &focalLen);
    fscanf(f, "%f%f%f", &center[0], &center[1], &center[2]);
    fscanf(f, "%f%f%f%f%f%f%f%f%f", &rotMat[0][0], &rotMat[0][1], &rotMat[0][2],
                                    &rotMat[1][0], &rotMat[1][1], &rotMat[1][2],
                                    &rotMat[2][0], &rotMat[2][1], &rotMat[2][2]);
    fclose(f);
}

void help() {
    cout << "Usage: ./a.out <matching file name |eg 00000001>" <<endl;
}

int main(int argc, char** argv) {

    help();
    src_img = string(argv[1]);

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	
	//Create the window
	glutCreateWindow("Pose");
    initRendering();

    readCamFile();

    pmodel = glmReadOBJ(OBJ_FILE);
    curModel = glmReadOBJ(OBJ_FILE_CUR);
    glmUnitize(pmodel);
    glmUnitize(curModel);
    glmVertexNormals(pmodel, 90.0, GL_TRUE);
    glmVertexNormals(curModel, 90.0, GL_TRUE);
    //Set handler functions
	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
	
	glutMainLoop();
	return 0;
}


