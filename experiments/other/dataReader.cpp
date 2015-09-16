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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
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
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "GLText.h" 

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 60
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
bool fullscreen=false;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  0;//-2.5 ;
static const double alignmentY =  0;//20.0;
static const double focalDistance= -418.5;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, fixationPoint, projPoint, fixationPointFull;
vector <Vector3d> markers(18);
Eigen::ParametrizedLine<double,3> pline;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* Timing variables  ************************/
Vector3d eyeCalibration(0,0,0);

char *filename=NULL;

/*************************** FUNCTIONS ***********************************/

void drawText()
{
    ifstream file;

    file.open(filename);

    string line;
    
    GLText text;
    text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed, GLUT_BITMAP_HELVETICA_18);

    text.enterTextInputMode();
    if ( file.good() )
    {
        while ( !file.eof() )
        {
            getline(file,line);
            if ( line.c_str()[0]=='\r' )
                continue;
            text.draw(line);
        }
    }
    else
    {
        text.draw("File " + string(filename) + " doesn't exist, press Q to quit" );
    }
    text.leaveTextInputMode();
}

void mouseFunc(int button, int state, int _x , int _y)
{
    glutPostRedisplay();
}

void initStreams()
{

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
}

void initRendering()
{  glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);

    /** LIGHTS **/
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /** END LIGHTS **/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d::Zero());
    drawText();
    glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y)
{  switch (key)
    {     //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
        break;
    case 'f':
        if ( !fullscreen )
            glutFullScreen();
        else
        {
            glutReshapeWindow (SCREEN_WIDTH,SCREEN_HEIGHT);
            glutPositionWindow(0,0);
        }
        break;
    }
}


void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}


void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);

    cam.init(screen);

}
int main(int argc, char*argv[])
{
    initScreen();
    if ( argv[1]==NULL )
    {
        cerr << "Usage: ./datareader [filename]\nWhere filename is a text file to show" << endl;
        string sfile;
        cerr << "Remember the base dir here is C:\\cygwin\\home\\Fulvio\\VisualStudio\\OpenGL\\OpenGL" << endl;
        cin >> sfile;
        filename = (char*)sfile.c_str();
    }
    else
        filename=argv[1];
    
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
    glutCreateWindow("DATAREADER");
    initRendering();
    initStreams();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(TIMER_MS, update, 0);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
