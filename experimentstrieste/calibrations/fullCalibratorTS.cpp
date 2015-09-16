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
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

#include "IncludeGL.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
//#include "Optotrak.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "Util.h"
#include "VmxLinearMotor.h"
#include "LatestCalibrationTrieste.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
//using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor2 headEyeCoords;

// Alignment between optotrak z axis and screen z axis
double tmpAlignmentX =  0;
double tmpAlignmentY =  0;

double focalDistance= baseFocalDistance;
static const Vector3d center(0,0,focalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, translationFactor;
vector <Marker> markers(20);
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool isDrawInfo=true;
/********* TRIAL VARIABLES *********/
static const int CALIBRATIONMODE=0;
static const int CUBEMODE=1;
int trialMode = CALIBRATIONMODE;

void drawCircle(double radius, double x, double y, double z)
{
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
}

/*************************** FUNCTIONS ***********************************/
void cleanup()
{
    // Stop the optotrak
    optotrak->stopCollection();
    delete optotrak;
}

void drawInfo()
{
    if ( isDrawInfo )
    {
        GLText text;

        if ( gameMode )
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
        else
            text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
        text.enterTextInputMode();

        switch ( headCalibrationDone )
        {
        case 0:
        {
            if ( allVisibleHead )
                text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
            else
                text.draw("Be visible with the head and glasses");
        }
        break;
        case 1:
        case 2:
        {
            if ( allVisiblePatch )
                text.draw("Move the head in the center");
            else
                text.draw("Be visible with the patch");
        }
        break;
        }

        text.draw("Eye Right");
        text.draw( stringify< Eigen::Matrix<int,1,3> > (eyeRight.transpose().cast<int>())+ " [mm]" );
        text.draw("Eye Left");
        text.draw( stringify< Eigen::Matrix<int,1,3> > (eyeLeft.transpose().cast<int>())+ " [mm]" );

        text.draw("Head Yaw,Pitch,Roll");
        text.draw("Yaw= " + stringify<int>(mathcommon::toDegrees(headEyeCoords.getYaw()) ));
        text.draw("Pitch= " + stringify<int>(mathcommon::toDegrees(headEyeCoords.getPitch())));
        text.draw("Roll= " + stringify<int>(mathcommon::toDegrees(headEyeCoords.getRoll()) ));

        text.draw(stringify< Eigen::Matrix<double,1,3> > (markers[6].p.transpose().cast<double>())+ " [mm]" );
        text.draw("Alignment XY");
        text.draw(stringify<double>(tmpAlignmentX)+", "+stringify<double>(tmpAlignmentY));

        text.draw("Marker 15 (" + stringify<double>(markers[15].p.x()) + ", " + stringify<double>(markers[15].p.y()) + ", " + stringify<double>(markers[15].p.z()));

        text.leaveTextInputMode();
    }
}

// The circle here is needed to evaluate the width/height/centering correctness
void drawCalibration()
{
    // Draw the little circles
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3fv(glWhite);
    drawCircle(2,0,0,focalDistance);	// center
    drawCircle(2,50,0,focalDistance);	// right
    drawCircle(2,-50,0,focalDistance);	// left
    drawCircle(2,0,50,focalDistance);	// high
    drawCircle(2,0,-50,focalDistance);	// low

    // Draw a red circle 5 cm radius in the center
    glColor3fv(glRed);
    drawCircle(50,0,0,focalDistance);

    // Draw the marker 15
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex3dv(markers[15].p.data());
    glEnd();
    glPopAttrib();
}

// The cube here is needed to evaluate the depth correctness
void drawCube()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Draw a 5x5x5 edge cube in the center
    glColor3fv(glRed);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutWireCube(50);
    glPopMatrix();

    // Draw the marker 15
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex3dv(markers[15].p.data());
    glEnd();

    glPopAttrib();
}


void drawTrial()
{   switch ( trialMode )
    {
    case CALIBRATIONMODE:
    {
        drawCalibration();
    }
    break;
    case CUBEMODE:
    {
        drawCube();
    }
    break;
    }
}

void drawGLScene()
{
    if (stereo)
    {
        glDrawBuffer(GL_BACK);
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
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        cleanup();
        exit(0);
    }
    break;
    case 'i':
    case 'I':
    {
        isDrawInfo=!isDrawInfo;
    }
    break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            Vector3d leftChinRest = markers[18].p - Vector3d(ChinRestWidth,0.0,0.0);
            Vector3d rightChinRest= markers[18].p;
            headEyeCoords.init(leftChinRest,rightChinRest, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=1;
            break;
        }
        // Second calibration, you must look a fixed fixation point try to be in the (0,0,0)
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=2;
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=3;
        }
    }
    break;
    case '+':
    {
        trialMode++;
        trialMode=trialMode%2;
    }
    break;
    case '2':
    {
        tmpAlignmentY-=0.25;
        screen.translate(0,-0.25,0);

    }
    break;
    case '8':
    {
        tmpAlignmentY+=0.25;
        screen.translate(0,0.25,0);
    }
    break;
    case '4':
    {
        tmpAlignmentX-=0.25;
        screen.translate(-0.25,0,0);
    }
    break;
    case '6':
    {
        tmpAlignmentX+=0.25;
        screen.translate(0.25,0,0);
    }
    break;
    }
    cam.updateMonitorPoint(screen);
}


void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(tmpAlignmentX,tmpAlignmentY);
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
    optotrak->updateMarkers();
    markers = optotrak->getAllMarkers();
    // Coordinates picker
    allVisiblePatch = isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p);
    allVisibleHead = allVisiblePatch && isVisible(markers[18].p);

    if ( allVisiblePatch )
        headEyeCoords.update(markers[1],markers[2],markers[3],(double)TIMER_MS);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;
}

void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(frameOrigin);
    optotrak->init(LastAlignedFile,TS_N_MARKERS,TS_FRAMERATE,TS_MARKER_FREQ,TS_DUTY_CYCLE,TS_VOLTAGE);
    optotrak->updateMarkers();
    markers = optotrak->getAllMarkers();
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
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

int main(int argc, char*argv[])
{
    cout << "Please insert your IOD (typically around 65 mm) and press Enter: IOD= ";
    cin >> interoculardistance;

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {
        glutInitWindowSize( 640,480 );
        glutCreateWindow("EXP WEXLER");
    }
    else
    {
        glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
