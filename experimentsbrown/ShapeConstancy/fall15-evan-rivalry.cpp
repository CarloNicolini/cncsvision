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

int rivalry = 1;

double spinCounter = 0;
bool spin = false;
double spinDelta = 5;

// Square
bool isSquareDrawn=false;
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;

/********* EYES AND MARKERS ****************************/
// fingers markers numbers
int ind1 = 13, ind2 = 14, ind3 = 16;
int thu1 = 15, thu2 = 17, thu3 = 18;
int calibration1 = 1, calibration2 = 2;
int calibrationobj1 = 8, calibrationobj2 = 11, calibrationobj3 = 12;
int screen1 = 19, screen2 = 20, screen3 = 21;
int mirror1 = 6, mirror2 = 7;
int phidgets1 = 3, phidgets2 = 4;

int obj_visible_marker = 0;
int size1 = 9, size2 = 10, size3 = 24, size4 = 23;
int stepper = 22;

Vector3d eyeLeft, eyeRight, ind, thm, wrist, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker, upperPinMarker(0,0,0), lowerPinMarker(0,0,0);
vector <Marker> markers;
static double interoculardistance=62;
double IOD = 0.0;
bool headCalibration=false;
double beta = 0.0;
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
// Experiment variables
ParametersLoader parameters; //from parameters file
BalanceFactor<double> trial; //constructed based on parameters file
map <std::string, double> factors; //each trial contains a factors list

// centercal is typically used as a reference point for moving the motors.
// It should correspond to the starting position (after homeEverything) of the point of interest.
Vector3d centercal(0.0,-127.0,-285.6);

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
bool fingers_calibrated = false;
bool training = true;
bool handAtStart = true;
bool started = false;
bool reachedObject = false;
bool finished = false;
bool showFingers = true;

// Display
double displayDepth = -400;
float displayLeft = -160;
float displayRight = 100;
float displayTop = 75;
float displayBottom = -125;

// Virtual target objects
double cylRad = 4;
double cylHeight;
double cyl_x=45, cyl_y, cyl_z=displayDepth, cyl_pitch=90;
double platterHeight = 0.0;

int illusion=0;
bool openloop = false;

bool light = false;
GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 100.0f, -100.0f, 1.0f};

// object-specific shifts
const int numObj=2;
int objId = 1;
double objSizes[numObj] = {38.0, 54.0};
double x_shift[numObj] = {0.0, 0.0};
double y_shift[numObj] = {0.0, 0.0};
double z_shift[numObj] = {0.0, 0.0};
// Size and position variables for the target objects
double targetOriginsX[numObj] = {45.0, 45.0};
double targetOriginsY[numObj] = {0.0, 0.0};
double targetOriginsZ[numObj] = {-400.0, -400.0};
double targetOriginX, targetOriginY, targetOriginZ; // holders for the current target

double platterRadius = 60;

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

// stepper variable
double objStepperPosns[numObj] = {0, 40};
__int64 pos;

// Incremented when stepping thru calibration procedure
// Make sure that drawInfo() and handleKeypress() are in agreement about this variable!
int fingerCalibrationDone = 0;
bool triangulate = true;

//rivalry stimulus
const int numCylsInMask = 20;
double maskSizeAdd[numCylsInMask];
double maskXAdd[numCylsInMask];
double maskYAdd[numCylsInMask];
double maskZAdd[numCylsInMask];
bool maskBuilt = false;

/********** FUNCTION PROTOTYPES *****/
void advanceTrial();
void beepOk(int tone);
void calibration_fingers(int phase);
void cleanup();
void drawCylinder(double height, int eye);
void drawGLScene();
void drawInfo();
void drawNoFingers();
void drawStimulus(int eye);
void drawTrial(int eye);
void draw_mask();
void drawFingers(double offsetZ,int eye);
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

/*************************** EXPERIMENT SPECS ****************************/
// experiment directory
string experiment_directory = "S:/Domini-ShapeLab/evan/fall15-rivalry/";
// paramters file directory and name
string parametersFile_directory = experiment_directory + "fall15-rivalry_parameters.txt";
// summary file headers
string trialFile_headers = "subjName\ttrialN\tobjId\tobjHeight\tframeN\tfingersOccluded\ttime\tdistanceToObject\treachedObject\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\ttargetX\ttargetY\ttargetZ";


