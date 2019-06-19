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
#include <stdexcept>
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

/*** INCLUDE BOOST HEADERS ***/
#include <boost/filesystem.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "GLText.h"
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "EulerExtractor.h"
#include "LatestCalibration.h"

/*** NAMESPACE DIRECTIVES ***/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using util::str2num;
using namespace boost::filesystem;

/**** CAMERA, EYES EXTRACTOR, OBJECTS ***/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
Optotrak2 optotrak;
EulerExtractor eulerAngles;

/*** EYES, SCREEN AND MARKERS ***/
static const double focalDistance= -418.5;
Screen screen;
vector <Marker> markers(18);
Vector3d eyeLeft, eyeRight, projPoint,eyeCalibration;
static double interoculardistance=65;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );
int headCalibrationDone=0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/*** OUTPUT STREAMS ***/
ofstream debugFile("debug.txt");

/*** TIMING VARIABLES ***/
double timeFrame=0;
Timer responseTimer;
Timer stimulusTimer;
Timer totalTimer;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane,stripPlane;
StimulusDrawer stripDrawer,stimDrawer;
Eigen::Affine3d stimTransformation;
bool allVisibleHead=false;
enum StimulusMotion
{
    SAWTOOTH_MOTION,
    TRIANGLE_MOTION,
    SINUSOIDAL_MOTION
} stimMotion;

/*** POINTS STRIP VARIABLES ***/
double periodicValue=0.0;
double oscillationAmplitude=1.0;
static const  int N_STRIP_POINTS=1E4;
static const int STRIP_WIDTH=1E4;
static const int STRIP_HEIGHT=300;
double stripDisplacementIncrement=0.0;

/*** USER INTERACTION  VARIABLES **/
double probeAngle=0;
double probeStartingAngle=0;

/*** TRIAL VARIABLES ***/
int trialNumber=0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
enum TrialMode
{
    FIXATIONMODE,
    STIMULUSMODE,
    PROBEMODE
} trialMode;

/*** SOUND THINGS ***/
boost::mutex beepMutex;
void beepOk()
{
    boost::mutex::scoped_lock lock(beepMutex);
   #ifdef _WIN32
    Beep(440,440);
    #endif
    return;
}

void clearAndWait(double milliSeconds)
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glutSwapBuffers();
    Timer waitTimer;
    waitTimer.sleep(milliSeconds);
}

void drawCircle(double radius, double x, double y, double z, const GLfloat *color)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor3fv(color);
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glPopAttrib();
}


/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{
    switch (trialMode)
    {
    case FIXATIONMODE:
    {
        stimulusTimer.start();
        clearAndWait(2);
        trialMode=STIMULUSMODE;
    }
    break;
    case STIMULUSMODE:
    {
        trialMode=PROBEMODE;
        stimulusTimer.start();
    }
    break;
    case PROBEMODE:
    {
        responseTimer.start();
        trialMode=FIXATIONMODE;
    }
    }
    factors = trial.getNext();
}

void keyPressed()
{
    // Reset the translation of strip
    stripDisplacementIncrement = 0;
    double responseTime=responseTimer.getElapsedTimeInMilliSec();
    timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in
    advanceTrial();

    // Imposta stimolo e drawer
    redDotsPlane.setNpoints(75);
    redDotsPlane.setDimensions(50,50,0.1);
    redDotsPlane.compute();
    stimDrawer.initList(&redDotsPlane,glRed,2);
}

void drawPointsField()
{
    stripDisplacementIncrement+=factors.at("FlowXIncrement");
    glPushMatrix();
    glLoadIdentity();
    glTranslated(stripDisplacementIncrement,0,focalDistance);
    stripDrawer.draw();
    glPopMatrix();
}

