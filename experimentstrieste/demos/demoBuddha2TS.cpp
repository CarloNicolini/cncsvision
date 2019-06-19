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

#include <boost/thread.hpp>

#include "IncludeGL.h"
#ifdef __linux__
#include "beep.h"
#endif

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "Marker.h"
#include "LatestCalibrationTrieste.h"

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

double focalDistance= baseFocalDistance;


/********** EYES AND MARKERS **********************/
Vector3d eyeLeft(0,0,0);
Vector3d eyeRight(0,0,0);
vector <Marker> markers;
double interoculardistance=65;
/********* REAL SCREEN POINTS 28/Febbraio/2011 ****/
Screen screen;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=false;
bool stereo=true;

static const int StillBuddha=0,RotatingBuddha=1,ExpandingBuddha=2,MovingBuddha=3;
int buddhaMode=StillBuddha;
double frame=0.0;
/******* FUNCTIONS ***********************************/
void drawSomething()
{
    switch (buddhaMode)
    {
    case StillBuddha:
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance);
        glScalef(800,800,800);
        model.draw();
        glPopMatrix();
    }
    break;
    case RotatingBuddha:
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance);
        glScalef(800,800,800);
        glRotated(100*frame,0,1,0);
        model.draw();
        glPopMatrix();
    }
    break;
    case ExpandingBuddha:
    {
        double scaleFactor=1+(sin(frame*10));
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance);
        glScalef(400,400,400);
        glScalef(scaleFactor,scaleFactor,scaleFactor);
        model.draw();
        glPopMatrix();
    }
    break;
    case MovingBuddha:
    {
        double movefactor=sin(10*frame);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance+30*movefactor);
        glScalef(800,800,800);
        model.draw();
        glPopMatrix();
    }
    }
}

void cleanup()
{   optotrak.stopCollection();
    for (int i=0; i<3; i++)
        Beep(880,220);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
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
{
    if (stereo)
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

/*void handleKeypress(unsigned char key, int x, int y)
{
   switch (key)
   {     //Quit program
      case 'q':
      case 27:
      {  cleanup();
         exit(0);
      }
      break;
	  case ' ':
		{
			buddhaMode++;
			buddhaMode%=4;
		}
		  break;
   }
}*/

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
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case 'I':
        interoculardistance-=1;
        headEyeCoords.setInterOcularDistance(interoculardistance);
        break;
    case 'w':
        focalDistance-=1;
        cerr << focalDistance << endl;
        break;
    case 's':
        focalDistance+=1;
        cerr << focalDistance << endl;
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
    markers =optotrak.getAllMarkers();
    headEyeCoords.update(markers[1].p,markers[2].p,markers[3].p);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    frame+=1.0/(10*TIMER_MS);

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void recordHeadEyeRelativePositions()
{   bool allVisible=false;
    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers=optotrak.getAllMarkers();
        allVisible= isVisible(markers.at(18).p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
        cerr << "Move the head such that all markers are visible, press 'Enter' to confirm: Trial num " << i << endl;
        cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        if ( allVisible )
        {
            Vector3d lefteyeinit = markers.at(18).p - Vector3d(ChinRestWidth,0,0);
            headEyeCoords.init(lefteyeinit,markers.at(18).p,
                               markers[1].p,markers[2].p,markers[3].p,interoculardistance);
            break;
        }
    }
}

void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void initOptotrak()
{
    optotrak.setTranslation(frameOrigin);
    optotrak.init(LastAlignedFile,TS_N_MARKERS,TS_FRAMERATE,TS_MARKER_FREQ,TS_DUTY_CYCLE,TS_VOLTAGE);

    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}


int main(int argc, char*argv[])
{
    initOptotrak();

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
    {   glutGameModeString(TS_GAME_MODE_STRING);
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
    {
        glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        glutCreateWindow("CNCSVISION Example 2 HappyBuddha");
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    model.load("C:/cncsvisiondata/objmodels/happyBuddha.obj");
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);
    /* Application main loop */
    glutMainLoop();

    cleanup();
    return 0;
}
