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
#include <algorithm>

/********* BOOST MULTITHREADED LIBRARY ****************/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#include <MMSystem.h>
#include "SOIL.h"
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
#include "BrownMotorFunctions.h"
#include "BrownPhidgets.h"

/***** CALIBRATION FILE *****/
#include "LatestCalibration.h"

/***** DEFINE SIMULATION *****/
//#define SIMULATION
#ifndef SIMULATION
	#include <direct.h> // mkdir
#endif

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace BrownMotorFunctions;
using namespace BrownPhidgets;

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 11                               // 85 hz
#define SCREEN_WIDTH  1024 //1280                 // 1024 pixels
#define SCREEN_HEIGHT 768 //1024                  // 768 pixels

static const Vector3d center(0,0,focalDistance);

Screen screen;
double mirrorAlignment = 0.0;
double screenAlignmentY = 0.0;
double screenAlignmentZ = 0.0;
double phidgetsAlignment=0.0;

/************** PHIDGETS VARIABLES ********************/
const int axisZ = 1;
CPhidgetStepperHandle rotTable;

/********* VARIABLES OBJECTS  **************************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;
Timer timer;
Timer globalTimer;
clock_t t;
GLUquadric* qobj;
/********* VISUALIZATION AND STIMULI *******************/
#ifndef SIMULATION
	static const bool gameMode=true;
	static const bool stereo=true;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif

// Display
double displayDepth = -400;
float displayLeft = -160;
float displayRight = 100;
float displayTop = 75;
float displayBottom = -125;

// Virtual target objects
double cylRad = 4;
double cylHeight = 40;
double cyl_x=45, cyl_y=cylHeight/2, cyl_z=displayDepth, cyl_pitch=90;

double spinCounter = 0;
bool spin = false;
double spinDelta = 5;

// Square
bool isSquareDrawn=false;
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;

// int texture[2];
int illusion=0;

bool openloop = false;

bool light = true;
GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 100.0f, -100.0f, 1.0f};

/********* EYES AND MARKERS ****************************/
// fingers markers numbers
int ind1 = 13, ind2 = 14, ind3 = 16;
int thu1 = 15, thu2 = 17, thu3 = 18;
int calibration1 = 1, calibration2 = 2;
int calibrationobj1 = 8, calibrationobj2 = 11, calibrationobj3 = 12;
int screen1 = 19, screen2 = 20, screen3 = 21;
int mirror1 = 6, mirror2 = 7;
int phidgets1 = 3, phidgets2 = 4;

Vector3d eyeLeft, eyeRight, ind, thm, wrist, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker, upperPinMarker(0,0,0), lowerPinMarker(0,0,0);
vector <Marker> markers;
static double interoculardistance=0;
bool headCalibration=false;
/********* VISIBILITY VARIABLES ************************/
#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif

bool allVisiblePatch=markers_status;
bool allVisibleIndex=markers_status;
bool allVisibleThumb=markers_status;
bool allVisibleFingers=markers_status;
bool allVisibleObject=markers_status;

bool visibleInfo=true;

/********* STREAMS *************************************/
ofstream responseFile, trialFile;

/*************************************************************************************/
/*** Everything above this point stays more or less the same between experiments.  ***/
/*************************************************************************************/

/********* VARIABLES THAT CHANGE IN EACH EXPERIMENT *************************/
string experiment_directory;

// centercal is typically used as a reference point for moving the motors.
// It should correspond to the starting position (after homeEverything) of the point of interest.
//Vector3d centercal(16.5,-127.0,-285.6);//updated 10/6/14 //(29.75,-133.94,-296.16); //updated 9/25/14
//static const Vector3d centercal ...
// Variables for counting trials, frames, and lost frames
int trainTrialNumber = 0;
int trialNumber = 0;
double frameN = 0; // needs to be a double for division
double TGA_frame = 0; // needs to be a double for division
double start_frame = 0;
double num_lost_frames = 0; // needs to be a double for division
int fingersOccluded = 0;
int attempt = 1;
double maxTime; // for duration

// Flags for important states in the experiment
bool training = true;
bool handAtStart = true;
bool started = false;
bool reachedObject = false;
bool finished = false;
bool showFingers = true;

const int num_blocks = 5;
int block = 0;
vector<int> block_order;

