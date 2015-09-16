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
double focalDistance = -270.0;
double homeFocalDistance =-270.0;
static const Vector3d calibration(160,179,-75);
static const Vector3d center(0,0,focalDistance);

Screen screen;
double mirrorAlignment = 0.0;
double screenAlignmentY = 0.0;
double screenAlignmentZ = 0.0;
/********* VARIABLES OBJECTS  **************************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;
Timer timer;
Timer globalTimer;
clock_t t;
GLUquadric* qobj;
/********* VISUALIZATION AND STIMULI *******************/

// Display
double displayDepth = -400;
float displayLeft = -160;
float displayRight = 100;
float displayTop = 75;
float displayBottom = -125;

// Virtual target objects
double cylL_x=-64, cylL_y=-5, cylL_z=displayDepth, cylL_pitch=90;
double cylR_x=38, cylR_y=-5, cylR_z=displayDepth, cylR_pitch=90;
double cylRad = 5;
double cylHeight = 40;

double spinCounter = 0;
bool spin = false;
double spinDelta = 5;

// Square
bool isSquareDrawn=false;
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;

int texture[2];
int illusionID=0;

bool openloop = false;

bool light = true;
GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 100.0f, -100.0f, 1.0f};

/********* EYES AND MARKERS ****************************/
Vector3d eyeLeft, eyeRight, index, thumb, wrist, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker, upperPinMarker(0,0,0), lowerPinMarker(0,0,0);
vector <Marker> markers;
static double interoculardistance=60.5;
bool headCalibration=false;
/********* VISIBILITY VARIABLES ************************/
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisibleWrist=false;
bool allVisiblePlatform1=false;
bool allVisiblePlatform2=false;
bool allVisiblePlatform3=false;
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
double perceptual_estimate;
double maxTime; // for duration

// Flags for important states in the experiment
bool training = true;
int estimate_given = 0;
bool handAtStart = true;
bool started = false;
bool reachedObject = false;
bool finished = false;
bool showFingers = true;

/* Trial modes can be used to create variation outside of parameters
const int CLOSED = 2;
const int OPEN = 1
const int BLIND = 0
int visualConditions[3] = {BLIND OPEN CLOSED};
const int EBB = 2;
const int PON = 1
const int MUL = 0
int illusions[3] = {MUL PON EBB};
*/

const int num_blocks = 3;
int block = 0;
vector<int> block_order;

//bool burn_in = true;
//int burn_count = 1;

// Experimental variables
//ParametersLoader parameters; //from parameters file
//BalanceFactor<double> trial; //constructed based on parameters file
ParametersLoader parameters[num_blocks]; //from parameters file
BalanceFactor<double> trial[num_blocks]; //constructed based on parameters file
map <std::string, double> factors; //each trial contains a factors list

// Boundaries of the starting position
double startPosTop;
double startPosFront;
double startPosRight;
double startPosLeft;
double startPosRear;
double startPosBottom;
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

//double hapticSize = 40;
//double visualSize = 40;
//double objDepth = 10;
//double objWidth = 10;

//double leftObjX;
//double leftObjY;
//double leftObjZ;
//double rightObjX;
//double rightObjY;
//double rightObjZ;