/*************************** FUNCTIONS ***********************************/

// First, make sure the filenames in here are correct and that the folders exist.
// If you mess this up, data may not be recorded!
void initStreams()
{
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	string subjectName = parameters.find("SubjectName");

	// trialFile directory
	string dirName  = experiment_directory + subjectName;
	mkdir(dirName.c_str()); // windows syntax

	if (util::fileExists(dirName+"/"+subjectName+"_trial0.txt"))
	{
		string error_on_file_io = dirName+"/"+subjectName+"_trial0.txt" + string(" already exists");
		cerr << error_on_file_io << endl;
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
		exit(0);
	}

	globalTimer.start();
}

// Edit case 'f' to establish calibration procedure
// Also contains other helpful button presses (ESC for quit, i for info)
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {

	case ' ':
	{
		showFingers=!showFingers;
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

	case 'r':
	{
		if(rivalry)
			rivalry=0;
		else
			rivalry=1;
	}
	break;

    case 27:	// ESC
    {   
		homeEverything(5000,4500);
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
			fingers_calibrated = true;
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
		
		if ( fingerCalibrationDone==4 && (allVisibleFingers || !fingers_calibrated) )
		{
			// start experiment
			centercal[1] = markers[stepper].p.y();
			centercal[2] = markers[stepper].p.z();
			fingerCalibrationDone=5;
			beepOk(0);
			visibleInfo=false;
			factors = trial.getNext();
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

	case 'a':
		targetOriginsX[objId] -= 0.5;
		break;
	case 'd':
		targetOriginsX[objId] += 0.5;
		break;
	case 's':
		targetOriginsY[objId] -= 0.5;
		break;
	case 'w':
		targetOriginsY[objId] += 0.5;
		break;
	case 'e':
		targetOriginsZ[objId] += 0.5;
		break;
	case 'q':
		targetOriginsZ[objId] -= 0.5;
		break;
    case 'z':
        cyl_pitch -= 1.0;
        break;
    case 'c':
        cyl_pitch += 1.0;
        break;

	case 'l':
		light = !light;
		if(!light)
			glDisable(GL_LIGHTING);
		if(light)
			glEnable(GL_LIGHTING);

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
			text.draw("# Name: " +parameters.find("SubjectName"));
			text.draw("# IOD: " +stringify<double>(interoculardistance));
			text.draw("# Block: " +stringify<double>(ceil(trialNumber/str2num<double>(parameters.find("Repetitions")))));
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
			
            /////// Only displayed during calibration ////////
            /////// Finger Calibration ////////
			glColor3fv(glWhite);
			text.draw("#######################");
			text.draw("Calibration Step= " + stringify<int>(fingerCalibrationDone) );

            glColor3fv(glWhite);
			if (fingerCalibrationDone<4){

				///// CALIBRATION OBJECT /////
				glColor3fv(glWhite);
				text.draw("Calibration Object" );
				if ( allVisibleObject )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Platform Three " +stringify< Eigen::Matrix<double,1,3> > (markers[8].p.transpose())+ " [mm]\n" +stringify< Eigen::Matrix<double,1,3> > (markers[11].p.transpose())+ " [mm]\n" +stringify< Eigen::Matrix<double,1,3> > (markers[12].p.transpose())+ " [mm]" );

				///// INDEX FINGER ///////
				glColor3fv(glWhite);
				text.draw(" " );
				text.draw("Index" );
				if ( isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p) )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Marker " + stringify<int>(13)+stringify< Eigen::Matrix<double,1,3> > (markers[13].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(14)+stringify< Eigen::Matrix<double,1,3> > (markers[14].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(16)+stringify< Eigen::Matrix<double,1,3> > (markers[16].p.transpose())+ " [mm]" );

				/////// THUMB //////
				glColor3fv(glWhite);
				text.draw(" " );
				text.draw("Thumb" );
				if ( isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p) )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Marker " + stringify<int>(15)+stringify< Eigen::Matrix<double,1,3> > (markers[15].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(17)+stringify< Eigen::Matrix<double,1,3> > (markers[17].p.transpose())+ " [mm]" );
				text.draw("Marker " + stringify<int>(18)+stringify< Eigen::Matrix<double,1,3> > (markers[18].p.transpose())+ " [mm]" );
			}

            /////// Index and Thumb Positions ////////
            if (fingerCalibrationDone>3){
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
            }


			//////// OTHER INFO /////
			glColor3fv(glGreen);
			text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
			text.draw("Frame= " + stringify<int>(frameN));
			text.draw("Start Frame = " + stringify<double>(start_frame));
			glColor3fv(glWhite);
			text.draw("--------------------");
			glColor3fv(glGreen);
			text.draw("Object Position: " + stringify<double>(cyl_x) + stringify<double>(cyl_y) + stringify<double>(cyl_z));
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
		if(light)
			glEnable(GL_LIGHTING);
	}
}

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
        drawStimulus(0);
		drawInfo();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawStimulus(1);
		drawInfo();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawStimulus(0);
        drawStimulus(1);
		drawInfo();
        glutSwapBuffers();
    }
}

