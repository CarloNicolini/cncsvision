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
#include <math.h>
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
#include <boost/thread/thread.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "EulerExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"

#include "Util.h"

#ifdef __linux__
#include "beep.h"
#endif

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak optotrak;
CoordinatesExtractor headEyeCoords;
ObjLoader model;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5;
static const double alignmentY =  20.0;
double focalDistance= -418.5;
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),center );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, translationFactor(0,0,0),cyclopeanEye,projPointEyeRight,projPointEyeLeft,projPointCyclopeanEye, eyeCalibration;
vector <Vector3d> markers(18);
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* Timing variables  ************************/
// Timing variables
Timer rythmMaker;
Timer stimulusDuration;
/********* RIGID BODIES VARIABLES *********/
EulerExtractor eulerAngles;

/*** Streams File ***/
ofstream markersFile;
ofstream responseFile;
ofstream anglesFile;
ofstream trajFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
double probeAngle=0,probeStartingAngle=0;

int headCalibrationDone=0;
bool allVisibleHead=false;
bool passiveMode=false;
// Trial related things
int trialNumber=0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
bool conditionInside=true;
bool wasInside=true;
int sumOutside=0;
int stimulusFrames=0;
deque<bool> signs;
double instantPlaneSlant=45;

double val=0;
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void drawFixation();
void beepOk()
{
	boost::mutex::scoped_lock lock(beepMutex);
	Beep(440,440);
	return;
}

void tweeter()
{
	boost::mutex::scoped_lock lock(beepMutex);
	Beep(840,440);
	return;
}

void woofer()
{
	boost::mutex::scoped_lock lock(beepMutex);
	Beep(240,440);
	return;
}

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
int trialMode = FIXATIONMODE;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
	markersFile.close();
	anglesFile.close();
	trajFile.close();
	// Stop the optotrak
	optotrak.stopCollection();
}

