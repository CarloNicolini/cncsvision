// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2012 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <string>

#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <boost/circular_buffer.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "Util.h"
#include "TrialGenerator.h"
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, indexCoords;

static const Vector3d rodTipHome(-73.6,327.82,-386.2);

static const double focalDistance= -418.5;
static double interoculardistance=65;
static const Vector3d center(0,0,focalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight,realIndex, physicalRigidBodyTip,  platformIndex;
vector <Marker> markers;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/*** Streams File ***/
//ofstream markersFile;
//ofstream responseFile;
const int BUFFER_CAPACITY=100;
/*** STIMULI and TRIAL variables ***/
int headCalibrationDone=0, fingerCalibrationDone=0, platformCalibrationDone=0;
bool allVisibleHead=false, allVisiblePatch=false,allVisibleIndex=false, allVisibleFingers=false, allVisiblePlatform=false;
bool passiveMode=false;
int zState=0;
ParametersLoader parameters;

// To display funny points strips!
//boost::circular_buffer<Vector3d> thumbBuffer(BUFFER_CAPACITY);
boost::circular_buffer<Vector3d> indexBuffer(BUFFER_CAPACITY);

#ifdef _WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}
#endif

void cleanup()
{   // Stop the optotrak (Optotrak class should be RAII but for every evenience we stop it...)
    optotrak.stopCollection();
    for (int i=0; i<3; i++)
        Beep(880,220);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void drawInfo()
{   switch ( headCalibrationDone )
    {   case 0:
        {   allVisibleHead = isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
            if ( allVisibleHead )
                glClearColor(0.0,1.0,0.0,1.0); //green light
            else
                glClearColor(1.0,0.0,0.0,1.0); //red light
        }
        break;
        case 1:
        {   
		if ( fingerCalibrationDone<2 )
		{
			GLText text;
			text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
            text.enterTextInputMode();
			if (fingerCalibrationDone==0)
				text.draw("Press 'F' to calibrate platform markers");
            if ( fingerCalibrationDone==1)
				text.draw("Now calibrate thumb and index against platform markers");

			text.draw("EyeLeft= " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()));
            text.draw("EyeRight= " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()));
			text.draw("Index= " + stringify<int>(realIndex.x() ) + " " + stringify<int>(realIndex.y() ) + " " + stringify<int>(realIndex.z()));
			text.leaveTextInputMode();
		}
		}
        break;
    }
    // end if ( headCalibrationDone )
}

void drawTrial()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POINT_BIT );
	glBegin(GL_POINTS);
	glColor3fv(glRed);
	glPushMatrix();
	for (int i=0; i< BUFFER_CAPACITY; i++)
	{
		glLoadIdentity();
		glTranslated(indexBuffer[i].x(),indexBuffer[i].y(),indexBuffer[i].z());
		switch (zState)
		{
		case 0:
			{
				glTranslated(0,0,0);
				break;
			}
		case 1:
			{
				glTranslated(0,0,-100);
				break;
			}
		}
		glutSolidSphere(2.0,10,10);
	}
	glPopMatrix();
	glEnd();
	glPopAttrib();	
}

void initVariables()
{
	for (int i=0; i< BUFFER_CAPACITY; i++)
		indexBuffer.push_back(realIndex);
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

    /** END LIGHTS **/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {       //Quit program
        case 'q':
        case 27:
        {   cleanup();
            exit(0);
        }
        break;
		case ' ':
        {   // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
            if ( headCalibrationDone==0 && allVisibleHead )
            {   headEyeCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
                headCalibrationDone=1;
                beepOk();
            }
			break;
        }
        break;

        case 'f':
        case 'F':
        {   // Here we record the finger tip physical markers
            if ( allVisiblePlatform && (fingerCalibrationDone==0) )
            {   platformIndex=markers.at(16).p;
                fingerCalibrationDone=1;
                beepOk();
                break;
            }
			if ( (fingerCalibrationDone==1) && allVisibleIndex )
            {   indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
                fingerCalibrationDone=3;
                beepOk();
				RoveretoMotorFunctions::homeObjectAsynchronous(3500);
                break;
            }	
        }
        break;
		case 'a':
		{
			zState++;
			zState=zState%2;
			break;
		}
}
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
	RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,-418.5,3500);
}

void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();

	allVisiblePlatform = isVisible(markers.at(16).p);
    
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p);

	// update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p);
	
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

	realIndex = indexCoords.getP1();

	indexBuffer.push_back(realIndex);
}

void initOptotrak()
{   optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {   cleanup();
        exit(0);
    }

    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}

int main(int argc, char*argv[])
{
	RoveretoMotorFunctions::homeScreen(3500);
	RoveretoMotorFunctions::homeObject(3500);
	RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,3500);
	randomizeStart();

// Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP MIRROR FINGER");
        glutFullScreen();
    }
    else
    {   glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    initVariables();

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