void drawInfo()
{
    if (headCalibrationDone > 2 )
        return;

    GLText text;
    text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
    text.enterTextInputMode();
    switch ( headCalibrationDone )
    {
    case 0:
    {   allVisibleHead = isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
        if ( allVisibleHead )
            glClearColor(0.0,1.0,0.0,1.0); //green light
        else
            glClearColor(1.0,0.0,0.0,1.0); //red light
    }
    break;
    case 1:
    case 2:
        //case 3:
    {
        text.draw("EL  " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()) );
        text.draw("ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()) );
        text.draw("PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
        text.draw("YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
        text.draw("ROLL " + stringify<int>(toDegrees(eulerAngles.getRoll())));
        text.draw("Press SPACEBAR to calibrate again or ENTER to confirm calibration.");
        text.draw("Delta " + stringify<int>(eyeRight.z()- eyeCalibration.z()));
        Vector3d angles = headEyeCoords.getRigidStart().getFullTransformation().rotation().eulerAngles(0,1,2);
        text.draw("YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
        text.draw("PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
        text.draw("GlassesL" + stringify<int>(markers[17].p.x() ) + " " + stringify<int>(markers[17].p.y() ) + " " + stringify<int>(markers[17].p.z()) );
        text.draw("GlassesR" + stringify<int>(markers[18].p.x() ) + " " + stringify<int>(markers[18].p.y() ) + " " + stringify<int>(markers[18].p.z()) );
        text.draw("X" + stringify<Eigen::RowVector3i>(markers[18].p.transpose().cast<int>() ));
    }
    break;
    }
    text.leaveTextInputMode();
}


void drawCalibration()
{
    static int timeInsideCircle=0;
    glPushAttrib(GL_POINT_BIT | GL_COLOR_BUFFER_BIT );
    double circleRadius=5.0;
    glColor3fv(glRed);
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    /*
    // Draw the calibration circle
    if ( pow(mirror[2].x(),2)+pow(mirror[2].y(),2) <= circleRadius*circleRadius && abs(eyeRight.z()) < str2num<double>(parameters.find("MaxZOscillation")) )
    {   timeInsideCircle++;

    drawCircle(circleRadius,0,0,focalDistance,glGreen50);
    if ( timeInsideCircle > 20 )
    {
    trialMode=FIXATIONMODE;
    boost::thread okBeep( beepOk );
    okBeep.detach();
    sumOutside=0;
    signs.front()=false;	// XXX
    signs.back()=true;	// XXX
    timeInsideCircle=0;
    }
    else
    timeInsideCircle=0;
    }
    else
    {
    drawCircle(circleRadius,0,0,focalDistance,glRed);
    }
    */
    glPopAttrib();
}

void drawRedDotsPlane()
{
    //drawPointsStripFlow();
    int planeTilt = (int)factors.at("Tilt");
    int planeSlant = (int)factors.at("Slant");
    double planeDef = factors.at("Def");
    double planeOnset=1;
    bool backprojectionActive=true;
    double theta=0.0;
    Eigen::Affine3d modelTransformation;

    glPushMatrix();
    glLoadIdentity();
    glMultMatrixd(stimTransformation.data());
    switch ( planeTilt )
    {
    case 0:
    {
        theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
        glRotated(toDegrees(theta),0,1,0);
        if (backprojectionActive)
            glScaled(1/sin(toRadians( -90-planeSlant)),1,1);	//backprojection phase
    }
    break;
    case 90:
    {
        theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
        glRotated( toDegrees(theta),1,0,0);
        if (backprojectionActive)
            glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
    }
    break;
    case 180:
    {
        theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
        glRotated(toDegrees(theta),0,1,0);
        if (backprojectionActive)
            glScaled(1/sin(toRadians( -90-planeSlant )),1,1); //backprojection phase
    }
    break;
    case 270:
    {
        theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
        glRotated( toDegrees(theta),1,0,0);
        if (backprojectionActive)
            glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
    }
    break;
    default:
        cerr << "Error, select tilt to be {0,90,180,270} only" << endl;
    }
    stimDrawer.draw();
    glGetDoublev(GL_MODELVIEW_MATRIX,modelTransformation.matrix().data());
    modelTransformation.matrix() = modelTransformation.matrix().eval();
    glPopMatrix();
    //cout << modelTransformation.matrix() << endl;
}

void drawProbe()
{
    glPushAttrib(GL_POINT_BIT);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    glPopAttrib();
}

void drawFixation()
{

    drawPointsField();
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glColor3fv(glRed);
    glutSolidSphere(1,20,20);
    glPopMatrix();
    glPopAttrib();
}

void drawTrial()
{
    switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();
    }
    break;
    case PROBEMODE :
    {   drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
    }
    break;
    }
}

void initVariables()
{
    optotrak.init(LastAlignedFile);

    totalTimer.start();
    interoculardistance = str2num<double>(parameters.find("IOD"));
    trial.init(parameters);
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1

    // Imposta stimolo e drawer
    redDotsPlane.setNpoints(75);
    redDotsPlane.setDimensions(50,50,0.1);
    redDotsPlane.compute();
    stimDrawer.initList(&redDotsPlane,glRed);

    // Imposta striscia del fixation e drawer
    stripPlane.setNpoints(N_STRIP_POINTS);
    stripPlane.setDimensions(STRIP_WIDTH,STRIP_HEIGHT,0);
    stripPlane.compute();
    stripDrawer.initList(&stripPlane,glRed);

    stimMotion=SINUSOIDAL_MOTION;

    headEyeCoords.init(Vector3d(-32.5,0,0),Vector3d(32.5,0,0), Vector3d(0,0,0),Vector3d(0,10,0),Vector3d(0,0,10),interoculardistance );
    eyeCalibration=headEyeCoords.getRightEye();
}

void mouseFunc(int button, int state, int _x , int _y)
{
    if ( trialMode == PROBEMODE )
    {
    }

    glutPostRedisplay();
}

void initStreams()
{
    ifstream inputParameters;
    string parametersPassiveFileName("paramsPointsStrip.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);
}

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d(interoculardistance/2,0,0));
    drawInfo();
    drawTrial();
    glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y)
{   
    switch (key)
    {   //Quit program
    case ' ':
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        switch ( headCalibrationDone )
        {
        case 0:
        {
            if (allVisibleHead )
            {
                headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
                headCalibrationDone=1;
                boost::thread beepThread(beepOk);
            }
            break;
        }
        // Second calibration, you must look a fixed fixation point
        case 1:
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=2;
            break;
        }
        case 2:
        {   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            break;
        }
        }

        break;
    case 13:
    {
        if ( headCalibrationDone == 2 && abs(eyeRight.z()) <= str2num<double>(parameters.find("MaxZOscillation")))
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=3;
        }
    }
    break;
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case '2':
    {
    }
    break;
    case '8':
    {
    }
    break;
    case '4':
    {
    }
    break;
    case '6':
    {
    }
    break;
    case '+':
    {
        keyPressed();
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

void idle()
{
    Timer frameTimer;
    frameTimer.start();
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    // Timing things
    timeFrame+=1;

    headEyeCoords.update(markers[1].p,markers[2].p,markers[3].p);
    Affine3d active = headEyeCoords.getRigidStart().getFullTransformation();

    eulerAngles.init( active.rotation() );
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    Vector3d fixationPoint = (active * ( Vector3d(0,0,focalDistance) ) );
    // Projection of view normal on the focal plane
    Eigen::ParametrizedLine<double,3> pline = Eigen::ParametrizedLine<double,3>::Through(eyeRight,fixationPoint);
    projPoint = pline.intersection(focalPlane)*((fixationPoint - eyeRight).normalized()) + eyeRight;

    stimTransformation.matrix().setIdentity();
    stimTransformation.translation() <<0,0,focalDistance;

    Timer sleepTimer;
    sleepTimer.sleep((TIMER_MS - frameTimer.getElapsedTimeInMilliSec())/2);
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
    mathcommon::randomizeStart();
    initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("PointsStrip flow");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();

    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
