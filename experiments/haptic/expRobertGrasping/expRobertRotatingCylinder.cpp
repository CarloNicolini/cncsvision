// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2013 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include "IncludeGL.h"

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Marker.h"
#include "Util.h"
//#include "Optotrak2.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"

//#define FRAME_RATE 59.95
#define FRAME_RATE 100.0

using namespace std;
using namespace Eigen;

// Function prototypes
void generateStimulus(double XRadius, double zAxis, double height);
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;
double homeFocalDistance=-418.5;

/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords;
Vector3d eyeLeft;
Vector3d eyeRight;

Vector3d physicalRigidBodyTip;
Vector3d visualStimCenter;
static double interoculardistance=65;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=3;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool infoDraw=true;
bool isSaving=true;
bool upperViewDebugMode=false;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
//Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
TrialGenerator<double> trial;
unsigned int trialNumber=0;
static const int BLACK_MODE=0;
static const int STILL_STIMULUS_MODE = 1;
static const int ROTATING_STIMULUS_MODE = 2;
int trialMode = -1;

/********* STIMULI *******/
CylinderPointsStimulus cylinderStimulus;
StimulusDrawer stimDrawer;


/****** TIMING *****/
Timer trialTimer,globalTimer;
double frame=0.0;
unsigned int trialFrame=0;

/***** TRIAL VARIABLES *****/

/**** FILES VARIABLES ***/
ofstream responseFile;
ofstream timingFile;
int paramIndex=0;
char *parametersFileNames[] = {"C:/cncsvisiondata/parametersFiles/Robert/expRotatingCylinder/parametersExpRobertRotatingCylinder.txt",
                               "C:/cncsvisiondata/parametersFiles/Robert/expRotatingCylinder/parametersExpRobertRotatingCylinder_training.txt"
                              };
/********* SOUNDS ********/
#ifdef WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepInvisible()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(1000,5);
    return;
}
void beepLong()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(220,880);
    return;
}
void beepOk()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}
void beepBad()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(200,200);
}

void beepTrial()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(660,200);
    return;
}
#endif

/**
 * @brief drawInfo
 */
void drawInfo()
{   if ( infoDraw )
    {   glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        GLText text;
        if ( gameMode )
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
        else
            text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
        text.enterTextInputMode();
        switch ( headCalibrationDone )
        {
        case 0:
        {   if ( allVisibleHead )
                text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
            else
                text.draw("Be visible with the head and glasses");
        }
            break;
        case 1:
        case 2:
        {   if ( allVisiblePatch )
                text.draw("Cyclopean(x,y,z)= " + stringify<int>((eyeRight.x()+eyeLeft.x())/2)+", "+ stringify<int>((eyeRight.y()+eyeLeft.y())/2)+", "+ stringify<int>((eyeRight.z()+eyeLeft.z())/2)+", " );
            else
                text.draw("Be visible with the patch");
        }
            break;
        }
        if ( headCalibrationDone==3 )
        {
            text.draw("Time= "+ util::stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
            text.draw("Trial frame [s]= "+util::stringify<double>(frame));
            text.draw("FinalDeltaAngle= "+util::stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getState()));
            text.draw("Angle(t)= " + util::stringify<double>(
                          frame*trial.getCurrent().second->getCurrentStaircase()->getState()/(parameters.get("RotationTime")/1000.0)
                          +trial.getCurrent().first.at("CylinderOrientation") )
                      );
            text.draw("TrialMode="+stringify<int>(trialMode));
            /*
            double CylinderHeight = cylinder.getHeight();
            double cylDiameterX = 2*cylinder.getXAxis();
            double cylDiameterZ = 2*cylinder.getZAxis();
            text.draw("CylDiamX= " + stringify<double>(CylinderDiameterX) );
            text.draw("CylDiamZ= " + stringify<double>(StartCylinderDiameterZ) );
            text.draw("CylinderHeight= " + stringify<double>(CylinderHeight) );
            text.draw("ScaleAdjustment*Z= " + stringify<double>(1+adjustmentZ/cylDiameterZ) );
            text.draw("AdjustedZ= " + stringify<double>( (1+adjustmentZ/cylDiameterZ)*cylDiameterZ ) );
            */
        }
        text.leaveTextInputMode();
    }
}

