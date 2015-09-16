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

#include "IncludeGL.h"
#include "LatestCalibration.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Marker.h"
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace util;
using namespace Eigen;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;
ObjLoader model;

static const double focalDistance= -418.5;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft(0,0,0);
Vector3d eyeRight(0,0,0);
vector <Marker> markers;
double interoculardistance=65;
/********* REAL SCREEN POINTS 28/Febbraio/2011 ****/
Screen screen;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=false;
bool stereo=false;
double modelZ=focalDistance;

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
{   optotrak.stopCollection();
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
    case 'i':
        interoculardistance+=1;
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case 'I':
        interoculardistance-=1;
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case 'w':
        modelZ-=1;
        break;
    case 's':
        modelZ+=1;
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
{   markers = optotrak.getAllMarkers();
    headEyeCoords.update(markers[1].p,markers[2].p,markers[3].p);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void recordHeadEyeRelativePositions()
{   bool allVisible=false;
    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers=optotrak.getAllMarkers();
        allVisible= isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
        cerr << "Move the head such that all markers are visible: Trial num " << i << endl;
        cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        cerr << allVisible << endl;
        cerr << markers[17].p.transpose() << " " << markers[18].p.transpose() << endl;
        if ( allVisible )
        {   headEyeCoords.init(markers[17].p,markers[18].p,
                               markers[1].p,markers[2].p,markers[3].p,interoculardistance);
            break;
        }
    }
}


int main(int argc, char*argv[])
{

    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {   cleanup();
        exit(0);
    }

    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);

    recordHeadEyeRelativePositions();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode )
    {
        glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
    }
    else
    {
        glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        glutCreateWindow("CNCSVISION Example 2 HappyBuddha");
        glutFullScreen();
    }
    initRendering();

    model.load("../../data/objmodels/happyBuddha.obj");
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
