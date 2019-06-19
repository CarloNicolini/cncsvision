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

#define SIMULATION

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
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0;
Screen screen;
static const Vector3d centercal(29.75,-133.94,-296.16); //updated 9/25/14
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer draw_ernst_banks[3];
CylinderPointsStimulus ernst_banks[3];
Timer timer;
Timer globalTimer;
double indexObjectDistance, thumbObjectDistance, offsetZ;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, ind, thu, platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999);
vector <Marker> markers;
static double interoculardistance=0.0;
#ifdef SIMULATION
vector<Marker> simMarkers;
Timer simTimer;
bool grasp = false;
#endif

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int fingerCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;

/********* TRIAL VARIABLES *********/
static const int THREERODS=0;
int trialMode = THREERODS;
int trialNumber = 0;

ParametersLoader parameters;

TrialGenerator<double> trial;

int fingersOccluded=0, framesOccluded=0;
int responsekey=0, overwrite=0;
int frameN=0;
double stimulus_height[2] = {0.0, 0.0};
int first_interval = 0, second_interval = 0, whichStimulus = 0;
double absDepth = 0.0, stimulus_noise = 0.0;
bool showStimuli = false;

bool indexDisappeared = false, thumbDisappeared = false, fingersDisappeared = false;
bool isStimulusDrawn = false;

double *s_pointsX;
double *s_pointsY;
double *s_pointsZ;
double *c_pointsX;
double *c_pointsY;
double *c_pointsZ;
int numPoints=0;
double *sizePoints;
double sizePointsF = 0.0;
double max_c_pointX = 0.0;
double min_c_pointX = 0.0;
double max_c_pointZ = 0.0;
double min_c_pointZ = 0.0;
int h_stimulus = 0;
Vector3d whereTop(0.0,0.0,0.0), whereBottom(0.0,0.0,0.0);
double Vtranslation = 0.0;
int randSpeed = 0;
bool experimentBegun = false;
double v_delta = 0.0, h_delta = 0.0;
bool endTrial = false;
bool indexOnPlace = false;


/********** STREAMS **************/
ofstream responseFile, summaryFile;

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
void advanceTrial(bool response);
void initTrial();
void drawFingers();
void repeatTrial();
void calibration_fingers(int phase);
void calibration_head(int phase);
void drawSquare();
void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres);
void buildComparison(double height, double fluffiness, double distance);
void buildStandard(double fluffiness, double distance, double delta);
void drawStandardComparison();
void prepareSecondHeight();
void moveAway();
void mouseFunc(int button, int state, int x, int y);

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/fall14-1_ernstbanksreplica/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/fall14-1_ernstbanksreplica/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_fall14ErnstBanks_test1_VH.txt";

// response file name
string responseFile_name = "fall14-1_ernstbanksreplica_VH_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tframesOccluded\tAbsDepth\tstairID\tbar_height\treversals\tstairAscending\tstepsDone\tfirst_visual_stimulus\tsecond_visual_stimulus\thaptic_height\tresponsekey";

// summary file headers
string summaryFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tstairID\tbar_height\treversals\tstairAscending\tstepsDone\tfirst_stimulus\tsecond_stimulus\tnoise_level\thaptic_standard\tvisual_standard\tresponsekey";

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
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
		}
		break;
	case 2:
		{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
		}
		break;
	}
}

