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


/**** BOOOST MULTITHREADED LIBRARY *********/
//#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"


/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using util::str2num;
using util::fileExists;
using namespace boost::filesystem;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
CalibrationHelper calibratorHelper;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  0;//-2.5 ;
static const double alignmentY =  0;//20.0;
static const double focalDistance= -418.5;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, fixationPoint, projPoint, fixationPointFull;
vector <Vector3d> markers(18);
static double interoculardistance=65;
Eigen::ParametrizedLine<double,3> pline;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* Timing variables  ************************/
// Timing variables
double timeFrame=0;
double oldvariable=0;
double variable =0;
//double BASEPERIOD=1000; // is the period of triangular wave
int stimulusEmiCycles= 0;
/*** Streams File ***/
ofstream transformationFile;
ofstream trialFile;
ofstream anglesFile;
ofstream eyePosFile;
ofstream responseFile;

ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
Vector3d eyeCalibration(0,0,0);
bool allVisibleHead=false;
double circleRadius=85;

// Trial related things
int trialNumber=0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
bool conditionInside=true;
bool wasInside=true;
deque<bool> signsX;
deque<bool> signsY;
int sumOutside=-1;

/***** SOUND THINGS *****/
//boost::mutex beepMutex;
void beepOk()
{
    //boost::mutex::scoped_lock lock(beepMutex);
    //Beep(440,440);
    return;
}

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
int trialMode = FIXATIONMODE;

/********** CONTROLS AND DEVICES **********/
int mouseButton=-1;
Timer responseTimer;
Timer stimulusTimer;
/** EXPERIMENTAL VARIABLES **/
string subjectName;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
    transformationFile.close();
    trialFile.close();
    anglesFile.close();
}

/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{   trialMode++;
    trialMode=trialMode%3;
    if (trialMode==PROBEMODE)
        responseTimer.start();
    if ( trialMode!=PROBEMODE)
        stimulusTimer.start();
}

void drawCircle()
{
    glBegin(GL_LINE_LOOP);
    for (int angle=0; angle<360; angle+=5)
    {
        float angle_radians = angle * (float)3.14159 / (float)180;
        float x =  circleRadius * (float)cos(angle_radians);
        float y =  circleRadius * (float)sin(angle_radians);
        glVertex3f(x,y,focalDistance);
    }
    glEnd();
}

void drawRedDotsPlane()
{
    // Draw the stimulus ( red-dots plane )
    bool isInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius);

    glPushMatrix();
    // Reset every previous transformation, put the stimulus in (0,0,0) with no rotation ( fronto-parallel )
    glLoadIdentity();
    if ( (int) factors["Anchored"]==0 )
        glTranslated(0,0,fixationPoint.z());
    else
        glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());
    //double k=0.34657;

    double theta = 0.0;
    double angle = 0.0;

    switch ( (int)factors["Tilt"] )
    {
    case 0:
        glRotated( -90-factors["Slant"] - variable*factors["RotationSpeed"], 0,1,0 );
        break;
    case 90:
        glRotated( -90+factors["Slant"] - variable*factors["RotationSpeed"], 1,0,0 );
        break;
    case 180:
        glRotated( -90+factors["Slant"] - variable*factors["RotationSpeed"], 0,1,0 );
        break;
    case 270:
        glRotated( -90-factors["Slant"] - variable*factors["RotationSpeed"], 1,0,0 );
        break;
    }

    if (isInside )
        stimDrawer.draw();
    glPopMatrix();

    // Draw fixation (Always @ full-speed)
    if ( isInside )
    {
        glColor3fv(glRed);
        glPushMatrix();
        if ( (int) factors["Anchored"]==0 )
            glTranslated(0,0,fixationPoint.z());
        else
            glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());
        glutSolidSphere(1,10,10);
        glPopMatrix();
    }

    //drawCircle();
}


void drawProbe()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    glColor3fv(glWhite);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3f(0,0,focalDistance);
    glEnd();
    glColor3fv(glRed);
    glPointSize(1);

    //drawCircle();
}

void drawFixation()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    /** DRAW THE FIXATION POINT **/
    glColor3fv(glRed);
    glPushMatrix();
    if ( (int) factors["Anchored"]==0 )
        glTranslated(0,0,fixationPoint.z());
    else
        glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());

    glutSolidSphere(1,10,10);
    glPopMatrix();

    //drawCircle();
}

void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();
        //drawOcclusion();
    }
    break;
    case PROBEMODE :
    {   drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
        //drawOcclusion();
    }
    break;
    default:
    {
        drawFixation();
        //drawOcclusion();
    }
    }
}

