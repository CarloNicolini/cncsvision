// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2013 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#endif

#include "Util.h"
#include "Mathcommon.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;

CylinderPointsStimulus frontStimulus,backStimulus;
StimulusDrawer stimDrawerFront,stimDrawerBack;

// Trial related things
ParametersLoader parameters;
/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight;
vector <Marker> markers;
bool allVisibleHead=false;
int headCalibrationDone=0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;
static const double focalDistance=-418.5;
double interocularDistance=65;

/***** SOUND THINGS *****/
#include <boost/thread/mutex.hpp>
boost::mutex beepMutex;
void beepOk()
{
    #ifdef _WIN32
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
#endif
    return;
}

/**
 * @brief drawInfo
 */
void drawInfo()
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);

    switch ( headCalibrationDone )
    {
    case 0:
    {
        if ( allVisibleHead )
            text.draw("PRESS SPACEBAR TO CALIBRATE");
        else
            text.draw("BE VISIBLE...");
        break;
    }
    case 1:
    {
        break;
    }
    }
}

/**
 * @brief initOptotrak
 */
void initOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init("cameraFiles/Aligned20110823") != 0)
    {
        exit(0);
    }

    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
}

/**
 * @brief idle
 */
void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    allVisibleHead = isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
    int argc=0; char **argv;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO);
    glutGameModeString("1024x768:32@100");
    glutEnterGameMode();
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    // MUST BE CALLED WITHIN A VALID OPENGL CONTEXT
    Screen screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH, alignmentX, alignmentY, focalDistance );
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);

    // Generate the frontStimulus
    frontStimulus.setNpoints(150);
    frontStimulus.setAperture(0.0,2*M_PI);
    frontStimulus.setRadiusAndHeight(10,50);
    frontStimulus.setFluffiness(0.001);
    frontStimulus.compute();

    // Generate the back stimulus
    backStimulus.setNpoints(150);
    backStimulus.setAperture(-M_PI/3,M_PI/3);
    backStimulus.setRadiusAndHeight(10,50);
    backStimulus.setFluffiness(0.001);
    backStimulus.compute();

    // Generate the front stimulus
    stimDrawerFront.setSpheres(false);
    stimDrawerFront.setStimulus(&frontStimulus);
    stimDrawerFront.initList(&frontStimulus,glRed,3);

    // Generate the back stimulus
    stimDrawerBack.setSpheres(false);
    stimDrawerBack.setStimulus(&backStimulus);
    stimDrawerBack.initList(&backStimulus,glBlue,3);
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,focalDistance);
    stimDrawerFront.draw();

    glLoadIdentity();
    glTranslated(-50.0,0.0,focalDistance);
    stimDrawerBack.draw();

    glPopMatrix();
}

/**
 * @brief paintGL
 */
void paintGL()
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

/**
 * @brief keyPressEvent
 * @param key
 * @param x
 * @param y
 */
void keyPressEvent(unsigned char key, int x, int y)
{
    switch (key)
    {       //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            headEyeCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
            headCalibrationDone=1;
            beepOk();
        }
        break;
    }
    }
}

/**
 * @brief update
 * @param value
 */
void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char*argv[])
{
    // Initialize the random seed
    randomizeStart();
    cerr << "Insert Interocular distance, then press Enter" << endl;
    cin >> interocularDistance ;

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();

    // Initialize the OpenGL context
    initializeGL();
    initializeExperiment();
    // Setup the callback functions
    glutDisplayFunc(paintGL);
    glutKeyboardFunc(keyPressEvent);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();

}
