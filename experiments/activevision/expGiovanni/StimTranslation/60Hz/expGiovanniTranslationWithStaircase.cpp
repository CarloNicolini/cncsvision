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
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "EulerExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BoundChecker.h"
#include "ParametersLoader.h"

#include "Util.h"
#include "TrialGenerator.h"

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
using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam,camPassive;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
static const double focalDistance= -568.5;
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),center );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight,cyclopeanEye,projPointEyeRight,projPointEyeLeft,projPointCyclopeanEye, eyeCalibration;
vector <Marker> markers;
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* Timing variables  ************************/
// Timing variables
Timer rythmMaker;
Timer stimulusDuration;
Timer frameTimer;
Timer responseTimer;
/********* RIGID BODIES VARIABLES *********/
EulerExtractor eulerAngles;

/*** Streams File ***/
ofstream markersFile;
ofstream responseFile;
ofstream anglesFile;
ofstream trajFile;
ofstream matrixFile;
ofstream stimFile;
ofstream velocityFile;
ofstream projPointFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
int answer=0;
int headCalibrationDone=0;
bool allVisibleHead=false;
bool passiveMode=false;
Affine3d objectActiveTransformation=Affine3d::Identity();
Affine3d objectPassiveTransformation=Affine3d::Identity();
// Trial related things
int trialNumber=0;
ParametersLoader parameters;
TrialGenerator<double> trial;
map <string, double> factors;
pair<map <string, double>,ParStaircase*> factorStaircasePair;
bool conditionInside=true;
bool wasInside=true;
int sumOutside=0;
int stimulusFrames=0;
double stimulusTime=0;
int timeInsideCircle=0;
deque<bool> signs;
double instantPlaneSlant=45;
bool orthographicMode=false;
bool stimOutside=false;

void drawFixation();
void idle();

void drawCircle(double radius, double x, double y, double z)
{
glBegin(GL_LINE_LOOP);
double deltatheta=toRadians(5);
for (double i=0; i<2*M_PI; i+=deltatheta)
	glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
glEnd();
}
#ifdef _WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
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

void beepBad()
{
	boost::mutex::scoped_lock lock(beepMutex);
	Beep(200,200);
	Beep(400,200);
	Beep(200,200);
}
void outsideBeep()
{
	boost::mutex::scoped_lock lock(beepMutex);
	Beep(660,66);
	Beep(330,33);
	return;
}
#endif
static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
static const int CALIBRATIONMODE=3;
int trialMode = FIXATIONMODE;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
	markersFile.close();
	anglesFile.close();
	trajFile.close();
	matrixFile.close();
	stimFile.close();
	// Stop the optotrak
	optotrak.stopCollection();
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
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
			glPrintText(10, SCREEN_HEIGHT-40,SCREEN_WIDTH,SCREEN_HEIGHT, "EL  " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()) );
			glPrintText(10, SCREEN_HEIGHT-60,SCREEN_WIDTH,SCREEN_HEIGHT, "ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()) );
			glPrintText(10, SCREEN_HEIGHT-80,SCREEN_WIDTH,SCREEN_HEIGHT, "EC" + stringify<int>(cyclopeanEye.x())+" " + stringify<int>(cyclopeanEye.y())+" " + stringify<int>(cyclopeanEye.z()));
			glPrintText(10, SCREEN_HEIGHT-100,SCREEN_WIDTH,SCREEN_HEIGHT, "Dist " + stringify<int>(cyclopeanEye.z()-focalDistance));
			glPrintText(10, SCREEN_HEIGHT-120,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
			glPrintText(10, SCREEN_HEIGHT-140,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
			glPrintText(10, SCREEN_HEIGHT-160,SCREEN_WIDTH,SCREEN_HEIGHT, "ROLL " + stringify<int>(toDegrees(eulerAngles.getRoll())));
			glPrintText(10, SCREEN_HEIGHT-180,SCREEN_WIDTH,SCREEN_HEIGHT, "Press SPACEBAR to calibrate again or ENTER to confirm calibration.");
			glPrintText(10, SCREEN_HEIGHT-200,SCREEN_WIDTH,SCREEN_HEIGHT, "Delta " + stringify<int>(eyeRight.z()- eyeCalibration.z()));
			Vector3d angles = headEyeCoords.getRigidStart().getFullTransformation().rotation().eulerAngles(0,1,2);
			
			glPrintText(10, SCREEN_HEIGHT-220,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
			glPrintText(10, SCREEN_HEIGHT-240,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
			if ( !passiveMode )
				glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "Active");
			else
				glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "Passive");
			glPrintText(10, SCREEN_HEIGHT-280,SCREEN_WIDTH,SCREEN_HEIGHT, "OBJ " + stringify<int>(projPointEyeRight.x() ) + " " + stringify<int>(projPointEyeRight.y() ) + " " + stringify<int>(projPointEyeRight.z()) );
			glPrintText(10, SCREEN_HEIGHT-300,SCREEN_WIDTH,SCREEN_HEIGHT,"Slant= " + stringify<int>(factors["Slant"]) + " " + stringify<int>((instantPlaneSlant)));
			glPrintText(10, SCREEN_HEIGHT-320,SCREEN_WIDTH,SCREEN_HEIGHT, "GlassesL" + stringify<int>(markers[17].p.x() ) + " " + stringify<int>(markers[17].p.y() ) + " " + stringify<int>(markers[17].p.z()) );
			glPrintText(10, SCREEN_HEIGHT-340,SCREEN_WIDTH,SCREEN_HEIGHT, "GlassesR" + stringify<int>(markers[18].p.x() ) + " " + stringify<int>(markers[18].p.y() ) + " " + stringify<int>(markers[18].p.z()) );
	}
		break;
	}
	/*
	glPrintText(10, SCREEN_HEIGHT-360,SCREEN_WIDTH,SCREEN_HEIGHT, "StimTime " + stringify<int>(stimulusDuration.getElapsedTimeInMilliSec() ) );
		glPrintText(10, SCREEN_HEIGHT-380,SCREEN_WIDTH,SCREEN_HEIGHT, "RespTime " + stringify<int>(responseTimer.getElapsedTimeInMilliSec() ) );
	*/
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopMatrix();
	// end if ( headCalibrationDone )
}


