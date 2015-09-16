// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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
#include <direct.h>
/********* BOOOST MULTITHREADED LIBRARY ****************/
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
/********* INCLUDE CNCSVISION LIBRARY HEADERS **********/
//#include "Optotrak.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "EllipsoidPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "Util.h"

#define BROWN 
#ifdef BROWN
#include "BrownMotorFunctions.h"
#else
#include "RoveretoMotorFunctions.h"
#endif
/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace BrownMotorFunctions;
/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 11                               // 85 hz
#define SCREEN_WIDTH  1024 //1280                 // 1024 pixels
#define SCREEN_HEIGHT 768 //1024                  // 768 pixels
static const double SCREEN_WIDE_SIZE = 306; //360 // 306 millimeters
/********* CALIBRATION ON CHIN REST (18 October 2011) **/
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0;
double homeFocalDistance=-270.0;
static const Vector3d calibration(160,179,-75);
static const Vector3d center(0,0,focalDistance);
Screen screen;
double mirrorAlignment = 0.0;
double screenAlignmentY = 0.0;
double screenAlignmentZ = 0.0;
/********* VARIABLES OBJECTS  **************************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords;
Timer timer;
Timer globalTimer;
clock_t t;
GLUquadric* qobj;
/********* VISUALIZATION AND STIMULI *******************/
//StimulusDrawer stimDrawer;
//CylinderPointsStimulus cylinder;
//EllipsoidPointsStimulus ellipsoid;
// Square
bool isSquareDrawn=false;
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;
/********* EYES AND MARKERS ****************************/
Vector3d eyeLeft, eyeRight, index, thumb, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
vector <Marker> markers;
static double interoculardistance=60.5;
bool headCalibration=false;
/********* VISIBILITY VARIABLES ************************/
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool visibleFingers=false;
/********* STREAMS *************************************/
ofstream responseFile, trialFile;

/*************************************************************************************/
/*** Everything above this point stays more or less the same between experiments.  ***/
/*************************************************************************************/

/********* VARIABLES THAT CHANGE IN EACH EXPERIMENT *************************/

// Graphics modes
static const bool gameMode = true;
static const bool stereo = true;

// centercal is typically used as a reference point for moving the motors.
// It should correspond to the starting position (after homeEverything) of the point of interest.
Vector3d centercal(16.5,-127.0,-285.6);//updated 10/6/14 //(29.75,-133.94,-296.16); //updated 9/25/14
//static const Vector3d centercal ...
// Variables for counting trials, frames, and lost frames
int trialNumber = 0;
double frameN = 0; // needs to be a double for division
double TGA_frame = 0; // needs to be a double for division
double start_frame = 0;
double num_lost_frames = 0; // needs to be a double for division
int fingersOccluded = 0;
int attempt = 1;

// Flags for important states in the experiment
bool training=false;
bool handAtStart = true;
bool started=false;
bool reachedObject = false;
bool finished = false;

// Trial modes can be used to create variation outside of parameters
int trialMode = 0;
int numTrialModes = 1;


//const int num_blocks = 7; 
//int block = 0; // remember this is zero-indexed, but num_blocks is a count!
//bool newBlock = false;
//vector<int> block_order;
bool burn_in = true;
int burn_count = 1;


// Experimental variables
ParametersLoader parameters; //from parameters file
BalanceFactor<double> trial; //constructed based on parameters file
//ParametersLoader parameters[num_blocks]; //from parameters file
//BalanceFactor<double> trial[num_blocks]; //constructed based on parameters file
map <std::string, double> factors; //each trial contains a factors list

// Boundaries of the starting position
double startPosTop;
double startPosFront;
double startPosRight;
double startPosLeft;
double startPosRear;
double startPosBottom;

// Position variables for keeping track of distances
double grip_Origin_X;
double grip_Origin_Y;
double grip_Origin_Z;
double grip_aperture;
double distanceGripCenterToObject;
double distanceBetweenSurfaces;
double x_dist = 999;
double y_dist = 999;
double z_dist = 999;

// Size and position variables for the target object(s)
int objId=0;
double hapticSize = 60;
double visualSize = 60;
double objDepth = -400;
double objWidth = 20;
double objLeft;
double objTop;
double targetOriginX;
double targetOriginY;
double targetOriginZ;

double xAdjust = 0;
double yAdjust = 0;