/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial(bool response, int responseKey)
{
    if ( trial.isEmpty() )
    {
        exit(0);
    }
	
    responseFile << parameters.find("SubjectName") << "\t" <<
                    trialNumber << "\t" <<
                    parameters.find("FocalDistance") << "\t" <<
                    parameters.find("CylinderDiameterX") << "\t" <<
                    parameters.find("CylinderHeight") << "\t" <<
                    parameters.find("StartCylinderDiameterZ") << "\t" <<
                    parameters.get("EndCylinderDiameterZ") << "\t" <<
                    trial.getCurrent().first.at("CylinderOrientation") << "\t" <<
                    parameters.find("RotationTime") << "\t" <<
                    parameters.find("RefreshMode") << "\t" <<
					trial.getCurrent().second->getCurrentStaircase()->getID() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getTrialsDone() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getReversals() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
					frame*trial.getCurrent().second->getCurrentStaircase()->getState()/(parameters.get("RotationTime")/1000.0) +trial.getCurrent().first.at("CylinderOrientation") << "\t" <<
					responseKey << "\t" <<
                    response << "\t" <<
                    endl;

    trialNumber++;
    trial.next(response);

    if ( trial.isEmpty() )
    {
        exit(0);
    }
    generateStimulus(parameters.get("CylinderDiameterX"),parameters.get("StartCylinderDiameterZ"),parameters.get("CylinderHeight"));
    trialTimer.start();

}

void generateStimulus(double cylinderDiameterX, double cylinderDiameterZ, double cylinderHeight)
{
    int nStimulusPoints=(int)(parameters.get("StimulusDensity")*cylinderHeight*mathcommon::ellipseCircumferenceBetter(cylinderDiameterX,cylinderDiameterZ));

    cylinderStimulus.setNpoints(nStimulusPoints);
    cylinderStimulus.setAperture(0,2*M_PI);
    cylinderStimulus.setRadiusAndHeight(cylinderDiameterX,cylinderDiameterZ,cylinderHeight);
    cylinderStimulus.setFluffiness(0.0);
    cylinderStimulus.compute();
    stimDrawer.setSpheres(false);
    stimDrawer.initList(&cylinderStimulus,glRed,2.5);
}

/**
 * @brief idle
 */
