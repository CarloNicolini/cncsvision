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

#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
#define HAVE_LIGHT
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
#define SCREEN_WIDE_SIZE 310.0    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace util;
using namespace Eigen;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
ObjLoader model;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const double m_xc = -419.5;
static const double m_yc = 500.0;
static const double m_zc = 440.0;

// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
static const double focalPlane= -418.5;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft(0,0,0);
Vector3d eyeRight(0,0,0);
Vector3d probeCalibrated(0,0,0);
Vector3d probeEnd(0,0,0);
vector <Vector3d> markers;
static const Vector3d calibration(m_xc, m_yc, m_zc);
double interoculardistance=65;
/********* REAL SCREEN POINTS 28/Febbraio/2011 ****/
Screen screen;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=true;
bool stereo=true;
double modelZ=focalPlane;

/******* FUNCTIONS ***********************************/
void drawSomething()
{

    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TRANSLUCENT
    glTranslatef(0,0,modelZ);
    glScalef(800,800,800);
    model.draw();
    glPopMatrix();
}

void cleanup()
{
}

void initRendering()
{

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    BLUEREDLIGHTS
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void drawGLScene()
{   if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeRight);
        drawSomething();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeLeft);
        drawSomething();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
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

    case '2':
    {   screen.translate(0,-0.5,0);
        cam.updateMonitorPoint(screen);
    }
    break;
    case '8':
    {   screen.translate(0,0.5,0);
        cam.updateMonitorPoint(screen);
    }
    break;
    case '4':
    {   screen.translate(-0.5,0,0);
        cam.updateMonitorPoint(screen);
    }
    break;
    case '6':
    {   screen.translate(0.5,0,0);
        cam.updateMonitorPoint(screen);
    }
    break;

    case 'i':
        interoculardistance+=1;
        break;
    case 'I':
        interoculardistance-=1;
        break;
    case 'w':
        modelZ-=1;
        cerr << modelZ << endl;
        break;
    case 's':
        modelZ+=1;
        cerr << modelZ << endl;
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
{
    eyeLeft  = Vector3d(-interoculardistance/2-30,0,0);
    eyeRight = Vector3d(interoculardistance/2-30,0,0);

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}



int main(int argc, char*argv[])
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalPlane);
    cam.init(screen);


    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode )
    {   glutGameModeString("1024x768:32@60");
        if ( glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
            glutEnterGameMode();
        else
        {   cerr << "This resolution is not available\nPress ENTER to continue" << flush;
            cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
            cleanup();
            return -1;
        }
    }
    else
    {   glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        glutCreateWindow("CNCSVISION Example 2 HappyBuddha");
        glutFullScreen();
    }
    initRendering();

    model.load("../data/objmodels/happyBuddha.obj");
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