void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glPushMatrix();     // PUSH MATRIX
	glLoadIdentity();
	glMultMatrixd(objectActiveTransformation.data());
	/*
	switch ( (int) factors["Tilt"] )
    {
    case 0:
        glRotated( instantPlaneSlant ,0,1,0);
        //objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitY() );
        glScaled(1/sin(toRadians( -90-factors["Slant"])),1,1);	//backprojection phase
        break;
    case 90:
        glRotated( -instantPlaneSlant ,1,0,0);
        //objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitX() );
        glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
        break;
    case 180:
        glRotated( -instantPlaneSlant ,0,1,0);
        //objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitY() );
        glScaled(1/sin(toRadians( -90-factors["Slant"] )),1,1); //backprojection phase
        break;
    case 270:
        glRotated( instantPlaneSlant ,1,0,0);
        //objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitX() );
        glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
        break;
    }
	*/
	glGetDoublev(GL_MODELVIEW_MATRIX,objectActiveTransformation.data());
	BoundChecker stimBoundariesActive(&cam, &redDotsPlane);
	BoundChecker stimBoundariesPassive(&camPassive, &redDotsPlane);
	
	stimOutside = ( stimBoundariesActive.checkOutside(objectActiveTransformation) || stimBoundariesPassive.checkOutside(objectActiveTransformation));
	stimDrawer.draw();
	glPopMatrix();	// POP MATRIX
}

void drawProbe()
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glPointSize(5);

	double circleRadius=2.5;
	glColor3fv(glRed);
	glBegin(GL_POINTS);
	glVertex3d(0,0,focalDistance);
	glEnd();
	glPointSize(1);
	
}

void drawCalibration()
{
	double circleRadius=5.0;
	glColor3fv(glRed);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3d(0,0,focalDistance);
	glVertex3dv(projPointEyeRight.data());
	glEnd();
	glPointSize(1);
	// Draw the calibration circle
	if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius )
	{	timeInsideCircle++;
		glColor3fv(glGreen50);
		drawCircle(circleRadius,0,0,focalDistance);
	if ( timeInsideCircle > 20 )
	{	
		trialMode=FIXATIONMODE;
		boost::thread okBeep( beepOk );
		okBeep.detach();
		sumOutside=0;
		rythmMaker.start();	// XXX
		signs.front()=false;	// XXX 
		signs.back()=true;	// XXX
		timeInsideCircle=0;
	}
	}
	else
	{	glColor3fv(glRed);
		drawCircle(circleRadius,0,0,focalDistance);
	}	
}