// Experimental variables
//ParametersLoader parameters; //from parameters file
//BalanceFactor<double> trial; //constructed based on parameters file
ParametersLoader parameters[num_blocks]; //from parameters file
BalanceFactor<double> trial[num_blocks]; //constructed based on parameters file
map <std::string, double> factors; //each trial contains a factors list

// The starting position
double startPosX;
double startPosY;
double startPosZ;
double x_dist_home;
double y_dist_home;
double z_dist_home;
double distanceGripCenterToHome;
double start_dist;

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
int objId = 2;
double targetOriginX;
double targetOriginY;
double targetOriginZ;

// Incremented when stepping thru calibration procedure
// Make sure that drawInfo() and handleKeypress() are in agreement about this variable!
int fingerCalibrationDone = 0;
bool triangulate = true;

/********** FUNCTION PROTOTYPES *****/
void advanceTrial();
void beepOk(int tone);
void calibration_fingers(int phase);
void cleanup();
void drawCylinder(int cylID);
void drawGLScene();
void drawInfo();
void drawNoFingers();
void drawStimulus();
void drawTrial();
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
//void LoadGLTextures();
void onMouseClick(int button, int state, int x, int y);
void update(int value);
void updateTheMarkers();

// online operations
void online_apparatus_alignment();
void online_fingers();
void online_trial();

/*************************** FUNCTIONS ***********************************/

// First, make sure the filenames in here are correct and that the folders exist.
// If you mess this up, data may not be recorded!
void initStreams()
{
	//ifstream parametersFile;
	//string paramFn = "C:/workspace/cncsvisioncmake/experimentsbrown/parameters/evanHapticIllusion15/parametersEvanHapticIllusion15.txt";
	//parametersFile.open(paramFn.c_str());
	//parameters.loadParameterFile(parametersFile);

	//string subjectName = parameters.find("SubjectName");

	// For experiments with multiple blocks:
	for(int ii=0; ii<num_blocks; ii++)
	{
		ifstream parametersFile;
		int jj = ii+1;
		#ifndef SIMULATION
			string paramFn = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/parametersEvanMoreIllusions"+stringify<int>(jj)+".txt";
		#else
			string paramFn = "/media/shapelab/Domini-ShapeLab/evan/summer15-moreIllusions/parametersEvanMoreIllusions"+stringify<int>(jj)+".txt";
		#endif
		parametersFile.open(paramFn.c_str());
		parameters[ii].loadParameterFile(parametersFile);
	}
	// Subject name - only needs to be set in the **Block 1** parameters file
    string subjectName = parameters[0].find("SubjectName");

    // experiment directory
	#ifndef SIMULATION
		experiment_directory = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/";
	#else
		experiment_directory = "/media/shapelab/Domini-ShapeLab/evan/summer15-moreIllusions/";
	#endif

	// Markersfile directory
	string dirName  = experiment_directory + subjectName;
	#ifdef SIMULATION
		mkdir(dirName.c_str(), 777); // linux syntax
	#else
		mkdir(dirName.c_str()); // windows syntax
	#endif

	globalTimer.start();
}