void drawInfo()
{
	glPushMatrix();
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	markers = optotrak.getAllPoints();

	switch ( headCalibrationDone )
	{
	case 0:
		{   allVisibleHead = isVisible(markers[17]) && isVisible(markers[18]) && isVisible(markers[1]) && isVisible(markers[2]) && isVisible(markers[3]) ;
		if ( allVisibleHead )
			glClearColor(0.0,1.0,0.0,1.0); //green light
		else
			glClearColor(1.0,0.0,0.0,1.0); //red light
		}
		break;
	case 1:
	case 2:
	{
			glPrintText(10, SCREEN_HEIGHT-40,SCREEN_WIDTH,SCREEN_HEIGHT, "EL  " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()) );
			glPrintText(10, SCREEN_HEIGHT-60,SCREEN_WIDTH,SCREEN_HEIGHT, "ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()) );
			glPrintText(10, SCREEN_HEIGHT-80,SCREEN_WIDTH,SCREEN_HEIGHT, "EC" + stringify<int>(cyclopeanEye.x())+" " + stringify<int>(cyclopeanEye.y())+" " + stringify<int>(cyclopeanEye.z()));
			glPrintText(10, SCREEN_HEIGHT-100,SCREEN_WIDTH,SCREEN_HEIGHT, "Dist " + stringify<int>(cyclopeanEye.z()-focalDistance));
			glPrintText(10, SCREEN_HEIGHT-120,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
			glPrintText(10, SCREEN_HEIGHT-140,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
			glPrintText(10, SCREEN_HEIGHT-160,SCREEN_WIDTH,SCREEN_HEIGHT, "Press SPACEBAR to calibrate again or ENTER to confirm calibration.");
			glPrintText(10, SCREEN_HEIGHT-180,SCREEN_WIDTH,SCREEN_HEIGHT, "Delta " + stringify<int>(eyeRight.z()- eyeCalibration.z()));
			Vector3d angles = headEyeCoords.getRigidStart().getFullTransformation().rotation().eulerAngles(0,1,2);
			
			glPrintText(10, SCREEN_HEIGHT-200,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
			glPrintText(10, SCREEN_HEIGHT-220,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
			if ( !passiveMode )
				glPrintText(10, SCREEN_HEIGHT-240,SCREEN_WIDTH,SCREEN_HEIGHT, "Active");
			else
				glPrintText(10, SCREEN_HEIGHT-240,SCREEN_WIDTH,SCREEN_HEIGHT, "Passive");
			glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "OBJ " + stringify<int>(projPointEyeRight.x() ) + " " + stringify<int>(projPointEyeRight.y() ) + " " + stringify<int>(projPointEyeRight.z()) );
			Vector3d screenCenter = 0.5*(screen.pa + screen.pd);
			glPrintText(10, SCREEN_HEIGHT-280,SCREEN_WIDTH,SCREEN_HEIGHT, "SCREEN " + stringify<int>(screenCenter.x() ) + " " + stringify<int>(screenCenter.y() ) + " " + stringify<int>(screenCenter.z()) );
		}
		break;
	default:
		break;
	}
//glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "Translation " + stringify<int>(factors["Translation"]));

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopMatrix();
	// end if ( headCalibrationDone )
}

/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void drawCube()
{
	glColor3fv(glGreen);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(projPointEyeRight.x(), projPointEyeRight.y(), projPointEyeRight.z());
	glutWireCube(20);
	glPopMatrix();

	/*
	// YELLOW CUBE :
	// Fixed in 0,0,-418.5 in active, moving in passive
	glColor3fv(glYellow);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,focalDistance);
	glutWireCube(20);
	glPopMatrix();
	
	// BLUE CUBE:
	// Moving in active, following the eye projection, fixed but rotating in passive
	glColor3fv(glBlue);
	Affine3d passive = (headEyeCoords.getRigidStart().getFullTransformation())*Translation3d(Vector3d(0,0,focalDistance+val)-eyeCalibration);
	Vector3d cubePassive = passive*Vector3d(0,0,0);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(cubePassive.x(),cubePassive.y(), cubePassive.z());
	glutWireCube(20);
	glPopMatrix();
	*/
}
void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	Affine3d transformation(headEyeCoords.getRigidStart().getFullTransformation());
	transformation.translation() = projPointEyeRight;
	
	glPushMatrix();     // PUSH MATRIX
	glLoadIdentity();
	glMultMatrixd(transformation.data());
	
	Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight-eyeCalibration);
	instantPlaneSlant = factors["SlantDirection"]*toDegrees(acos( (focalDistance-posAlongLineOfSight.z() )/(sqrt(2.0)*(focalDistance ))));
	
	switch ( (int) factors["Tilt"] )
	{
	case 0:
		glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
		glRotated( -instantPlaneSlant ,0,1,0);
		glScaled(1/sin(toRadians( -90-factors["Slant"])),1,1);	//backprojection phase
		break;
	case 90:
		glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
		glRotated( -instantPlaneSlant ,1,0,0);
		glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
		break;
	case 180:
		glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
		glRotated( -instantPlaneSlant ,0,1,0);
		glScaled(1/sin(toRadians( -90-factors["Slant"] )),1,1); //backprojection phase
		break;
	case 270:
		glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
		glRotated( -instantPlaneSlant ,1,0,0);
		glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
		break;
	}

	// In this special case we overwrite the precomputed transformation with a M=I*T_{\mathbf{c}}*R_{\theta}
	if ((int)factors["Translation"]==-1)
	{ 
		glLoadIdentity();
		glTranslated( 0,0, center.z() );
		switch ( (int) factors["Tilt"] )
		{
		case 0:
			glRotated( -instantPlaneSlant ,0,1,0);
			glScaled(1/sin(toRadians( -90-factors["Slant"])),1,1);	//backprojection phase
			break;
		case 90:
			glRotated( -instantPlaneSlant ,1,0,0);
			glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
			break;
		case 180:
			glRotated( -instantPlaneSlant ,0,1,0);
			glScaled(1/sin(toRadians( -90-factors["Slant"] )),1,1); //backprojection phase
			break;
		case 270:
			glRotated( -instantPlaneSlant ,1,0,0);
			glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
			break;
		}
	}
	stimDrawer.draw();
	glPopMatrix();	// POP MATRIX
}

void drawProbe()
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glColor3fv(glRed);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3dv(center.data());
	glEnd();
	glColor3fv(glRed);
	glPointSize(1);
}

void drawFixation()
{
	switch ( headCalibrationDone  )
	{   
	case 1:
		// Fixed stimulus
		glColor3fv(glWhite);
		glDisable(GL_BLEND);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3d(0,0,focalDistance);
		glEnd();
		glPointSize(1);
		break;
	case 2:
		// Fixed stimulus + projected points
		glColor3fv(glWhite);
		glDisable(GL_BLEND);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3d(0,0,focalDistance);
		glColor3fv(glRed);
		glVertex3dv(projPointEyeRight.data());
		glEnd();
		glPointSize(1);
		break;
	/*
	case 3:
		// DRAW THE FIXATION POINT 
		glColor3fv(glRed);
		glPushMatrix();
		glTranslated(projPointEyeRight.x(),projPointEyeRight.y(),projPointEyeRight.z());
		glutSolidSphere(1,10,10);
		glPopMatrix();
		break;
	*/
	}
}