void calibration_head(int phase)
{
	switch (phase)
	{
	case 1:
		{
			headEyeCoords.init(markers[3].p-Vector3d(70,0,0),markers[3].p, markers[10].p,markers[11].p,markers[12].p,interoculardistance );

		}
		break;
	case 2:
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
		}
		break;
	}
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
		
		// check if IOD has been input
		if(interoculardistance>45.0)
		{
			text.draw("####### SUBJECT #######");
			text.draw("#");
			text.draw("# Name: " + parameters.find("SubjectName"));
		} else {
			text.draw(" "); text.draw(" "); text.draw(" ");
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

		// if IOD has been input
		if(interoculardistance>45.0)
		{
			glColor3fv(glWhite);
			text.draw("# trial: " +stringify<int>(trialNumber));
			text.draw("# time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
			text.draw("# simTime: " +stringify<int>(simTimer.getElapsedTimeInMilliSec()));

			text.draw("FingerCalibration= " + stringify<int>(fingerCalibrationDone) );
			glColor3fv(glWhite);
			text.draw("Calibration Platform" );
			
			if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			
			text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );
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
			text.draw(" " );
			text.draw("indexOnPlace: "+ stringify<int>(indexOnPlace));
			text.draw("Index: "+ stringify< Eigen::Matrix<double,1,3> > (ind.transpose())+ " [mm]" );
			text.draw("Top y = " +stringify<double>(whereTop.y()));
			text.draw("Index Speed= " + stringify<double>(markers[14].v.norm()));
			text.draw("Haptic height: "+ stringify<double>(markers[4].p.y() - markers[3].p.y())+ " [mm]" );

			text.leaveTextInputMode();
		}
	}
}

void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres)
{
	glColor3fv(glRed);

	for (int i = 0; i < numSpheres; ++i )
	{ 
		glPushMatrix();
		glTranslated(px[i], py[i], pz[i]);
		glutSolidSphere(sizeSpheres[i],10,10);
		glPopMatrix();
	}
}

void drawStandardComparison()
{
	switch(whichStimulus)
	{
		case 0: // standard
		{
			drawErnstBanksSpheres(s_pointsX, s_pointsY, s_pointsZ, numPoints, sizePoints);
		}
		break;
		case 1: // comparison
		{
			drawErnstBanksSpheres(c_pointsX, c_pointsY, c_pointsZ, numPoints, sizePoints);
		}
		break;
	}
}

void drawStimulus()
{
	if (fingerCalibrationDone==3)
	{
		if(showStimuli)
		{	
			glLoadIdentity();

			if(!indexOnPlace)
			{
				drawFingers();
				
				glPushMatrix();
				glTranslated(0, whereTop.y(), absDepth+10);
				glutSolidSphere(1.25,10,10);
				glPopMatrix();
			}else
			{		
				glColor3fv(glWhite);
				glPushMatrix();
				glTranslated(0, whereTop.y(), absDepth);
				glutSolidSphere(1.25,10,10);
				glPopMatrix();		
				glColor3fv(glRed);

				if(h_stimulus == 1)
				{
					glTranslatef(0, whereTop.y()-(stimulus_height[first_interval]/2), absDepth);
					whichStimulus = first_interval;
					drawStandardComparison();
				} 
				if(h_stimulus == 2)
				{
					glTranslatef(0, whereTop.y()-(stimulus_height[second_interval]/2), absDepth);
					whichStimulus = second_interval;
					drawStandardComparison();
				} 
			}
		}

		// prepare the second height
		if(timer.getElapsedTimeInMilliSec() > 1150 && timer.getElapsedTimeInMilliSec() < 1200 && h_stimulus==1)
		{
			prepareSecondHeight();
		}

		// after roughly 1 second from onset of second stimulus, move the object back
		// to wait for the next trial
		if(timer.getElapsedTimeInMilliSec() > 1150 && timer.getElapsedTimeInMilliSec() < 1200 && h_stimulus==2)
		{
			moveAway();
		}
	}

}

void drawSquare()
{
	glColor3fv(glBlack);
	glBegin(GL_QUADS);
	glVertex3f(-75.0f, 75.0f, 0.0f);		// Top Left
	glVertex3f( 75.0f, 75.0f, 0.0f);		// Top Right
	glVertex3f( 75.0f,-75.0f, 0.0f);		// Bottom Right
	glVertex3f(-75.0f,-75.0f, 0.0f);		// Bottom Left
	glEnd();
}