// Edit case 'f' to establish calibration procedure
// Also contains other helpful button presses (ESC for quit, i for info)
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {

	case ' ':
	{
		
	}
	break;

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
		//interoculardistance += 0.5;
		//headEyeCoords.setInterOcularDistance(interoculardistance);
	}
	break;
	
	case 'n':
	{
		//interoculardistance -= 0.5;
		//headEyeCoords.setInterOcularDistance(interoculardistance);
	}

	case '*':
	{
		illusion += 1;
		illusion = illusion%3;
	}
	break;

	case 'r':
	{
		
	}
	break;

    case 27:	// ESC
    {   
    	stepper_close(rotTable);
		cleanup();
        exit(0);
    }
    break;

	case 'f':
	case 'F':
	{
		// calibration_fingers handles step 1 with the block
        
		if ( fingerCalibrationDone==2 && allVisibleFingers &&  allVisibleObject )
		{
			// set index and thumb tips
			calibration_fingers(3);
			fingerCalibrationDone=3;
			beepOk(0);
			break;
		}
		if ( fingerCalibrationDone==3 && allVisibleFingers )
		{
			// set start posn
			startPosX = ind.x();
			startPosY = ind.y();
			startPosZ = ind.z();
			fingerCalibrationDone=4;
			beepOk(0);
			break;
		}
		
		if ( fingerCalibrationDone==4 && allVisibleFingers )
		{
			// start experiment
			fingerCalibrationDone=5;
			beepOk(0);
			visibleInfo=false;
			factors = trial[block_order[block]].getNext();
			initTrial();
		}
	}
	break;

    case '.':
		advanceTrial();
		break;

	case 't':
		training=false;
		beepOk(0);
		// Close the trial file
		trialFile.close();
		// Initialize the first non-training trial
		initTrial();
		break;

	// case '1':
	// 	cylR_x -= 1.0;
	// 	break;
	// case '3':
	// 	cylR_x += 1.0;
	// 	break;
	// case '2':
	// 	cylR_y -= 1.0;
	// 	break;
	// case '5':
	// 	cylR_y += 1.0;
	// 	break;
	// case '4':
	// 	cylR_z += 1.0;
	// 	break;
	// case '6':
	// 	cylR_z -= 1.0;
	// 	break;
 //    case '7':
 //        cylR_pitch -= 1.0;
 //        break;
 //    case '9':
 //        cylR_pitch += 1.0;
 //        break;

	case 'a':
		cyl_x -= 1.0;
		break;
	case 'd':
		cyl_x += 1.0;
		break;
	case 's':
		cyl_y -= 1.0;
		break;
	case 'w':
		cyl_y += 1.0;
		break;
	case 'e':
		cyl_z += 1.0;
		break;
	case 'q':
		cyl_z -= 1.0;
		break;
    case 'z':
        cyl_pitch -= 1.0;
        break;
    case 'c':
        cyl_pitch += 1.0;
        break;

	// case 'o':
	// 	openloop = !openloop;
	// 	break;
	}
}

void onMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
  { 
     
  }	
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
  {
	  
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  {
	 
  }
}

/*** GRASP ***/
void calibration_fingers(int phase)
{
	switch (phase)
	{
		case 1:
		{
			if(allVisibleObject)
			{
				upperPinMarker=markers.at(calibration1).p;
				upperPin.init(upperPinMarker, markers.at(calibrationobj1).p, markers.at(calibrationobj2).p, markers.at(calibrationobj3).p );
			}
		} break;
		case 2:
		{
			if(allVisibleObject)
			{
				lowerPinMarker=markers.at(calibration2).p;
				lowerPin.init(lowerPinMarker, markers.at(calibrationobj1).p, markers.at(calibrationobj2).p, markers.at(calibrationobj3).p );
			}
		} break;
		case 3:
		{
			indexCoords.init(upperPin.getP1(), markers.at(ind1).p, markers.at(ind2).p, markers.at(ind3).p );
			thumbCoords.init(lowerPin.getP1(), markers.at(thu1).p, markers.at(thu2).p, markers.at(thu3).p );
		} break;
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

			switch (fingerCalibrationDone)
			{
			case 0:
				text.draw("Expose three and upper pin");
				break;
			case 1:
				text.draw("Expose three and lower pin");
				break;
			case 2:
				text.draw("Grasp the pins, expose three and posts, press F");
				break;
			case 3:
				text.draw("Place hand in start position, then press F");
				break;
			case 4:
				text.draw("Press F to begin!");
				break;
			} // end switch(fingerCalibrationDone)

            /////// Header ////////
			text.draw("####### ####### #######");
			text.draw("#");
			//text.draw("# Name: " +parameters.find("SubjectName"));
			text.draw("# Name: " +parameters[0].find("SubjectName"));
			text.draw("# IOD: " +stringify<double>(interoculardistance));
			text.draw("# Block: " +stringify<double>(block+1));
			text.draw("# Trial: " +stringify<double>(trialNumber));
            
            /////// Mirror and Screen Alignment ////////
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
            
            /////// Finger Calibration ////////
			glColor3fv(glWhite);
			text.draw("#######################");
			text.draw("Calibration Step= " + stringify<int>(fingerCalibrationDone) );

			glColor3fv(glWhite);
			text.draw("Calibration Object" );
			if ( allVisibleObject )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Platform Three "
				+stringify< Eigen::Matrix<double,1,3> > (markers[8].p.transpose())+ " [mm]\n"
				+stringify< Eigen::Matrix<double,1,3> > (markers[11].p.transpose())+ " [mm]\n"
				+stringify< Eigen::Matrix<double,1,3> > (markers[12].p.transpose())+ " [mm]" );
			if ( isVisible(markers[1].p) )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Index Marker " + stringify<int>(1)
				+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
			if ( isVisible(markers[2].p) )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Thumb Marker " + stringify<int>(2)
				+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
			
            /////// Only displayed during calibration ////////
            glColor3fv(glWhite);
			if (fingerCalibrationDone!=5){

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
            
            /////// Index and Thumb Positions ////////
			glColor3fv(glWhite);
			text.draw("--------------------");
			if (allVisibleIndex)
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(ind.transpose()));
			if (allVisibleThumb)
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thm.transpose()));
			glColor3fv(glWhite);
			text.draw("--------------------");
            
            /////// Extra Info ////////
			glColor3fv(glGreen);
			text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
			text.draw("Frame= " + stringify<int>(frameN));
			glColor3fv(glWhite);
			text.draw("--------------------");
			glColor3fv(glGreen);
			text.draw("Distance To Object = " + stringify<double>(distanceGripCenterToObject));
			text.draw("Distance To Start = " + stringify<double>(distanceGripCenterToHome));
			//text.draw("Object Size Check = " + stringify<double>(distanceBetweenSurfaces));
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
            if (training){
                text.draw("TRAINING MODE");
            }
		}
		text.leaveTextInputMode();
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}
}