void drawTrial()
{   switch ( trialMode )
{
case FIXATIONMODE:
	{   drawFixation();
		//drawCube();
	}
	break;
case PROBEMODE :
	{   drawProbe();
	}
	break;
case STIMULUSMODE:
	{
//		if ( conditionInside )
			drawRedDotsPlane();
		//drawCube();
	}
	break;
default:
	{
		drawFixation();
	}
}
}

void initVariables()
{
	interoculardistance = str2num<double>(parameters.find("IOD"));
	trial.init(parameters);
	factors = trial.getNext(); // Initialize the factors in order to start from trial 1

	redDotsPlane.setNpoints(75);  //XXX controllare densita di distribuzione dei punti
	redDotsPlane.setDimensions(50,50,0.1);

	model.load("../data/objmodels/occluder.obj");

	redDotsPlane.compute();
	stimDrawer.setStimulus(&redDotsPlane);
	stimDrawer.initList(&redDotsPlane);
	/** Bound check things **/
	signs.push_back(false);
	signs.push_back(false);
	rythmMaker.start();
}

void keyPressed()
{
	if ( trialMode == PROBEMODE )
	{
		if ( trialNumber==0)
		{	responseFile << setw(6) << left << 
		"# TrialNumber" << "Tilt Slant SlantDir Translation Onset TranslationConstant EyeCal.x EyeCal.y EyeCal.z StimFrames ProbeAngle" << endl;
		}
		stimulusDuration.stop();
		responseFile << setw(6) << left <<
			trialNumber << " " << 
			factors["Tilt"] << " "  <<
			factors["Slant"] << " " <<
			factors["SlantDirection"] << " " <<
			factors["Translation"] << " " <<
			factors["Onset"] << " " <<
			factors["TranslationConstant"] << " " <<
			eyeCalibration.transpose() << " " <<
			stimulusFrames << " " <<
			probeAngle << endl;
		if ( trial.getRemainingTrials()==0 )
		{
			cleanup();
			exit(0);
		}
		else
		{
			factors = trial.getNext();
			trialNumber++;
		}
		trialMode++;
		trialMode=trialMode%3;
		double possibleAngles[]={0,90,180,270};
		probeStartingAngle=possibleAngles[rand()%4];
		redDotsPlane.compute();
		stimDrawer.initList(&redDotsPlane);
	}
}

void initStreams()
{
	inputParameters.open("parametersActiveZFixed.txt");
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
	if ( !boost::filesystem::exists(baseDir) )
		boost::filesystem::create_directory(baseDir);
	// Subject name
	string subjectName = parameters.find("SubjectName");

	// Principal streams file
	string responseFileName =	"responseFile_"	+ subjectName + ".txt";
	string markersFileName =	"markersFile_"	+ subjectName + ".txt";
	string anglesFileName =		"anglesFile_"	+ subjectName + ".txt";
	string trajFileName =		"trajFile_"		+ subjectName + ".txt";

	// Check for output file existence
	/** Response file **/
	if ( !fileExists((baseDir+responseFileName)) )
		responseFile.open((baseDir+responseFileName).c_str());

	if (!fileExists((baseDir+markersFileName)) )
		markersFile.open((baseDir+markersFileName).c_str());

	if (!fileExists((baseDir+anglesFileName)) )
		anglesFile.open((baseDir+anglesFileName).c_str());

	if (!fileExists((baseDir+trajFileName)) )
		trajFile.open((baseDir+trajFileName).c_str());
	
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
	if ( trialMode == STIMULUSMODE )
		cam.setEye(eyeRight+translationFactor);
	else
		cam.setEye(eyeRight);
	drawInfo();
	drawTrial();

	// Draw right eye view
	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	if ( trialMode == STIMULUSMODE )
		cam.setEye(eyeLeft+translationFactor);
	else
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
	if ( trialMode == STIMULUSMODE )
		cam.setEye(eyeRight+translationFactor);
	else
		cam.setEye(eyeRight);
	drawInfo();
	drawTrial();
	glutSwapBuffers();
	}
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
{   //Quit program
case 'a':
	val+=1;
	cout << val << endl;
	break;
case 'z':
	val-=1;
	cout << val << endl;
	break;
case 'p':
	passiveMode=!passiveMode;
	break;
case 'q':
case 27:
	{   cleanup();
	exit(0);
	}
	break;
case ' ':
	{
		// Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
		if ( headCalibrationDone==0 && allVisibleHead )
		{
			headEyeCoords.init(markers[17],markers[18], markers[1],markers[2],markers[3],interoculardistance );
			headCalibrationDone=1;
			break;
		}
		// Second calibration, you must look a fixed fixation point
		if ( headCalibrationDone==1 )
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1], markers[2],markers[3],interoculardistance );
			eyeCalibration=headEyeCoords.getRightEye();
			headCalibrationDone=2;
			break;
		}
		if ( headCalibrationDone==2 )
		{   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1], markers[2],markers[3],interoculardistance );
		eyeCalibration=headEyeCoords.getRightEye();
		break;
		}
	}
	break;
	// Enter key: press to make the final calibration