void drawFixation()
{
	switch ( headCalibrationDone )
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
		glColor3fv(glBlue);
		glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
		glEnd();
		glPointSize(1);
	
		// Draw the calibration circle
		glColor3fv(glWhite);
		drawCircle(3,0,0,focalDistance);
		break;
	
	case 3:
		// DRAW THE FIXATION POINT 
		double eyeToCenterAngleX= toDegrees(atan(eyeRight.x()/(-focalDistance-eyeRight.z()) ));
		double eyeToCenterAngleY= toDegrees(atan(eyeRight.y()/(-focalDistance-eyeRight.z()) ));
		double projPointAngleX = toDegrees( atan( (projPointEyeRight.x()-eyeRight.x())/abs(projPointEyeRight.z())));
		double maxAllowedTranslationYaw = str2num<double>(parameters.find("MaxAllowedTranslationYaw"));

		if ( ((int) factors["Rotation"]) == 0 )
		{	
			if ( abs(projPointAngleX)< maxAllowedTranslationYaw )	
				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}
		if ( ((int) factors["Rotation"]) == 1 )
		{
			if ( abs(eyeToCenterAngleY) < maxAllowedTranslationYaw )
				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}
		if ( ((int) factors["Rotation"]) == 2 )
		{
			if ( abs(eyeToCenterAngleX) < maxAllowedTranslationYaw )
				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}

	glPushMatrix();
	glTranslated(projPointEyeRight.x(),projPointEyeRight.y(),projPointEyeRight.z());
	glutSolidSphere(1,10,10);
	glPopMatrix();
	break;	
	}
	
}