// Can check for various conditions that might affect how the graphics in here
void drawStimulus(int eye) //0==left, 1==right
{
	if (showFingers)
		drawFingers(0,eye);
	if (frameN==20)
		beepOk(2);
	// don't draw stimulus until calibration is over & not finished & motors have moved
	if (fingerCalibrationDone==5 && !finished && frameN>20) 
	{
		drawTrial(eye);
	}
}

// this draws the actual stimulus
void drawTrial(int eye) //0==left, 1==right
{
	if (eye==0 && !reachedObject)
    	drawCylinder(cylHeight,1);
    if (eye==1 && rivalry)
    	draw_mask();
}

// draw a cylinder
void drawCylinder(double height, int eye)
{
    targetOriginX = targetOriginsX[objId]+x_shift[objId];
	targetOriginY = targetOriginsY[objId]+y_shift[objId];
	targetOriginZ = targetOriginsZ[objId]+z_shift[objId];
	cyl_x = targetOriginX;
	cyl_y = targetOriginY+cylHeight/2;
	cyl_z = displayDepth-cylRad;
    
	glPushMatrix();
    glLoadIdentity();
    glColor3f(0.2f, 0.2f, 0.2f);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, LightAmbient);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, LightDiffuse);
	glTranslated(cyl_x, cyl_y, cyl_z);
	glRotated(cyl_pitch,1,0,0);

	// Cylinder
	GLUquadricObj *cyl1;
	cyl1 = gluNewQuadric();
	gluQuadricDrawStyle(cyl1, GLU_SILHOUETTE);
	gluCylinder(cyl1, cylRad, cylRad, height, 4, 1);

	glColor3f(0.2f, 0.2f, 0.2f);

	gluQuadricDrawStyle(cyl1, GLU_FILL);
	gluQuadricNormals(cyl1, GLU_SMOOTH);
	gluCylinder(cyl1, cylRad, cylRad, height, 4, 1);
	// End-disk 1
	gluQuadricOrientation(cyl1, GLU_INSIDE);
	glTranslated(0.0, 0.0, 0.0);
	gluDisk(cyl1,0,cylRad,4,1);
	// End-disk 2
	gluQuadricOrientation(cyl1, GLU_OUTSIDE);
	glTranslated(0.0, 0.0, height);
	gluDisk(cyl1,0,cylRad,4,1);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

void draw_mask()
{
	int numTilesX = 8;
	int numTilesY = 16;
	double tileSize = 4;

	for (int xpos=0-(numTilesX/2); xpos<(numTilesX/2); xpos++){
		for (int ypos=0-(numTilesY/2); ypos<(numTilesY/2); ypos++){
			
			double randNum = unifRand(0,3);
			
			if (randNum>2.0)
				glColor3f(1.0f, 0.0f, 0.0f);
			if (randNum>1.0 && randNum<=2.0)
				glColor3f(0.0f, 0.6f, 0.0f);
			if (randNum<=1.0)
				glColor3f(0.0f, 0.0f, 0.2f);

			double x1 = targetOriginX+xpos*tileSize;
			double x2 = targetOriginX+5+xpos*tileSize;
			double y1 = targetOriginY+ypos*tileSize;
			double y2 = targetOriginY+5+ypos*tileSize;

			glBegin(GL_POLYGON); 
  				glVertex3f(x1,y1,cyl_z); 
  				glVertex3f(x2,y1,cyl_z); 
  				glVertex3f(x2,y2,cyl_z); 
  				glVertex3f(x1,y2,cyl_z); 
 			glEnd();
		}
	}
}

