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
#include <direct.h>

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
#define TIMER_MS 11 // 85 hz
#define SCREEN_WIDTH  1280 //1024      // pixels
#define SCREEN_HEIGHT 1024//768       // pixels
static const double SCREEN_WIDE_SIZE = 360; //306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
static const Vector3d centercal(120.0,-307.0, -170.0); //updated 5/11/13
Screen screen;
double mirrorAlignment=0.0;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;

/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer;
CylinderPointsStimulus cylinder;
EllipsoidPointsStimulus ellipsoid;
//bool isStimulusDrawn=true;
bool isSquareDrawn=false; //evan-comment
int leftButtonDown = 0, rightButtonDown = 0;
// square measures
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;
Timer timer;
Timer globalTimer;
double tFHP=0.0;
bool pause = false, paused = false, isStimulusDrawn=false;
GLfloat myColor;
bool pulsingColors = true;
int stimPosn;
int LEFT = 1;
int RIGHT = 2;
float tFrac;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
vector <Marker> markers;
static double interoculardistance=57.5;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=3;
int platformCalibrationDone=0;
int fingerCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool visibleFingers=false;

/********* TRIAL VARIABLES *********/
int trialMode = 0; //unused at the moment, can be incremented by keypress
int trialNumber = 0; //
double frameN=0;
double TGA_frame=0;

double x_dist = 999;
double y_dist = 999;
double z_dist = 999;

ParametersLoader parameters;
BalanceFactor<double> trial[4];
map <std::string, double> factors;
static const Vector3d objLocation; //unused, this data type is for motor movement

int attempt = 1;
double num_lost_frames = 0;
int fingersOccluded=0;

bool training=false;
bool handAtStart = true;
bool started=false;
bool reachedObject = false;

int rand_match;

GLUquadric* qobj;

clock_t t;

/********** EVAN NEW ONES ********/
int numTrialModes = 1;

double startPosTop; //1
double startPosFront; //1
double startPosRight; //1
double startPosLeft; //2
double startPosRear;//2

double target_Height;
double target_Width;
double target_Depth;
double target_Origin_X;
double target_Origin_Y; //3
double target_Origin_Z; //3

double grip_Origin_X;
double grip_Origin_Y;
double grip_Origin_Z;
double grip_aperture;
double distanceGripCenterToObject;

double objA_Height;
double objA_Width = 10;
double objA_Depth;
double objA_Origin_X; //5
double objA_Origin_Y;
double objA_Origin_Z;
double objA_Left;
double objA_Top;

double objB_Height;
double objB_Width = 10;
double objB_Depth;
double objB_Origin_X; //4
double objB_Origin_Y;
double objB_Origin_Z;
double objB_Left;
double objB_Top;

double objC_Height;
double objC_Width = 10;
double objC_Depth;
double objC_Origin_X; //3
double objC_Origin_Y;
double objC_Origin_Z;
double objC_Left;
double objC_Top;

double leftSize;
double middleSize;
double rightSize;
double targetSize;

double motor_Y = -140;
double motor_Z = -350;
double stim_translation_z = motor_Z;
int active_stim = 1;

int visualSizeDifference[4] = {0, -4, 0, 4};
int block_num = 0;
int block_order[4]= {0, 0, 0, 0};
int num_blocks = 4;
int block;

bool finished = false;

//int graspOrientationAxis;
int objectsCalibration;
int startPosCalibration;
bool objectsInPosition=false;

/********** STREAMS **************/
ofstream responseFile, trialFile;

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
TCHAR pressanykey( const TCHAR* prompt);


/***** SOUND THINGS *****/
void beepOk(int tone)
{
	switch(tone)
	{
	case 0:
    // Remember to put double slash \\ to specify directories!!!
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 1:
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 2:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-8.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 3:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-10.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	}
	return;
}

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