// void LoadGLTextures()
// {
// 	// load image files directly as new OpenGL textures
// 	texture[0] = SOIL_load_OGL_texture("SOIL_textures/PonzoDisplayFinal.png",
// 		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
// 	//Typical texture generation using data from the bitmap
// 	glBindTexture(GL_TEXTURE_2D,texture[0]);
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

// 	if (texture[0]==0)
// 		beepOk(3);

// 	texture[1] = SOIL_load_OGL_texture("SOIL_textures/MullerLyerDisplayFinal.png",
// 		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
// 	//Typical texture generation using data from the bitmap
// 	glBindTexture(GL_TEXTURE_2D,texture[1]);
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
// }

// This will be called at 85hz in the main loop
// Not too much to change here usually, sub-functions do the work.
void drawGLScene() 
{
	online_apparatus_alignment();
	online_fingers();
	online_trial();

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
	if (frameN==100)
		beepOk(2);
	// don't draw stimulus until calibration is over & not finished & motors have moved
	if (fingerCalibrationDone>3 && !finished && frameN>100) 
	{
		if(!(openloop && !handAtStart && started))
			drawTrial();
		if (showFingers)
			drawFingers(0);
	}
}

// this draws the actual stimulus
void drawTrial()
{
    drawCylinder(0); //left cylinder
    //drawCylinder(1); //right cylinder

    int verticalDistance = 100;
    int horizontalDistance = 180;
    double planeAngle=0;
    
    // control background
    if(illusion==1){
    	planeAngle = 67;
    	horizontalDistance = 450;
    }else if(illusion==2){
    	planeAngle = -67;
    	horizontalDistance = 450;
    }

	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,-500);
	glRotated(planeAngle,0,1,0);
	if(illusion==1){
		glTranslated(75,0,0);
	}else if(illusion==2){
		glTranslated(-75,0,0);
	}
	glBegin(GL_LINE_LOOP);
	glVertex3f((-horizontalDistance/2),verticalDistance/2,0);
	glVertex3f((horizontalDistance/2),verticalDistance/2,0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f((-horizontalDistance/2),-verticalDistance/2,0);
	glVertex3f((horizontalDistance/2),-verticalDistance/2,0);
	glEnd();
	double spacing = horizontalDistance/9;
	for(int i=0;i<10;i++){
		glBegin(GL_LINE_LOOP);
		glVertex3f((-horizontalDistance/2)+(i*spacing),verticalDistance/2,0);
		glVertex3f((-horizontalDistance/2)+(i*spacing),-verticalDistance/2,0);
		glEnd();
	}
	glPopMatrix();

}