int groupNum;
int posnSequence[20];
int posnSequence1[20]={1,0,0,1,1,1,0,0,1,0,1,0,1,1,0,0,1,1,0,0};
int posnSequence2[20]={1,1,0,1,0,0,1,1,0,1,1,0,0,1,0,1,1,0,0,0};
int posnSequence3[20]={0,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,1,1};
int posnSequence4[20]={0,0,1,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,1,1};
int posnSequence5[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int posnSequence6[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

// Incremented when stepping thru calibration procedure
// Make sure that drawInfo() and handleKeypress() are in agreement about this variable!

int fingerCalibration = 0;
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
void LoadGLTextures();
void onMouseClick(int button, int state, int x, int y);
void update(int value);

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
		string paramFn = "C:/workspace/cncsvisioncmake/experimentsbrown/parameters/evanMoreIllusions/parametersEvanMoreIllusions"+stringify<int>(jj)+".txt";
		parametersFile.open(paramFn.c_str());
		parameters[ii].loadParameterFile(parametersFile);
	}
	// Subject name - only needs to be set in the **Block 1** parameters file
    string subjectName = parameters[0].find("SubjectName");

	// Markersfile directory
	string dirName  = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/" + subjectName;
	mkdir(dirName.c_str());

	// Principal streams file
	string responseFileName = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/" + subjectName + "/exp01_" + subjectName + ".txt";

	// Check for output file existence 
	/// Response file
	if ( !fileExists((responseFileName)) ) {
        responseFile.open((responseFileName).c_str());
	} else {
		exit(0);
	}

	responseFile << fixed << "subjName\tblock\tillusion\tobjID\ttrialN\ttrialDuration\tobjectSize\tperceptualEstimate\topenloop\ttrainingMode" << endl;
	globalTimer.start();
}

// Edit case 'f' to establish calibration procedure
// Also contains other helpful button presses (ESC for quit, i for info)
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {

	case ' ':
	{
		if (allVisibleFingers){
			perceptual_estimate=grip_aperture;
			estimate_given=1;
			beepOk(9);
		}
		else
			beepOk(12);
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
		illusionID += 1;
		illusionID = illusionID%2;
	}
	break;

	case 'r':
	{
		beepOk(objId+7);
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
		// calibration_fingers handles step 1 with the block
        
		if ( fingerCalibration==2 && allVisibleFingers && allVisiblePlatform3 )
		{
			// set index and thumb tips
			calibration_fingers(3);
			fingerCalibration=3;
			beepOk(0);
			break;
		}
		if ( fingerCalibration==3 && allVisibleFingers )
		{
			// set start posn
			startPosLeft = index.x() - 20;
			startPosRight = index.x() + 20;
			startPosBottom = index.y() - 20;
			startPosTop = index.y() + 20;
			startPosFront = index.z() - 20;
			startPosRear = index.z() + 20;
			fingerCalibration=6;
			beepOk(0);
			break;
		}
		/*if ( fingerCalibration==4 && allVisibleFingers )
		{
			// set left obj position (origin for drawing isn't center...)
            
            //leftObjX = index.x();
			//leftObjY = index.y();
			//leftObjZ = index.z();
            
            cylL_x = index.x(); // leftObjX - cylRad/2;
            cylL_y = index.y(); // leftObjY + cylHeight/2;
            cylL_z = displayDepth; //leftObjZ - cylRad/2;

            fingerCalibration=5;
			beepOk(0);
			break;
		}
		if ( fingerCalibration==5 && allVisibleFingers )
		{
			// set right obj position (origin for drawing isn't center...)
			cylR_x = index.x();
			cylR_y = index.y();
			cylR_z = displayDepth;
			//cylR_x = rightObjX - cylRad/2;
			//cylR_y = rightObjY + cylHeight/2;
			//cylR_z = rightObjZ - cylRad/2;
			fingerCalibration=6;
			beepOk(0);
			break;
		}*/
		if ( fingerCalibration==6 && allVisibleFingers )
		{
			// start experiment
			fingerCalibration=7;
			beepOk(0);
			visibleInfo=false;
			//factors = trial.getNext();
			factors = trial[block_order[block]].getNext();
			initTrial();
		}
	}
	break;

    case '.':
		advanceTrial();
		break;

	case 't':
		if(training)
			block++;
		training=false;
		beepOk(0);
		// Close the trial file
		trialFile.close();
		// Initialize the first non-training trial
		initTrial();
		break;

	case '1':
		cylR_x -= 1.0;
		break;
	case '3':
		cylR_x += 1.0;
		break;
	case '2':
		cylR_y -= 1.0;
		break;
	case '5':
		cylR_y += 1.0;
		break;
	case '4':
		cylR_z += 1.0;
		break;
	case '6':
		cylR_z -= 1.0;
		break;
    case '7':
        cylR_pitch -= 1.0;
        break;
    case '9':
        cylR_pitch += 1.0;
        break;

	case 'a':
		cylL_x -= 1.0;
		break;
	case 'd':
		cylL_x += 1.0;
		break;
	case 's':
		cylL_y -= 1.0;
		break;
	case 'w':
		cylL_y += 1.0;
		break;
	case 'e':
		cylL_z += 1.0;
		break;
	case 'q':
		cylL_z -= 1.0;
		break;
    case 'z':
        cylL_pitch -= 1.0;
        break;
    case 'c':
        cylL_pitch += 1.0;
        break;

	case 'o':
		openloop = !openloop;
		break;
	}

}

void onMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
  { 
     //store the x,y value where the click happened
     handAtStart=true;
  }	
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
  {
	  handAtStart=false;
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
	  if (allVisibleFingers){
		  perceptual_estimate=grip_aperture;
		  estimate_given=1;
		  beepOk(9);
	  }
	  else
		  beepOk(12);
  }
}

void calibration_fingers(int phase)
{
	switch (phase)
	{
		case 1:
		{
			if(allVisiblePlatform1)
			{
				upperPinMarker = markers[1].p;
				upperPin.init(upperPinMarker, markers.at(8).p, markers.at(11).p, markers.at(12).p );
			}
		} break;
		case 2:
		{
			if(allVisiblePlatform2)
			{
				lowerPinMarker=markers[2].p;
				lowerPin.init(lowerPinMarker, markers.at(8).p, markers.at(11).p, markers.at(12).p );
			}
		} break;
		case 3:
		{
			if(allVisiblePlatform3 && allVisibleFingers)
			{
				indexCoords.init(upperPin.getP1(), markers.at(13).p, markers.at(14).p, markers.at(16).p );
				thumbCoords.init(lowerPin.getP1(), markers.at(15).p, markers.at(17).p, markers.at(18).p );
			}
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

			switch (fingerCalibration)
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
				text.draw("Touch the position of the left object with your index finger, then press F");
				break;
			case 5:
				text.draw("Touch the position of the right object with your index finger, then press F");
				break;
			case 6:
				text.draw("Press F to begin!");
				break;
			} // end switch(fingerCalibration)

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
			text.draw("Calibration Step= " + stringify<int>(fingerCalibration) );

			glColor3fv(glWhite);
			text.draw("Calibration Platform" );
			if ( allVisiblePlatform3 )
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
			if (fingerCalibration!=7){

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
			text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
			if (allVisibleThumb)
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
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

void LoadGLTextures()
{
	// load image files directly as new OpenGL textures
	texture[0] = SOIL_load_OGL_texture("SOIL_textures/PonzoDisplayFinal.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	//Typical texture generation using data from the bitmap
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (texture[0]==0)
		beepOk(3);

	texture[1] = SOIL_load_OGL_texture("SOIL_textures/MullerLyerDisplayFinal.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	//Typical texture generation using data from the bitmap
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

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
	if (fingerCalibration>3 && !finished && frameN>0) 
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
    drawCylinder(1); //right cylinder
    
	// draw background
	glEnable(GL_TEXTURE_2D); // enable texture mapping
	glPushMatrix();
    glTranslated(0,0,displayDepth);
    glRotated(0,1,0,0);
	glBindTexture(GL_TEXTURE_2D, texture[illusionID]);
    glNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing toward screen
	glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex3f(displayLeft, displayTop, 0.0f);         // Top Left
		glTexCoord2f(1,0); glVertex3f(displayRight, displayTop, 0.0f);			// Top Right
		glTexCoord2f(1,1); glVertex3f(displayRight,  displayBottom, 0.0f);		// Bottom Right
		glTexCoord2f(0,1); glVertex3f(displayLeft,  displayBottom, 0.0f);		// Bottom Left
	glEnd();
	glDisable(GL_TEXTURE_2D);
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
	if(cylID==0){
		glTranslated(cylL_x, cylL_y, cylL_z);
		glRotated(cylL_pitch,1,0,0);
	}
	if(cylID==1){
		glTranslated(cylR_x, cylR_y, cylR_z);
		glRotated(cylR_pitch,1,0,0);
	}

	// Cylinder
	GLUquadricObj *cyl1;
	cyl1 = gluNewQuadric();
	gluQuadricDrawStyle(cyl1, GLU_LINE);
	gluCylinder(cyl1, cylRad, cylRad, cylHeight, 4, 1);
	// The object should change color to indicate that it's been "reached"
	if (!reachedObject && objId==cylID) {
		glColor3f(0.7f, 0.0f, 0.2f);
	} else {
		glColor3f(0.0f, 0.2f, 0.7f);
	}
	gluQuadricDrawStyle(cyl1, GLU_FILL);
	gluQuadricNormals(cyl1, GLU_SMOOTH);
	gluCylinder(cyl1, cylRad, cylRad, cylHeight, 4, 1);
	// End-disk 1
	gluQuadricOrientation(cyl1, GLU_INSIDE);
	glTranslated(0.0, 0.0, 0.0);
	gluDisk(cyl1,0,cylRad,4,1);
	// End-disk 2
	gluQuadricOrientation(cyl1, GLU_OUTSIDE);
	glTranslated(0.0, 0.0, cylHeight);
	gluDisk(cyl1,0,cylRad,4,1);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}


// this draws the fingertip points
void drawFingers(double offsetZ)
{
	// Compute distance in the sagittal plane (assuming object is at (x,z)=(0,objDepth))
	//double indexObjectDistance = sqrt( pow(index.x(), 2) 
	//	+ pow( index.z() - objDepth , 2) );
	//double thumbObjectDistance = sqrt( pow(thumb.x(), 2) 
	//	+ pow( thumb.z() - objDepth , 2) );
	
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

// called at the beginning of every trial
void initTrial()
{
	// initializing all variables
	frameN=0;
	TGA_frame = 0;
	start_frame = 0;
	started = false;
	//handAtStart = true;
	reachedObject = false;
	fingersOccluded = 0;
	num_lost_frames = 0;
	estimate_given = 0;

	// roll on
	drawGLScene();
	// move the screen
	initProjectionScreen(displayDepth);

	objId = posnSequence[trialNumber];

	if (training)
		if (groupNum<5)
			objId = rand() % 2;
		else
			objId = 1;

	// Find center of physical haptic stimulus
	if (objId==0){
        targetOriginX = cylL_x;
        targetOriginY = cylL_y-cylHeight/2;
        targetOriginZ = cylL_z-cylRad/2;
	} else if (objId==1) {
		targetOriginX = cylR_x;
		targetOriginY = cylR_y-cylHeight/2;
        targetOriginZ = cylR_z-cylRad/2;
	}

	// Open a new trial file and give it a header
	string trialFileName;
	if (!training){
		trialFileName = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/" + parameters[0].find("SubjectName") + "/" + parameters[0].find("SubjectName") + "_block" + stringify<double>(block) + "_trial" + stringify<double>(trialNumber) + ".txt";
	}else{
		trialFileName = "S:/Domini-ShapeLab/evan/summer15-moreIllusions/" + parameters[0].find("SubjectName") + "/" + parameters[0].find("SubjectName") +"_training" + stringify<double>(trainTrialNumber) + ".txt";
	}
	
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexXYZraw\tthumbXYZraw\tdistanceToObject\tfingersOccluded\treachedObject" << endl;

	//if (groupNum<5)
	
	beepOk(objId+7);
	
	/*else {
		if (trialNumber==10)
			beepOk(1);
		else
			beepOk(2);
	}*/

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
	} else if (training){
		overwrite=1;
		trainTrialNumber++;
		cout << "Training Mode!" << endl;
	}


	// If we're not reinserting, write trial-specific data to response file
	if (!overwrite || training)
	{
		responseFile.precision(3);
		responseFile << parameters[0].find("SubjectName") << "\t" <<
						block << "\t" <<
						illusionID << "\t" <<
						objId << "\t" <<
						trialNumber << "\t" <<
						maxTime << "\t" <<
						cylHeight << "\t" <<
						perceptual_estimate << "\t" <<
						openloop << "\t" <<
						training <<
						endl;
	}
	// Close the trial file
	trialFile.close();

	// If there are more trials in the current block
	//if( trialNumber<20 ) {
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
		if (block==2)
			illusionID++;
		trialNumber=0;
		//newBlock=true;
		factors = trial[block_order[block]].getNext();
		initTrial();
	}
}


void idle()
{
	// get new marker positions from optotrak
	optotrak->updateMarkers();
	markers = optotrak->getAllMarkers();

	// check visibility
	if (triangulate){
		allVisiblePlatform3 = isVisible(markers[8].p) && isVisible(markers[11].p) && isVisible(markers[12].p);
		allVisiblePlatform1 = isVisible(markers[1].p) && allVisiblePlatform3;
		allVisiblePlatform2 = isVisible(markers[2].p) && allVisiblePlatform3;
		allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
		allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
		allVisibleFingers = allVisibleIndex && allVisibleThumb;// && allVisibleWrist;
		allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
		allVisibleHead = allVisiblePatch && isVisible(markers[1].p);
	} else {
		allVisibleIndex = isVisible(markers[9].p);
		allVisibleThumb = isVisible(markers[10].p);
		allVisibleFingers = allVisibleThumb && allVisibleIndex;
		allVisibleWrist = isVisible(markers[22].p);
	}


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
		if(triangulate){
			indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
			thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		}
		//wristCoords = markers[wristMarkerNum].p;
		fingersOccluded = 0;
	}
	if (triangulate){
		if ( allVisiblePlatform1 && fingerCalibration==0 )
			{
				// get upper pin
				calibration_fingers(1);
				fingerCalibration=1;
			}
			if ( fingerCalibration==1 && allVisiblePlatform2 )
			{
				// get lower pin
				calibration_fingers(2);
				fingerCalibration=2;
				beepOk(0);
			}
		if ( allVisiblePlatform3 && fingerCalibration<3 )
		{
			upperPin.update(markers[8].p, markers[11].p, markers[12].p );
			lowerPin.update(markers[8].p, markers[11].p, markers[12].p );
		}

		// update the finger position in the objects we actually use
		if (allVisibleIndex)
			index = indexCoords.getP1();
		if (allVisibleThumb)
			thumb = thumbCoords.getP1();
	}
	if (!triangulate){
		if (fingerCalibration==0)
			fingerCalibration=3;
		index = markers[9].p;
		thumb = markers[10].p;
		wrist = markers[22].p;
	}

	//////////////////////////////////////
	// While the experiment is running! //
	//////////////////////////////////////
	if (fingerCalibration==7 && !finished)
	{
		// Check for finger occlusion
		if ( !allVisibleFingers )
		{
			fingersOccluded = 1;
			//if (!started)
			//	beepOk(4);
			if (started && !reachedObject) // only increment if we're in flight
			{
				num_lost_frames += 1;
			}
		}
		
		// Advance frame number
		frameN++;

		// find distance from grip center to object center
		grip_Origin_X = (index.x()+thumb.x())/2;
		grip_Origin_Y = (index.y()+thumb.y())/2;
		grip_Origin_Z = (index.z()+thumb.z())/2;
		x_dist = abs(grip_Origin_X - targetOriginX);
		y_dist = abs(grip_Origin_Y - targetOriginY);
		z_dist = abs(grip_Origin_Z - targetOriginZ);
		distanceGripCenterToObject = sqrt((x_dist*x_dist)+(y_dist*y_dist)+(z_dist*z_dist));

		x_dist_home = abs(grip_Origin_X - (startPosLeft + startPosRight)/2);
		y_dist_home = abs(grip_Origin_Y - (startPosTop + startPosBottom)/2);
		z_dist_home = abs(grip_Origin_Z - (startPosFront + startPosRear)/2);
		distanceGripCenterToHome = sqrt((x_dist_home*x_dist_home)+(y_dist_home*y_dist_home)+(z_dist_home*z_dist_home));

		// compute grip aperture
		grip_aperture = sqrt(
			(index.x() - thumb.x())*(index.x() - thumb.x()) + 
			(index.y() - thumb.y())*(index.y() - thumb.y()) + 
			(index.z() - thumb.z())*(index.z() - thumb.z())
			);

		/* Check that both fingers are in the start position
		if( ( (index.y() < startPosTop) && // index below ceiling
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
			  (thumb.z() < startPosRear) ) // thumb in front of rear wall
			  || (estimate_given==0) )*/
		if (handAtStart || (estimate_given==0))
		{	
			// if we already gave the estimate and are returning to the start position
			if (estimate_given==1){
				estimate_given=2;
				beepOk(10);
			} 
			// keep resetting timer
			//handAtStart = true;
			maxTime = timer.getElapsedTimeInMilliSec();
			timer.start();
		} else if (estimate_given==2) { 
			// otherwise we are in flight, so set flags and let the timer run
			if (start_frame==0){
				start_frame=frameN;
				start_dist = distanceGripCenterToHome;
			}
			//handAtStart = false;
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

	// recompute the eye coordinates for drawing so we can change IOD online
	if(headCalibration){
		eyeLeft = headEyeCoords.getLeftEye();
		eyeRight = headEyeCoords.getRightEye();
	}else{
		eyeRight = Vector3d(interoculardistance/2,0,0);
		eyeLeft = -eyeRight;
	}

	// Write to trialFile once calibration is over
	if (fingerCalibration==7 )
	{
		trialFile << fixed <<
		//parameters.find("SubjectName") << "\t" <<	//subjName
		parameters[0].find("SubjectName") << "\t" <<	//subjName
		trialNumber << "\t" <<							//trialN
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		frameN << "\t" <<								//frameN
		index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
		thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
		//wrist.transpose() << "\t" <<
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

///////////////////////////////////////////////////////////
/////// USUALLY DON'T NEED TO EDIT THESE FUNCTIONS ////////
///////////////////////////////////////////////////////////

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
	LoadGLTextures(); 

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
	return;
}

///////////////////////////////////////////////////////////
////////////////////// MAIN FUNCTION //////////////////////
///////////////////////////////////////////////////////////

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	cout << "Please enter the subject's group number (1-4): " << endl;
	cin >> groupNum;

	switch (groupNum){
		case 1:
			{
				for (int n=0; n<20; n++)
					posnSequence[n] = posnSequence1[n];
			}
			break;
		case 2:
			{
				for (int n=0; n<20; n++)
					posnSequence[n] = posnSequence2[n];
			}
			break;
		case 3:
			{
				for (int n=0; n<20; n++)
					posnSequence[n] = posnSequence3[n];
			}
			break;
		case 4:
			{
				for (int n=0; n<20; n++)
					posnSequence[n] = posnSequence4[n];
			}
			break;
	}

	fingerCalibration=0;
	
	for (int i=0; i<num_blocks; ++i)
		block_order.push_back(i);

	//random_shuffle(block_order.begin(), block_order.end());

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();

	// push back the apparatus
	Vector3d object_reset_position(0,0,0);
	object_reset_position = markers[3].p.transpose();
	// calculate where the object has to go
	Vector3d object_position(0.0,object_reset_position.y(),-800);
	// move the object to position
	moveObjectAbsolute(object_position, object_reset_position, 5000);

    glutInit(&argc, argv);
	if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("Evan Experiment");
		glutFullScreen();
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
	HWND hwnd = FindWindow( "GLUT", "Evan Experiment" );
	//SetWindowPos( hwnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOREPOSITION | SWP_NOSIZE );
	SetForegroundWindow(hwnd);
    glutMainLoop();

    cleanup();
    return 0;
}