void initVariables()
{

    interoculardistance = str2num<double>(parameters.find("IOD"));
    stimulusEmiCycles= atoi(parameters.find("StimulusEmiCycles").c_str());
    trial.init(parameters);
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1

    redDotsPlane.setNpoints(75);  //XXX controllare densita di distribuzione dei punti
    redDotsPlane.setDimensions(50,50,0.1);
    redDotsPlane.setSlantTilt(factors["Slant"], (int) factors["Tilt"] );
    redDotsPlane.compute();
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane);

    /** Bound check things **/
    signsX.push_back(false);
    signsX.push_back(false);

    signsY.push_back(false);
    signsY.push_back(false);

    headEyeCoords.init(Vector3d(-32.5,0,0),Vector3d(32.5,0,0), Vector3d(0,0,0),Vector3d(0,10,0),Vector3d(0,0,10),interoculardistance );
    eyeCalibration=headEyeCoords.getRightEye();
}

void mouseFunc(int button, int state, int _x , int _y)
{
    if ( trialMode == PROBEMODE )
    {
        glutWarpPointer(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

        if ( button == GLUT_LEFT_BUTTON && (state==GLUT_DOWN) )
            mouseButton=1;
        if ( button == GLUT_RIGHT_BUTTON && (state==GLUT_DOWN) )
            mouseButton=-1;

        if( (button== GLUT_LEFT_BUTTON || button==GLUT_RIGHT_BUTTON) && (state==GLUT_UP) && (mouseButton!=0) )
        {
            double responseTime=responseTimer.getElapsedTimeInMilliSec();
            double clickDelay=100;	//milliseconds before the click is valid
            if ( responseTime > clickDelay )
            {
                timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in
                if ( trial.isEmpty() )
                {
                    responseFile << setw(6) << left <<  trialNumber << " " << factors["Def"] <<" " << factors["Tilt"] <<" " << factors["Slant"] << " " << factors["Anchored"] << " " << factors["RotationSpeed"] << " " << factors["FollowingSpeed"] << " " << factors["Onset"] << " " << (focalDistance - eyeCalibration.z()) << " " << mouseButton << " " << responseTime << endl;
                    cleanup();
                    exit(0);
                }
                else
                {
                    responseFile << setw(6) << left <<  trialNumber << " " << factors["Def"] <<" " << factors["Tilt"] <<" " << factors["Slant"] << " " << factors["Anchored"] << " " << factors["RotationSpeed"] << " " << factors["FollowingSpeed"] << " " << factors["Onset"] << " " << (focalDistance - eyeCalibration.z()) << " " << mouseButton << " " << responseTime << endl;
                    factors = trial.getNext();
                    trialNumber++;

                }
                advanceTrial();
                redDotsPlane.setDimensions(50,50,0.1);
                redDotsPlane.setSlantTilt(factors["Slant"], (int) factors["Tilt"] );
                redDotsPlane.compute();
                stimDrawer.initList(&redDotsPlane);
                mouseButton=0;
            }
        }
    }
    glutPostRedisplay();
}

void initStreams()
{
    inputParameters.open("parametersPassive.txt");
    if ( !inputParameters.good() )
    {
        cerr << "File doesn't exist" << endl;
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !exists(baseDir) )
        create_directory(baseDir);

    // Subject name
    string subjectName = parameters.find("SubjectName");

    // Principal streams file
    string transformationFileName("transformationFile_");
    string trialFileName("trialFile_");
    string anglesFileName("anglesFile_");
    string responseFileName("responseFile_");

    // Add the subject name to file extension
    transformationFileName	+=string(subjectName)+".txt";
    trialFileName			+=string(subjectName)+".txt";
    anglesFileName			+=string(subjectName)+".txt";
    responseFileName		+=string(subjectName)+".txt";

    // Check for output file existence
    /** Transformation file **/
    if ( !fileExists((baseDir+transformationFileName)) )
        transformationFile.open((baseDir+transformationFileName).c_str() );

    /** Trial file **/
    if ( !fileExists((baseDir+trialFileName)) )
        trialFile.open((baseDir+trialFileName).c_str());

    /** Angles file **/
    if ( !fileExists((baseDir+anglesFileName)) )
        anglesFile.open((baseDir+anglesFileName).c_str());

    /** Response file **/
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());

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
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /** END LIGHTS **/
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
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawTrial();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawTrial();

        glutSwapBuffers();
    }
    else	// MONOCULAR
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if ( (int) factors["Anchored"] != 0 )
            cam.setEye(Vector3d(fixationPoint.x(),fixationPoint.y(),0));
        else
            cam.setEye(Vector3d::Zero());
        drawTrial();
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
    case 'p':
    {
        // Print the current trial
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            cerr << iter->first << " " << iter->second << " ";
        }
        cerr << endl;
    }
    break;
    case '2':
    {   screen.translate(0,-0.5,0);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;
    case '8':
    {   screen.translate(0,0.5,0);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;
    case '4':
    {   screen.translate(-0.5,0,0);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;
    case '6':
    {   screen.translate(0.5,0,0);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;

    case 'g':
        calibratorHelper.switchGrid();
        break;
    case 'b':
        calibratorHelper.switchBoxes();
        break;
    case 'f':
        glutFullScreen();
        break;
    }
}


void specialPressed(int key, int x, int y)
{   switch (key)
    {   /** Calibration movements **/
    case GLUT_KEY_PAGE_DOWN:
    {   screen.translate(0,0,-0.5);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;
    case GLUT_KEY_PAGE_UP:
    {   screen.translate(0,0,0.5);
        cam.updateMonitorPoint(screen);
        calibratorHelper.setScreen(screen);
    }
    break;
    /** Helper Informations **/
    case GLUT_KEY_F1:
    {   calibratorHelper.switchCalibration();
    }
    break;
    case GLUT_KEY_F2:
    {   calibratorHelper.switchInstructions();
    }
    break;
    case GLUT_KEY_F3:
    {   calibratorHelper.switchFPS();
    }
    break;
    case GLUT_KEY_F4:
    {   calibratorHelper.switchHeadCalibration();
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

void checkBounds()
{
    if ((trialMode!=PROBEMODE) )
    {
        conditionInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius-2.5) ; // here we subtract 4 to work with trianglewave precision
        // If this condition is met, then this means that the point is outside the screen area!
        if ( !( conditionInside || (!wasInside) ) )
        {
            switch ( (int) factors["Anchored"] )
            {
            case 0:	//anchored x
                signsX.pop_back();
                signsX.push_front( factors["Onset"]* projPoint.x() > 0 );
                if ( (signsX.front() != signsX.back() ) )
                    sumOutside++;
                break;
            case 1:	//anchored x
                signsX.pop_back();
                signsX.push_front( factors["Onset"]* projPoint.x() > 0 );
                if ( (signsX.front() != signsX.back() ) )
                    sumOutside++;
                break;
            case 2:	//anchored y
                signsY.pop_back();
                signsY.push_front( factors["Onset"]*projPoint.y() < 0 );
                if ( (signsY.front() != signsY.back() ) )
                    sumOutside++;
                break;
            }
        }
        wasInside = conditionInside;
    }
}

void update(int value)
{
    // Timing things

    if ( trialMode != PROBEMODE )
    {

        oldvariable = variable;
        variable = -factors["Onset"]*mathcommon::trianglewave( timeFrame , factors["StimulusDuration"]/(TIMER_MS*factors["FollowingSpeed"]) );

        timeFrame+=1;

        bool isInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius);

        if ( (isInside && (sumOutside > stimulusEmiCycles))  )
        {
            sumOutside=0;
            advanceTrial();
            //cerr << "stim time= " << stimulusTimer.getElapsedTimeInMilliSec() << endl;
        }
    }

    // Simulate head translation
    // Coordinates picker
    markers[1] = Vector3d(0,0,0);
    markers[2] = Vector3d(0,10,0);
    markers[3] = Vector3d(0,0,10);

    Vector3d translation(0,0,0);
    switch ( (int) factors["Anchored"] )
    {
    case 0:
        translation = Vector3d((circleRadius+1)*variable,0,0);
        break;
    case 1:
        translation = Vector3d((circleRadius+1)*variable,0,0);
        break;
    case 2:
        translation = Vector3d(0,(circleRadius+1)*variable,0);
        break;
    }

    markers[1]+=translation;
    markers[2]+=translation;
    markers[3]+=translation;

    headEyeCoords.update(markers[1],markers[2],markers[3]);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();



    fixationPoint = (headEyeCoords.getRigidStart().getFullTransformation() * ( Vector3d(0,0,focalDistance) ) );
    // Projection of view normal on the focal plane
    pline = Eigen::ParametrizedLine<double,3>::Through(eyeRight,fixationPoint);
    projPoint = pline.intersection(focalPlane)*((fixationPoint - eyeRight).normalized()) + eyeRight;

    checkBounds();

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);

    cam.init(screen);
    calibratorHelper.setScreen(screen);
    calibratorHelper.setWidthHeightFocalPlane(SCREEN_WIDTH, SCREEN_HEIGHT, focalDistance);

}
int main(int argc, char*argv[])
{   initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP WEXLER");
//glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@60");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();

    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(specialPressed);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(TIMER_MS, update, 0);
//glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
