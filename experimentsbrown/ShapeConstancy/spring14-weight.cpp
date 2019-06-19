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
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
//static const Vector3d centercal(-40.0,-331.6,-258.0);
Vector3d centercal(0.0,-300.0,-178.0), platformPos(0.0,0.0,0.0);
double verticalMountToCentercal = 40.0; //evces 121813: this is 14 now, maybe we should use it
Screen screen;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;

/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[6];
CylinderPointsStimulus cylinder[6];
// how many depths does the object have?
EllipsoidPointsStimulus ellipsoid[5];
Timer timer;
Timer globalTimer;
bool isStimulusDrawn=false, fingersShown = true;
string trialFileName, condition;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
vector <Marker> markers;
static double interoculardistance=57.5;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
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
static const int ELLIPSOID=1;
int trialMode = ELLIPSOID; 
int trialNumber = 0;
ParametersLoader parameters;
static const int ROUNDS = 5;
BalanceFactor<double> trial[ROUNDS];
int round = 0, allTrials = 0;
//static const Vector3d objLocation;
int fingersOccluded=0;
bool frameShown=true;
int frameN=0, calib = 0, sizeNow=0, sizeNext=0, noVisibleFingers=0, diameter=0, objectOccluded=0;
bool handAtStart = true, half=true, training = true, pickedUp = false;
double theta=0.0, magnetPos = 0.0, middlePos = 0.0, objLocation = 0.0, mirrorAlignment=0.0;

// build ellipsoid
int depthCyl[5] = { 38, 47, 57, 67, 77 };

/********** STREAMS **************/
ofstream responseFile, trialFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void cleanup();
void drawInfo();
void drawStimulus();
void drawTrial(double delz);
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
void drawEllipsoid(double delz);
void drawNoFingers();
void drawFingers();
void drawBlackSquare();

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
	}
	return;
}

/*************************** FUNCTIONS ***********************************/
void cleanup()
{
// Stop the optotrak
    optotrak->stopCollection();
    delete optotrak;
}

void drawInfo()
{
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

		switch ( headCalibrationDone )
			{
		case 0:
			{
			if ( allVisibleHead )
				text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
			else
				text.draw("Be visible with the head and glasses");
			}
			break;
		case 1:
		case 2:
			{
			if ( allVisiblePatch )
				text.draw("Move the head in the center");
			else
				text.draw("Be visible with the patch");
			}
			break;
		case 3:	// When the head calibration is done then calibrate the fingers
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

	if ((GetKeyState(VK_CAPITAL) & 0x0001)!=0)
	{
	  text.draw("#");
	  text.draw("#");
	  text.draw("# ----------------------------> Caps Lock ON!");
	  text.draw("#");
	  text.draw("#");
	}

	text.draw("####### SUBJECT #######");
	text.draw("#");
	text.draw("# Name: " +parameters.find("SubjectName"));
	text.draw("# IOD: " +stringify<double>(interoculardistance));
	text.draw("#");
	if ( abs(mirrorAlignment - 45.0) < 0.2 )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("# Mirror Alignment = " + stringify<double>(mirrorAlignment));
	glColor3fv(glWhite);
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

	glColor3fv(glWhite);
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

	text.draw("Alignment(X,Y)= " +stringify<double>(alignmentX)+","+stringify<double>(alignmentY));
	text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
	text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
	text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
	text.draw("Frame= " + stringify<int>(frameN));
	text.draw("Training= " + stringify<int>(training));
	glColor3fv(glRed);
	text.draw("CurSize= " + stringify<int>(trial[round].getCurrent()["Size"]));	
	text.leaveTextInputMode();
	}else
	{

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	GLText text1;

	if ( gameMode )
		text1.init(SCREEN_WIDTH,SCREEN_HEIGHT,glBlue,GLUT_BITMAP_HELVETICA_18);
	else
		text1.init(640,480,glBlue,GLUT_BITMAP_HELVETICA_12);
	text1.enterTextInputMode();

	text1.draw("                                                                                                                                                                                   Round= " + stringify<int>(round));
	text1.draw("  ");
	text1.draw("                                                                                                                                                                                   #-NOW-#");

	if(training)
		text1.draw("                                                                                                                                                                                   Training");
	else
		text1.draw("                                                                                                                                                                                   Catch");
	text1.draw("                                                                                                                                                                                   Trial= " + stringify<int>(trialNumber));

	text1.draw("                                                                                                                                                                                   SizeNow= " + stringify<int>(sizeNow));
	text1.draw("  ");
	text1.draw("                                                                                                                                                                                   #-NEXT-#");
	glColor3fv(glRed);
	text1.draw("                                                                                                                                                                                   SizeNext= " + stringify<int>(sizeNext));


	text1.leaveTextInputMode();
	}
}

