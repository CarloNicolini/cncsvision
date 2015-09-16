// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.
// Monitor tilt in mezzo
// Monitor zoom -23

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "GLMaterial.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "GLText.h"
#include "GLMaterial.h"
#include "GLLight.h"

#include "Util.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
//#define HAVE_LIGHT
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
#define SCREEN_WIDE_SIZE 310.0    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

void initProjectionScreen(double _focalDist, const Affine3d &_transformation);

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak optotrak;
CoordinatesExtractor headEyeCoords,modelCoordinates;
Affine3d lastTransformation=Affine3d::Identity();

/********* CALIBRATION 28/Febbraio/2011   **********/
static const double m_xc = -419.5;
static const double m_yc = 500.0;
static const double m_zc = 440.0;
static const Vector3d calibration(m_xc, m_yc, m_zc);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  -2.5;
double alignmentY =  24.0;
static const double focalDistance= -418.5;
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight;
vector <Vector3d> markers;
double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=false;
bool stereo=true;
bool gridshown=false;
int nAvanti=0;
int nDietro=0;
double scaleZ=1;
double rotZ=0;

int headCalibrationDone=0;
bool allVisibleHead=false,allVisibleModel=false;
/** GRASPING TASK RELATED VARIABLES **/
Vector3d translation;
Vector3d p1,p2,p3,p4;
Vector3d p1t,p2t,p3t;
RigidBody rigidCurrent,rigidAux;

/*** CURVES RELATED VARIABLES **/
Vector3d controlPoint1, controlPoint2;
ObjLoader model;

/******* FUNCTIONS ***********************************/
void drawSomething()
{
    //WHITELIGHT
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixd( rigidAux.getFullTransformation().data() );
    glTranslated(5.5/100,0,7.5/100);
    glScalef(1,1,scaleZ);

    model.draw();
    glPopMatrix();
}

void cleanup()
{   optotrak.stopCollection();
}

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    /* Set depth buffer clear value */
    glClearDepth(1.0);

    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);

    /* Set depth function */
    glDepthFunc(GL_LEQUAL);


    glEnable(GL_COLOR_MATERIAL);
    //Enable a single OpenGL light.
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse );
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawInfo()
{
    markers = optotrak.getAllPoints();
    if ( headCalibrationDone==0 )
    {
        allVisibleHead = isVisible(markers[17]) && isVisible(markers[18]) && isVisible(markers[1]) && isVisible(markers[2]) && isVisible(markers[3]) ;
        allVisibleModel = (markers[10]+markers[14]+markers[19]+markers[20]).norm() < 1E20;
        if ( allVisibleHead )
            glClearColor(0.0,1.0,0.0,1.0); //green light
        else
            glClearColor(1.0,0.0,0.0,1.0); //red light
    }

    GLText text;
    text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
    text.enterTextInputMode();
    text.draw("Head is visible? " + stringify<int>(allVisibleHead));
    text.draw("Full model is visible? " + stringify<bool>(allVisibleModel));
    text.draw( stringify<double>((markers[14]-modelCoordinates.getFinger()).norm()));
    text.leaveTextInputMode();
}

void drawGLScene()
{   if (stereo)
    {   glDrawBuffer(GL_BACK);

        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawInfo();
        drawSomething();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawInfo();
        drawSomething();

        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawInfo();
        drawSomething();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program

    case 'q':
    case 27:
    {   cleanup();
        exit(0);
    }
    break;
    case '4':
        alignmentX-=0.5;
        initProjectionScreen(focalDistance,Affine3d::Identity());
        break;
    case '6':
        alignmentX+=0.5;
        initProjectionScreen(focalDistance,Affine3d::Identity());
        break;
    case '2':
        alignmentY+=0.5;
        initProjectionScreen(focalDistance,Affine3d::Identity());
        break;
    case '8':
        alignmentY-=0.5;
        initProjectionScreen(focalDistance,Affine3d::Identity());
        break;
    case '+':
        scaleZ+=0.01;
        break;
    case '-':
        scaleZ-=0.01;
        break;
        /*	  case 'a':
        		  rotZ+=0.5;
        		  break;
        	  case 'z':
        		  rotZ-=0.5;
        		  break;
        */
    case ' ':
    {
        if ( headCalibrationDone==0 && allVisibleHead && allVisibleModel )
        {
            headEyeCoords.init(markers[17],markers[18], markers[1],markers[2],markers[3],interoculardistance );
            modelCoordinates.init(markers[14],markers[10],markers[19],markers[20]);
            headCalibrationDone=1;
            break;
        }
    }
    break;
    }
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{   optotrak.updatePoints();
    markers = optotrak.getAllPoints();

    headEyeCoords.update(markers[1],markers[2],markers[3]);
    modelCoordinates.update(markers[10],markers[19],markers[20]);
    if ( isVisible(markers[20]+markers[19]+markers[10]) )
    {
        rigidCurrent.setRigidBody(markers[20], markers[19],modelCoordinates.getFinger() );
        rigidAux.computeTransformation(rigidCurrent,true);	// true the scaling matrix
    }
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void initObjectReferencePoints()
{
    Vector3d chin(5.57*1E-4,-5.9659*1E-1, 9.4033*1E-1);	// maps to marker 20
    Vector3d rightEar(-0.953309,0.019987,-0.562745);		// maps to marker 14
    Vector3d leftEar(0.956295,0.0019349,-0.561826);		// maps to marker 19
    rigidAux.setRigidBody(chin,leftEar,rightEar);
}

void initOptotrak()
{   optotrak.setTranslation(calibration);
    optotrak.setMarkersToSave(vlist_of<unsigned int>(1)(2)(3)(4)(14)(17)(18)(19)(20));
    if ( optotrak.init(20,NULL,NULL,NULL) != 0)
    {   cleanup();
        exit(0);
    }
//boost::thread optothread( &Optotrak::startRecording, &optotrak);
//optothread.detach();
}


void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}


int main(int argc, char*argv[])
{
    initOptotrak();
    model.load("../data/objmodels/face.obj");
    initObjectReferencePoints();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP WEXLER");
        glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@60");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