void drawTrial()
{   switch ( trialMode )
{
case FIXATIONMODE:
	{   
		drawFixation();
	}
	break;
case PROBEMODE :
	{
		//drawProbe();
	}
	break;
case STIMULUSMODE:
	{
		responseTimer.start();
		drawRedDotsPlane();
	}
	break;
case CALIBRATIONMODE:
	{
		drawCalibration();
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
	// Create the factors-staircase object TrialGenerator
	trial.init(parameters);
    factorStaircasePair = trial.getCurrent();
	cout << "First trial with staircase value= " << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;
    factors = factorStaircasePair.first;
   
	redDotsPlane.setNpoints(10);  //XXX controllare densita di distribuzione dei punti
	redDotsPlane.setDimensions(0.1,0.1,0.1);

	redDotsPlane.compute();
	//stimDrawer.drawSpecialPoints();
	stimDrawer.setStimulus(&redDotsPlane);
	stimDrawer.setSpheres(true);
	stimDrawer.setRadius(5);
	stimDrawer.initList(&redDotsPlane);
	/** Bound check things **/
	signs.push_back(false);
	signs.push_back(true);
	rythmMaker.start();
	stimulusDuration.start();
	responseTimer.start();
}

void keyPressed()
{
	if ( trialMode == PROBEMODE )
	{
		if ( trial.isEmpty() )
		{
			cleanup();
			exit(0);
		}
		if ( trialNumber==0)
		{	
		responseFile << "TrialNumber Rotation FollowingSpeed Onset EyeCalx EyeCaly EyeCalz StimFrames StimDuration ResponseTime Answer" << endl;
		}
			responseFile << fixed << 
			trialNumber << " " << 
			factors["Rotation"] << " " <<
			factorStaircasePair.second->getCurrentStaircase()->getID() << " " <<
			factorStaircasePair.second->getCurrentStaircase()->getState() << " " <<
			factorStaircasePair.second->getCurrentStaircase()->getInversions() << " " <<
			factors["Onset"] << " " <<
			eyeCalibration.transpose() << " " <<
			stimulusFrames << " " <<
			stimulusTime << " " <<
			responseTimer.getElapsedTimeInMilliSec() << " " << 
			answer << endl;
			// go to next trial things
			responseTimer.start();
			stimulusDuration.start();
			trial.next(answer);
			factors = trial.getCurrent().first;
			trialNumber++;
		
		trialMode++;
		trialMode=trialMode%4;
		redDotsPlane.compute();
		stimDrawer.initList(&redDotsPlane);

		signs.clear();
		signs.push_back(false);
		signs.push_back(true);
		idle();
		rythmMaker.start();
		boost::thread beepOkThread(beepOk);
	}
}

void initStreams()
{
	string parametersFileName = "parametersActiveTranslationStaircase.txt";
	inputParameters.open(parametersFileName.c_str());
	if ( !inputParameters.good() )
	{
		cerr << "File " << parametersFileName  << "doesn't exist" << endl;
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
	string responseFileName =	"responseFile_"	+ subjectName + ".txt";
	string markersFileName =	"markersFile_"	+ subjectName + ".txt";
	string anglesFileName =		"anglesFile_"	+ subjectName + ".txt";
	string trajFileName =		"trajFile_"		+ subjectName + ".txt";
	string matrixFileName =		"matrixFile_"	+ subjectName + ".txt";
	string stimFileName	  =		"stimFile_"		+ subjectName + ".txt";
	string velocityFileName = "velocityFile_"+subjectName +".txt";
	string projPointFileName = "projPointFile_" + subjectName + ".txt";
	
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
	
	if ( !fileExists(baseDir+matrixFileName))
		matrixFile.open((baseDir+matrixFileName).c_str());

	if ( !fileExists(baseDir+stimFileName))
		stimFile.open((baseDir+stimFileName).c_str());

	if ( !fileExists(baseDir+velocityFileName))
		velocityFile.open((baseDir+velocityFileName).c_str());
		
	if ( !fileExists(baseDir+projPointFileName))
		projPointFile.open((baseDir+projPointFileName).c_str());
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
	//if ( (int) factors["Rotation"] == 0 )	//vecchia scelta, quella corretta è mantenere l'occhio come COP
		//cam.setEye(Vector3d(eyeRight.x()*(factorStaircasePair.second->getCurrentStaircase()->getState()),eyeRight.y(),eyeRight.z()));
	drawInfo();
	drawTrial();
	glutSwapBuffers();
	}
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
{   //Quit program
case 'o':
	orthographicMode=!orthographicMode;
	cam.setOrthoGraphicProjection(orthographicMode);
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
			headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
			headCalibrationDone=1;
			break;
		}
		// Second calibration, you must look a fixed fixation point
		if ( headCalibrationDone==1 )
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
			eyeCalibration=headEyeCoords.getRightEye();
			headCalibrationDone=2;
			break;
		}
		if ( headCalibrationDone==2 )
		{   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
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
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
			eyeCalibration=headEyeCoords.getRightEye();
			headCalibrationDone=3;
		}
	}
	break;
case '4':
	{
		if ( trialMode == PROBEMODE )
		{
		answer=false;
		keyPressed();
		cout << "Trying with staircase value= " << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;
		}
	}
	break;
case '6':
	{
		if ( trialMode == PROBEMODE )
		{
		answer=true;
		keyPressed();
		cout << "Trying with staircase value= " << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;
		}
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

/* OLD CHECKBOUNDS FOR TRANSLATIONS
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
	
	if ( (stimOutside) && (trialMode==STIMULUSMODE) )
	{
		outsideBeep();
		stimOutside=false;
		trialMode=CALIBRATIONMODE;
		sumOutside=0;
	}
}
*/
void checkBoundsRotationYaw()
{  
	double eyeToCenterAngleX= toDegrees(atan(eyeRight.x()/(-focalDistance-eyeRight.z()) ));
	double eyeToCenterAngleY= toDegrees(atan(eyeRight.y()/(-focalDistance-eyeRight.z()) ));
	double projPointAngleX = toDegrees( atan( (projPointEyeRight.x()-eyeRight.x())/abs(projPointEyeRight.z()) ));
	//cerr << fixed << eyeToCenterAngleX << " " << projPointAngleX  << endl;

	double maxOscTime = str2num<double>(parameters.find("MaxOscillationTime"));
	double minOscTime = str2num<double>(parameters.find("MinOscillationTime"));
	double maxOscillation=str2num<double>(parameters.find("MaxOscillation"));
	double maxAllowedTranslationYaw = str2num<double>(parameters.find("MaxAllowedTranslationYaw"));

	if ( (headCalibrationDone > 2) && (trialMode!=PROBEMODE) )
	{
		// fixation dentro area 12 cm
		double val=0;
		if ( (int) factors["Rotation"] == 2 || (int) factors["Rotation"] == 0)
			val = (projPointEyeRight.x());
		
		if ( (int) factors["Rotation"] == 1 )
			val = (projPointEyeRight.y());

		conditionInside = abs(val) <= (maxOscillation); //50 to avoid that 1.5 speed stimulus goes outside
		// If this condition is met, then this means that the point is outside the screen area!
		if ( !( conditionInside || (!wasInside) ) )
		{
			signs.pop_back();
			signs.push_front( val > 0 );
			rythmMaker.stop();
			if ( signs.front() != signs.back() )
			{	// se siam in stimulusMode bippa sempre quando vengon raggiunti i limiti
				if ( trialMode == STIMULUSMODE )
				{
					boost::thread beepOkThread(beepOk);
					beepOkThread.detach();
					sumOutside++;
				}

					 double timeElapsed = rythmMaker.getElapsedTimeInMilliSec();
				if ( (timeElapsed > minOscTime) && (timeElapsed < maxOscTime) && (trialMode!=STIMULUSMODE && (trialMode!=CALIBRATIONMODE)) )
				{
				boost::thread beepOkThread(beepOk);
				beepOkThread.detach();
				switch ( (int) factors["Rotation"] )
				{//conta un ciclo fatto solo se ha fatto una pura traslazione, cioè tiene -3<yaw<3
				case 0:
				{
				if ( (factors["Onset"]*markers[3].v.x() < 0))
				{	// prima era eulerAngles.getYaw()
					if (( abs(projPointAngleX)< maxAllowedTranslationYaw )&& (sumOutside>0))
						sumOutside++;
					else 
						sumOutside=0;
				} else 
					{
					if ( abs(projPointAngleX)< maxAllowedTranslationYaw )
						sumOutside++;
					else
						sumOutside=0;
					}
				}
				break;
				case 1:
				{
					if ( (factors["Onset"]*markers[3].v.y() > 0))
					{
					if (( abs(eyeToCenterAngleY) < maxAllowedTranslationYaw )&& (sumOutside>0))
						sumOutside++;
					else
						sumOutside=0;
					} else 
						{
					if ( abs(eyeToCenterAngleY) < maxAllowedTranslationYaw )
						sumOutside++;
					else
						sumOutside=0;
						}
				}
				break;
				case 2:
				{

					if ((factors["Onset"]*markers[3].v.x() > 0))
					{
					if (( abs(eyeToCenterAngleX) < maxAllowedTranslationYaw ) && (sumOutside>0)) // doppiamente MONA: forsa ad iniziare a conteggiare quando lo spostamento è solo da una parte (a seconda di ONSET)
						sumOutside++;
					else
						sumOutside=0;
						} else // necessario per far contare tutte le volte che passa da un'estremo
						{
					if ( abs(eyeToCenterAngleX) < maxAllowedTranslationYaw )
						sumOutside++;
					else
						sumOutside=0;
						}
					
				}
				break;
				}
				}

				if ( (timeElapsed < minOscTime) && ( trialMode==FIXATIONMODE ) )
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
		
	if ( (stimOutside) && (trialMode==STIMULUSMODE) )
	{
		outsideBeep();
		stimOutside=false;
		trialMode=CALIBRATIONMODE;
		sumOutside=0;
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

Vector2d getPlaneDimensions()
	{
	Point3D *pUpCenter = redDotsPlane.specialPointsRand[5];
	Point3D *pLowCenter = redDotsPlane.specialPointsRand[6];

	Point3D *pRightCenter = redDotsPlane.specialPointsRand[7];
	Point3D *pLeftCenter = redDotsPlane.specialPointsRand[8];

	Vector3d vUpCenter = objectActiveTransformation*Vector3d( pUpCenter->x, pUpCenter->y, pUpCenter->z);
	Vector3d vLowCenter = objectActiveTransformation*Vector3d( pLowCenter->x, pLowCenter->y, pLowCenter->z);
	Vector3d vRightCenter = objectActiveTransformation*Vector3d( pRightCenter->x, pRightCenter->y, pRightCenter->z);
	Vector3d vLeftCenter = objectActiveTransformation*Vector3d( pLeftCenter->x, pLeftCenter->y, pLeftCenter->z);
	
	double junkz=0;
	double vUpCenterWinx,vUpCenterWiny, vLowCenterWinx,vLowCenterWiny, vRightCenterWinx,vRightCenterWiny, vLeftCenterWinx,vLeftCenterWiny;
	
	gluProject(vUpCenter.x(),vUpCenter.y(),vUpCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vUpCenterWinx,&vUpCenterWiny,&junkz);
	gluProject(vLowCenter.x(),vLowCenter.y(),vLowCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLowCenterWinx,&vLowCenterWiny,&junkz);

	gluProject(vRightCenter.x(),vRightCenter.y(),vRightCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vRightCenterWinx,&vRightCenterWiny,&junkz);
	gluProject(vLeftCenter.x(),vLeftCenter.y(),vLeftCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLeftCenterWinx,&vLeftCenterWiny,&junkz);

	double planeHeight = abs( vUpCenterWiny - vLowCenterWiny);
	double planeWidth = abs( vLeftCenterWinx - vRightCenterWinx );

	return Vector2d(planeHeight,planeWidth);
	}

void update(int value)
{
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
frameTimer.start();
double optotime = optotrak.updateMarkers();
markers = optotrak.getAllMarkers();
	if (conditionInside && (sumOutside > str2num<int>(parameters.find("FixationCycles"))) && (trialMode ==FIXATIONMODE )  )
	{
		sumOutside=0;
		trialMode++;
		trialMode=trialMode%4;
		stimulusDuration.reset();
		stimulusDuration.start();
	}	

	// Conta i cicli di presentazione dello stimolo
	if ( (sumOutside > str2num<int>(parameters.find("StimulusCycles")) ) &&  (trialMode == STIMULUSMODE) )
	{
		sumOutside=0;
		trialMode++;
		trialMode=trialMode%4;
		stimulusDuration.stop();
		stimulusTime = stimulusDuration.getElapsedTimeInMilliSec();
	}

	if ( trialMode == STIMULUSMODE )
		stimulusFrames++;
	
	if ( trialMode == FIXATIONMODE )
		stimulusFrames=0;

	Screen screenPassive;
	screenPassive.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screenPassive.setOffset(alignmentX,alignmentY);
	screenPassive.setFocalDistance(0);
	screenPassive.transform(headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(center));
	camPassive.init(screenPassive);
	camPassive.setDrySimulation(true);
	camPassive.setEye(eyeRight);
	objectPassiveTransformation = ( camPassive.getModelViewMatrix()*objectActiveTransformation );
	// Coordinates picker
	if ( isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) )
		headEyeCoords.update(markers[1].p,markers[2].p,markers[3].p);
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
	// second projection the fixation point computed with z non constant but perfectly parallel to projPointEyeRight
	lineOfSightRightDistanceToFocalPlane= (( active.rotation()*(center)) - eyeRight).norm();
	Vector3d secondProjection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
	// da mettere se si vuole z sul piano focale o meno
	projPointEyeRight=secondProjection ;

	// Compute the translation to move the eye in order to avoid share components
	Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight -eyeCalibration);
	// XXX
	if ( trialMode == STIMULUSMODE )
	{
	// IMPORTANT Reset the previous status of transformation
	objectActiveTransformation.setIdentity();
	double followingSpeed = factorStaircasePair.second->getCurrentStaircase()->getState();
	double xStim = followingSpeed*(projPointEyeRight.x() + str2num<double>(parameters.find("MaxOscillation")) );
	objectActiveTransformation.translation() = Vector3d(xStim,0,focalDistance);
		}
	// end XXX
	if ( passiveMode )
		initProjectionScreen(0,headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance)));
	else
		initProjectionScreen(focalDistance,Affine3d::Identity());
	
	checkBoundsRotationYaw();

	double followingSpeed = factorStaircasePair.second->getCurrentStaircase()->getState();
	/**** Save to file part ****/
	// Markers file save the used markers and time-depending experimental variable to a file 
	// (Make sure that in passive experiment the list of variables has the same order)
	markersFile << fixed << trialNumber << " " << headCalibrationDone << " " << trialMode << " " ;
	markersFile <<markers[1].p.transpose() << " " << markers[2].p.transpose() << " " << markers[3].p.transpose() << " " << markers[17].p.transpose() << " " << markers[18].p.transpose() << " " ;
	
	markersFile <<	fixed << 
					factors["Rotation"] << " " << 
					followingSpeed << " " <<
					factors["Onset"] << " " << 
					endl;
	// XXX inserire la giusta lista di fattori
	// Scrittura su data file da leggere in tempo reale
	ofstream outputfile;
	outputfile.open("data.dat");
	outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
	outputfile << "Passive matrix:" << endl << objectPassiveTransformation.matrix() << endl;
	outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl;
	outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().transpose() << endl;
	outputfile << "EyeRight: " << headEyeCoords.getRightEye().transpose() << endl << endl;
	outputfile << "Slant: " << instantPlaneSlant << endl;
	outputfile << "(Width,Height) [px]: " << getPlaneDimensions().transpose() << " " << endl;
	outputfile << "Factors:" << endl;
	for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
	{
		outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
	}
	outputfile << "Trials done: " << trialNumber << endl;
	outputfile << "Last response: " << answer << endl;
	outputfile << "Sumoutside: " << sumOutside << endl;
	outputfile << "StimCenter: " << objectActiveTransformation.translation().transpose() << endl;
	outputfile << "Staircase ID" << factorStaircasePair.second->getCurrentStaircase()->getID() << " " << factorStaircasePair.second->getCurrentStaircase()->getState() << " " << answer << endl;

	// now rewind the file
	outputfile.clear();
	outputfile.seekp(0,ios::beg);

	// Write down frame by frame the trajectories and angles of eyes and head
	if ( trialMode == STIMULUSMODE && headCalibrationDone > 2 )
	{
		velocityFile << fixed << objectActiveTransformation.translation().transpose() << endl;

		velocityFile << frameTimer.getElapsedTimeInMilliSec() << " " << optotime*1000 << endl;
		trajFile << setw(6) << left << 
			trialNumber << " " << 
			stimulusFrames << " " <<
			eyeRight.transpose() << endl;

		anglesFile << setw(6) << left << 
			trialNumber << " " << 
			stimulusFrames << " " <<
			toDegrees(eulerAngles.getPitch()) << " " <<
			toDegrees(eulerAngles.getRoll()) << " " << 
			toDegrees(eulerAngles.getYaw()) << " " <<
			instantPlaneSlant << endl;

		matrixFile << setw(6) << left <<
			trialNumber << " " << 
			stimulusFrames << " " ;
			for (int i=0; i<3; i++)
				matrixFile << objectPassiveTransformation.matrix().row(i) << " " ;
			matrixFile << endl;

			// Write the 13 special extremal points on stimFile
			stimFile << setw(6) << left <<
			trialNumber << " " << 
			stimulusFrames << " " ;
			double winx=0,winy=0,winz=0;
			
			for (PointsRandIterator iRand = redDotsPlane.specialPointsRand.begin(); iRand!=redDotsPlane.specialPointsRand.end(); ++iRand)
			{   Point3D *p=(*iRand);
			Vector3d v = objectActiveTransformation*Vector3d( p->x, p->y, p->z);
			
			gluProject(v.x(),v.y(),v.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &winx,&winy,&winz);
			stimFile << winx << " " << winy << " " << winz << " ";
			}
			stimFile << endl;
			
		projPointFile << fixed << trialNumber << "\t" << projPointEyeRight.transpose() << endl;	
	}
	
}
void initOptotrak()
{   optotrak.setTranslation(calibration);
if ( optotrak.init("cameraFiles/Aligned20110823",NULL,NULL) != 0)
{   cleanup();
exit(0);
}

for (int i=0; i<10; i++)
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
}
}

int main(int argc, char*argv[])
{
srand(time(0));
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
glutIdleFunc(idle);
glutMainLoop();

cleanup();

return 0;
}
