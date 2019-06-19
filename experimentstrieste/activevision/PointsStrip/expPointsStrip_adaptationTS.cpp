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
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "GLText.h"
#include "LatestCalibrationTrieste.h"
//#include "../../../experiments/calibrations/LatestCalibration.h"

/*** NAMESPACE DIRECTIVES ***/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using util::str2num;

/**** CAMERA, EYES EXTRACTOR, OBJECTS ***/
VRCamera cam;
/*** EYES, SCREEN ***/
static const double focalDistance= -418.5;
Screen screen;
static double interoculardistance=65;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/*** OUTPUT STREAMS ***/
ofstream timingFile("time.dat");
ofstream responseFile;

/*** TIMING VARIABLES ***/
double timeFrame=0;
Timer responseTimer;
Timer stimulusTimer;
Timer totalTimer;
Timer frameTimer;
/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane,stripPlane;
StimulusDrawer stimDrawer;
Eigen::Affine3d stimTransformation;
bool allVisibleHead=false;
bool drawInfo=false;
bool experimentStarted=false;
// Stimuli properties
double dotsSize=1;
double ISO=0.0;
int DFixation=0;
int nstrip=0;

// Stimuli and adaptation durations
double fixationDurationInMilliSeconds=1;
double stimulusDurationInMilliSeconds=1000;
double initialAdaptationTimeInMilliSeconds=10.0;
double initialAdaptationFlowIncrement=1.0;

/*** POINTS STRIP VARIABLES ***/
double periodicValue=0.0;
double oscillationAmplitude=1.0;
static const int STRIP_WIDTH=SCREEN_WIDE_SIZE;
static const int STRIP_HEIGHT=SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH;
double stripDisplacementIncrement=0.0;
class Dots
{
public:
    double x,y,totPathLength;
};

std::vector<Dots> pointsField;
std::vector<double> times;
bool useCircularMask=false;
int circularMaskRadius=100;
double maxDotPathLengthInMM=0.0;
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
    INITIALADAPTATION,
    FIXATIONMODE,
    STIMULUSMODE,
    PROBEMODE
} trialMode;

/*** SOUND THINGS ***/
//boost::mutex beepMutex;
void beepOk()
{
    //Beep(440,440);
    return;
}

void clearAndWait(double milliSeconds)
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearStencil(0);
    glClearDepth(1.0);
    glutSwapBuffers();
    Timer waitTimer;
    waitTimer.sleep(milliSeconds);
}

void resetPointStrip()
{
    // Imposta striscia del fixation e drawer
    for( int i = 0; i < nstrip; i++ )
    {
        pointsField[i].x = mathcommon::unifRand(-STRIP_WIDTH/2,STRIP_WIDTH/2);
        pointsField[i].y = mathcommon::unifRand(-STRIP_HEIGHT/2,STRIP_HEIGHT/2);
        pointsField[i].totPathLength = 0.0;
    }
}
/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{
    cout << "======" << endl;
    double average=0.0;
    for (int i=0; i<times.size(); ++i)
    {
        average+=times.at(i);
    }
    average/=times.size();
    if ( trial.isEmpty() )
    {
#ifdef WIN32
        for (int i=0; i<3; i++)
        {
            Beep(220,440);
            totalTimer.sleep(20);
        }
#endif
        exit(0);
    }

    switch (trialMode)
    {
    case FIXATIONMODE:
    {
#ifdef WIN32
        Beep(440,440);
#endif
        totalTimer.start();
        stimulusTimer.start();
        //clearAndWait(2);
        trialMode=STIMULUSMODE;
    }
    break;
    case STIMULUSMODE:
    {
#ifdef WIN32
        Beep(440,440);
#endif
        trialMode=PROBEMODE;
        totalTimer.start();
        stimulusTimer.start();
    }
    break;
    case PROBEMODE:
    {
#ifdef WIN32
        Beep(880,440);
#endif
        resetPointStrip();
        responseTimer.start();
        totalTimer.start();
        trialMode=FIXATIONMODE;
    }
    }
    resetPointStrip();
    factors = trial.getNext();
    trialNumber++;
    responseFile << setw(6) << left <<
                 trialNumber << "\t" <<
                 factors["Tilt"] << "\t"  <<
                 factors["Slant"] << "\t" <<
                 factors["Def"] << "\t" <<
                 factors["FlowDirection"] << "\t" <<
                 factors["FlowIncrement"] << "\t" << endl;

    double responseTime=responseTimer.getElapsedTimeInMilliSec();
    timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in
}