void drawEllipsoid(double delz)
{
	int z = delz;
	glPushMatrix();
	stimDrawer[z].draw();
	glPopMatrix();
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
void drawStimulus()
{
	if (fingerCalibrationDone==3 && isStimulusDrawn && !pickedUp)
	{

		if(markers[3].p.transpose()[1]-magnetPos > 2.5)
			pickedUp = true;
		else
			pickedUp = pickedUp;

		glLoadIdentity();

		// check for fingers' visibility
		if(fingersOccluded==0)
			drawFingers();
		else
			drawNoFingers();

		// location of the object depends on the condition
		if(training)
		{
			// and on the visibility of the magnet
			if(!objectOccluded)
			{
				// place object at training pos
				glTranslated(0.0,markers[3].p.transpose()[1]-magnetPos,str2num<double>(parameters.find("TrainingDist")));
				// rotation available for debugging purposes (vertical by default)
				glRotated(theta,1,0,0);
				// black square in the middle to present only half of the cylinder
				if(half)
					drawBlackSquare();
				// draw the object
				drawEllipsoid(sizeNow);
			}else
			{
				glTranslated(0.0,0.0,-400.0);
				glRotated(M_PI*90.0,0,0,1);
				drawEllipsoid(sizeNow);
			}
		}else
		{
			if(!objectOccluded)
			{
				// place object at training pos
				glTranslated(0.0,markers[3].p.transpose()[1]-magnetPos,str2num<double>(parameters.find("CatchDist")));
				// rotation available for debugging purposes (vertical by default)
				glRotated(theta,1,0,0);
				// black square in the middle to present only half of the cylinder
				if(half)
					drawBlackSquare();
				// draw the object
				drawEllipsoid(sizeNow);
			}else
			{
				glTranslated(0.0,0.0,-400.0);
				glRotated(M_PI,0,0,1);
				drawEllipsoid(sizeNow);
			}
		}
	}
}

void drawFingers()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(index.x(),index.y(),index.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslated(thumb.x(),thumb.y(),thumb.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
}

void drawTrial(double delz)
{
	drawEllipsoid(delz);
}
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
void drawBlackSquare()
{
	glColor3fv(glBlack);
	glBegin(GL_QUADS);
		glVertex3f(-30.0f, 50.0f, 0.0f);		// Top Left
		glVertex3f( 30.0f, 50.0f, 0.0f);		// Top Right
		glVertex3f( 30.0f,-50.0f, 0.0f);		// Bottom Right
		glVertex3f(-30.0f,-50.0f, 0.0f);		// Bottom Left
	glEnd();
}
void initTrial()
{
	// --- initializing all variables
	// pick up[ the current size
	if(training)
		sizeNow = trial[round].getCurrent()["Size"];
	else
		sizeNow = round;

	diameter = depthCyl[sizeNow];
	
	if(training)
		condition = "training";
	else
		condition = "catch";

	// pick up the next size
	// if it's training...
	if(training)
	{
		// if there is another trial after the current...
		if(trial[round].hasNext())
			sizeNext = trial[round].getFullExtractions().at(trialNumber+1)["Size"];
		else 		// else if the next trial is a catch trial
			sizeNext = round;
	} 
	// else if it's catch...
	else
	{
		// if there is another catch trial...
		if(trialNumber+1 < str2num<int>(parameters.find("numCatchTrials")))
			sizeNext = round;
		else if(round<ROUNDS-1) // else if there is another round after the current...
			sizeNext = trial[round+1].getFullExtractions().at(0)["Size"];
		else 		// else the next trial is 99 (exp will be over)
			sizeNext = 99;
	}

	pickedUp=false; // has the obj been lifted?
	noVisibleFingers=0;
	frameN=0;
	handAtStart=true;
	fingersOccluded = 0;

	// turn the screen black
	isStimulusDrawn=false;

	// rebuffer the openGL scene
	drawGLScene();

	// move the platform
	// if it's the first trial of the first round (of the training)...
	if(trialNumber==0 && training && round==0)
	{
		// calibrate the platform
		platformPos = markers[4].p.transpose();
		// move it first up
		Vector3d pos0(0,-175,0); // desired location
		moveObjectAbsolute(pos0, platformPos, 6000);
	}
	// move object to middle position
	Vector3d pos1(0,-175,middlePos); // desired location
	moveObjectAbsolute(pos1, platformPos, 6000);

	// object's exact location depends on the current test condition
	// if it's a training trial[round]...
	if(training)
		objLocation = str2num<double>(parameters.find("TrainingDist"));
	else // else if it's a catch trial[round]...
		objLocation = str2num<double>(parameters.find("CatchDist"));

	// move object to location
	Vector3d pos2(0,-175,objLocation); // desired location
	moveObjectAbsolute(pos2, platformPos, 6000);
	
	// update markers
	optotrak->updateMarkers();
	// put new values into markers
	markers = optotrak->getAllMarkers();
	
	// initialize the pagnet's position to magnetPos
	magnetPos = markers[3].p.transpose()[1];

	// move the screen to middle position
	initProjectionScreen(middlePos);
	// move the screen to location
	initProjectionScreen(objLocation);

	// open the trial file, where we record online movements
	// setting file's name
	if(training)
		trialFileName = "C:/Users/visionlab/Google Drive/DATA/SizeWeightIllusion/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "_round_" + stringify<double>(round) + "trial_" + stringify<double>(trialNumber) + "training.txt";
	else
		trialFileName = "C:/Users/visionlab/Google Drive/DATA/SizeWeightIllusion/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "_round_" + stringify<double>(round) + "trial_" + stringify<double>(trialNumber) + "catch.txt";

	// creating-opening the file
	trialFile.open(trialFileName.c_str());
	// writing the first line of the file (headers)
	trialFile << fixed << "subjName" << "\t" <<
						  "trialN" << "\t" <<
						  "time" << "\t" <<
						  "frameN" << "\t" <<
						  "indexXraw" << "\t" <<
						  "indexYraw" << "\t" <<
						  "indexZraw" << "\t" <<
						  "thumbXraw" << "\t" <<
						  "thumbYraw" << "\t" <<
						  "thumbZraw" << "\t" <<
						  "eyeRXraw" << "\t" <<
						  "eyeRYraw" << "\t" <<
						  "eyeRZraw" << "\t" <<
						  "eyeLXraw" << "\t" <<
						  "eyeLYraw" << "\t" <<
						  "eyeLZraw" << "\t" <<
						  "fingersOccluded" << "\t" <<
						  "objectPosX" << "\t" <<
						  "objectPosY" << "\t" <<
						  "objectPosZ" << "\t" <<
						  "isStimulusDrawn" << "\t" <<
						  "noVisibleFingers" << "\t" <<
						  "round" << "\t" <<
						  "AbsDepth" << "\t" <<
						  "size" << "\t" <<
						  "diameter" << "\t" <<
						  "condition" << "\t" <<
						  "IOD" << "\t" <<
						  "allTrials" <<
						  endl;

	// release the cylinder!!
	isStimulusDrawn=true;
	
	// trial can start
	beepOk(0);
	timer.start();

}
void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3);
    responseFile << 
					parameters.find("SubjectName") << "\t" <<
					interoculardistance << "\t" <<
					trialNumber << "\t" <<
					objLocation <<"\t" <<
                    sizeNow <<"\t" <<
					diameter <<"\t" <<
                    timer.getElapsedTimeInMilliSec() << "\t" <<
					training << "\t" <<
					round << "\t" <<
					endl;
	
	trialFile.close();

	trialNumber++;
	allTrials++;

	// if fingers were invisible for more than 20% of the trial
	double noVisibleFingersD = noVisibleFingers;
	double frameND = frameN;

	if(noVisibleFingersD/frameND > .2)
		trial[round].reinsert(trial[round].getCurrent()); // repeat this trial at the end
	
	if(training)
	{
		if( !trial[round].isEmpty() )
		{
			trial[round].next();
			initTrial();
		}
		else
		{
			trialNumber = 0;
			training=false;
			initTrial();
		}
	}else
	{
		if(trialNumber < str2num<int>(parameters.find("numCatchTrials")))
		{
			trial[round].next();
			initTrial();
		}else if(round+1<ROUNDS) // if there is another round after the current
		{
			trialNumber = 0;
			round++;
			trial[round].next();
			training=true;
			initTrial();
		}else
		{
			cleanup();
			exit(0);
		}
	}
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   
	case 'i':
		visibleInfo=!visibleInfo;
	break;
	
	case 'm':
		interoculardistance += 0.5;
	break;
	
	case '.':
	{
		trial[round].next();
		initTrial();
	}
	break;

	case 'n':
		interoculardistance -= 0.5;
	break;
	
	case 'Q':
	case 'q':
    case 27:	//corrisponde al tasto ESC
    {   
		cleanup();
        exit(0);
    }
    break;

	case 'd':
		{
		fingersShown = !fingersShown;
		}
	break;
 
	case 'f':
	case 'F':
		{
			if(calib == 1){

		// Here we record the finger tip physical markers
		if ( allVisiblePlatform && (fingerCalibrationDone==0) )
			{
			//platformFingers=markers[1].p;
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
			//centercal = markers[4].p;
			fingerCalibrationDone=1;
			beepOk(0);
			break;
			}
		if ( (fingerCalibrationDone==1) && allVisibleFingers )
			{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
			fingerCalibrationDone=2;
			trial[round].next();
			beepOk(0);
			break;
			}
		if ( fingerCalibrationDone==2  && allVisibleFingers )
			{
			beepOk(0);
			fingerCalibrationDone=3;
			platformPos = markers[4].p.transpose();
			visibleInfo=!visibleInfo;
			initTrial();
			break;
			}
			}else
			{
			beepOk(0);
			fingerCalibrationDone=3;
			platformPos = markers[4].p.transpose();
			visibleInfo=!visibleInfo;
			trial[round].next();
			initTrial();
			break;
			}
		}
		break;

		// Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[5].p, markers[6].p,markers[7].p,interoculardistance );
            headCalibrationDone=3;
			visibleInfo=false;
        }
    }
    break;
 
	case ' ':
      {  
		  advanceTrial();
	  }
     break;
	case '5':
      {  
		  theta -= M_PI/2.0;
      }
      break;
      case '8':
      {  
		  theta += M_PI/2.0;
	  }
      break;
	  case '0':
		  {
			  half = !half;
		  }

	}
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


