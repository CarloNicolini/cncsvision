///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#include <MMSystem.h>
#endif


/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "Util.h"
/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

//#define SIMULATION

#ifdef SIMULATION
	#include "Optotrak2Sim.h"
	#include "MarkerSim.h"
	#include "BrownMotorFunctionsSim.h"
	using namespace BrownMotorFunctionsSim;
#else
	#include <direct.h>
	#include "Optotrak2.h"
	#include "Marker.h"
	#include "BrownMotorFunctions.h"
	using namespace BrownMotorFunctions;
#endif

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 11 // 85 hz
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX = 33.5;
double alignmentY = 33;// - 49 - 55.0/2.0;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0, platformXZtheta = 0.0;
Screen screen;
static const Vector3d centercal(29.75,-133.94,-296.16); //updated 9/25/14
/********* VISUALIZATION VARIABLES *****************/
#ifndef SIMULATION
	static const bool gameMode=true;
	static const bool stereo=true;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[3];
CylinderPointsStimulus cylinder[3];
Timer timer;
Timer globalTimer;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, ind, thu, platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999), upperPinMarker(0,0,0), lowerPinMarker(0,0,0), test(0,0,0);
vector <Marker> markers;
static double interoculardistance=0.0;
#ifdef SIMULATION
vector<Marker> simMarkers;
Timer simTimer;
#endif

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int fingerCalibrationDone=0;

#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif

bool allVisibleHead=markers_status;
bool allVisiblePatch=markers_status;
bool allVisibleIndex=markers_status;
bool allVisibleThumb=markers_status;
bool allVisibleFingers=markers_status;
bool allVisibleObject=markers_status;
bool allVisiblePlatform=markers_status;
bool visibleInfo=true;
bool expFinished=false;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
double objwidth = 0.0, objdepth = 0.0;
bool isStimulusDrawn = false;
bool iGrasped = false, isHandHome = true;
double dist_to_home = 0.0, dist_to_target = 0.0;
int fingersOccluded = 0, framesOccluded = 0, frameN = 0, good_trial = 0;
string subjectName = "junk";
Vector3d object_reset_position(0,0,0), object_position(0,0,0);
int frameToGrasp = 0;

/********** STREAMS **************/
ofstream responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void cleanup();
void drawInfo();
void drawStimulus();
void drawGLScene();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void update(int value);
void idle();
void initMotors();
void initGLVariables();
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial();
void initTrial();
void calibration_fingers(int phase);
void calibration_head(int phase);
void drawNoFingers();
void drawFingers();

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/armlength/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/armlength/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_armlength-grasp.txt";

// markers file name
string markersFile_name = "spring15-armlength-grasp_";

// response file headers
string markersFile_headers = "subjName\tIOD\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tframesOccluded\tAbsDepth\tRelDepthObj\tframesToGrasp";

string responseFile_name = "spring15-armlength-grasp_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjwidth\tRelDepthObj\ttrialDuration\tgood_trial";

/*************************** FUNCTIONS ***********************************/

/***** SOUNDS *****/
void beepOk(int tone)
{
#ifndef SIMULATION
	switch(tone)
	{
	case 0:
    // Remember to put double slash \\ to specify directories!!!
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 1:
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	}
	return;
#endif
}

void updateTheMarkers()
{
#ifdef SIMULATION
	optotrak->updateMarkers(simMarkers);
	markers = optotrak->getAllMarkers();
#else
	optotrak->updateMarkers();
	markers = optotrak->getAllMarkers();
#endif
}

void cleanup()
{
	// Stop the optotrak
	optotrak->stopCollection();
	delete optotrak;
}

void calibration_fingers(int phase)
{
	switch (phase)
	{
		case 1:
		{
			if(allVisibleObject)
			{
				upperPinMarker=markers[1].p;
				upperPin.init(upperPinMarker, markers.at(8).p, markers.at(11).p, markers.at(12).p );
			}
		} break;
		case 2:
		{
			if(allVisibleObject)
			{
				lowerPinMarker=markers[2].p;
				lowerPin.init(lowerPinMarker, markers.at(8).p, markers.at(11).p, markers.at(12).p );
			}
		} break;
		case 3:
		{
			indexCoords.init(upperPin.getP1(), markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(lowerPin.getP1(), markers.at(15).p, markers.at(17).p, markers.at(18).p );
		} break;
	}
}

void calibration_head(int phase)
{
	switch (phase)
	{
	case 1:
		{
			headEyeCoords.init(markers[3].p-Vector3d(70,0,0),markers[3].p, markers[10].p,markers[11].p,markers[12].p,interoculardistance );

		} break;
	case 2:
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
		} break;
	}
}