TCHAR pressanykey( const TCHAR* prompt = NULL )
{
  TCHAR  ch;
  DWORD  mode;
  DWORD  count;
  HANDLE hstdin = GetStdHandle( STD_INPUT_HANDLE );

  // Prompt the user
  if (prompt == NULL) prompt = TEXT( "Press any key to continue..." );
  WriteConsole(
    GetStdHandle( STD_OUTPUT_HANDLE ),
    prompt,
    lstrlen( prompt ),
    &count,
    NULL
    );

  // Switch to raw mode
  GetConsoleMode( hstdin, &mode );
  SetConsoleMode( hstdin, 0 );

  // Wait for the user's response
  WaitForSingleObject( hstdin, INFINITE );

  // Read the (single) key pressed
  ReadConsole( hstdin, &ch, 1, &count, NULL );

  // Restore the console to its previous state
  SetConsoleMode( hstdin, mode );

  // Return the key code
  return ch;
}

void drawInfo()
{
	if (training)
		visibleInfo = true;

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
			text.init(1280,1024,glWhite,GLUT_BITMAP_HELVETICA_18);
		
		text.enterTextInputMode();

		if (!training)
		{
		switch ( headCalibrationDone )
		{
		case 3:	// When the head calibration is done then calibrate the fingers
		{
			if (startPosCalibration<2)
			{
				switch (startPosCalibration)
				{
				case 0:
					text.draw("Place marker 4 at Front-Top-Right of start area + press F.");
					break;
				case 1:
					text.draw("Place marker 4 at Rear-Bottom-Left of start area + press F.");
					break;
				}
			}
			if ((startPosCalibration==2) && (objectsCalibration<6))
			{
				switch (objectsCalibration)
				{
				case 0:
					text.draw("Place marker 4 at Origin of Rightmost object + press F.");
					break;
				case 1:
					text.draw("Place marker 4 at Top-Left of Rightmost object + press F.");
					break;
				case 2:
					text.draw("Place marker 4 at Origin of Middle object + press F.");
					break;
				case 3:
					text.draw("Place marker 4 at Top-Left of Middle object + press F.");
					break;
				case 4:
					text.draw("Place marker 4 at Origin of Leftmost object + press F.");
					break;
				case 5:
					text.draw("Place marker 4 at Top-Left of Leftmost object + press F.");
					break;
				}
			}
			if ((startPosCalibration==2) && (objectsCalibration==6) && (fingerCalibrationDone<3))
			{
				switch ( fingerCalibrationDone )
				{
				case 0:
					text.draw("Press F to record platform markers");
					break;
				case 1:
					text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
					break;
				case 2:
					text.draw("Move index and thumb to rigidbody tip to define starting position, then press F");
					break;
				case 3:
					text.draw("Finger calibration successfull!");
					break;
				}
			}
			break;
		}
		}

		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
		text.draw("# IOD: " +stringify<double>(interoculardistance));
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
		text.draw("#######################");
		text.draw("HeadCalibration= " + stringify<int>(headCalibrationDone) );
		text.draw("FingerCalibration= " + stringify<int>(fingerCalibrationDone) );

		glColor3fv(glWhite);
		text.draw("Calibration Platform" );

		if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );
	
		glColor3fv(glWhite);
		text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );

/*		glColor3fv(glWhite);
		text.draw(" " );
		text.draw("Mirror" );

		if ( isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(5)+stringify< Eigen::Matrix<double,1,3> > (markers[5].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(6)+stringify< Eigen::Matrix<double,1,3> > (markers[6].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(7)+stringify< Eigen::Matrix<double,1,3> > (markers[7].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(8)+stringify< Eigen::Matrix<double,1,3> > (markers[8].p.transpose())+ " [mm]" );
*/
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

		glColor3fv(glWhite);

		//text.draw("Alignment(X,Y)= " +stringify<double>(alignmentX)+","+stringify<double>(alignmentY));
		if ( abs(mirrorAlignment - 45.0) < 0.2 )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Mirror Alignment = " +stringify<double>(mirrorAlignment));
		glColor3fv(glGreen);
		text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
		text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
		text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
		text.draw("Block= " + stringify<int>(block));
		text.draw("Trial= " + stringify<int>(trialNumber));
		text.draw("Frame= " + stringify<int>(frameN));
		text.draw("distanceToObject= " + stringify<double>(distanceGripCenterToObject));
		text.draw("startPosTop= " + stringify<double>(startPosTop));
		text.draw("startPosLeft= " + stringify<double>(startPosLeft));
		text.draw("startPosFront= " + stringify<double>(startPosFront));
		text.draw("startPosRear= " + stringify<double>(startPosRear));
		if(reachedObject){
			text.draw("Object Reached!");
		}

		}else if (training && !finished) {
			text.draw("Training Mode -- press 'n' to proceed.");
			text.draw("Block= " + stringify<int>(block));
			if ( abs(mirrorAlignment - 45.0) < 0.2 )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Mirror Alignment = " +stringify<double>(mirrorAlignment));

		}else if (training && finished) {
			text.draw("The experiment is over. Thank you! :)");
		}
		text.leaveTextInputMode();
	}
}