case 13:
	{
		if ( headCalibrationDone == 2)
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1], markers[2],markers[3],interoculardistance );
			eyeCalibration=headEyeCoords.getRightEye();
			headCalibrationDone=3;
			cout << eyeCalibration.transpose() << endl;
		}
	}
	break;

case '2':
	{
		probeAngle=270;
		keyPressed();
	}
	break;
case '8':
	{
		probeAngle=90;
		keyPressed();
	}
	break;
case '4':
	{
		probeAngle=0;
		keyPressed();
	}
	break;
case '6':
	{
		probeAngle=180;
		keyPressed();
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

void checkBounds()
{
	double maxOscTime = str2num<double>(parameters.find("MaxOscillationTime"));
	double minOscTime = str2num<double>(parameters.find("MinOscillationTime"));
	double maxHeadShift = str2num<double>(parameters.find("MaxHeadShift"));

	if ( (headCalibrationDone > 2) && (trialMode!=PROBEMODE) )
	{
		conditionInside = abs(eyeRight.z()-eyeCalibration.z()) < maxHeadShift;
		if ( !( conditionInside || (!wasInside) ) )
		{
			signs.pop_back();
			signs.push_front( (eyeRight.z()-eyeCalibration.z()) > 0 );
			rythmMaker.stop();
			if ( signs.front() != signs.back() )
			{   
				if ( trialMode == STIMULUSMODE )
				{
					boost::thread beepOkThread(beepOk);
					beepOkThread.detach();
					sumOutside++;
				}
				double timeElapsed = rythmMaker.getElapsedTimeInMilliSec();
				if ( (timeElapsed > minOscTime ) && (timeElapsed < maxOscTime ) && (trialMode==FIXATIONMODE) )
				{
					boost::thread beepOkThread(beepOk);
					beepOkThread.detach();
					if ( factors["Onset"]*optotrak.getAllVelocities()[3].z() > 0 )
						sumOutside++;
				}
				if ( (timeElapsed < minOscTime ) && ( trialMode==FIXATIONMODE ) )
				{
					boost::thread tweeterThread(tweeter);
					tweeterThread.detach();
					sumOutside=0;
				}
				if ( (timeElapsed > maxOscTime ) && ( trialMode==FIXATIONMODE ) )
				{
					boost::thread wooferThread(woofer);
					wooferThread.detach();
					sumOutside=0;
				}
			}
			rythmMaker.start();
		}
		wasInside = conditionInside;
	}
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screen.setOffset(alignmentX,alignmentY);
	screen.setFocalDistance(_focalDist);
	screen.transform(_transformation);
	cam.init(screen);
}

void update(int value)
{
	// Conta i cicli di presentazione dello stimolo
	if ( (sumOutside > str2num<int>(parameters.find("StimulusCycles")) ) &&  (trialMode == STIMULUSMODE) )
	{
		sumOutside=0;
		trialMode++;
		trialMode=trialMode%3;
	}

	if (conditionInside && (sumOutside*2 > str2num<int>(parameters.find("FixationCycles"))) && (trialMode ==FIXATIONMODE )  )
	{
		sumOutside=0;
		trialMode++;
		trialMode=trialMode%3;
		stimulusDuration.start();
	}
	if ( trialMode == STIMULUSMODE )
		stimulusFrames++;
	if ( trialMode == FIXATIONMODE )
		stimulusFrames=0;

	// Coordinates picker
	markers = optotrak.getAllPoints();
	headEyeCoords.update(markers[1],markers[2],markers[3]);
	Affine3d active = headEyeCoords.getRigidStart().getFullTransformation();

	eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );

	eyeLeft = headEyeCoords.getLeftEye();
	eyeRight = headEyeCoords.getRightEye();

	cyclopeanEye = (eyeLeft+eyeRight)/2.0;

	// Projection of view normal on the focal plane
	Vector3d directionOfSight = (active.rotation()*Vector3d(0,0,-1)).normalized();
	Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( eyeRight , eyeRight+directionOfSight );
	Eigen::ParametrizedLine<double,3> lineOfSightLeft  = Eigen::ParametrizedLine<double,3>::Through( eyeLeft, eyeLeft+directionOfSight );
	
	double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);
	double lineOfSightLeftDistanceToFocalPlane = lineOfSightLeft.intersection(focalPlane);
	
	projPointEyeRight = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
	projPointEyeLeft= lineOfSightLeftDistanceToFocalPlane * (directionOfSight) + (eyeLeft);

	// Compute the translation to move the eye in order to avoid share components
	Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight -eyeCalibration);
	switch ( (int)factors["Translation"] )
	{
	case 0:
		translationFactor.setZero();
		break;
	case 1:
		translationFactor = factors["TranslationConstant"]*Vector3d(posAlongLineOfSight.z(),0,0);
		break;
	case 2:
		translationFactor = factors["TranslationConstant"]*Vector3d(0,posAlongLineOfSight.z(),0);
		break;
	}

	if ( passiveMode )
		initProjectionScreen(0,headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance)-eyeCalibration));
	else
		initProjectionScreen(focalDistance,Affine3d::Identity());
	
	checkBounds();

	/**** Save to file part ****/
	// Write down frame by frame the trajectories and angles of eyes and head
	if ( trialMode == STIMULUSMODE && headCalibrationDone > 2 )
	{
		trajFile << setw(6) << left << 
			trialNumber << " " << 
			stimulusFrames << " " <<
			eyeRight.transpose() << endl;

		anglesFile << setw(6) << left << 
			trialNumber << " " << 
			stimulusFrames << " " <<
			eulerAngles.getPitch() << " " <<
			eulerAngles.getRoll() << " " << 
			eulerAngles.getYaw() << endl;
	}
	// Markers file save the used markers and time-depending experimental variable to a file 
	// (Make sure that in passive experiment the list of variables has the same order)
	markersFile << trialNumber << " " << headCalibrationDone << " " << trialMode << " " ;
	markersFile <<markers[1].transpose() << " " << markers[2].transpose() << " " << markers[3].transpose() << " " << markers[17].transpose() << " " << markers[18].transpose() << " " ;

	markersFile <<	factors["Tilt"] << " " << 
					factors["Slant"] << " " << 
					factors["SlantDirection"] << " " << 
					factors["Translation"] << " " << 
					factors["Onset"] << " " << 
					factors["TranslationConstant"] <<
					endl; 

	// Creazione matrice passiva
	Affine3d passive = headEyeCoords.getRigidStart().getFullTransformation()* Translation3d(Vector3d(0,0,focalDistance)-eyeCalibration) ;
	ofstream outputfile;
	outputfile.open("data.dat");
	outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
	outputfile << "Passive matrix:" << endl << passive.matrix() << endl;
	outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << "\tPitch: " << toDegrees(eulerAngles.getPitch()) << endl;
	outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().transpose() << endl;
	outputfile << "EyeRight: " << headEyeCoords.getRightEye().transpose() << endl << endl;
	outputfile << "Factors:" << endl;
	for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
	{
		outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
	}
	outputfile << "Trial remaining: " << trial.getRemainingTrials()+1 << endl;

	// now rewind the file
	outputfile.clear();
	outputfile.seekp(0,ios::beg);

	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

void initOptotrak()
{   optotrak.setTranslation(calibration);
optotrak.setMarkersToSave(vlist_of<unsigned int>(1)(2)(3)(4)(17)(18));
if ( optotrak.init(18,NULL,NULL,NULL) != 0)
{   cleanup();
exit(0);
}
boost::thread optothread( &Optotrak::startRecording, &optotrak);
optothread.detach();
}

int main(int argc, char*argv[])
{
// Initializes the optotrak and starts the collection of points in background
initOptotrak();
glutInit(&argc, argv);
if (stereo)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
else
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

if (gameMode==false)
{   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
glutCreateWindow("EXP WEXLER");
glutFullScreen();
}
else
{   glutGameModeString("1024x768:32@60");
glutEnterGameMode();
glutFullScreen();
}
initRendering();
initProjectionScreen(focalDistance,Affine3d::Identity());
initStreams();
initVariables();

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
