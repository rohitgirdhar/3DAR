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
#include "CameraNVMParser.hpp"
#include "CameraTransformer.hpp"
#include <opencv2/opencv.hpp>

GLMmodel *pmodel, *pmodel_act;

using namespace std;
using namespace cv;

void snapshot(char*);

void computeCamera();
//Called when a key is pressed

int ptCld = 1;
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			exit(0);
        case 'p':
            snapshot("snap.jpg");
            break;
        case 't':
            ptCld = (ptCld + 1) % 2;
            break;
	}
}

//Initializes 3D rendering
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	//glShadeModel(GL_SMOOTH); //Enable smooth shading
}



float _angle = -70.0f;
glm::mat3x4 res;

//Draws the 3D scene

GLdouble scale = 3;

void mouseControl(int button, int state, int x, int  y) {
    if (button == GLUT_LEFT_BUTTON) {
            cout << "UP" << endl;
            scale += 0.5;
    } else {
        scale -= 0.5;
    }
}

mat3x4 Pcam;
mat4x3 Pinv;
vec3 camC;
vec4 qRot;
double focal = 1200;
mat3x3 intr;
mat3x4 extr;



vec3 center, look, up;
void drawScene() {
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
/*    gluLookAt(camC[0],camC[1],camC[2],
            camC[0] + extr[2][0],
            camC[1] + extr[2][1],
            camC[2] + extr[2][2],
            -extr[1][0],
            -extr[1][1],
            -extr[1][2]); 
*/
    gluLookAt(center[0], center[1], center[2],
            center[0] + look[0],
            center[1] + look[1],
            center[2] + look[2],
            up[0],
            up[1],
            up[2]); 

    glMatrixMode(GL_MODELVIEW);
	
    //Add ambient light
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
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
    GLfloat lightPos1[] = {-extr[2][0], -extr[2][1], -extr[2][2], 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	
	glColor3f(1.0f, 1.0f, 0.0f);

    int mode = 0;    

    mode = mode | GLM_SMOOTH;

    mode = mode | GLM_2_SIDED;

    mode = mode | GLM_MATERIAL;

    mode = mode | GLM_TEXTURE;

    glPushMatrix();
    glLoadIdentity(); 
//    glmDraw(pmodel, mode);
    glRotatef(180, 1,0,0);
    glRotatef(60, 0,1,0);
    glTranslatef(0, 0, 2.9);
    glTranslatef(7.4, 0, 0);
    glTranslatef(0,1,0);  // inc to go up
    glScalef(3.8, 3.8, 3.8);
    glmDraw(pmodel_act, mode);

    glPopMatrix();

	glutSwapBuffers();
}

void update(int value) {
	_angle += 1.5f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	glutTimerFunc(2, update, 0);
}


void computeCamera() {
    CameraNVMParser::getCameraMatrix(
            "00000072.jpg", 
            intr, 
            extr, 
            camC, 
            focal,
            qRot);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0, 1024, 768, 0, 0, 100);
//    glMultMatrixf(value_ptr(intr));
//    glMatrixMode(GL_MODELVIEW);
//    glLoadMatrixf(value_ptr(extr));
//    cout << res[0][2] << " " << res[1][2] << " " << res[2][2] << endl;
//    cout << res[0][3] << " " << res[1][3] << " " << res[2][3] << endl;
    
    Pcam = extr * intr;
    Pinv = inverse(transpose(Pcam) * Pcam) * transpose(Pcam);
    cout << Pinv[0][0] << " " << Pinv[0][1] << " " << Pinv[0][2] << endl;
}

int curW = 0, curH = 0;

//Called when the window is resized
void handleResize(int w, int h) {
    curW = w; curH = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    // compute fovy given focal len
    GLdouble fovy = 2 * atan(h/(2.0*focal)) * 180/3.14;
    cout << "using fovy = " << fovy << endl;
	gluPerspective(fovy, (double)w / (double)h, 1.0, 2000.0);
}

void snapshot(char* filename) {
    Mat img(curH, curW, CV_8UC3);
    glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
    glReadPixels((GLint) 0, (GLint) 0,
            (GLint) curW - 1, (GLint) curH - 1,
            GL_BGR, GL_UNSIGNED_BYTE, img.data);
    Mat flipped;
    flip(img, flipped, 0);
    imwrite(filename, flipped);
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	
	//Create the window
	glutCreateWindow("Pose");
    initRendering();

    computeCamera();
    cout << "Camera Center: " << camC[0] << " " << camC[1] << " " << camC[2] << endl;
   
    CameraTransformer ob;
    ob.recalibrateCamera("00000001", "00000001", center, look, up);

/*    gluLookAt(camC[0],camC[1],camC[2],
            camC[0] + extr[2][0],
            camC[1] + extr[2][1],
            camC[2] + extr[2][2],
            -extr[1][0],
            -extr[1][1],
            -extr[1][2]); 
    
  */ 

//    glMatrixMode(GL_PROJECTION);
//    gluPerspective(90, 1.33, 0.1, 1000);
//    gluLookAt(5,5,5,0,0,0,0,1,0);

    GLdouble mdl[16];
    float camera_org[3];
    glGetDoublev(GL_MODELVIEW_MATRIX, mdl);
    cout << mdl[0] << " " << mdl[1] << " " << mdl[2] << endl;


    pmodel = glmReadOBJ("dense.0.mesh.obj");
    pmodel_act = glmReadOBJ("../../mesh.obj");
//    pmodel = glmReadOBJ("final.obj");
 //   glmUnitize(pmodel);
    glmUnitize(pmodel_act);
    glmVertexNormals(pmodel, 90.0, GL_TRUE);
    glmVertexNormals(pmodel_act, 90.0, GL_TRUE);
    //Set handler functions
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseControl);
	glutReshapeFunc(handleResize);
	
//	glutTimerFunc(2, update, 0); //Add a timer
	
	glutMainLoop();
	return 0;
}