void drawNoFingers()
{
		glLoadIdentity();
		glTranslated(0.0,0,-600);
		
		edge = 100;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,0.0);
		glVertex3d(edge,-edge,0.0);
		glVertex3d(-edge,-edge,0.0);
		glVertex3d(-edge,edge,0.0);
		glEnd();
}

void drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.setEye(eyeRight);
	drawStimulus();
	drawInfo();
	glutSwapBuffers();
}

void drawStimulus()
{
	if (fingerCalibrationDone==3 && !finished)
	{
		glLoadIdentity();
		drawTrial(100, 100);

		if(handAtStart && started)
			advanceTrial();
	}
}

void drawTrial(int sl, int st)
{
	// Left Rod
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objA_Left, objA_Top, stim_translation_z);
	glRotated(theta, 1,0,0);
	if (trial[block].getCurrent()["ObjPosn"]==1 && !reachedObject) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.5, 0.0, 0.0);
	}
	if (training)
		gluCylinder(qobj, 4.0, 4.0, leftSize, sl, st);
	else
		gluCylinder(qobj, 4.0, 4.0, leftSize+visualSizeDifference[block], sl, st);
	glPopMatrix();

	// Middle Rod
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objB_Left, objB_Top, stim_translation_z);
	glRotated(theta, 1,0,0);
	if (trial[block].getCurrent()["ObjPosn"]==2 && !reachedObject) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.5, 0.0, 0.0);
	}
	if (training)
		gluCylinder(qobj, 4.0, 4.0, middleSize, sl, st);
	else
		gluCylinder(qobj, 4.0, 4.0, middleSize+visualSizeDifference[block], sl, st);
	glPopMatrix();

	// Right Rod
	glPushMatrix();
	glLoadIdentity();
	glTranslated(objC_Left, objC_Top, stim_translation_z);
	glRotated(theta, 1,0,0);
	if (trial[block].getCurrent()["ObjPosn"]==3 && !reachedObject) {
		glColor3f(1.0, 0.0, 0.0);
	} else {
		glColor3f(0.5, 0.0, 0.0);
	}
	if (training)
		gluCylinder(qobj, 4.0, 4.0, rightSize, sl, st);
	else
		gluCylinder(qobj, 4.0, 4.0, rightSize+visualSizeDifference[block], sl, st);
	glPopMatrix();
}


void initTrial()
{
	// initializing all variables
	frameN=0;
	started = false;
	handAtStart=true;
	reachedObject = false;
	fingersOccluded = 0;
	num_lost_frames = 0;

	if (trial[block].getCurrent()["ObjPosn"]==1)
	{
		target_Origin_X = objA_Origin_X;
		target_Origin_Y = objA_Origin_Y;
		target_Origin_Z = objA_Origin_Z;
		targetSize = leftSize;
		//target_Height = objA_Height;
		//target_Width = objA_Width;
		//target_Depth = objA_Depth;
	}
	else if (trial[block].getCurrent()["ObjPosn"]==2)
	{
		target_Origin_X = objB_Origin_X;
		target_Origin_Y = objB_Origin_Y;
		target_Origin_Z = objB_Origin_Z;
		targetSize = middleSize;
		//target_Height = objB_Height;
		//target_Width = objB_Width;
		//target_Depth = objB_Depth;
	}
	else if (trial[block].getCurrent()["ObjPosn"]==3)
	{
		target_Origin_X = objC_Origin_X;
		target_Origin_Y = objC_Origin_Y;
		target_Origin_Z = objC_Origin_Z;
		targetSize = rightSize;
		//target_Height = objC_Height;
		//target_Width = objC_Width;
		//target_Depth = objC_Depth;
	}
			

	string trialFileName = "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/tabletop/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "trial_" + stringify<double>(trialNumber) + "_" + stringify<int>(block) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexXYZraw\tthumbXYZraw\tdistanceToObject\tfingersOccluded\treachedObject" << endl;

	isStimulusDrawn=true;
	beepOk(2);
	timer.start();
}