void mouseFunc(int button, int state, int x, int y)
{
	if (button==GLUT_LEFT_BUTTON)
	{
		 if(state==GLUT_DOWN)
		 {
		    if(experimentBegun)
			{
				responsekey=4;
				if(h_stimulus==2 && endTrial)
				{
					advanceTrial(stimulus_height[first_interval] > stimulus_height[second_interval] && stimulus_height[1] == stimulus_height[first_interval]);
				}
			}
		 }
	}
	
	if (button==GLUT_RIGHT_BUTTON)
	{
		 if(state==GLUT_DOWN)
		 {
		    if(experimentBegun)
			{
				responsekey=6;
				if(h_stimulus==2 && endTrial)
				{
					advanceTrial(stimulus_height[second_interval] > stimulus_height[first_interval] && stimulus_height[1] == stimulus_height[second_interval]);
				}
			}
		 }
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
/*
	glPushMatrix();
	glLoadIdentity();
	glTranslated(thu.x(),thu.y(),thu.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
*/
}

void drawGLScene()
{
    glDrawBuffer(GL_BACK);

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

// INITTRIAL TO CHECK (MOSTLY FOR FINGERS-RELATED EVENTS)
void initTrial()
{
	// if the current is not an empty trial...
	if(!trial.isEmpty())
	{
		cerr << "trial #" << trialNumber << endl;
		// these variables are reset
		endTrial = false;
		showStimuli = false;
		h_stimulus = 1;
		fingersOccluded = 0;
		framesOccluded=0;
		frameN = 0;

		// random seed
		srand(time(NULL));
		// pick 0 or 1 at random and assign it to first_interval
		first_interval = rand() % 2;
		// pick the complementary (0 or 1) and assign it to second_interval
		second_interval = 1 - first_interval;

		// set the parameters to build the stimulus:
		// HEIGHT
		//--- standard haptic
		stimulus_height[0] = 55.0 + h_delta; 
		//--- comparison (both)
		stimulus_height[1] = trial.getCurrent().second->getCurrentStaircase()->getState(); 
		// EGOCENTRIC DISTANCE
		absDepth = trial.getCurrent().first["AbsDepth"];

		// build the visual stimuli		
		buildStandard(stimulus_noise, absDepth, v_delta);		
		buildComparison(stimulus_height[1], stimulus_noise, absDepth);

		// roll on
		drawGLScene();
		initProjectionScreen(absDepth);

		// generate random speed between 3500 and 5000
		randSpeed = rand() % 1501 + 3500;
		// move bottom surface up to topY - stimulus_height[first_interval]
		Vector3d moveHeight(0.0,whereTop.y()-stimulus_height[first_interval],absDepth);
		moveObjectAbsolute(moveHeight, whereBottom, randSpeed);
		beepOk(0);

		showStimuli = true;

		timer.start();
	} else
	{	
		responseFile.close();
		summaryFile.close();

		cleanup();
		exit(0);
	}
}

void prepareSecondHeight()
{
	showStimuli = false;
	indexOnPlace = false;
	timer.start();
	drawGLScene();
	// move bottom surface up to (topY - 40.0)
	Vector3d moveReset(0.0,whereTop.y()-50.0,absDepth);
	moveObjectAbsolute(moveReset, whereBottom, 5000);

	srand(time(NULL));
	randSpeed = rand() % 1501 + 3500;
	// then move bottom surface down to (topY - stimulus_height[1])
	Vector3d moveHeight(0.0,whereTop.y()-stimulus_height[second_interval],absDepth);
	moveObjectAbsolute(moveHeight, whereBottom, randSpeed);
	h_stimulus = 2;
	showStimuli = true;
	timer.start();
	beepOk(0);
}

void moveAway()
{
	showStimuli = false;
	timer.start();
	timer.stop();
	drawGLScene();
	// move bottom surface up to (topY - 20.0)
	Vector3d moveBack(0.0,whereTop.y()-50.0,absDepth);
	moveObjectAbsolute(moveBack, whereBottom, 5000);
	beepOk(1);
	endTrial = true;
}

void advanceTrial(bool response)
{
	//summaryFile.precision(3);
	summaryFile << parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			absDepth <<"\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getAscending() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<	
			stimulus_height[first_interval] << "\t" <<	
			stimulus_height[second_interval] << "\t" <<	
			stimulus_noise << "\t" <<	
			stimulus_height[0] << "\t" <<	
			55.0 + v_delta << "\t" <<	
			responsekey <<
			endl;

	trialNumber++;
	trial.next(response);
	initTrial();
}

void buildStandard(double fluffiness, double distance, double delta)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / 2.0 / abs(distance))*180/M_PI*2.0;
	// density is 9 dots per degree
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle * stimulus_visual_angle);
	
	s_pointsX = new double[numPoints];
	s_pointsY = new double[numPoints];
	s_pointsZ = new double[numPoints];
	sizePoints = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		s_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		s_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		s_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(30.0*fluffiness/100.0))) - (30.0*fluffiness/100.0)/2.0;
		if(s_pointsY[i] < (55.0 + delta)/2.0 && s_pointsY[i] > -(55.0 + delta)/2.0)
			s_pointsZ[i] = s_pointsZ[i] + 30.0;
	}
	// the size of the spheres is also dependent on distance
	for(int i = 0; i < numPoints; ++i ) 
	{
		sizePoints[i] = 8.0/60.0*M_PI/180.0 * abs(distance+s_pointsZ[i]) / 2.0;
	}

}