// draw a cylinder
void drawCylinder(int cylID)
{
    //glMatrixMode(GL_MODELVIEW);
    
	glPushMatrix();
    glLoadIdentity();
    glColor3f(0.1f, 0.1f, 0.1f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, LightAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, LightDiffuse);
	glTranslated(cyl_x, cyl_y, cyl_z);
	glRotated(cyl_pitch,1,0,0);

	// Cylinder
	GLUquadricObj *cyl1;
	cyl1 = gluNewQuadric();
	gluQuadricDrawStyle(cyl1, GLU_LINE);
	gluCylinder(cyl1, cylRad, cylRad, cylHeight, 16, 1);
	// The object should change color to indicate that it's been "reached"
	if (!reachedObject) {
		glColor3f(0.7f, 0.0f, 0.2f);
	} else {
		glColor3f(0.0f, 0.2f, 0.7f);
	}
	gluQuadricDrawStyle(cyl1, GLU_FILL);
	gluQuadricNormals(cyl1, GLU_SMOOTH);
	gluCylinder(cyl1, cylRad, cylRad, cylHeight, 16, 1);
	// End-disk 1
	gluQuadricOrientation(cyl1, GLU_INSIDE);
	glTranslated(0.0, 0.0, 0.0);
	gluDisk(cyl1,0,cylRad,16,1);
	// End-disk 2
	gluQuadricOrientation(cyl1, GLU_OUTSIDE);
	glTranslated(0.0, 0.0, cylHeight);
	gluDisk(cyl1,0,cylRad,16,1);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

// this draws the fingertip points
void drawFingers(double offsetZ)
{
	// Compute distance in the sagittal plane (assuming object is at (x,z)=(0,objDepth))
	//double indexObjectDistance = sqrt( pow(ind.x(), 2) 
	//	+ pow( ind.z() - objDepth , 2) );
	//double thumbObjectDistance = sqrt( pow(thm.x(), 2) 
	//	+ pow( thm.z() - objDepth , 2) );
	
	glPushMatrix();
	glLoadIdentity();
	glTranslated(ind.x(),ind.y(),ind.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	glTranslated(thm.x(),thm.y(),thm.z());
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
}

// called at the beginning of every trial
void initTrial()
{
	// initializing all variables
	frameN=0;
	TGA_frame = 0;
	start_frame = 0;
	started = false;
	reachedObject = false;
	fingersOccluded = 0;
	num_lost_frames = 0;

	// Get current trial info
	illusion = trial[block_order[block]].getCurrent()["illusion"];
	int objSize = trial[block_order[block]].getCurrent()["objSize"];

	if(illusion==0){ // CONTROL
		if(objSize==1){
			cylHeight=38;
			objId=2;
		}else if(objSize==2){
			cylHeight=40;
			objId=5;
		}else if(objSize==3){
			cylHeight=42;
			objId=8;
		}
	}else if(illusion==1){ // REDUCE
		if(objSize==1){
			cylHeight=39;
			objId=3;
		}else if(objSize==2){
			cylHeight=41;
			objId=6;
		}else if(objSize==3){
			cylHeight=43;
			objId=9;
		}
	}else if(illusion==2){ // EXPAND
		if(objSize==1){
			cylHeight=37;
			objId=1;
		}else if(objSize==2){
			cylHeight=39;
			objId=4;
		}else if(objSize==3){
			cylHeight=41;
			objId=7;
		}
	}

	if (training)
		objId = (rand() % 9)+1;

	// move the screen
	initProjectionScreen(displayDepth);

	// set the phidgets
	stepper_rotate(rotTable, (objId-1) * 30.0 / M_PI);
	//phidgets_linear_move(objside-30.0, axisZ);

	// Find center of physical haptic stimulus
    targetOriginX = cyl_x;
    targetOriginY = cyl_y-cylHeight/2;
    targetOriginZ = cyl_z-cylRad/2;

	// Open a new trial file and give it a header
	string trialFileName;
	if (!training){
		trialFileName = experiment_directory + parameters[0].find("SubjectName") + "/" + parameters[0].find("SubjectName") + "_block" + stringify<double>(block) + "_trial" + stringify<double>(trialNumber) + ".txt";
	}else{
		trialFileName = experiment_directory + parameters[0].find("SubjectName") + "/" + parameters[0].find("SubjectName") +"_training" + stringify<double>(trainTrialNumber) + ".txt";
	}
	
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\tblockN\ttrialN\tobjId\tobjHeight\tillusion\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\tdistanceToObject\tfingersOccluded\treachedObject" << endl;

	// roll on
	drawGLScene();

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
	bool bad_start = start_dist > 40;

	#ifndef SIMULATION
	// Check whether there is some reason to reinsert
	if ( !reachedObject || not_enough_frames || bad_start ) {
		beepOk(3);
		if (training){
			cout << "Training Mode!" << endl;
			trainTrialNumber++;
		}
		if (!reachedObject)
			cout << "I don't think you reached the object..." << endl;
		if (not_enough_frames){
			if (training)
				beepOk(11);
			cout << "Be visible!" << endl;
		}
		// Reinsert and set the overwrite flag for respFile
		//map<std::string,double> currentFactorsList = trial.getCurrent();
		//trial.reinsert(currentFactorsList);
		//map<std::string,double> currentFactorsList = trial[block_order[block]].getCurrent();
		//trial[block_order[block]].reinsert(currentFactorsList);
		overwrite = 1;
	}
	#endif

	 if (training){
		overwrite=1;
		trainTrialNumber++;
		cout << "Training Mode!" << endl;
	}

	// Close the trial file
	trialFile.close();

	// If there are more trials in the current block (block is not empty)
	if( !trial[block_order[block]].isEmpty() ) {
		// If we're not overwriting due to a bad trial
		if (!overwrite){
			// Increment trial number and getNext
			trialNumber++;
			trial[block_order[block]].next();
			//factors = trial.getNext();
		}
		// Get the next trial and initialize it!
		initTrial();

	// If we have reached the end of the final block
	} else if (block == num_blocks-1) { 
		finished = true;
		responseFile.close();

	// Otherwise we need to advance block
	} else {
		block++;
		trialNumber=0;
		//newBlock=true;
		factors = trial[block_order[block]].getNext();
		initTrial();
	}
}

void idle()
{
	// get new marker positions from optotrak
	updateTheMarkers();

	// eye coordinates
	eyeRight = Vector3d(0.0/2,0,0);
	eyeLeft = Vector3d(-0.0/2,0,0);

	// Write to trialFile once calibration is over
	
	if (fingerCalibrationDone==5)
	{
		trialFile << fixed <<
		//parameters.find("SubjectName") << "\t" <<	//subjName
		parameters[0].find("SubjectName") << "\t" <<	//subjName
		block << "\t" <<								//blockN
		trialNumber << "\t" <<							//trialN
		objId << "\t" <<								//objId
		cylHeight << "\t" <<							//objHeight
		illusion << "\t" <<								//illusion
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		frameN << "\t" <<								//frameN
		ind.transpose() << "\t" <<						//indexXraw, indexYraw, indexZraw
		thm.transpose() << "\t" <<						//thumbXraw, thumbYraw, thumbZraw
		distanceGripCenterToObject << "\t" <<			//distanceToObject
		fingersOccluded << "\t" <<						//fingersOccluded
		reachedObject << endl;							//reachedObject
	}

	// conditions for trial advance
	if(handAtStart && started)
		advanceTrial();
}



/*** Online operations ***/
void online_apparatus_alignment()
{
	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers.at(mirror1).p.z()-markers.at(mirror2).p.z()))/
			sqrt(
			pow(markers.at(mirror1).p.x()-markers.at(mirror2).p.x(), 2) +
			pow(markers.at(mirror1).p.z()-markers.at(mirror2).p.z(), 2)
			)
			)*180/M_PI;

	// phidgets alignment check
	phidgetsAlignment = asin(
			abs((markers.at(phidgets1).p.z()-markers.at(phidgets2).p.z()))/
			sqrt(
			pow(markers.at(phidgets1).p.x()-markers.at(phidgets2).p.x(), 2) +
			pow(markers.at(phidgets1).p.z()-markers.at(phidgets2).p.z(), 2)
			)
			)*180/M_PI;

	// screen Y alignment check
	screenAlignmentY = asin(
			abs((markers.at(screen1).p.y()-markers.at(screen3).p.y()))/
			sqrt(
			pow(markers.at(screen1).p.x()-markers.at(screen3).p.x(), 2) +
			pow(markers.at(screen1).p.y()-markers.at(screen3).p.y(), 2)
			)
			)*180/M_PI;

	// screen Z alignment check
	screenAlignmentZ = asin(
			abs(markers.at(screen1).p.z()-markers.at(screen2).p.z())/
			sqrt(
			pow(markers.at(screen1).p.x()-markers.at(screen2).p.x(), 2) +
			pow(markers.at(screen1).p.z()-markers.at(screen2).p.z(), 2)
			)
			)*180/M_PI*
			abs(markers.at(screen1).p.x()-markers.at(screen2).p.x())/
			(markers.at(screen1).p.x()-markers.at(screen2).p.x());
}