void advanceTrial()
{
	// Determine if previous trial was a good trial
	double percent_occluded_frames = num_lost_frames/TGA_frame;
	bool not_enough_frames = percent_occluded_frames > 0.10;
	int overwrite = 0;

	// If any of these things are true when advanceTrial is called, reinsert!
	if (training || !reachedObject || not_enough_frames ) {
		if (training)
			cout << "Training Mode!" << endl;
		if (!reachedObject)
			cout << "I don't think you reached the object..." << endl;
		if (not_enough_frames)
			cout << "Be visible!" << endl;
		map<std::string,double> currentFactorsList = trial[block].getCurrent();
		trial[block].reinsert(currentFactorsList);
		overwrite = 1;
	}

	// Write previous trial to response file
	if (!overwrite)
	{
		double timeElapsed = timer.getElapsedTimeInMilliSec();
		responseFile.precision(3);

		int objSize;
		if (trial[block].getCurrent()["ObjPosn"]==1)
			objSize = leftSize;
		else if (trial[block].getCurrent()["ObjPosn"]==2)
			objSize = middleSize;
		else if (trial[block].getCurrent()["ObjPosn"]==3)
			objSize = rightSize;

		responseFile << 
						parameters.find("SubjectName") << "\t" <<
						trialNumber << "\t" <<
						timer.getElapsedTimeInMilliSec() << "\t" <<
						trial[block].getCurrent()["ObjPosn"] << "\t" <<
						objSize << "\t" <<
						target_Origin_X << "\t" <<
						target_Origin_Y << "\t" <<
						target_Origin_Z << "\t" <<
						visualSizeDifference[block] << "\t" <<
						block <<
						//target_Width << "\t" <<
						//target_Height << "\t" <<
						//target_Depth << "\t" <<
						//graspOrientationAxis <<
						endl;
	}

	// Close previous trial file
	trialFile.close();

	// If there are more trials
	if( !trial[block].isEmpty() )
	{
		// If we're not overwriting due to a bad trial
		if (!overwrite)
			trialNumber++; // increment

		// Get the next trial and initialize it!
		factors = trial[block].getNext();
		initTrial();
	}
	else
	{
		block_num++;
		training = true;
		trialNumber = 0;

		if (block_num < num_blocks)
		{
			block = block_order[block_num];

			string block_string;
			responseFile.close();
			if (block > 0)
			{
				if (visualSizeDifference[block]>=0)
					block_string = "_H+" + stringify<int>(visualSizeDifference[block]);
				else
					block_string = "_H" + stringify<int>(visualSizeDifference[block]);
			}
			else
			{
				block_string = "_fullVision";
			}
			string responseFileName =  "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/tabletop/" + parameters.find("SubjectName") + "/evan-haptic_" + parameters.find("SubjectName") + block_string + ".txt";
			if ( !fileExists((responseFileName)) )
			{
				responseFile.open((responseFileName).c_str());
			}
			responseFile << fixed << "subjName\ttrialN\ttrialDuration\tobjPosn\tobjSize\tobjOriginX\tobjOriginY\tobjOriginZ\tvisualSizeDifference\tblock" << endl;

			factors = trial[block].getNext();

			//beepOk(2);

			//clock_t t = clock();
			//while ( ((clock() - t)/CLOCKS_PER_SEC) < 2 )
			//{
				
			//}

			initTrial();
		}
		else
		{
			finished = true;
		}
	}

}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
	case 'x':
		// Facendo cosi si cancella lo stimolo durante il movimento (SINCRONO) del monitor.
		// Si imposta il isStimulusDrawn a FALSE e si riaggiorna la schermata con una drawGLScene()
		// infine si muove il monitor, la chiamata blocca il programma per x secondi, si 
		// simula lo spostamento dello schermo di proiezione ed infine si reimposta isStimulusDrawn a TRUE
		// cosi' la prossima chiamata di drawStimulus() lo disegna correttamente a schermo. Provare per credere...
		//factors = trial.getNext();
		//trial.next();
		//drawGLScene();
		//initProjectionScreen(trial.getCurrent()["AbsDepth"]);
	break;
	case 'i':
		visibleInfo=!visibleInfo;
	break;
	case 'n':
	{
		training = false;
		visibleInfo = false;
	}
	break;
	case '*':
	{
		// Il trucco per avanzare alla modalita' trial successiva: incrementi di uno e poi tieni il resto della 
		// divisione per due, in questo caso ad esempio sara' sempre:
		// 0,1,0,1,0,1
		// se metti il resto della divisione per 3 invece la variabile trialMode sar'
		// 0,1,2,0,1,2
		// ogni ciclo della variabile trialMode normalmente e' un trial (1)
		// puoi anche definire una funzione void advanceTrial() che si occupa di andare al trial successivo,
		// deve contenere una chiamata alla BalanceFactor::getNext() cosi' passi alla nuova lista di fattori
		// Ad esempio
		 trialMode++;
		 trialMode=trialMode%numTrialModes;
	}
	break;
	case 'Q':
	case 'q':
    case 27:	//corrisponde al tasto ESC
    {   
		// Ricorda quando chiami una funzione exit() di chiamare prima cleanup cosi
		// spegni l'Optotrak ed i markers (altrimenti loro restano accesi e li rovini) 
		cleanup();
        exit(0);
    }
    break;
	case 'f':
	case 'F':
	{
		if (!objectsInPosition)
		{
			// move the screen
			initProjectionScreen(motor_Z); //evces 031414
			Vector3d pos1(0,motor_Y,motor_Z);
			moveObjectAbsolute(pos1, centercal, 7500);
			objectsInPosition = true;
			beepOk(0);
			break;
		}
		if (startPosCalibration==0 && objectsInPosition)
		{
			startPosFront = markers[4].p.z();
			startPosRight = markers[4].p.x();
			startPosTop = markers[4].p.y();
			startPosCalibration=1;
			beepOk(0);
			break;
		}
		if (startPosCalibration==1)
		{
			startPosRear = markers[4].p.z();
			startPosLeft = markers[4].p.x();
			startPosCalibration=2;
			beepOk(0);
			break;
		}
		if ((startPosCalibration==2) && (objectsCalibration==0))
		{
			objA_Origin_Y = objB_Origin_Y = objC_Origin_Y = markers[4].p.y();
			objA_Origin_Z = objB_Origin_Z = objC_Origin_Z = markers[4].p.z();
			
			objA_Top = objA_Origin_Y + leftSize/2;
			objB_Top = objB_Origin_Y + middleSize/2;
			objC_Top = objC_Origin_Y + rightSize/2;

			objC_Origin_X = markers[4].p.x()-5;
			objC_Left = objC_Origin_X-10;
			
			objectsCalibration=2;
			beepOk(0);
			break;
		}
		/*
		if ((startPosCalibration==2) && (objectsCalibration==1))
		{
			objC_Top = markers[4].p.y();
			objC_Left = markers[4].p.x();
			objectsCalibration=2;
			beepOk(0);
			break;
		}
		*/
		if ((startPosCalibration==2) && (objectsCalibration==2))
		{
			objB_Origin_X = markers[4].p.x()-5;
			objB_Left = objB_Origin_X-10;
			
			objectsCalibration=4;
			beepOk(0);
			break;
		}
		/*
		if ((startPosCalibration==2) && (objectsCalibration==3))
		{
			objB_Top = markers[4].p.y();
			objB_Left = markers[4].p.x();
			objectsCalibration=4;
			beepOk(0);
			break;
		}
		*/
		if ((startPosCalibration==2) && (objectsCalibration==4))
		{
			objA_Origin_X = markers[4].p.x()-5;
			objA_Left = objA_Origin_X-10;
			
			objectsCalibration=6;
			beepOk(0);
			break;
		}
		/*
		if ((startPosCalibration==2) && (objectsCalibration==5))
		{
			objA_Top = markers[4].p.y();
			objA_Left = markers[4].p.x();
			objectsCalibration=6;
			beepOk(0);
			break;
		}
		*/
		// Here we record the finger tip physical markers
		if ( (objectsCalibration==6) && allVisiblePlatform && (fingerCalibrationDone==0) )
		{
			//platformFingers=markers[1].p;
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
			//centercal = markers[4].p;
			fingerCalibrationDone=1;
			beepOk(0);
			break;
		}
		if ( (objectsCalibration==6) && fingerCalibrationDone==1 && allVisibleFingers )
		{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
			fingerCalibrationDone=2;
			beepOk(0);
			break;
		}
		if ( (objectsCalibration==6) && fingerCalibrationDone==2  && allVisibleFingers )
		{
			beepOk(0);
			fingerCalibrationDone=3;
			visibleInfo=!visibleInfo;
			factors = trial[block].getNext();
			training=true;
			initTrial();
			break;
		}
	}
	break;
    case 's':
    {  
		advanceTrial();
	}
    break;
	case '0':
	{
		active_stim += 1;
		if (active_stim > 3)
			active_stim = 1;
	}
	break;
	case 'e':
	{
		cout << "Hand at start: " << handAtStart << endl;
		cout << "Started: " << started << endl;
		cout << "Reached Object: " << reachedObject << endl;
	}
	break;
	case 'j':
	{
		cout << "Object Center: " << target_Origin_X << "\t" << target_Origin_Y << "\t" << target_Origin_Z << endl;
		cout << "Distance to Object: sqrt(" << x_dist << "^2 + " << y_dist << "^2 + " << z_dist << ") = " << distanceGripCenterToObject << endl;
		cout << "Start Pos Corner: " << startPosRight << "\t" << startPosTop << "\t" << startPosFront << endl;
	}
	break;
	case '5':
	{
		if (active_stim==1)
		{
			objA_Top -= .5;
			objA_Origin_Y -= .5;
		}
		if (active_stim==2)
		{
			objB_Top -= .5;
			objB_Origin_Y -= .5;
		}
		if (active_stim==3)
		{
			objC_Top -= .5;
			objC_Origin_Y -= .5;
		}
	}
    break;
    case '8':
    {
		if (active_stim==1)
		{
			objA_Top += .5;
			objA_Origin_Y += .5;
		}
		if (active_stim==2)
		{
			objB_Top += .5;
			objB_Origin_Y += .5;
		}
		if (active_stim==3)
		{
			objC_Top += .5;
			objC_Origin_Y += .5;
		}
    }
    break;
	case '4':
    {
		if (active_stim==1)
		{
			objA_Left -= .5;
			objA_Origin_X -= .5;
		}
		if (active_stim==2)
		{
			objB_Left -= .5;
			objB_Origin_X -= .5;
		}
		if (active_stim==3)
		{
			objC_Left -= .5;
			objC_Origin_X -= .5;
		}
	}
    break;
    case '6':
    {
		if (active_stim==1)
		{
			objA_Left += .5;
			objA_Origin_X += .5;
		}
		if (active_stim==2)
		{
			objB_Left += .5;
			objB_Origin_X += .5;
		}
		if (active_stim==3)
		{
			objC_Left += .5;
			objC_Origin_X += .5;
		}
    }
	break;
	case '1':
    {  
		stim_translation_z -= 1;
		objA_Origin_Z = objB_Origin_Z = objC_Origin_Z -= 1;
	}
    break;
    case '3':
    {  
		stim_translation_z += 1;
		objA_Origin_Z = objB_Origin_Z = objC_Origin_Z += 1;
    }
	break;
	/*
	case '+':
    {
		visualSizeDifference -= 1;
	}
    break;
    case '-':
    {
		visualSizeDifference += 1;
    }
	break;
	*/
	}
}