void idle()
{
    eyeLeft = Vector3d(-interoculardistance/2,0,0);//headEyeCoords.getLeftEye().p;
    eyeRight = Vector3d(interoculardistance/2,0,0);//headEyeCoords.getRightEye().p;

    if ( headCalibrationDone==3 )
    {
        if (!isSaving)
            return;
    }
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    //double theta=frame*trial.getCurrent().second->getCurrentStaircase()->getState()+trial.getCurrent().first.at("CylOrientation");
    double omega = trial.getCurrent().second->getCurrentStaircase()->getState()/(parameters.get("RotationTime")/1000.0); // /1000.0 because time is inserted in milliseconds
    double theta = frame*omega+trial.getCurrent().first.at("CylinderOrientation");

    double deformationSpeed = (parameters.get("EndCylinderDiameterZ")-parameters.get("StartCylinderDiameterZ"))/(parameters.get("RotationTime")/1000.0);
    double relativeDeformation = 1.0+(frame*deformationSpeed)/parameters.get("StartCylinderDiameterZ");


    // Draw the frontal half cylinder
    if (!upperViewDebugMode)
    {
        glPushMatrix();
        glTranslated(0.0,0.0,parameters.get("FocalDistance"));
        glRotated(theta,0.0,1.0,0.0);
        glScaled(1.0,1.0,relativeDeformation);
        //glColor3fv(glGreen);
        glColor3fv(glBlack);
        drawCylinderElliptic(cylinderStimulus.getDiameterX()/2.0,cylinderStimulus.getHeight(),cylinderStimulus.getDiameterZ()/2.0,0.25);
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.0,0.0,parameters.get("FocalDistance"));
        glRotated(theta,0.0,1.0,0.0);
        glScaled(1.0,1.0,relativeDeformation);
        glRotated(180,1,0,0);
        glColor3fv(glBlack);
        drawCylinderElliptic(cylinderStimulus.getDiameterX()/2.0,cylinderStimulus.getHeight(),cylinderStimulus.getDiameterZ()/2.0,0.25);
        glPopMatrix();
    }
    // Use polygon offset to compute occlusions
    
    glPolygonOffset(1.0,1.0);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,parameters.get("FocalDistance"));
    if (upperViewDebugMode)
    {
        glRotated(90,1,0,0); //for lateral debug purpose
        glRotated(theta,0,1,0);
        glScaled(1.0,1.0,relativeDeformation);
    }
    else
    {
        glRotated(theta,0,1,0);
        glScaled(1.0,1.0,relativeDeformation);
    }
    stimDrawer.draw();
    glPopMatrix();
    glDisable(GL_POLYGON_OFFSET_POINT);
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch ( trialMode )
    {
    case BLACK_MODE:
    {
        break;
    }
    case ROTATING_STIMULUS_MODE:
    case STILL_STIMULUS_MODE:
    {
        drawStimulus();
    }
        break;
    }
}

/**
 * @brief paintGL
 */
void paintGL()
{
    if (stereo)
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
        cam.setEye(Vector3d(0,0,0));
        //cam.setEye(Vector3d(0,250,-200));
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

/**
 * @brief updateGL
 */
void updateGL(int val)
{
	++trialFrame;
    if (trialTimer.getElapsedTimeInMilliSec() >= parameters.get("WaitTime") && trialMode==BLACK_MODE)
    {
        frame=0.0;
        trialMode++;
        trialTimer.start();
    }
    if (trialTimer.getElapsedTimeInMilliSec() >= parameters.get("StillTime") && trialMode==STILL_STIMULUS_MODE)
    {
        frame=0.0;
        trialMode++;
        trialTimer.start();
    }
	if (trialMode==ROTATING_STIMULUS_MODE)
        frame=trialTimer.getElapsedTimeInSec();
		//frame+=1.0/FRAME_RATE;
	
	if (trialTimer.getElapsedTimeInMilliSec() >= parameters.get("RotationTime") && trialMode==ROTATING_STIMULUS_MODE)
    {
        trialMode++;
        trialTimer.start();
    }

    if (parameters.find("RefreshMode")=="Dynamic" && ( (trialFrame%((int)parameters.get("ModuloRefresh"))) ==0 ) )
    {
        cylinderStimulus.compute();
        stimDrawer.initList(&cylinderStimulus,glRed,2.5);
    }
	
	timingFile << trialNumber << "\t" << trialMode << "\t" << trialTimer.getElapsedTimeInMilliSec() << endl;

	//timingFile << globalTimer.getElapsedTimeInMilliSec() << endl;

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS*50, updateGL, 0);
}

/**
 * @brief initializeOptotrakMonitor
 */