void drawPointsField()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3fv(glRed);
    glPointSize(dotsSize);
    glBegin(GL_POINTS);
    double flowIncr = factors.at("FlowIncrement");
    if ( trialMode == INITIALADAPTATION )
        flowIncr = initialAdaptationFlowIncrement;
    int flowDir = (int)factors.at("FlowDirection");

    for( int i = 0; i < nstrip; i++ )
    {
        switch (flowDir)
        {
        case 0:
        {
            pointsField[i].x += flowIncr;
            //pointsField[i].x+= ( (i%2) ? flowIncr : -flowIncr) ;
            if (flowIncr>0)
            {
                if (pointsField[i].x > STRIP_WIDTH )
                    pointsField[i].x-=2*STRIP_WIDTH;
            }
            else
            {
                if (pointsField[i].x < -STRIP_WIDTH )
                    pointsField[i].x+=2*STRIP_WIDTH;
            }
        }
        break;
        case 1:
            pointsField[i].y += flowIncr;
            //pointsField[i].y+= ( (i%2) ? flowIncr : -flowIncr) ;
            if (flowIncr>0)
            {
                if (pointsField[i].y > STRIP_HEIGHT )
                    pointsField[i].y-=2*STRIP_HEIGHT;
            }
            else
            {
                if (pointsField[i].y < -STRIP_HEIGHT )
                    pointsField[i].y+=2*STRIP_HEIGHT;
            }
            break;
        }
        pointsField[i].totPathLength+=abs(flowIncr);

        if ( pointsField[i].totPathLength > unifRand(0.0,maxDotPathLengthInMM) )
        {
            pointsField[i].y=unifRand(-(double)STRIP_HEIGHT/2, (double)STRIP_HEIGHT/2);
            pointsField[i].x=unifRand(-(double)STRIP_WIDTH/2, (double)STRIP_WIDTH/2);
            pointsField[i].y=unifRand(-(double)STRIP_HEIGHT/2, (double)STRIP_HEIGHT/2);
            pointsField[i].x=unifRand(-(double)STRIP_WIDTH/2, (double)STRIP_WIDTH/2);

            pointsField[i].totPathLength =0.0;
        }
        glVertex3d(pointsField[i].x,pointsField[i].y,focalDistance);
    }
    glEnd();
    //glPopAttrib();
}

void drawRedDotsPlane()
{
    int planeTilt = (int)factors.at("Tilt");
    int planeSlant = (int)factors.at("Slant");
    double planeDef = factors.at("Def");
    double planeOnset=1;
    bool backprojectionActive=true;
    double theta=0.0;
    Eigen::Affine3d modelTransformation;

    glPushMatrix();
    //glLoadIdentity();
    //glTranslated(0,0,focalDistance);
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
    timingFile << theta*180/M_PI << " " << periodicValue << " " << timeFrame << " " << timeFrame/(stimulusDurationInMilliSeconds) << " " << totalTimer.getElapsedTimeInMilliSec() << endl;
    stimDrawer.draw();
    glGetDoublev(GL_MODELVIEW_MATRIX,modelTransformation.matrix().data());
    modelTransformation.matrix() = modelTransformation.matrix().eval();
    glPopMatrix();
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
    // This draws the points field moving
    drawPointsField();

    if ( DFixation ==1 )
    {
        // This draws the central dot to stare
        glPushAttrib(GL_POINT_BIT);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glEnd();
        glPopMatrix();
        glPopAttrib();
    }
}

void drawTrial()
{
    if ( useCircularMask )
        circleMask(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,circularMaskRadius);

    if (!experimentStarted)
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed);
        text.enterTextInputMode();
        text.draw("Press ENTER key to start the experiment");
        text.leaveTextInputMode();
        return;
    }


    if (drawInfo)
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed);
        text.enterTextInputMode();
        text.draw(util::stringify<int>(totalTimer.getElapsedTimeInMilliSec()));
        text.leaveTextInputMode();
    }

    switch ( trialMode )
    {
    case INITIALADAPTATION:
    {
        drawFixation();
        break;
    }
    case FIXATIONMODE:
    {
        drawFixation();
        break;
    }
    case PROBEMODE :
    {   drawProbe();
        break;
    }
    case STIMULUSMODE:
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        drawRedDotsPlane();
        glPopAttrib();
        break;
    }
    }
    glGetError();
}