void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


void idle()
{
	// update optotrak
	optotrak->updateMarkers();
	// fill markers with updated coords
	markers = optotrak->getAllMarkers();

	// Coordinates picker
	allVisiblePlatform = isVisible(markers[1].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	// mirror visible
	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[1].p);

	if ( allVisiblePatch )
		headEyeCoords.update(markers[5].p,markers[6].p,markers[7].p);

	if ( allVisibleFingers )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}
	
	// during the trial (after the calibratioon)
	if (fingerCalibrationDone==3 )
	{
		
		// check for visibility of the fingers
		if ( !allVisibleFingers )
		{
			fingersOccluded=1;
			noVisibleFingers++;
		}
		
		// check for the visibility of the object
		if(!isVisible(markers[3].p))
			objectOccluded=1;
		else
			objectOccluded=0;
		
		// start the flow of the frames
		frameN++;
	
		// eye coords depend on whether the head calibration took place or not
		if(headCalibration)
		{
			// real time coords
			eyeLeft = headEyeCoords.getLeftEye();	
			eyeRight = headEyeCoords.getRightEye();
		} else	{
			// fixed coords
			eyeRight = Vector3d(interoculardistance/2,0,0);
			eyeLeft = -eyeRight;
		}
	
		// online position of fingers updated
		index = indexCoords.getP1();
		thumb = thumbCoords.getP1();
	
		// online posotion of the mirror updated
		mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;
		
		// online info written in the trialFile
		trialFile << fixed <<
			parameters.find("SubjectName") << "\t" <<		//subjName
			trialNumber << "\t" <<							//trialN
			timer.getElapsedTimeInMilliSec() << "\t" <<		//time
			frameN << "\t" <<								//frameN
			index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
			thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
			eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
			eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
			fingersOccluded << "\t" <<						//fingersOccluded
			markers[3].p.transpose() << "\t" <<				//objectPos
			isStimulusDrawn << "\t" <<
			noVisibleFingers << "\t" <<
			round << "\t" <<
			objLocation <<"\t" <<
            sizeNow <<"\t" <<
			diameter <<"\t" <<
			condition << "\t" <<
			interoculardistance << "\t" <<
			allTrials
			;
		trialFile << endl;
		
	}

}
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

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
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
	// Riempie la lista dei fattori a partire da quanto descritto nei fattori dell'oggetto parameters
	// Per dubbi prova i vari esempi di test del modulo experiments
	for(int i=0;i<ROUNDS;i++)
	{
		srand(timer.getElapsedTimeInMicroSec());
		timer.sleep(1000);
		trial[i].init(parameters);
		cerr << "round" << endl;
		trial[i].print();
	}
	middlePos = (str2num<double>(parameters.find("TrainingDist"))+str2num<double>(parameters.find("CatchDist")))/2.0;
}