void initializeOptotrakMonitor()
{
    // Move the monitor in the positions
    RoveretoMotorFunctions::homeMirror(3500);
    RoveretoMotorFunctions::homeScreen(3500);
    RoveretoMotorFunctions::homeObjectAsynchronous(3500);
	/*
    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
        exit(0);
	*/
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
    char *argv[] = { "", NULL }; //must be NULL terminated otherwise glut crashes
    int argc = 1;
    glutInit( &argc, argv );
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO | GLUT_MULTISAMPLE );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE );

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("expRobertRotatingCylinders");
    }
    else
    {   glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
	glEnable(GL_MULTISAMPLE);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 'Q':
    case 27:
    {   exit(0);
    }
        break;
    case 'd':
    {
        upperViewDebugMode=!upperViewDebugMode;
        break;
    }
    case 'i':
    case 'I':
    {
        infoDraw=!infoDraw;
    }
        break;
    case '4':
    {
        if (trialMode==3)
        {
            trialMode=BLACK_MODE;
#ifdef WIN32
			advanceTrial(false,4);
#else
			advanceTrial(true,4);
#endif
            trialTimer.start();
        }
        break;
    }
    case '6':
    {
        if (trialMode==3)
        {
            trialMode=BLACK_MODE;
#ifdef WIN32
			advanceTrial(true,6);
#else
			advanceTrial(false,6);
#endif
            trialTimer.start();
        }
        break;
    }
    case 13: //Enter pressed, experiment starting
    {
        trialMode=BLACK_MODE;
        trialTimer.start();
        globalTimer.start();
        break;
    }
    }
}

/**
 * @brief handleResize
 * @param w
 * @param h
 */
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

/**
* @brief initProjectionScreen
* @param _focalDist The focal distance to use
*/
void initProjectionScreen(double _focalDist)
{
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0,0,_focalDist));
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
}

/**
 * @brief initializeExperiment Set all the variables for the experiment, streams, parameter files and factors list
 */
void initializeExperiment()
{
    //ifstream inputParameters("/home/carlo/workspace/cncsvisioncmake/parameters.txt");
    ifstream inputParameters(parametersFileNames[paramIndex]);
    parameters.loadParameterFile(inputParameters);

    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string outputFileName  = baseDir + "responseFile_RotatingCylinder_" + subjectName + "_" + parameters.find("FocalDistance") + ".txt";
    string timingFileName  = baseDir + "timingFile_RotatingCylinder_" + subjectName + "_" + parameters.find("FocalDistance")  + ".txt";
    // Check for output file existence
    if ( !util::fileExists((outputFileName)) )
        responseFile.open(( outputFileName ).c_str());
    cerr << "File " << outputFileName << " loaded successfully" << endl;

    responseFile << "SubjectName\tTrialNumber\tFocalDistance\tCylinderX\tCylinderHeight\tStartCylinderZ\tEndCylinderZ\tCylinderOrientation\tRotationTime\tRefreshMode\tStaircaseID\tStaircaseTrials\tStaircaseReversals\tStaircaseRotation\tFinalAngle\tKeyPressed\tResponse" << endl;

    if ( !util::fileExists((timingFileName)) )
        timingFile.open(( timingFileName ).c_str());
    cerr << "File " << timingFileName << " loaded successfully" << endl;

    interoculardistance = parameters.get("IOD");
    infoDraw = (bool)parameters.get("DrawInfo");
    // Initialize the trial generator from parameters file
    trial.init(parameters);

    // Generate the correct focal distance
    initProjectionScreen(parameters.get("FocalDistance"));
    // Generate and initialize the stimulus with correct dimensions
    generateStimulus(parameters.get("CylinderDiameterX"),parameters.get("StartCylinderDiameterZ"),parameters.get("CylinderHeight"));
    trialTimer.start();
}

/**
 * @brief main
 */
int main()
{
    cout << "Please select the parametersFileName" << endl;
    for (int i=0; i<2; i++)
        cout << i << ") " << parametersFileNames[i] << endl;
    //cin >> paramIndex;
    cout << "Selected " << parametersFileNames[paramIndex] << endl;
    globalTimer.start();
    randomizeStart();
    RoveretoMotorFunctions::homeMirror(3500);
    RoveretoMotorFunctions::homeScreen(3500);

    //initializeOptotrakMonitor();
    initializeGL();
    initializeExperiment();
    // Define the callbacks functions
    glutIdleFunc(idle);
    glutDisplayFunc(paintGL);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS*50, updateGL, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    // Application main loop
    glutMainLoop();
    return 0;
}
