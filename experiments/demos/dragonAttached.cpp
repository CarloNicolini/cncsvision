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
#include "Optotrak.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 25
//#define HAVE_LIGHT
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
#define SCREEN_WIDE_SIZE 310.0    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak optotrak;
CoordinatesExtractor headEyeCoords;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const double m_xc = -419.5;
static const double m_yc = 500.0;
static const double m_zc = 440.0;
static const Vector3d calibration(m_xc, m_yc, m_zc);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
static const double focalPlane= -418.5;
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight;
vector <Vector3d> markers;
double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=true;
bool stereo=true;
bool gridshown=false;
int nAvanti=0;
int nDietro=0;
int frame=0;

/** GRASPING TASK RELATED VARIABLES **/
Vector3d translation;
Vector3d p1,p2,p3,p4;
Vector3d p1t,p2t,p3t;
RigidBody rigidStart,rigidCurrent;

/*** CURVES RELATED VARIABLES **/
Vector3d controlPoint1, controlPoint2;
ObjLoader model;

/******* FUNCTIONS ***********************************/
void drawSomething()
{   frame++;

    glPushMatrix();
    glLoadIdentity();
    glTranslated(markers[14].x(),markers[14].y(),markers[14].z());
    glutSolidSphere(1,10,20);
    glPopMatrix();
    /*********  Draw white cubes cube with edge=100,100,20 millimeters *********/
    double edge=50;
    for (int i=-nAvanti ; i<nDietro; i++)
    {   glPushMatrix();
        glTranslatef(0,0.0, focalPlane + i*edge);
        glColor3fv(glWhite);
        glutWireCube(edge);
        glPopMatrix();
    }

    glPushMatrix();
    glColor3fv(glWhite);
    glLineWidth(1);
    glMultMatrixd( rigidStart.getFullTransformation().data() );
    glutWireCube(60);
    glScalef(400,400,400);
    glRotated(90,0,1,0);
    model.draw();
    glPopMatrix();

    if (gridshown)
    {   glPushMatrix();
        glColor3fv(glWhite);
        glTranslatef(0.0,0.0,focalPlane);
        float cellsize=20.0;
        float cubesize=100;
        if ( gridshown )
        {   glBegin (GL_LINES);
            for ( float i1 = -100; i1 <= 100; i1+=cellsize )
            {   glVertex3f (-100, i1, 0);
                glVertex3f (100, i1, 0);
                glVertex3f (i1, -100, 0);
                glVertex3f (i1, 100, 0);
            }
            glEnd ();
            glTranslatef(0,0,-50);
            glBegin (GL_LINES);
            for ( float i1 = -100; i1 <= 100; i1+=cellsize )
            {   glVertex3f (-100, i1, 0);
                glVertex3f (100, i1, 0);
                glVertex3f (i1, -100, 0);
                glVertex3f (i1, 100, 0);
            }
            glEnd ();
            glTranslatef(0,0,100);
        }
        glPopMatrix();
    }
    /******* Draw ortographic point **********/
    glDisable(GL_BLEND);


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

#ifdef HAVE_LIGHT
    //Enable a single OpenGL light.
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    TRANSLUCENT
#endif
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    controlPoint1 = Vector3d(0.0,0,0.0);
    controlPoint2 = Vector3d(100,100.0,100.0);

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

    case 'g':
        gridshown=!gridshown;
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

    case '+':
        nAvanti++;
        nDietro++;
        break;
    case '-':
        nAvanti--;
        nDietro--;
        break;
    case 'i':
        interoculardistance+=1;
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case 'I':
        interoculardistance-=1;
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case ' ':
    {   controlPoint2 = Vector3d(unifRand(-100.0,100.0),unifRand(-100.0,100.0),unifRand(-100.0,100.0));
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
    rigidCurrent.setRigidBody(markers[11], markers[12],markers[13]);
    rigidStart.computeTransformation(rigidCurrent);
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void recordHeadEyeRelativePositions()
{   bool allVisibleHead=false;
    bool allVisibleFinger=false;
    Vector3d centroid;
    for (int i=0; i<10; i++)
    {   optotrak.updatePoints();
        markers=optotrak.getAllPoints();
        allVisibleHead= isVisible(markers[17]) && isVisible(markers[18]) && isVisible(markers[1]) && isVisible(markers[2]) && isVisible(markers[3]) ;
        centroid = ( markers[11]+markers[12]+markers[13] )/3;
        allVisibleFinger= isVisible(markers[11]) && isVisible(markers[12]) && isVisible(markers[13]) ;

        cerr << "Move the head and the finger such that all markers are visible: Press Enter to continue " << i << endl;
        cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        if ( allVisibleHead && allVisibleFinger )
        {   headEyeCoords.init(markers[17],markers[18],
                               markers[1],markers[2],markers[3],interoculardistance);
            rigidStart.setRigidBody(markers[11]-centroid,markers[12]-centroid,markers[13]-centroid);
            break;
        }
    }
}

int main(int argc, char*argv[])
{   optotrak.setTranslation(calibration);
    if ( optotrak.init(18,NULL,NULL,"optolog.dat") != 0)
    {   cleanup();
        exit(0);
    }

    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalPlane);
    cam.init(screen);
    model.load("../data/objmodels/dragon.obj");

    recordHeadEyeRelativePositions();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode )
    {   glutGameModeString("1024x768:32");
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
        glutCreateWindow("==== CNCSVISION Stereo Example 1 ====");
    }

    initRendering();

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