/**
@brief initVariables()
**/
void initVariables()
{
    totalTimer.start();
    interoculardistance = str2num<double>(parameters.find("IOD"));
    ISO = util::str2num<double>(parameters.find("ISO"));
    DFixation = str2num<int>(parameters.find("DFixation"));
    maxDotPathLengthInMM = util::str2num<double>( parameters.find("MaxDotsPathLengthInMM"));
    drawInfo = (int)util::str2num<int>(parameters.find("DrawInfo"));
    trial.init(parameters);
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1
    useCircularMask = util::str2num<int>( parameters.find("CircularMask")) == 1 ;
    circularMaskRadius = util::str2num<int>(parameters.find("CircularMaskRadius"));
    nstrip = util::str2num<int>(parameters.find("N_STRIP_POINT"));
    pointsField.resize(nstrip);
    fixationDurationInMilliSeconds = util::str2num<double>(parameters.find("AdaptationDurationInMilliSeconds"));
    // Set stimulus and StimulusDrawer
    redDotsPlane.setNpoints(util::str2num<int>(parameters.find("NumStimulusPoints")));
    redDotsPlane.setDimensions(util::str2num<int>(parameters.find("StimulusEdgeLength")),
                               util::str2num<int>(parameters.find("StimulusEdgeLength")),0.1);
    redDotsPlane.compute();
    stimDrawer.setSpheres(false);
    stimDrawer.initList(&redDotsPlane,glRed,dotsSize);

    frameTimer.start();
    resetPointStrip();

    stimulusDurationInMilliSeconds = util::str2num<double>(parameters.find("StimulusDuration"));
    initialAdaptationTimeInMilliSeconds = util::str2num<double>(parameters.find("InitialAdaptationTimeInMilliSeconds"));
    initialAdaptationFlowIncrement = util::str2num<double>(parameters.find("InitialAdaptationFlowIncrement"));
    trialMode = INITIALADAPTATION;
}

void mouseFunc(int button, int state, int _x , int _y)
{
    glutPostRedisplay();
}

void initStreams()
{
    ifstream inputParameters;
    string parametersPassiveFileName("C:/cncsvisioncmake/data/parametersFiles/parametersPointsStrip.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path" << endl;
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);

    string responseFileName = baseDir + "responseFileStripAdapt_" + parameters.find("SubjectName") +".txt";
    responseFile.open(responseFileName.c_str());

    responseFile << setw(6) << left <<
                 "Trial\tTilt\tSlant\tDef\tFlowDir\tFlowIncrement" << endl;
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glClearDepth(1.0);
    glClearStencil(0.0);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // To make the points field with the same properties as the plane
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_BLEND);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3fv(glRed);
}

/**
* @brief drawGLScene
**/
void drawGLScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d(0,0,0));
    drawTrial();
    glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'i':
    {
        drawInfo=!drawInfo;
        break;
    }
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case '2':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '8':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '4':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '6':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case 13:
    {
        if (!experimentStarted )
        {
#ifdef WIN32
            Beep(880,880);
#endif
            trialMode=INITIALADAPTATION;
            totalTimer.start();
            frameTimer.start();
            experimentStarted=true;
        }
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
    //double deltaT=frameTimer.getElapsedTimeInMilliSec();
    //times.push_back(deltaT);
    //frameTimer.start();
    //timeFrame+=deltaT;
    timeFrame=totalTimer.getElapsedTimeInMilliSec();
    periodicValue = timeFrame/1000 - (stimulusDurationInMilliSeconds/(2*1000));


    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
    if (totalTimer.getElapsedTimeInMilliSec() > initialAdaptationTimeInMilliSeconds
            && trialMode == INITIALADAPTATION && experimentStarted )
    {
        clearAndWait(ISO);
        timeFrame=0.0;
        trialMode = STIMULUSMODE;
        totalTimer.start();
    }

    if (totalTimer.getElapsedTimeInMilliSec() > fixationDurationInMilliSeconds && trialMode == FIXATIONMODE && experimentStarted )
    {
        clearAndWait(ISO);
        timeFrame=0.0;
        trialMode=STIMULUSMODE;
        totalTimer.start();
    }

    if (totalTimer.getElapsedTimeInMilliSec() >= stimulusDurationInMilliSeconds && trialMode == STIMULUSMODE && experimentStarted )
    {
        timeFrame=0.0;
        trialMode=PROBEMODE;
        totalTimer.start();
    }

    if (!experimentStarted)
        return;

    stimTransformation.matrix().setIdentity();
    stimTransformation.translation() <<0,0,focalDistance;
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    //screen.setOffset(alignmentX,alignmentY);
    screen.setOffset(0,0);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
}

/**
* @brief main(int argc, char*argv[])
**/
int main(int argc, char*argv[])
{
    mathcommon::randomizeStart();
    initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_STEREO | GLUT_MULTISAMPLE );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH | GLUT_MULTISAMPLE );

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("PointsStrip flow");
        //glutFullScreen();
    }
    else
    {
        glutGameModeString(TS_GAME_MODE_STRING);
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
    glutMainLoop();
    return 0;
}