void buildComparison(double height, double fluffiness, double distance)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / 2.0 / abs(distance))*180/M_PI*2.0;
	// density is 9 dots per degree^2
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle * stimulus_visual_angle);
	
	c_pointsX = new double[numPoints];
	c_pointsY = new double[numPoints];
	c_pointsZ = new double[numPoints];
	sizePoints = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		c_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(30.0*fluffiness/100.0))) - (30.0*fluffiness/100.0)/2.0;
		if(c_pointsY[i] < height/2.0 && c_pointsY[i] > -height/2.0)
			c_pointsZ[i] = c_pointsZ[i] + 30.0;
	}
	// the size of the spheres is also dependent on distance
	for(int i = 0; i < numPoints; ++i ) 
	{
		sizePoints[i] = 8.0/60.0*M_PI/180.0 * abs(distance+c_pointsZ[i]) / 2.0;
	}

}

void repeatTrial()
{
	//beepOk(1);
	//initTrial();
}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   
		case 'i':
			visibleInfo=!visibleInfo;
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
			// Here we record the finger tip physical markers
			if ( allVisiblePlatform && fingerCalibrationDone==0 )
			{
				fingerCalibrationDone=1;
				calibration_fingers(fingerCalibrationDone);
				beepOk(0);
				break;
			}
			if ( (fingerCalibrationDone==1) && allVisibleFingers )
			{
				fingerCalibrationDone=2;
				calibration_fingers(fingerCalibrationDone);
				beepOk(0);
				break;
			}
			if ( fingerCalibrationDone==2  && allVisibleFingers )
			{
				fingerCalibrationDone=3;
				beepOk(0);
				visibleInfo=false;
				drawGLScene();

				// prepare the object
				// check where top and bottom surfaces are
				whereTop = markers[4].p.transpose();
				whereBottom = markers[3].p.transpose();
				// what's next distance Z?
				absDepth = trial.getCurrent().first["AbsDepth"];
				// bring screen at Z
				initProjectionScreen(absDepth);
				// bring object at Z, with size 2 cm
				Vector3d moveBack(0.0,whereTop.y()-50.0,absDepth);
				moveObjectAbsolute(moveBack, whereBottom, 5000);
				break;
			}
		}
		break;
		case ' ':
		{
			beepOk(1);
		}
		break;
		case 13:
		{
			if(trialNumber == 0)
			{
				experimentBegun = true;
				initTrial();
			}
		}
		break;
		case 'e':
			ind.y() += 10;
		break;
		case 'd':
			ind.y() -= 10;
		break;
		case 'y':
			ind.z() += 10;
		break;
		case 'h':
			ind.z() -= 10;
		break;
		case 'j':
			ind.x() -= 10;
		break;
		case 'g':
			ind.x() += 10;
		break;
		case 'w':
			simMarkers[4].p.y() += 10;
		break;
		case 's':
			simMarkers[4].p.y() -= 10;
		break;
		case 'b':
		{
			simTimer.start();
			grasp = true;
		}
		break;
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

	optotrak->updateMarkers(simMarkers);
	markers = optotrak->getAllMarkers();

	// Visibility check
	allVisiblePlatform = isVisible(markers[1].p) && isVisible(markers[2].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisiblePatch = isVisible(markers[10].p) && isVisible(markers[11].p) && isVisible(markers[12].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[9].p);
	
	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
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

	// vertical translation (due to physical stimulus being lower than usual settings)
	Vtranslation = whereTop.y()-(stimulus_height[whichStimulus]/2);

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,Vtranslation,0);
	eyeLeft = Vector3d(-interoculardistance/2,Vtranslation,0);

	// fingers coordinates
	if ( allVisibleIndex )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}else
	{
		fingersOccluded=1;
		framesOccluded++;
	}

	// what the program checks online during the actual experiment
	if (fingerCalibrationDone==3 )
	{
		// frames counting
		frameN++;
#ifndef SIMULATION
		// index coordinates
		if(allVisibleIndex)
			ind = indexCoords.getP1();

		// thumb coordinates
		if(allVisibleThumb)
			thu = thumbCoords.getP1();
#endif

		if(ind.y() > whereTop.y() - 5 && ind.y() < whereTop.y() + 5 && ind.z() < absDepth + 20 && markers[14].v.norm() < 35)
			indexOnPlace = true;
		else
		{
			indexOnPlace = false;
			timer.start();
		}

		if(grasp && !indexOnPlace)
			ind.z() = -0.5*simTimer.getElapsedTimeInMilliSec();
		else
		{
			ind.z() = ind.z();
			grasp = false;
		}

		// Write to responseFile

		responseFile << fixed <<
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			timer.getElapsedTimeInMilliSec() << "\t" <<		//time
			frameN << "\t" <<								//frameN
			ind.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
			thu.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
			eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
			eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
			fingersOccluded	<< "\t" <<						//fingersOccluded
			framesOccluded	<< "\t" <<						//framesOccluded
			absDepth <<"\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getAscending() << "\t" <<
			trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<	
			stimulus_height[first_interval] << "\t" <<	
			stimulus_height[second_interval] << "\t" <<	
			markers[4].p.y() - markers[3].p.y() << "\t" <<	
			responsekey <<
			endl;

	}
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
        optotrak->updateMarkers(simMarkers);
        markers = optotrak->getAllMarkers();
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
	// initialize the trial matrix
	trial.init(parameters);
	// initialize the noise level
	stimulus_noise = str2num<double>(parameters.find("NoiseLevel"));
	// initialize the delta
	v_delta = str2num<double>(parameters.find("Delta"));
	h_delta = - v_delta;
}

