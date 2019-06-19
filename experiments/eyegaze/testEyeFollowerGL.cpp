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

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <math.h>
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

#include <boost/filesystem.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "Util.h"
#include "GLText.h"
#include "EyeFollower.h"
#include "GL2DPainter.h"
#include "Timer.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 5
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam,camPassive;
Vector3d eyeRight,eyeLeft;
EyeFollower eyeFollower;
/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
static const double focalDistance= -568.5;
static const Vector3d center(0,0,focalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* Timing variables  ************************/

/*** STIMULI and TRIAL variables ***/
Affine3d objectActiveTransformation=Affine3d::Identity();
Affine3d objectPassiveTransformation=Affine3d::Identity();
bool orthographicMode=false;
bool passiveMode=false;
Eigen::Vector2i gaze;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
}

void drawInfo()
{
	GLText text;
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
    text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
    text.enterTextInputMode();
	text.draw( util::stringify<int>(gaze.x()) + " " + util::stringify<int>(gaze.y()) );
	text.leaveTextInputMode();
}

void drawPixels(int _x, int _y, int _screenwidth, int _screenheight, const GLfloat _color[4], unsigned int _pointSize )
{
	GL2DPainter painter;
	painter.init(_screenwidth,_screenheight);
	painter.begin();
	painter.setColor((_color[0]*255),(_color[1]*255),(_color[2]*255));
	painter.drawPoint(_x,_y,_pointSize);
	painter.end();
}

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/* Set depth buffer clear value */
glClearDepth(1.0);
/* Enable depth test */
glEnable(GL_DEPTH_TEST);
/* Set depth function */
glDepthFunc(GL_LEQUAL);

/** LIGHTS **/
/*
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glEnable (GL_BLEND);
glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
glEnable(GL_LIGHT0);
glEnable(GL_LIGHTING);
*/
/** END LIGHTS **/
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

void drawGLScene()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.setEye(eyeRight);
	
	// Draw pixels center
	drawPixels(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT, glRed,2);
	drawPixels(SCREEN_WIDTH/4,SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT, glRed,2);
	drawPixels(3*SCREEN_WIDTH/4,3*SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT, glRed,2);
	drawPixels(SCREEN_WIDTH/4,3*SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT, glRed,2);
	drawPixels(3*SCREEN_WIDTH/4,SCREEN_HEIGHT/4,SCREEN_WIDTH,SCREEN_HEIGHT, glRed,2);

	drawInfo();
	
	drawPixels(mathcommon::clamp<int>(gaze.x(),0,SCREEN_WIDTH), mathcommon::clamp<int>(gaze.y(),0,SCREEN_HEIGHT),SCREEN_WIDTH,SCREEN_HEIGHT,eyeFollower.isVisible() ? glGreen : glRed,3);
	
	glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
{   //Quit program
case 'o':
	orthographicMode=!orthographicMode;
	cam.setOrthoGraphicProjection(orthographicMode);
	break;
case 'p':
	passiveMode=!passiveMode;
	break;
case 'q':
case 27:
	{   cleanup();
	exit(0);
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

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screen.setOffset(alignmentX,alignmentY);
	screen.setFocalDistance(_focalDist);
	screen.transform(_transformation);
	cam.init(screen);
}

void update(int value)
{	
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
	Timer timer;
	timer.start();

	eyeFollower.querySystem();
	gaze = eyeFollower.getGazePos();
}

int main(int argc, char*argv[])
{
	cout << "Current directory set succeed? " << SetCurrentDirectory("C:\\Eyegaze\\") << endl;
	eyeFollower.init( false,SCREEN_WIDTH,SCREEN_HEIGHT,"SOCKET","192.168.231.179");
	cerr << "init passed" << endl;
// Initializes the optotrak and starts the collection of points in background
glutInit(&argc, argv);
if (stereo)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
else
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

if (gameMode==false)
{   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
glutCreateWindow("EXP EYEFOLLOWER");
glutFullScreen();
}
else
{
	glutGameModeString("1024x768:32@60");
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
glutIdleFunc(idle);
glutMainLoop();

cleanup();

return 0;
}