void initGLVariables()
{
    for(int i=0; i<5; i++)
	{
		int dotDensity = ceil(depthCyl[i]*4.0);
/*
		int depth[5] = { 0,0,0,0,0 };
		int dotDensity = 150;
*/
		ellipsoid[i].setNpoints(dotDensity);
		ellipsoid[i].setAxesAndHeight(40,depthCyl[i],60);
		ellipsoid[i].setFluffiness(1E-6);
		ellipsoid[i].compute();
		stimDrawer[i].setStimulus(&ellipsoid[i]);
		stimDrawer[i].setSpheres(true);
		stimDrawer[i].initList(&ellipsoid[i], glRed);
	}
}
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open("C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersSizeWeightIllusion.txt");
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	
	// Markersfile directory
	string dirName  = "C:/Users/visionlab/Google Drive/DATA/SizeWeightIllusion/" + subjectName;
	mkdir(dirName.c_str());

	// Principal streams file
    string responseFileName =  "C:/Users/visionlab/Google Drive/DATA/SizeWeightIllusion/" + subjectName + "/SizeWeight_" + subjectName + ".txt";

	// Check for output file existence
	/// Response file
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());

	responseFile << fixed << "subjName" << "\t" <<
							"IOD" << "\t" <<
							"trialN" << "\t" <<
							"AbsDepth" << "\t" <<
							"Size" << "\t" <<
							"Diameter" << "\t" <<
							"trialDuration" << "\t" <<
							"training" << "\t" <<
							"round" <<
							endl;

	globalTimer.start();
}
void initMotors()
{
	homeEverything(5000,3500);
}

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	cout << "Calibration? (1=Y; 0=N)";
	cin >> calib;
	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   
		glutInitWindowSize( 640,480 );
        glutCreateWindow("EXP WEXLER");
		//glutFullScreen();
    }
    else
	{   glutGameModeString("1024x768:32@85");
        glutEnterGameMode();
        glutFullScreen();
    }
	// Prima inizializza l'ambiente di rendering dopodiche' puoi inizializzare le variabili (questo 
	// ragionamento vale se una delle variabili che inizializzi e' del modulo GLViz cioe' richiede OpenGL attivo)
    initRendering();
	initStreams();
	initGLVariables();
	initVariables(); // variable "trial" is built

	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
	// Alla fine e dopo aver disegnato tutto nero inizializza in background
	boost::thread initVariablesThread(&initVariables);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