void online_fingers()
{
	// Visibility check
	allVisibleIndex = isVisible(markers.at(ind1).p) && isVisible(markers.at(ind2).p) && isVisible(markers.at(ind3).p);
	allVisibleThumb = isVisible(markers.at(thu1).p) && isVisible(markers.at(thu2).p) && isVisible(markers.at(thu3).p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisibleObject = isVisible(markers.at(calibrationobj1).p) && isVisible(markers.at(calibrationobj2).p) && isVisible(markers.at(calibrationobj3).p);

	// fingers coordinates, fingersOccluded and framesOccluded
	if ( allVisibleFingers )
	{
		indexCoords.update(markers.at(ind1).p, markers.at(ind2).p, markers.at(ind3).p );
		thumbCoords.update(markers.at(thu1).p, markers.at(thu2).p, markers.at(thu3).p );
	}

	// Record the calibration platform's position and home position
	if ( isVisible(markers.at(calibration1).p) && allVisibleObject && fingerCalibrationDone==0 )
	{
		fingerCalibrationDone=1;
		calibration_fingers(fingerCalibrationDone);
	}

	// Record the calibration platform's position and home position
	if ( isVisible(markers.at(calibration2).p) && allVisibleObject && fingerCalibrationDone==1 )
	{
		fingerCalibrationDone=2;
		calibration_fingers(fingerCalibrationDone);
		beepOk(2);
	}

	if ( allVisibleObject && fingerCalibrationDone==2 )
	{
		upperPin.update(markers.at(calibrationobj1).p, markers.at(calibrationobj2).p, markers.at(calibrationobj3).p );
		lowerPin.update(markers.at(calibrationobj1).p, markers.at(calibrationobj2).p, markers.at(calibrationobj3).p );
	}

	#ifndef SIMULATION
	// index coordinates
	if(allVisibleIndex)
		ind = indexCoords.getP1();

	// thumb coordinates
	if(allVisibleThumb)
		thm = thumbCoords.getP1();
	#endif
}

void online_trial()
{
	//////////////////////////////////////
	// While the experiment is running! //
	//////////////////////////////////////
	if (fingerCalibrationDone==5 && !finished)
	{
		// Check for finger occlusion
		if ( !allVisibleFingers )
		{
			fingersOccluded = 1;
			if (started && !reachedObject) // only increment if we're in flight
			{
				num_lost_frames += 1;
			}
		}
		
		// Advance frame number
		frameN++;

		// find distance from grip center to object center
		grip_Origin_X = (ind.x()+thm.x())/2;
		grip_Origin_Y = (ind.y()+thm.y())/2;
		grip_Origin_Z = (ind.z()+thm.z())/2;
		x_dist = abs(grip_Origin_X - targetOriginX);
		y_dist = abs(grip_Origin_Y - targetOriginY);
		z_dist = abs(grip_Origin_Z - targetOriginZ);
		distanceGripCenterToObject = sqrt((x_dist*x_dist)+(y_dist*y_dist)+(z_dist*z_dist));

		x_dist_home = abs(grip_Origin_X - startPosX);
		y_dist_home = abs(grip_Origin_Y - startPosY);
		z_dist_home = abs(grip_Origin_Z - startPosZ);
		distanceGripCenterToHome = sqrt((x_dist_home*x_dist_home)+(y_dist_home*y_dist_home)+(z_dist_home*z_dist_home));
		handAtStart = distanceGripCenterToHome<30;

		// compute grip aperture
		grip_aperture = sqrt(
			(ind.x() - thm.x())*(ind.x() - thm.x()) + 
			(ind.y() - thm.y())*(ind.y() - thm.y()) + 
			(ind.z() - thm.z())*(ind.z() - thm.z())
			);

		// If we haven't started yet
		if (handAtStart)
		{	
			// keep resetting timer
			maxTime = timer.getElapsedTimeInMilliSec();
			timer.start();
		} else {
			// otherwise we are in flight, so set flags and let the timer run
			if (start_frame==0){
				start_frame=frameN;
				start_dist = distanceGripCenterToHome;
			}
			started = true;
		}

		// if we are still approaching object
		if (!reachedObject && started) {
			// when conditions for "end criterion" are satisfied (usually re: GA and distanceToObject)
			if ( (distanceGripCenterToObject<=10) && (grip_aperture<(cylHeight+10)) ){
				// set flag and record the frame (for computing % missing frames)
				reachedObject = true;
				//spin = true;
				TGA_frame = frameN;
			}
		}

	}
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

///////////////////////////////////////////////////////////
/////// USUALLY DON'T NEED TO EDIT THESE FUNCTIONS ////////
///////////////////////////////////////////////////////////


void updateTheMarkers()
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
}

// change the body of this one if we have multiple blocks filled with same parameters
void initVariables() 
{
	//trial.init(parameters);
	//trial.print();
	// For blocked experiments:
	for(int ii=0; ii<num_blocks; ii++)
	{
		trial[ii].init(parameters[ii]);
	}
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
	// Clear buffers
	glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Set depth buffer clear value */
    glClearDepth(1.0);

    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);

	// Not sure...
	//glEnable(GL_CULL_FACE);

	// Load in the illusions as textures
	//LoadGLTextures(); 

    /* Set depth function */
    glDepthFunc(GL_LEQUAL);

	// Nice perspective calculations
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Set up the lighting
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	// Clean modelview matrix to start
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glLineWidth(1.5);
}