// Funzione che gestisce il ridimensionamento della finestra
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

// Questa funzione definisce la distanza e l'orientazione dello schermo di proiezione virtuale, e' importante che la lasci cosi almeno che tu non voglia:
// 1) simulare uno schermo di proiezione che si muove o comunque con un orientamento diverso da quello standard cioe' il piano a z=focalDistance
// 2) spostare il piano di proiezione cambiando alignmentX ed alignmentY ma per quello ti consiglio di contattarmi o comunque utilizzare il file headCalibration.cpp che ha legato ai tasti 2,4,6,8 il movimento dello schermo per allineare mondo virtuale e mondo optotrak
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

// Questa funzione si occupa di fare il refresh della schermata ed e' chiamata ogni TIMER_MS millisecond, tienila cosi'
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


// Questa funzione e' quella che in background fa tutti i conti matematici, quindi qui devi inserire 
// 1) Scrittura su file continua delle coordinate che vuoi salvare
// 2) Estrazione delle coordinate a partire dai corpi rigidi precedentemente definiti vedi ad esempio
// come e' fatto per eyeLeft e eyeRight oppure per thumb ed index
void idle()
{
	optotrak->updateMarkers();
	markers = optotrak->getAllMarkers();

	// Coordinates picker
	allVisiblePlatform = isVisible(markers[1].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;
	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[1].p);

	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	if ( allVisiblePatch )
		headEyeCoords.update(markers[5].p,markers[6].p,markers[7].p);

	if ( allVisibleFingers )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}

	//////////////////////////
	/// Real Stuff
	//////////////////////////

	if (fingerCalibrationDone==3 && !finished) // After we've calibrated
	{
		// First, check for finger occlusion
		if ( !allVisibleFingers )
		{
			fingersOccluded = 1;
			num_lost_frames += 1;
		}
		
		// Always advance frame number right away
		frameN++;

		// Second, check that we're at the start position
		if( (index.y() < startPosTop) && // index below ceiling
			(index.x() > startPosLeft) && // index right of left wall
			(index.z() > startPosFront) &&  // index behind front wall
			(index.z() < startPosRear) ) // index in front of rear wall
		{
			// distance from index to thumb less than 1.5cm in each direction
			if ((abs(index.y() - thumb.y()) < 15) &&
				(abs(index.x() - thumb.x()) < 15) && 
				(abs(index.z() - thumb.z()) < 15) ) 
			{
				// if so, keep resetting timer
				handAtStart = true;
				timer.start();
			}

		} else { // we've moved from the start, begin counting 

			handAtStart = false;
			started = true;
		}

		// find y_dist and z_dist
		grip_Origin_X = (index.x()+thumb.x())/2;
		grip_Origin_Y = (index.y()+thumb.y())/2;
		grip_Origin_Z = (index.z()+thumb.z())/2;
		x_dist = abs(grip_Origin_X - target_Origin_X);
		y_dist = abs(grip_Origin_Y - target_Origin_Y);
		z_dist = abs(grip_Origin_Z - target_Origin_Z);

		grip_aperture = sqrt(
			(abs(index.x() - thumb.x())*abs(index.x() - thumb.x())) + 
			(abs(index.y() - thumb.y())*abs(index.y() - thumb.y())) + 
			(abs(index.z() - thumb.z())*abs(index.z() - thumb.z())));

		distanceGripCenterToObject = sqrt((x_dist*x_dist)+(y_dist*y_dist)+(z_dist*z_dist));

		if (!reachedObject && started) {
			
			if ( (distanceGripCenterToObject <= 10) && (grip_aperture < (targetSize+15)) ) {
				reachedObject = true;
				//cout << "Reached object!" << endl;
				TGA_frame = frameN;
			}
		}
	}

	///////////////////
	//Some Nonsense...
	///////////////////

	if(headCalibration)
	{
		eyeLeft = headEyeCoords.getLeftEye();
		eyeRight = headEyeCoords.getRightEye();
	}
	else
	{
		eyeRight = Vector3d(interoculardistance/2,0,0);
		eyeLeft = -eyeRight;
	}

	index = indexCoords.getP1();
	thumb = thumbCoords.getP1();

	// Write to trialFile
	if (fingerCalibrationDone==3 )
	{
		trialFile << fixed <<
		parameters.find("SubjectName") << "\t" <<		//subjName
		trialNumber << "\t" <<							//trialN
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		frameN << "\t" <<								//frameN
		index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
		thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
		distanceGripCenterToObject << "\t" <<
		fingersOccluded << "\t" <<						//fingersOccluded
		reachedObject << endl;
	}

}