void initGLVariables()
{

}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	// Subject name
	string subjectName = parameters.find("SubjectName");

	initVariables();

	// Principal streams files

	// response file (if haptic)
	string responseFileName = experiment_directory + responseFile_name + subjectName + "_" + stringify<int>(stimulus_noise) + ".txt";

	// Check for output file existence
	if (util::fileExists(responseFileName))
	{
		string error_on_file_io = responseFileName + string(" already exists");
		cerr << error_on_file_io << endl;
#ifndef SIMULATION
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
#endif
		exit(0);
	}
	else
	{
		responseFile.open(responseFileName.c_str());
		cerr << "File " << responseFileName << " loaded successfully" << endl;
	}

	responseFile << fixed << responseFile_headers << endl;

	// summary file
	string summaryFileName = experiment_directory + responseFile_name + subjectName + "_" + stringify<int>(stimulus_noise) + "_summary.txt";

	// Check for output file existence
	if (util::fileExists(summaryFileName))
	{
		string error_on_file_io = summaryFileName + string(" already exists");
		cerr << error_on_file_io << endl;
#ifndef SIMULATION
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
#endif
		exit(0);
	}
	else
	{
		summaryFile.open(summaryFileName.c_str());
		cerr << "File " << summaryFileName << " loaded successfully" << endl;
	}

	summaryFile << fixed << summaryFile_headers << endl;
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
	initStreams();

	// glut callback
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(mouseFunc);
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