// Incremented when stepping thru calibration procedure
// Make sure that drawInfo() and handleKeypress() are in agreement about this variable!
int fingerCalibration = 0;

/********** FUNCTION PROTOTYPES *****/
void advanceTrial();
void beepOk(int tone);
void cleanup();
void drawCircle(double radius, double x, double y, double z);
void drawGLScene();
void drawInfo();
void drawNoFingers();
void drawStimulus();
void drawTrial(int sl, int st);
void drawFingers(double offsetZ);
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void idle();
void initGLVariables();
void initMotors();
void initOptotrak();
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void initRendering();
void initStreams();
void initTrial();
void initVariables();
void update(int value);

/*************************** FUNCTIONS ***********************************/

// First, make sure the filenames in here are correct and that the folders exist.
// If you mess this up, data may not be recorded!
void initStreams()
{
	ifstream parametersFile;
	string paramFn = "C:/workspace/cncsvisioncmake/experimentsbrown/parameters/EvanHapticContext/parametersEvanHapticContext.txt";
	parametersFile.open(paramFn.c_str());
	parameters.loadParameterFile(parametersFile);

	string subjectName = parameters.find("SubjectName");

	// For experiments with multiple blocks:
	/*for(int ii=0; ii<num_blocks; ii++)
	{
		ifstream parametersFile;
		int jj = ii+1;
		string paramFn = "C:/workspace/cncsvisioncmake/experimentsbrown/parameters/EvanHaptic/parametersEvanHaptic"+stringify<int>(jj)+".txt";
		parametersFile.open(paramFn.c_str());
		parameters[ii].loadParameterFile(parametersFile);
	}
	// Subject name - only needs to be set in the **Block 1** parameters file
    string subjectName = parameters[0].find("SubjectName");
	*/

	// Markersfile directory
	string dirName  = "S:/Domini-ShapeLab/evan/fall14-hapticContext/" + subjectName;
	mkdir(dirName.c_str());

	// Principal streams file
	string responseFileName = "S:/Domini-ShapeLab/evan/fall14-hapticContext/" + subjectName + "/exp01_" + subjectName + ".txt";

	// Check for output file existence 
	/// Response file
	if ( !fileExists((responseFileName)) ) {
        responseFile.open((responseFileName).c_str());
	} else {
		exit(0);
	}

	responseFile << fixed << "subjName\ttrialN\ttrialDuration\thapticSize\tobjID\tvisualSize\tobjOriginX\tobjOriginY\tobjOriginZ\tcheckSize" << endl;
	//responseFile << fixed << "subjName\tblockN\ttrialN\ttrialDuration\thapticSize\tvisualSize\tobjOriginX\tobjOriginY\tobjOriginZ\tcheckSize" << endl;
	globalTimer.start();
}

// Edit case 'f' to establish calibration procedure
// Also contains other helpful button presses (ESC for quit, i for info)
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {

	case 'x':
	{
		beepOk(0);
		cout << "Finished?: " << finished << endl;
	}
	break;

	case 'i':
	{
		visibleInfo=!visibleInfo;
	}
	break;

	case 'm':
	{
		interoculardistance += 0.5;
		headEyeCoords.setInterOcularDistance(interoculardistance);
	}
	break;
	
	case 'n':
	{
		interoculardistance -= 0.5;
		headEyeCoords.setInterOcularDistance(interoculardistance);
	}

	case '*':
	{
		 trialMode++;
		 trialMode=trialMode%numTrialModes;
	}
	break;

    case 27:	// ESC
    {   
		cleanup();
        exit(0);
    }
    break;

	case 'f':
	case 'F':
	{
		if ( allVisiblePlatform && fingerCalibration==0 )
		{
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
			objTop = markers[4].p.y();
			objLeft = markers[4].p.x()-13;
			centercal[1] = markers[3].p.y();
			centercal[2] = markers[3].p.z();
			fingerCalibration=1;
			beepOk(0);
			break;
		}
		if ( fingerCalibration==1 && allVisibleFingers )
		{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
			fingerCalibration=2;
			beepOk(0);
			break;
		}
		if ( fingerCalibration==2 && allVisibleFingers )
		{
			startPosLeft = index.x() - 140;
			startPosRight = index.x() + 10;
			startPosBottom = index.y() - 20;
			startPosTop = index.y() + 20;
			startPosFront = index.z() - 20;
			startPosRear = index.z() + 20;
			fingerCalibration=3;
			beepOk(0);
			break;
		}
		if ( fingerCalibration==3 && allVisibleFingers )
		{
			fingerCalibration=4;
			beepOk(0);
			visibleInfo=false;
			factors = trial.getNext();
			//factors = trial[block_order[block]].getNext();
			initTrial();
			break;
		}
	}
	break;

    case '.':
    {  
		advanceTrial();
	}
    break;

	case '1':
	case 'a':
	{
		xAdjust -= 0.1;
		targetOriginX -= 0.1;
	}
	break;

	case '3':
	case 'd':
	{
		xAdjust += 0.1;
		targetOriginX += 0.1;
	}
	break;
	
	case '2':
	case 's':
	{
		yAdjust -= 0.1;
		targetOriginY -= 0.1;
	}
	break;

	case '5':
	case 'w':
	{
		yAdjust += 0.1;
		targetOriginY += 0.1;
	}
	break;
	}
}