// Questa funzione inizializza l'optotrak passandogli una traslazione di default "calibration" che 
// in questo caso rappresenta la coordinata dell'occhio ciclopico (in realta' andrebbe bene qualsiasi valore ma questo 
// allinea meglio coordinate optotrak e coordinate opengl cosicche abbiano lo zero molto vicino. IMPORTANTE: vedi di
// tenere "calibration" lo stesso in tutti gli esperimenti perche' altrimenti devi modificare anche 
// alignmentX e alignmentY
// Se qualcosa nell'inizializzazione dell'optotrak non va hai due possibilita'
// 1) Leggerti il log che sta qui sotto
// 2) Leggerti il log generato nel file opto.err che normalmente dovrebbe essere nella cartella dove lanci l'eseguibile // altrimenti fatti una ricerca file.
// La funzione initOptotrak deve stare all'inizio di tutto.
void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    int numMarkers=20;
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

// Questa funzione inizializza l'ambiente di rendering OpenGL. Tienila cosi perche' cosi' va bene nel 99% dei casi
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

	// Tieni questa riga per evitare che con l'antialiasing attivo le linee siano piu' sottili di un pixel e quindi ballerine (le vedi vibrare)
	glLineWidth(1.5);
}

void initVariables()
{
	for(int ii=0; ii<num_blocks; ii++)
	{
		cout<< ii <<endl;
		trial[ii].init(parameters);
		//trial[ii].addFactor("VisualSizeDifference", vlist_of<double>(visualSizeDifference[ii]));
		//trial[ii].print();
	}
}