// this draws the fingertip points
void drawFingers(double offsetZ, int eye)
{
	if (eye==1 && rivalry){
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
	maskBuilt=false;
	// Get current trial info
	if (training){
		objId = trainTrialNumber%2; // use only the largest one during training
	}else{
		objId = trial.getCurrent()["objId"];
	}
	cylHeight = objSizes[objId];
	double target_angle = objStepperPosns[objId];

	// move the screen
	initProjectionScreen(displayDepth);
	// move the arm
	Vector3d moveTo(0.0, -999.9, displayDepth-platterRadius);
	moveObjectAbsolute(moveTo,centercal,3500);
	// turn the stepper
	stepper_rotate(rotTable, target_angle);

		// Open a new trial file and give it a header
	string trialFileName;
	string subjectName = parameters.find("SubjectName");
	if (!training){
		trialFileName = experiment_directory + subjectName + "/" + subjectName + "_trial" + stringify<double>(trialNumber) + ".txt";
	}else{
		trialFileName = experiment_directory + subjectName + "/" + subjectName + "_training" + stringify<double>(trainTrialNumber) + ".txt";
	}
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << trialFile_headers;

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
		overwrite = 1;

		if (!reachedObject)
			cout << "I don't think you reached the object..." << endl;
		if (not_enough_frames){
			if (training)
				beepOk(11);
			cout << "Be visible!" << endl;
		}
		if (bad_start)
			cout << "Start was not from home position." << endl;
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
	if( !trial.isEmpty() ) {
		// If we're not overwriting due to a bad trial
		if (!overwrite){
			// Increment trial number and getNext
			trialNumber++;
			trial.next();
		}
		// Repeat trial or get the next one
		initTrial();
	} else {
		// If we're not overwriting
		if (!overwrite) { 
			// End the experiment
			finished = true;
		} else {
			// Repeat the last trial
			initTrial();
		}
	}
}

void idle()
{
	// get new marker positions from optotrak
	updateTheMarkers();

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

	// Write to trialFile once calibration is over
	if (fingerCalibrationDone==5)
	{
		trialFile << fixed <<
		parameters.find("SubjectName") << "\t" <<	//subjName
		trialNumber << "\t" <<							//trialN
		objId << "\t" <<								//objId
		cylHeight << "\t" <<							//objHeight
		frameN << "\t" <<								//frameN
		fingersOccluded << "\t" <<						//fingersOccluded
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		distanceGripCenterToObject << "\t" <<			//distanceToObject
		reachedObject << "\t" <<						//reachedObject
		ind.transpose() << "\t" <<						//indexXraw, indexYraw, indexZraw
		thm.transpose() << "\t" <<						//thumbXraw, thumbYraw, thumbZraw
		targetOriginX << "\t" <<						// target object XYZ
		targetOriginY << "\t" <<
		targetOriginZ << endl;
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
	if(fingers_calibrated)
	{
		// index coordinates
		if(allVisibleIndex)
			ind = indexCoords.getP1();

		// thumb coordinates
		if(allVisibleThumb)
			thm = thumbCoords.getP1();
	}
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
				TGA_frame = frameN;
			}
		}

	}
}

///////////////////////////////////////////////////////////
/////// USUALLY DON'T NEED TO EDIT THESE FUNCTIONS ////////
///////////////////////////////////////////////////////////


void updateTheMarkers()
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
}

void initVariables() 
{
	trial.init(parameters);
	trial.print();
	interoculardistance = str2num<double>(parameters.find("IOD"));
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
	if(light)
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

	fingerCalibrationDone=0;

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();

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

	//Vector3d down(0.0,-200.0,displayDepth);
	//moveObjectAbsolute(down,centercal,3500);
	homeEverything(5000,4500);
    stepper_close(rotTable);
    cleanup();
    return 0;
}