// Provide text instructions for calibration, as well as information about status of experiment
void drawInfo()
{
	if (finished)
		visibleInfo = true;

	if ( visibleInfo )
	{
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		GLText text;
		if ( gameMode )
			text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
		else
			text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
		text.enterTextInputMode();

		if (finished) {
			glColor3fv(glWhite);
			text.draw("The experiment is over. Thank you! :)");
		}else{

			switch (fingerCalibration)
			{
			case 0:
			{
				text.draw("Press F to record platform markers");
				break;
			}
			case 1:
			{
				text.draw("Move index and thumb on platform markers to record finger tips, then press F");
				break;
			}
			case 2:
			{
				text.draw("Move index and thumb to starting position, then press F");
				break;
			}
			case 3:
			{
				text.draw("Place fingertips at start + press F.");
				break;
			}
			} // end switch(fingerCalibration)

			text.draw("####### SUBJECT #######");
			text.draw("#");
			text.draw("# Name: " +parameters.find("SubjectName"));
			//text.draw("# Name: " +parameters[0].find("SubjectName"));
			text.draw("# IOD: " +stringify<double>(interoculardistance));
			//text.draw("# Block: " +stringify<double>(block+1));
			text.draw("# Trial: " +stringify<double>(trialNumber));
			text.draw("# Haptic: " +stringify<double>(hapticSize));
			text.draw("# Visual: " +stringify<double>(visualSize));
			if ( abs(mirrorAlignment - 45.0) < 0.2 )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("# Mirror Alignment = " +stringify<double>(mirrorAlignment));
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
			text.draw("# Fixation Z = " +stringify<double>(markers[19].p.x()-120.0)+ " [mm]");
			text.draw("# Fixation X = " +stringify<double>(markers[19].p.z()+363.0)+ " [mm]");
			text.draw("#######################");
			text.draw("Calibration Step= " + stringify<int>(fingerCalibration) );

			glColor3fv(glWhite);
			text.draw("Calibration Platform" );
			if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Index Marker " + stringify<int>(1)
				+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
			text.draw("Thumb Marker " + stringify<int>(2)
				+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
			
			glColor3fv(glWhite);
			text.draw("Haptic Contacts" );
			if ( isVisible(markers[3].p) && isVisible(markers[4].p) )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Bottom Marker " + stringify<int>(3)
				+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
			text.draw("Top Marker " + stringify<int>(4)
				+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );


			if (fingerCalibration!=4){
				glColor3fv(glWhite);
				text.draw(" " );
				text.draw("Index" );
				if ( isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p) )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Marker " + stringify<int>(13)
					+stringify< Eigen::Matrix<double,1,3> > (markers[13].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(14)
					+stringify< Eigen::Matrix<double,1,3> > (markers[14].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(16)
					+stringify< Eigen::Matrix<double,1,3> > (markers[16].p.transpose())+ " [mm]" );

				glColor3fv(glWhite);
				text.draw(" " );
				text.draw("Thumb" );
				if ( isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p) )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Marker " + stringify<int>(15)
					+stringify< Eigen::Matrix<double,1,3> > (markers[15].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(17)
					+stringify< Eigen::Matrix<double,1,3> > (markers[17].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(18)
					+stringify< Eigen::Matrix<double,1,3> > (markers[18].p.transpose())+ " [mm]" );
			}

			glColor3fv(glWhite);
			text.draw("--------------------");
			glColor3fv(glGreen);
			text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
			text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
			glColor3fv(glWhite);
			text.draw("--------------------");
			glColor3fv(glGreen);
			text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
			text.draw("Trial= " + stringify<int>(trialNumber));
			text.draw("Frame= " + stringify<int>(frameN));
			glColor3fv(glWhite);
			text.draw("--------------------");
			glColor3fv(glGreen);
			text.draw("Distance To Object = " + stringify<double>(distanceGripCenterToObject));
			text.draw("Object Size Check = " + stringify<double>(distanceBetweenSurfaces));
			glColor3fv(glWhite);
			if(handAtStart){
				text.draw("Hand is in starting position.");
			}
			if(started){
				text.draw("Grasp in progress...");
			}
			if(reachedObject){
				text.draw("Object reached!");
			}
		}
		text.leaveTextInputMode();
	}
}

// This will be called at 85hz in the main loop
// Not too much to change here usually, sub-functions do the work.
void drawGLScene() 
{
	if (stereo)
    {   glDrawBuffer(GL_BACK);
		// Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawStimulus();
		drawInfo();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawStimulus();
		drawInfo();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawStimulus();
		drawInfo();
        glutSwapBuffers();
    }

}

// Can check for various conditions that might affect how the graphics in here
void drawStimulus()
{
	// don't draw stimulus until calibration is over & not finished & motors have moved
	if (fingerCalibration==4 && !finished && frameN>0) 
	{
		glLoadIdentity();
		drawTrial(18, 5);
		//if (!reachedObject)
		//	drawFingers(0);

		// not sure why I had this in here and not in idle()...
		//if(handAtStart && started)
		//	advanceTrial();
	}
}

// this draws the actual stimulus
void drawTrial(int sl, int st)
{
	//flanker offsets, objId specifies which object is aligned with haptic
	double xOffsetLeft;
	double xOffsetMiddle;
	double xOffsetRight;
	switch (objId){
		case 1:
			xOffsetLeft=     0;
			xOffsetMiddle=  30;
			xOffsetRight=   60;
			break;

		case 2:
			xOffsetLeft=   -30;
			xOffsetMiddle=   0;
			xOffsetRight=   30;
			break;

		case 3:
			xOffsetLeft=   -60;
			xOffsetMiddle= -30;
			xOffsetRight=    0;
			break;
	}

	glColor3fv(glBlack);
	glBegin(GL_QUADS);
		glVertex3f(-80.0f, 200.0f, -400.0f);		// Top Left
		glVertex3f( 80.0f, 200.0f, -400.0f);		// Top Right
		glVertex3f( 80.0f,-200.0f, -400.0f);		// Bottom Right
		glVertex3f(-80.0f,-200.0f, -400.0f);		// Bottom Left
	glEnd();

	double objDpth = objDepth;
	double objHght = visualSize;
	double objTopp = objTop;

	// Left Object
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objLeft+xAdjust+xOffsetLeft, objTopp+yAdjust, objDpth);
	glRotated(theta,1,0,0);
	// The object should change color to indicate that it's been "reached"
	if (!reachedObject && objId==1) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.0, 0.5, 0.0);
	}
	gluCylinder(qobj, 10.0, 10.0, objHght, sl, st); // draw a basic cylinder
	glPopMatrix();

	// Middle Object
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objLeft+xAdjust+xOffsetMiddle, objTopp+yAdjust, objDpth);
	glRotated(theta,1,0,0);
	// The object should change color to indicate that it's been "reached"
	if (!reachedObject && objId==2) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.0, 0.5, 0.0);
	}
	gluCylinder(qobj, 10.0, 10.0, objHght, sl, st); // draw a basic cylinder
	glPopMatrix();

	// Right Object
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objLeft+xAdjust+xOffsetRight, objTopp+yAdjust, objDpth);
	glRotated(theta,1,0,0);
	// The object should change color to indicate that it's been "reached"
	if (!reachedObject  && objId==3) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.0, 0.5, 0.0);
	}
	gluCylinder(qobj, 10.0, 10.0, objHght, sl, st); // draw a basic cylinder
	glPopMatrix();
}