void initGLVariables()
{
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluQuadricDrawStyle(qobj, GLU_FILL );
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open("C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersEvanTabletop.txt");
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	
	// Markersfile directory
	string dirName  = "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/tabletop/" + subjectName;
	mkdir(dirName.c_str());

	string block_string;
	if (block==0)
	{
		block_string = "_fullVision";
	}
	else
	{
		if (visualSizeDifference[block]>=0)
			block_string = "_H+" + stringify<int>(visualSizeDifference[block]);
		else
			block_string = "_H" + stringify<int>(visualSizeDifference[block]);
	}

	// Principal streams file
    string responseFileName =  "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/tabletop/" + parameters.find("SubjectName") + "/evan-haptic_" + subjectName + block_string + ".txt";

	// Check for output file existence 
	/// Response file
	if ( !fileExists((responseFileName)) ) {
        responseFile.open((responseFileName).c_str());
	}

	responseFile << fixed << "subjName\ttrialN\ttrialDuration\tobjPosn\tobjSize\tobjOriginX\tobjOriginY\tobjOriginZ\tvisualSizeDifference\tblock" << endl;
	globalTimer.start();
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(7500,4200);
}

int main(int argc, char*argv[])
{
	cout << "Enter size of leftmost object: ";
	cin >> leftSize;
	cout << "Enter size of middle object: ";
	cin >> middleSize;
	cout << "Enter size of rightmost object: ";
	cin >> rightSize;
	for (int i=0; i<num_blocks; i++)
	{
		cout << "Enter block numbers in order (hit RETURN after EACH entry): " << endl;
		cin >> block_order[i];
	}
	block = block_order[block_num];
	//cout << "Enter difference for MM (+ = VL, 0 = Same, - = HL): ";
	//cin >> visualSizeDifference[2];
	//cout << "Enter axis of grasp orientation (Depth = 0, Height = 1, Width = 2): ";
	//cin >> graspOrientationAxis;

	mathcommon::randomizeStart();

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();
	//cout << "initOptotrak() called." << endl;
	
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize( 1280, 1024); //640,480 );
	glutCreateWindow("EXP WEXLER");
	glutFullScreen();

	// Prima inizializza l'ambiente di rendering dopodiche' puoi inizializzare le variabili (questo 
	// ragionamento vale se una delle variabili che inizializzi e' del modulo GLViz cioe' richiede OpenGL attivo)
    initRendering();
	//cout << "initRendering() called." << endl;
	initGLVariables();
	//cout << "initGLVariables() called." << endl;
	// Inizializza gli stream di output o input. Consiglio di usare questo ordine perche' in initVariables
	// normalmente vorrai parsare un file di testo contenente i parametri
	initStreams();
	//cout << "initStreams() called." << endl;
	initVariables(); // variable "trial" is built
	//cout << "initVariables() called." << endl;
	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
	//cout << "glutFuncs called." << endl;
	// Alla fine e dopo aver disegnato tutto nero inizializza in background
	boost::thread initVariablesThread(&initVariables);
    /* Application main loop */
    glutMainLoop();
//	cout << "glutMainLoop() called." << endl;

    cleanup();

    return 0;
}