void initMotors()
{
	homeEverything(5000,4500);
}

void initOptotrak()
{
    optotrak.setTranslation(calibration);

    if ( optotrak.init(LastAlignedFile, OPTO_NUM_MARKERS, OPTO_FRAMERATE, OPTO_MARKER_FREQ, OPTO_DUTY_CYCLE,OPTO_VOLTAGE) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }

    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        updateTheMarkers();
    }
}

void cleanup()
{
	// Stop the optotrak
	optotrak.stopCollection();
}

void beepOk(int tone)
{
	#ifndef SIMULATION
		switch(tone)
		{
		case 0:
	    // Remember to put double slash \\ to specify directories!!!
	    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", 
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 1:
	    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\calibrate.wav", 
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
		case 7:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-left.wav", 
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 8:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-right.wav", 
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 9:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-home.wav", 
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 10:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-grasp.wav", 
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 11:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-marker.wav",
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 12:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-estimate.wav",
			NULL, SND_FILENAME | SND_ASYNC);
		break;
		}
	#endif
	return;
}

///////////////////////////////////////////////////////////
////////////////////// MAIN FUNCTION //////////////////////
///////////////////////////////////////////////////////////

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	// cout << "Please enter the subject's group number (1-4): " << endl;
	// cin >> groupNum;

	// switch (groupNum){
	// 	case 1:
	// 		{
	// 			for (int n=0; n<20; n++)
	// 				posnSequence[n] = posnSequence1[n];
	// 		}
	// 		break;
	// 	case 2:
	// 		{
	// 			for (int n=0; n<20; n++)
	// 				posnSequence[n] = posnSequence2[n];
	// 		}
	// 		break;
	// 	case 3:
	// 		{
	// 			for (int n=0; n<20; n++)
	// 				posnSequence[n] = posnSequence3[n];
	// 		}
	// 		break;
	// 	case 4:
	// 		{
	// 			for (int n=0; n<20; n++)
	// 				posnSequence[n] = posnSequence4[n];
	// 		}
	// 		break;
	// }

	fingerCalibrationDone=0;
	
	for (int i=0; i<num_blocks; ++i)
		block_order.push_back(i);

	//random_shuffle(block_order.begin(), block_order.end());

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();

	//    push back the apparatus
	//Vector3d object_reset_position(0,0,0);
	//object_reset_position = markers[3].p.transpose();
	//    calculate where the object has to go
	//Vector3d object_position(0.0,object_reset_position.y(),-800);
	//    move the object to position
	//moveObjectAbsolute(object_position, object_reset_position, 5000);

	reset_phidgets_linear();
	rotTable = stepper_connect();

    glutInit(&argc, argv);
	#ifdef SIMULATION
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutCreateWindow("Simulation test");
	#else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
		glutGameModeString(GAME_MODE_STRING);
		glutEnterGameMode();
		glutFullScreen();
	#endif

    initRendering();
	//initGLVariables();
	initStreams();
	initVariables(); // variable "trial" is built
	//printf( "EVAN EVAN EVAN" );
    glutDisplayFunc(drawGLScene);
	glutMouseFunc(onMouseClick);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);

	boost::thread initVariablesThread(&initVariables);

    /* Application main loop */
	//HWND hwnd = FindWindow( "GLUT", "Evan Experiment" );
	//SetWindowPos( hwnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOREPOSITION | SWP_NOSIZE );
	//SetForegroundWindow(hwnd);
    glutMainLoop();

    stepper_close(rotTable);
    cleanup();
    return 0;
}
