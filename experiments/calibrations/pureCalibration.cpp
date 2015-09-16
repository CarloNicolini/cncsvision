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
#include "IncludeGL.h"
/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "VRCamera.h"
#include "GLUtils.h"
#include "GLText.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
//using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam,camPassive;
Vector3d eyeRight,eyeLeft;
static const double focalDistance= -418.5;
static const Vector3d center(0,0,focalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* Timing variables  ************************/

/*** STIMULI and TRIAL variables ***/
Affine3d objectActiveTransformation=Affine3d::Identity();
Affine3d objectPassiveTransformation=Affine3d::Identity();
bool orthographicMode=false;
bool passiveMode=false;

void drawFixation()
{
	glLineWidth(3);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINES);
    glVertex3d(-75,0,focalDistance);
	glVertex3d(75,0,focalDistance);
    glVertex3d(0,75,focalDistance);
    glVertex3d(0,-75,focalDistance);
    glEnd();
	glPopMatrix();
        
}
void idle();

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
}

void drawInfo()
{
	GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glBlack);
	text.enterTextInputMode();
	text.draw("Measure the cross with a ruler, it must be 15 cm");
	text.draw("Measure the white area must be W=310 mm");
	text.leaveTextInputMode();
}

void initRendering()
{   glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        cam.setEye(Vector3d::Zero());
        drawFixation();
		drawInfo();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        cam.setEye(Vector3d::Zero());
		drawFixation();
        drawInfo();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
		drawFixation();
        drawInfo();
        glutSwapBuffers();
    }
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
    {   
		glutLeaveGameMode();
		cleanup();
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
}


int main(int argc, char*argv[])
{

	// Initializes the optotrak and starts the collection of points in background
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("CALIBRATION EXPERIMENT");
        glutFullScreen();
    }
    else
    {
        glutGameModeString(ROVERETO_GAME_MODE_STRING);
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