void drawFingers(double offsetZ)
{
	// Compute distance in the sagittal plane (assuming object is at (x,z)=(0,objDepth))
	double indexObjectDistance = sqrt( pow(index.x(), 2) 
		+ pow( index.z() - objDepth , 2) );
	double thumbObjectDistance = sqrt( pow(thumb.x(), 2) 
		+ pow( thumb.z() - objDepth , 2) );
	
	glPushMatrix();
	glLoadIdentity();
	glTranslated(index.x(),index.y(),index.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	glTranslated(thumb.x(),thumb.y(),thumb.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
}

void initTrial()
{
	// initializing all variables
	frameN=0;
	TGA_frame = 0;
	start_frame = 0;
	started = false;
	handAtStart = true;
	reachedObject = false;
	fingersOccluded = 0;
	num_lost_frames = 0;

	// roll on
	drawGLScene();
	// move the screen
	initProjectionScreen(objDepth);

	// Determine visual and haptic sizes based on block & burn-in
	if (!burn_in){
		visualSize = trial.getCurrent()["VisualSize"];
		hapticSize = trial.getCurrent()["HapticSize"];
		int hapticId = hapticSize;
		switch (hapticId){
		case 45:
			objId=1;
			break;
		case 50:
			objId=2;
			break;
		case 55:
			objId=3;
			break;
		}
	} else {
		objId=2;
		visualSize = 60;
		hapticSize = 60;
	}

	// Find center of physical haptic stimulus
	targetOriginX = objLeft+(0.5*objWidth)+xAdjust;
	targetOriginY = objTop-(0.5*hapticSize)+yAdjust;
	targetOriginZ = objDepth;

	// Set physical haptic apparatus with a motor command
	double yPosn = objTop - hapticSize;
	double zPosn = objDepth - 5;
	Vector3d moveTo(0.0,yPosn,zPosn);
	moveObjectAbsolute(moveTo, centercal, 3500);

	// Open a new trial file and give it a header
	string trialFileName = "S:/Domini-ShapeLab/evan/fall14-hapticContext/" 
		+ parameters.find("SubjectName") + "/" 
		+ parameters.find("SubjectName") +"_tr" + stringify<double>(trialNumber) + ".txt";
	
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexXYZraw\tthumbXYZraw\tdistanceToObject\tfingersOccluded\treachedObject" << endl;

	beepOk(2);
	timer.start();
}

// This function handles the transition from the end of one trial to the beginning of the next.
void advanceTrial()
{
	// Assume we shouldn't reinsert the trial[block_order[block]]....
	int overwrite = 0;

	// Compute % missing frames in order to decide whether or not to reinsert trial
	double percent_occluded_frames = num_lost_frames/(TGA_frame-start_frame);
	bool not_enough_frames = percent_occluded_frames > 0.10;
	//cout << num_lost_frames << " " << TGA_frame << " " << start_frame << " " << percent_occluded_frames << endl;

	// Check whether there is some reason to reinsert
	if (burn_in || !reachedObject || not_enough_frames ) {
		beepOk(3);
		if (training)
			cout << "Training Mode!" << endl;
		if (!reachedObject)
			cout << "I don't think you reached the object..." << endl;
		if (not_enough_frames)
			cout << "Be visible!" << endl;
		// Reinsert and set the overwrite flag for respFile
		map<std::string,double> currentFactorsList = trial.getCurrent();
		trial.reinsert(currentFactorsList);
		//map<std::string,double> currentFactorsList = trial[block_order[block]].getCurrent();
		//trial[block_order[block]].reinsert(currentFactorsList);
		overwrite = 1;
	}

	// If we're not reinserting, write trial-specific data to response file
	if (!overwrite)
	{
		responseFile.precision(3);
		responseFile << parameters.find("SubjectName") << "\t" <<
						//block_order[block] << "\t" <<
						trialNumber << "\t" <<
						timer.getElapsedTimeInMilliSec() << "\t" <<
						objId << "\t" <<
						hapticSize << "\t" <<
						visualSize << "\t" <<
						targetOriginX << "\t" << 
						targetOriginY << "\t" << 
						targetOriginZ << "\t" <<
						distanceBetweenSurfaces <<
						endl;
	}
	// Close the trial file
	trialFile.close();

	// If there are more trials in the current block
	if( !trial.isEmpty() ) {
	//if( !trial[block_order[block]].isEmpty() ) {
		// If we're not overwriting due to a bad trial
		if (!overwrite)
			// Increment trial number
			trialNumber++; 
		if (burn_in)
			if (reachedObject)
				burn_count++;
		if (burn_count > 10)
			burn_in = false;
		// Get the next trial and initialize it!
		factors = trial.getNext();
		//factors = trial[block_order[block]].getNext();
		initTrial();

	} else { 
		finished = true;
		responseFile.close();

	//// If we have reached the end of the final block
	//} else if (block == num_blocks-1) { 
	//	finished = true;
	//	responseFile.close();

	//// Otherwise we need to advance block
	//} else {
	//	block++;
	//	trialNumber++;
	//	newBlock=true;
	//	burn_in=true;
	//	burn_count=1;
	//	factors = trial[block_order[block]].getNext();
	//	initTrial();
	}
}


void idle()
{
	// get new marker positions from optotrak
	optotrak->updateMarkers();
	markers = optotrak->getAllMarkers();

	// check visibility
	allVisiblePlatform = isVisible(markers[1].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;
	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[1].p);

	// check equipment alignments
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;
	screenAlignmentY = asin(
			abs((markers[19].p.y()-markers[21].p.y()))/
			sqrt(
			pow(markers[19].p.x()-markers[21].p.x(), 2) +
			pow(markers[19].p.y()-markers[21].p.y(), 2)
			)
			)*180/M_PI;
	screenAlignmentZ = asin(
			abs(markers[19].p.z()-markers[20].p.z())/
			sqrt(
			pow(markers[19].p.x()-markers[20].p.x(), 2) +
			pow(markers[19].p.z()-markers[20].p.z(), 2)
			)
			)*180/M_PI*
			abs(markers[19].p.x()-markers[20].p.x())/
			(markers[19].p.x()-markers[20].p.x());

	// update head coordinates
	if ( allVisiblePatch )
		headEyeCoords.update(markers[5].p,markers[6].p,markers[7].p);

	// update finger coordinates (but we don't really use these directly!)
	if ( allVisibleFingers ) {
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}

	// update the finger position in the objects we actually use
	index = indexCoords.getP1();
	thumb = thumbCoords.getP1();

	//////////////////////////////////////
	// While the experiment is running! //
	//////////////////////////////////////
	if (fingerCalibration==4 && !finished)
	{
		// Check for finger occlusion
		if ( !allVisibleFingers )
		{
			fingersOccluded = 1;
			if (!started)
				beepOk(4);
			if (started && !reachedObject) // only increment if we're in flight
			{
				num_lost_frames += 1;
			}
		}
		
		// Advance frame number
		frameN++;

		// Check that both fingers are in the start position
		if( (index.y() < startPosTop) && // index below ceiling
			(index.y() > startPosBottom) && // index above floor
			(index.x() > startPosLeft) && // index right of left wall
			(index.x() < startPosRight) && // index left of right wall
			(index.z() > startPosFront) &&  // index behind front wall
			(index.z() < startPosRear) && // index in front of rear wall
			(thumb.y() < startPosTop) && // thumb below ceiling
			(index.y() > startPosBottom) && // thumb above floor
			(thumb.x() > startPosLeft) && // thumb right of left wall
			(thumb.x() < startPosRight) && // thumb left of right wall
			(thumb.z() > startPosFront) &&  // thumb behind front wall
			(thumb.z() < startPosRear) // thumb in front of rear wall
			)
		{	// if so, keep resetting timer
			handAtStart = true;
			timer.start();
		} else { // otherwise we are in flight, so set flags and let the timer run
			if (start_frame==0)
				start_frame=frameN;
			handAtStart = false;
			started = true;
		}

		// find distance from grip center to object center
		grip_Origin_X = (index.x()+thumb.x())/2;
		grip_Origin_Y = (index.y()+thumb.y())/2;
		grip_Origin_Z = (index.z()+thumb.z())/2;
		x_dist = abs(grip_Origin_X - targetOriginX);
		y_dist = abs(grip_Origin_Y - targetOriginY);
		z_dist = abs(grip_Origin_Z - targetOriginZ);
		distanceGripCenterToObject = sqrt((x_dist*x_dist)+(y_dist*y_dist)+(z_dist*z_dist));

		// compute grip aperture
		grip_aperture = sqrt(
			(index.x() - thumb.x())*(index.x() - thumb.x()) + 
			(index.y() - thumb.y())*(index.y() - thumb.y()) + 
			(index.z() - thumb.z())*(index.z() - thumb.z())
			);
		
		// use the markers on the haptic apparatus to check the physical distance between its surfaces
		distanceBetweenSurfaces = markers[3].p.y() - markers[4].p.y();

		// if we are still approaching object
		if (!reachedObject && started) {
			// when conditions for "end criterion" are satisfied (usually re: GA and distanceToObject)
			if ( (distanceGripCenterToObject<=20) && (grip_aperture<(hapticSize+10)) ){
				// set flag and record the frame (for computing % missing frames)
				reachedObject = true;
				TGA_frame = frameN;
			}
		}
	}

	// recompute the eye coordinates for drawing so we can change IOD online
	if(headCalibration){
		eyeLeft = headEyeCoords.getLeftEye();
		eyeRight = headEyeCoords.getRightEye();
	}else{
		eyeRight = Vector3d(interoculardistance/2,0,0);
		eyeLeft = -eyeRight;
	}

	// Write to trialFile once calibration is over
	if (fingerCalibration==4 )
	{
		trialFile << fixed <<
		parameters.find("SubjectName") << "\t" <<	//subjName
		//parameters[0].find("SubjectName") << "\t" <<	//subjName
		//block << "\t" <<								//block
		trialNumber << "\t" <<							//trialN
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		frameN << "\t" <<								//frameN
		index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
		thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
		distanceGripCenterToObject << "\t" <<			//distanceToObject
		fingersOccluded << "\t" <<						//fingersOccluded
		reachedObject << endl;							//reachedObject
	}

	// conditions for trial advance
	if(handAtStart && started)
		advanceTrial();
}

//////////////////////////////////
// Some extra draw functions... //
//////////////////////////////////

// An annoying display that could be shown to indicate when subjs are invisible
void drawNoFingers()
{
		glLoadIdentity();
		glTranslated(0,0,-600);
		edge = 100;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,0.0);
		glVertex3d(edge,-edge,0.0);
		glVertex3d(-edge,-edge,0.0);
		glVertex3d(-edge,edge,0.0);
		glEnd();
}

// ...a circle I guess?
void drawCircle(double radius, double x, double y, double z)
{
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
}

///////////////////////////////////////////////////////////
/////// USUALLY DON'T NEED TO EDIT THESE FUNCTIONS ////////
///////////////////////////////////////////////////////////

// change the body of this one if we have multiple blocks filled with same parameters
void initVariables() 
{
	trial.init(parameters);

	// For blocked experiments:
	//for(int ii=0; ii<num_blocks; ii++)
	//{
	//	trial[ii].init(parameters[ii]);
	//}
}

void initGLVariables()
{
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluQuadricDrawStyle(qobj, GLU_LINE);// GLU_FILL );
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

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

	glLineWidth(1.5);
}

void initMotors()
{
	homeEverything(3500,3500);
}

void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    int numMarkers=22; // updated 9/29/14
    float frameRate=85.0f;
    float markerFreq=4600.0f;
    float dutyCycle=0.4f;
    float voltage = 7.0f;
	if ( optotrak->init("C:/cncsvisiondata/camerafiles/Aligned20111014",numMarkers, frameRate, markerFreq, dutyCycle,voltage) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        optotrak->updateMarkers();
        markers = optotrak->getAllMarkers();
    }
}

void cleanup()
{
	// Stop the optotrak
    optotrak->stopCollection();
    delete optotrak;
}

void beepOk(int tone)
{
	switch(tone)
	{
	case 0:
    // Remember to put double slash \\ to specify directories!!!
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 1:
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 2:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-8.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 3:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-reject.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 4:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-visibility.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
	break;
	}
	return;
}

///////////////////////////////////////////////////////////
////////////////////// MAIN FUNCTION //////////////////////
///////////////////////////////////////////////////////////

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	
	//for (int i=0; i<num_blocks; ++i)
	//	block_order.push_back(i);

	//random_shuffle(block_order.begin(), block_order.end());

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();

    glutInit(&argc, argv);
	if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("EXP WEXLER");
		//glutFullScreen();
    }
    else
	{   glutGameModeString("1024x768:32@85");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
	initGLVariables();

	initStreams();
	initVariables(); // variable "trial" is built

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);

	boost::thread initVariablesThread(&initVariables);

    /* Application main loop */
    glutMainLoop();

    cleanup();
    return 0;
}