void drawInfo()
{
	if ( visibleInfo )
	{
		GLText text;	
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text.enterTextInputMode();

		switch (fingerCalibrationDone)
		{
			case 0:
				text.draw("Calibration object and index marker are visible, then press F");		
				break;

			case 1:
				text.draw("Calibration object and thumb marker are visible, then press F");		
				break;
			case 2:
				text.draw("Touch the two outer markers with the fingerpads then press F (fingers and calibration object visible)");
				break;

			case 3:
				text.draw("Set home position then press F to start the experiment");		
				break;
		}

		text.draw("# IOD: " +stringify<double>(interoculardistance));

		// check if mirror is calibrated
		if ( abs(mirrorAlignment - 45.0) < 0.2 )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("# Mirror Alignment = " +stringify<double>(mirrorAlignment));

		// check if monitor is calibrated
		if ( screenAlignmentY < 89.0 )
			glColor3fv(glRed);
		else
			glColor3fv(glGreen);
		text.draw("# Screen Alignment Y = " +stringify<double>(screenAlignmentY));
		if ( abs(screenAlignmentZ) < 89.0 )
			glColor3fv(glRed);
		else
			glColor3fv(glGreen);
		text.draw("# Screen Alignment Z = " +stringify<double>(screenAlignmentZ));
		glColor3fv(glWhite);
		// X and Z coords of simulated fixation
		text.draw("# Fixation Z = " +stringify<double>(markers[19].p.x()-120.0)+ " [mm]");
		text.draw("# Fixation X = " +stringify<double>(markers[19].p.z()+363.0)+ " [mm]");
		text.draw(" ");

		glColor3fv(glWhite);
		text.draw("Calibration Platform" );
		
		if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		
		text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );

		if ( isVisible(markers[3].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );

		glColor3fv(glWhite);
		text.draw(" " );
		text.draw("Index" );

		if ( isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(13)+stringify< Eigen::Matrix<double,1,3> > (markers[13].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(14)+stringify< Eigen::Matrix<double,1,3> > (markers[14].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(16)+stringify< Eigen::Matrix<double,1,3> > (markers[16].p.transpose())+ " [mm]" );

		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("Thumb" );

		if ( isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(15)+stringify< Eigen::Matrix<double,1,3> > (markers[15].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(17)+stringify< Eigen::Matrix<double,1,3> > (markers[17].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(18)+stringify< Eigen::Matrix<double,1,3> > (markers[18].p.transpose())+ " [mm]" );

		if ( allVisibleObject )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw(" " );
		text.draw("Marker "+ stringify<int>(11)+stringify< Eigen::Matrix<double,1,3> > (markers[11].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(12)+stringify< Eigen::Matrix<double,1,3> > (markers[12].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(8)+stringify< Eigen::Matrix<double,1,3> > (markers[8].p.transpose())+ " [mm]" );

		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("Index = " +stringify< Eigen::Matrix<double,1,3> > (ind.transpose()));
		text.draw("Thumb = " +stringify< Eigen::Matrix<double,1,3> > (thu.transpose()));
		text.draw("Platform Orientation = " + stringify<double>(platformXZtheta));
		text.draw("test = " +stringify< Eigen::Matrix<double,1,3> > (test.transpose()));

		text.leaveTextInputMode();
	}

	if ( expFinished )
	{
		GLText text2;	
		text2.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text2.enterTextInputMode();
		text2.draw("The experiment is finished.");
		text2.leaveTextInputMode();
	}

}

void drawStimulus()
{
	if(fingerCalibrationDone==3)
		if(!allVisibleFingers)
			drawNoFingers();

	if ( isStimulusDrawn && !isHandHome && !iGrasped )
	{
		glLoadIdentity();
		glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);

		// Left rear 
		glPushMatrix();
		glTranslated(-objwidth/2, 0, -objdepth/2);
		stimDrawer[0].draw();
		glPopMatrix();

		// Right rear rod
		glPushMatrix();
		glTranslated(objwidth/2, 0, -objdepth/2);
		stimDrawer[1].draw();
		glPopMatrix();

		// Front rod
		glPushMatrix();
		glTranslated(0, 0, objdepth/2);
		stimDrawer[2].draw();
		glPopMatrix();
	}
	
	if (iGrasped && isHandHome)
		advanceTrial();
}

void drawGLScene()
{
    if (stereo)
    {   
	// back on both windows (left and right)
	glDrawBuffer(GL_BACK);

	// now start switching between presentations:
	// Draw left eye view first
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    cam.setEye(eyeLeft);
	drawFingers();
	drawInfo();

    // Draw right eye view afterwards
    glDrawBuffer(GL_BACK_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    cam.setEye(eyeRight);
    drawFingers();
	drawInfo();

	// swap between buffers
        glutSwapBuffers();
    }
    else
    {   
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawStimulus();
	drawInfo();
        glutSwapBuffers();
    }
}

void initTrial()
{
	// reset counters
	frameN = 0;
	frameToGrasp = 0;
	framesOccluded = 0;

	// subject did not grasp yet
	iGrasped = false;

	// draw an empty screen
	isStimulusDrawn=false;

	// set the width of the object to be randomly between 30 and 70 mm
	objwidth = unifRand(30.0, 70.0);

	// set the depth of the object to be randomly between 30 and 70 mm
	objdepth = trial.getCurrent()["RelDepthObj"];

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(trial.getCurrent()["AbsDepth"]);

	// draw the stimulus		
	isStimulusDrawn=true;

	// markers file (if haptic)
	string markersFileName = experiment_directory + subjectName + "/" + markersFile_name + subjectName + "_markers-trial_" + stringify(trialNumber) + ".txt";

	markersFile.open(markersFileName.c_str());
	markersFile << fixed << markersFile_headers << endl;

	timer.start();
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	if(framesOccluded > 15)
	{
		beepOk(1);
		trial.reinsert(trial.getCurrent());
		good_trial = 0;
	} else
	{
		beepOk(0);
		good_trial = 1;
	}

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			trial.getCurrent()["AbsDepth"] << "\t" << 
			objwidth << "\t" << 
			objdepth << "\t" << 
			timeElapsed << "\t" << 
			good_trial
			<< endl;

	markersFile.close();

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		responseFile.close();
		expFinished = true;
	}

}

void drawNoFingers()
{
		glLoadIdentity();
		glTranslated(0.0,0,-600);
		
		int edge = 100;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,0.0);
		glVertex3d(edge,-edge,0.0);
		glVertex3d(-edge,-edge,0.0);
		glVertex3d(-edge,edge,0.0);
		glEnd();
}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		#ifdef SIMULATION
		case '0':
		{
			good_trial = 1;
			advanceTrial();
		} break;

		case '1':
			allVisibleFingers = !allVisibleFingers;
			break;

		case '2':
		{
			trial.reinsert(trial.getCurrent());
			good_trial = 0;
			advanceTrial();
		} break;

		#endif

		case 'i': // show info
			visibleInfo=!visibleInfo;
			break;

		case 'm': // increase IOD
			interoculardistance += 0.5;
			break;

		case 'n': // decrease IOD
			interoculardistance -= 0.5;
			break;

		case 27: // press escape to quit
		{   
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		// fingers calibration
		case 'f':
		case 'F':
		{
			// Record the calibration platform's position and home position
			if ( isVisible(markers[1].p) && allVisibleObject && fingerCalibrationDone==0 )
			{
				fingerCalibrationDone=1;
				calibration_fingers(fingerCalibrationDone);
				beepOk(0);
				break;
			}

			// Record the calibration platform's position and home position
			if ( isVisible(markers[2].p) && allVisibleObject && fingerCalibrationDone==1 )
			{
				fingerCalibrationDone=2;
				calibration_fingers(fingerCalibrationDone);
				beepOk(0);
				break;
			}

			// Interpolate the fingertip (fourth virtual marker)
			if ( fingerCalibrationDone==2 && allVisibleFingers && allVisibleObject )
			{
				fingerCalibrationDone=3;
				calibration_fingers(fingerCalibrationDone);
				beepOk(0);
				break;
			}

			// Start the experiment
			if ( fingerCalibrationDone==3  && allVisibleFingers )
			{
				fingerCalibrationDone=4;
				beepOk(0);
				visibleInfo=false;
				drawGLScene(); // this is needed otherwise the next motor command freezes the screen
				initProjectionScreen(-280);
			}
		} break;

		case ' ': // confirm that you grasped
		{
			if(allVisibleFingers)
			{
				iGrasped = true;
				isStimulusDrawn = false;
			}
		} break;
		
	}
}

void drawFingers()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(ind.x(),ind.y(),ind.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslated(thu.x(),thu.y(),thu.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();

	glLoadIdentity();
}

// Resize window
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

// move monitor to specific distance
void initProjectionScreen(double _focalDist, const Affine3d &_transformation, bool synchronous)
{
	focalDistance = _focalDist;	
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screen.setOffset(alignmentX,alignmentY);
	screen.setFocalDistance(_focalDist);
	screen.transform(_transformation);
	cam.init(screen);
	if ( synchronous )
		moveScreenAbsolute(_focalDist,homeFocalDistance,4500);
	else
		moveScreenAbsoluteAsynchronous(_focalDist,homeFocalDistance,4500);
}

// Handle refresh of the screen
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{

	updateTheMarkers();

	// Visibility check
	allVisiblePlatform = isVisible(markers[1].p) && isVisible(markers[2].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;
	
	allVisibleObject = isVisible(markers[8].p) && isVisible(markers[11].p) && isVisible(markers[12].p);

	allVisiblePatch = isVisible(markers[10].p) && isVisible(markers[11].p) && isVisible(markers[12].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[9].p);

	test = Vector3d(markers.at(1).p.x(), markers.at(2).p.y(), markers.at(3).p.z());

	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	platformXZtheta = asin(
			abs((markers[1].p.z()-markers[2].p.z()))/
			sqrt(
			pow(markers[1].p.x()-markers[2].p.x(), 2) +
			pow(markers[1].p.z()-markers[2].p.z(), 2)
			)
			)*180/M_PI;

	// screen Y alignment check
	screenAlignmentY = asin(
			abs((markers[19].p.y()-markers[21].p.y()))/
			sqrt(
			pow(markers[19].p.x()-markers[21].p.x(), 2) +
			pow(markers[19].p.y()-markers[21].p.y(), 2)
			)
			)*180/M_PI;

	// screen Z alignment check
	screenAlignmentZ = asin(
			abs(markers[19].p.z()-markers[20].p.z())/
			sqrt(
			pow(markers[19].p.x()-markers[20].p.x(), 2) +
			pow(markers[19].p.z()-markers[20].p.z(), 2)
			)
			)*180/M_PI*
			abs(markers[19].p.x()-markers[20].p.x())/
			(markers[19].p.x()-markers[20].p.x());

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

	// fingers coordinates, fingersOccluded and framesOccluded
	if ( allVisibleFingers )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
	}

	if ( allVisibleObject )
	{
		upperPin.update(markers[8].p, markers[11].p, markers[12].p );
		lowerPin.update(markers[8].p, markers[11].p, markers[12].p );
	}

	#ifndef SIMULATION
	// index coordinates
	if(allVisibleIndex)
		ind = indexCoords.getP1();

	// thumb coordinates
	if(allVisibleThumb)
		thu = thumbCoords.getP1();
	#endif

}

void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    int numMarkers=22;
    float frameRate=85.0f;
    float markerFreq=4600.0f;
    float dutyCycle=0.4f;
    float voltage = 7.0f;
#ifndef SIMULATION
    if ( optotrak->init("C:/cncsvisiondata/camerafiles/Aligned20111014",numMarkers, frameRate, markerFreq, dutyCycle,voltage) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
#else
	optotrak->init("any string applies here - it's just a simulation",numMarkers, frameRate, markerFreq, dutyCycle,voltage);
	simMarkers.resize(numMarkers+1);
#endif
    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        updateTheMarkers();
    }
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
	// scommenta solo se vuoi attivare lo shading degli stimoli
	// LIGHTS
	/*
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Tieni questa riga per evitare che con l'antialiasing attivo le linee siano piu' sottili di un pixel e quindi
	// ballerine (le vedi vibrare)
	glLineWidth(1.5);
}

void initVariables()
{

}

void initGLVariables()
{
	// Ora inizializza i 3 rods (stimoli)
	for (int i=0; i<3; i++)
	{
		cylinder[i].setNpoints(150);
		cylinder[i].setRadiusAndHeight(2,50); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[i].compute();
		stimDrawer[i].setStimulus(&cylinder[i]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[i].setSpheres(true);
		stimDrawer[i].initList(&cylinder[i], glRed);
	}
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{

}

// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(4051,3500);
}

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	// initializes optotrak and velmex motors
	initOptotrak();

	initMotors();

	// initializing glut
	glutInit(&argc, argv);
	#ifndef SIMULATION
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
		glutGameModeString("1024x768:32@85");
		glutEnterGameMode();
		glutFullScreen();
	#else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutCreateWindow("Simulation test");
	#endif

	// initializing experiment's parameters
	initRendering();
	initGLVariables();

	// glut callback
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutIdleFunc(idle);
	glutTimerFunc(TIMER_MS, update, 0);
	glutSetCursor(GLUT_CURSOR_NONE);

	boost::thread initVariablesThread(&initVariables);

	// Application main loop
	glutMainLoop();

	cleanup();

	return 0;
}
