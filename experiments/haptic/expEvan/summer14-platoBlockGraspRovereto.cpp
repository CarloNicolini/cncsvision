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
#include <stdio.h>
#include <conio.h>

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
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"
#include "AsyncSerial.h"
#include "SerialStream.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* #DEFINE DIRECTIVES **************************/
// Targets
#define TOP_OBJL 1
#define TOP_OBJR 2
#define TOPMID_OBJL 3
#define TOPMID_OBJR 4
#define BTMMID_OBJL 5
#define BTMMID_OBJR 6
#define BTM_OBJL 7
#define BTM_OBJR 8

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
Screen screen;
double homeFocalDistance=-418.5;

/********** TIMING ***************/
Timer timer;
Timer globalTimer;
bool paused = false;

/********** Vector3d VARIABLES **********************/
Vector3d eyeRight, index, thumb, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
vector <Marker> markers;
// This is the position that the motor starts in
static const Vector3d motorHomePosn(415.2,-323.8,-901.8); // this is sort of arbitrary at this point...
static const Vector3d mountDisplayPosn(415.2,-323.8,-601.8);
// Motor Positions for the 4 Boards
// X and Z should be the same, Y will vary by the distance between the boards
// MEASURE CAREFULLY AND TEST
Vector3d showTopBoard(415.2, -323.8, -601.8);
Vector3d showTopMidBoard(415.2, (-323.8+153.3), -601.8);
Vector3d showBtmMidBoard(415.2, (-323.8+(153.3+166)), -601.8);
Vector3d showBtmBoard(415.2, (-323.8+(153.3+166*2)), -601.8);
double speed = 3000; //motor speed

// For relative movements
Vector3d moveUpOne(0, 153.3, 0);
Vector3d moveUpTwo(0, (153.3*2), 0);
Vector3d moveUpThree(0, (153.3*3), 0);
Vector3d moveDownOne(0, (-153.3), 0);
Vector3d moveDownTwo(0, (-153.3*2), 0);
Vector3d moveDownThree(0, (-153.3*3), 0);

/********* CALIBRATION VARIABLES *********/
int startPosCalibration=0;
int fingerCalibrationDone=0;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool displayMounted = false;
bool objectsForward = false;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
double frameNumber = 0;
double TGA_frame = 0; //need this for checking percentOccludedFrames
double start_frame = 0;
double TGA_time = 0;

bool controlCondition = false;
int controlTrial = 1;
int num_control_trials = 16;

double x_dist = 999;
double y_dist = 999;
double z_dist = 999;

ParametersLoader parameters;
BalanceFactor<double> trial[2]; // now blocked! when using ObjPosn, make sure to +2*block_num
map <std::string, double> factors;

int block_num = 0;
int block_order[2]= {0, 1};
int num_blocks = 2;
int block;

double numLostFrames = 0;

bool training=false;
bool handAtStart=true;
bool started=false;
bool reachedObject=false;
bool endControl=false;

clock_t t;

GLUquadric* qobj;

int quickStart = 0;
int quickStartBoard = 0;
int currentObj = 0;
int previousObj = 0;

/********** SPATIAL POSITION VARIABLES ********/

// Start Position
double startPos_top;
double startPos_bottom;
double startPos_front;
double startPos_rear;
double startPos_right;
double startPos_left;

//int target_identity;
double target_height;
double target_width;
double target_depth;
double target_X;
double target_Y;
double target_Z;

// Grip 
double grip_X;
double grip_Y;
double grip_Z;
double grip_aperture;
double grip_distanceToTarget;
double plato_trigger;

// Eight Objects (2 per board)
// Top Board
double top_objL_height=42;
double top_objR_height=40;
// Top Middle Board
double topmid_objL_height=42;
double topmid_objR_height=40;
// Bottom Middle Board
double btmmid_objL_height=40;
double btmmid_objR_height=42;
// Bottom Board
double btm_objL_height=40;
double btm_objR_height=42;

// Left and Right Positions
double objL_X = 440;
double objL_Y = -100; //ignore
double objL_Z = -440;

double objR_X = 570;
double objR_Y = -100; //ignore
double objR_Z = -440;

// Width and Depth
double obj_width = 10;
double obj_depth = 10;

// Object Features
int con_inc = 0;
int sm_lg = 0;

bool finished = false;

/********** STREAMS **************/
ofstream responseFile, trialFile, controlResponseFile;

/********** FUNCTION PROTOTYPES *****/
void advanceTrial();
void beepOk(int tone);
void drawGLScene();
void drawInfo();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void idle();
void initGLVariables();
void initMotorsOptotrak();
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void initRendering();
void initStreams();
void initTrial();
void initVariables();
void update(int value);
TCHAR pressanykey( const TCHAR* prompt);

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

/************************ PLATO I/O STUFF ***************************/

/* Definitions in the build of inpout32.dll are:            */
/*   short _stdcall Inp32(short PortAddress);               */
/*   void _stdcall Out32(short PortAddress, short data);    */
/* prototype (function typedef) for DLL function Inp32: */
typedef short (_stdcall *inpfuncPtr)(short portaddr);
typedef void (_stdcall *oupfuncPtr)(short portaddr, short datum);

#define PPORT_BASE 0x378 // this is the port's signal pin

// Prototypes for Plato functions
int plato_init(void);
int plato_stop(void);
void plato_read(void);
void plato_write(short datum);

// Handle for the inpout32 library
HINSTANCE hLib;// = LoadLibrary("inpout32.dll");

// After successful init, these 2 variables will contain function pointers.*/
inpfuncPtr inp32fp;
oupfuncPtr oup32fp;

// Wrapper functions for the function pointers - call these to perform I/O. */
short  Inp32 (short portaddr){return (inp32fp)(portaddr);}
void  Out32 (short portaddr, short datum){(oup32fp)(portaddr,datum);}

int plato_init(void)
{
     // Load the library
     hLib = LoadLibrary("inpout32.dll");
     if (hLib == NULL) {
          fprintf(stderr,"LoadLibrary Failed.\n");
          return -1;
     }
     // get the addresses of the functions */
     inp32fp = (inpfuncPtr) GetProcAddress(hLib, "Inp32");
     if (inp32fp == NULL) {
          fprintf(stderr,"GetProcAddress for Inp32 Failed.\n");
          return -1;
     }
     oup32fp = (oupfuncPtr) GetProcAddress(hLib, "Out32");
     if (oup32fp == NULL) {
          fprintf(stderr,"GetProcAddress for Oup32 Failed.\n");
          return -1;
     }

	 return 0;
}

// Free the inpout32 library when finished
int plato_stop(void) {
	FreeLibrary(hLib);
	return 0;
}

// Read input from PPORT_BASE address
void plato_read(void) {
     short x;
     int i = PPORT_BASE;
	 x = Inp32(i);
	 fprintf(stdout, "Port read (%04X)= %04X\n",i,x);
}

// Write data from parameter
// Examples:
//	test_write_datum(0x00); //open both
//	test_write_datum(0x01); //close L
//	test_write_datum(0x02); //close R
//	test_write_datum(0x03); //close both
void plato_write(short datum) {
     short x;
     int i;

     i = PPORT_BASE;

	 if (datum==0)
		x = 0x00;
	 if (datum==1)
		x = 0x01;
	 if (datum==2)
		x = 0x02;
	 if (datum==3)
		x = 0x03;

     //  Write the data register */
     Out32(i,x);
     fprintf(stdout, "Port write to 0x%X, datum=0x%2X\n" ,i ,x);

     // And read back to verify  */
     x = Inp32(i);
     fprintf(stdout, "Port read (%04X)= %04X\n",i,x);
}


/***** SOUND THINGS *****/
boost::mutex beepMutex;
int tone;
void beepOk(int tone)
{
    boost::mutex::scoped_lock lock(beepMutex);
	switch(tone)
	{
        case 0: //invisible
            Beep(1000,50);
            break;
        case 1: //long
            Beep(220,880);
            break;
        case 2: //ok
            Beep(440,440);
            break;
        case 3: //bad
            Beep(200,200);
            break;
        case 4:
            Beep(440,1000);
            break;
		case 5:
			Beep(440,200);
			break;
		case 6:
		    PlaySound((LPCSTR) "C:\\workspace\\cncsvisioncmake\\experiments\\haptic\\expEvan\\a_sinistra.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 7:
			PlaySound((LPCSTR) "C:\\workspace\\cncsvisioncmake\\experiments\\haptic\\expEvan\\a_destra.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
    }
	return;
}

/*************************** FUNCTIONS ***********************************/
void drawInfo()
{
	if (paused)
		visibleInfo = true;

	if ( visibleInfo )
	{
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);

		GLText text;

		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
		
		text.enterTextInputMode();
        
        glColor3fv(glWhite);
		text.draw("####### EXPERIMENTER INSTRUCTIONS #######");

		if (!objectsForward)
		{
			text.draw("Press F to bring platform forward");
		}
        if ( objectsForward && (startPosCalibration==0) && (fingerCalibrationDone==0))
		{
			text.draw("Press F to record platform markers");
		}
        if ( objectsForward && (startPosCalibration==0) && (fingerCalibrationDone==1) )
		{
			text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
		}
		if ( objectsForward && (startPosCalibration==0) && (fingerCalibrationDone==2) && allVisibleFingers) 
		{
			text.draw("Move index and thumb to starting position, then press F");
		}
        if ( objectsForward && (startPosCalibration==1) && (fingerCalibrationDone==2)) 
		{
			text.draw("Mount the display board on the platform, then press F.");
		}
		if ( objectsForward && (startPosCalibration==2) && (fingerCalibrationDone==2)) 
		{
			text.draw("Press F to record position of left object (use straw).");
		}
		if ( objectsForward && (startPosCalibration==3) && (fingerCalibrationDone==2)) 
		{
			text.draw("Press F to record position of right object (use straw).");
		}
		if ( objectsForward && (startPosCalibration==4) && (fingerCalibrationDone==2) && allVisibleFingers) 
		{
			text.draw("Press F to begin!");
		}
		text.draw("#########################################");
		text.draw(" ");

		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
		text.draw("# Block: " +stringify<int>(block));
		text.draw("# Trial: " +stringify<int>(trialNumber));
		text.draw("#");
		text.draw("#######################");
		text.draw("StartPosCalibration = " + stringify<int>(startPosCalibration) );
		text.draw("FingerCalibration = " + stringify<int>(fingerCalibrationDone) );
		text.draw(" ");
        
		text.draw("Calibration Platform Markers" );
		if ( isVisible(markers[15].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Thumb Calibration Point (15) "+stringify< Eigen::Matrix<double,1,3> >(markers[15].p.transpose())+ " [mm]" );

		if ( isVisible(markers[16].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Index Calibration Point (16) "+stringify< Eigen::Matrix<double,1,3> >(markers[16].p.transpose())+ " [mm]" );

		if ( isVisible(markers[10].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Straw Marker (10) "+stringify< Eigen::Matrix<double,1,3> >(markers[10].p.transpose())+ " [mm]" );

		glColor3fv(glWhite);
		text.draw(" ");
		text.draw("Thumb" );
		if ( isVisible(markers[20].p) && isVisible(markers[21].p) && isVisible(markers[22].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(20)+stringify< Eigen::Matrix<double,1,3> > (markers[20].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(21)+stringify< Eigen::Matrix<double,1,3> > (markers[21].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(22)+stringify< Eigen::Matrix<double,1,3> > (markers[22].p.transpose())+ " [mm]" );

		glColor3fv(glWhite);
		text.draw(" ");
		text.draw("Index" );
		if ( isVisible(markers[19].p) && isVisible(markers[23].p) && isVisible(markers[24].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(11)+stringify< Eigen::Matrix<double,1,3> > (markers[19].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(12)+stringify< Eigen::Matrix<double,1,3> > (markers[23].p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(13)+stringify< Eigen::Matrix<double,1,3> > (markers[24].p.transpose())+ " [mm]" );

		glColor3fv(glGreen);
        text.draw(" ");
		text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
		text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
        text.draw(" ");
		text.draw("Trial= " + stringify<int>(trialNumber));
		text.draw("Frame= " + stringify<double>(frameNumber));
        text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
        text.draw(" ");
		
		text.draw("Target Object : " + stringify<int>(currentObj));
		if (handAtStart)
		{
			glColor3fv(glGreen);
			text.draw("Hand at start.");
		}
		else
		{
			glColor3fv(glWhite);
			text.draw("Hand in flight.");
		}

		text.draw(" ");

		
		if(reachedObject)
		{
            glColor3fv(glGreen);
			text.draw("Object Reached!");
		}
		else
		{
			glColor3fv(glRed);
			text.draw("Distance to object= " + stringify<double>(grip_distanceToTarget));
		}

		text.leaveTextInputMode();
    }
}

void drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.setEye(eyeRight);
	drawInfo();
	glutSwapBuffers();
}


void initTrial()
{
	// initializing all variables
	frameNumber = 0;
    numLostFrames = 0;
    
	handAtStart=true;
    started = false;
	reachedObject = false;

	plato_write(3);
    
	if (controlCondition){
		if (controlTrial%2==1)
			currentObj = 1;
		else if (controlTrial%2==0)
			currentObj = 2;
	}

	if (!controlCondition)
		currentObj = (trial[block].getCurrent().at("ObjPosn") + (2*(block+1)));


	switch (currentObj)
	{
		case TOP_OBJL:
		case TOP_OBJR:
			RoveretoMotorFunctions::moveObjectAbsolute(showTopBoard,motorHomePosn,4000);
			break;
		case TOPMID_OBJL:
		case TOPMID_OBJR:
			RoveretoMotorFunctions::moveObjectAbsolute(showTopMidBoard,motorHomePosn,4000);
			break;
		case BTMMID_OBJL:
		case BTMMID_OBJR:
			RoveretoMotorFunctions::moveObjectAbsolute(showBtmMidBoard,motorHomePosn,4000);
			break;
		case BTM_OBJL:
		case BTM_OBJR:
			RoveretoMotorFunctions::moveObjectAbsolute(showBtmBoard,motorHomePosn,4000);
			break;
	}

    switch (currentObj)
    {
        case TOP_OBJL:
        case TOPMID_OBJL:
        case BTMMID_OBJL:
        case BTM_OBJL:
            target_X = objL_X;
            target_Y = objL_Y;
            target_Z = objL_Z;
			tone = 6; //a_sinistra.ogg
            break;
        case TOP_OBJR:
        case TOPMID_OBJR:
        case BTMMID_OBJR:
        case BTM_OBJR:
            target_X = objR_X;
            target_Y = objR_Y;
            target_Z = objR_Z;
			tone = 7; //a_destra.ogg
            break;
    }

	if (controlCondition)
		target_Y = target_Y + 12.7;
    
    switch (currentObj)
    {
        case TOP_OBJL:
            target_height = 42;
			con_inc = 0;
			sm_lg = 2;
            break;
        case TOP_OBJR:
            target_height = 42;
			con_inc = 0;
			sm_lg = 2;
            break;
        case TOPMID_OBJL:
            target_height = 40;
			con_inc = 1;
			sm_lg = 1;
            break;
        case TOPMID_OBJR:
            target_height = 42;
			con_inc = 1;
			sm_lg = 2;
            break;
        case BTMMID_OBJL:
            target_height = 42;
			con_inc = 2;
			sm_lg = 2;
            break;
        case BTMMID_OBJR:
            target_height = 40;
			con_inc = 2;
			sm_lg = 1;
            break;
        case BTM_OBJL:
            target_height = 0;
			con_inc = 0;
			sm_lg = 0;
            break;
        case BTM_OBJR:
            target_height = 0;
			con_inc = 0;
			sm_lg = 0;
            break;
    }
			
	if (controlCondition){
	string trialFileName = "C:/Documents and Settings/Fulvio/My Documents/Google Drive/platoBlockGraspRovereto/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "_controlTrial_" + stringify<int>(controlTrial) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexX\tindexY\tindexZ\tthumbX\tthumbY\tthumbZ\tdistanceToObject\tfingersVisible\treachedObject" << endl;
	}
	
	if (!controlCondition) {
	string trialFileName = "C:/Documents and Settings/Fulvio/My Documents/Google Drive/platoBlockGraspRovereto/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "_trial_" + stringify<int>(trialNumber) + "_block" + stringify<int>(block) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexX\tindexY\tindexZ\tthumbX\tthumbY\tthumbZ\tdistanceToObject\tfingersVisible\treachedObject" << endl;
	}

	beepOk(tone);
	if (trial[block].getCurrent().at("OpenLoop"))
		plato_write(0);
	timer.start();
}

void advanceTrial()
{
	// Determine if previous trial was a good trial
	double percentOccludedFrames = numLostFrames/TGA_frame;
	bool notEnoughFrames = percentOccludedFrames > 0.10 ;
	int overwrite = 0;

	// If any of these things are true when advanceTrial is called, reinsert!
	if (training || !reachedObject || notEnoughFrames ) {
        beepOk(3); // 1000hz, 50ms
		if (training)
			cout << "Training Mode!" << endl;
		if (!reachedObject)
			cout << "I don't think you reached the object..." << endl;
		if (notEnoughFrames)
			cout << "Be visible!" << endl;

		if (!controlCondition) {
			map<std::string,double> currentFactorsList = trial[block].getCurrent();
			trial[block].reinsert(currentFactorsList);
		}

		overwrite = 1;
	}

	// Write previous trial to response file
	if (!overwrite && !controlCondition)
	{
		responseFile.precision(3);
		responseFile << 
						parameters.find("SubjectName") << "\t" <<
						block_num << "\t" <<
						trialNumber << "\t" <<
						TGA_time << "\t" <<
						currentObj << "\t" <<
                        sm_lg << "\t" <<
						con_inc << "\t" <<
						endl;
	} else if (!overwrite && controlCondition) {
		controlResponseFile.precision(3);
		controlResponseFile << 
						parameters.find("SubjectName") << "\t" <<
						controlTrial << "\t" <<
						TGA_time << "\t" <<
						currentObj << "\t" <<
                        sm_lg << "\t" <<
						con_inc << "\t" <<
						endl;
	}

	// Close previous trial file
	trialFile.close();

	// If we're in the control block
	if (controlCondition && controlTrial<num_control_trials ) {
		if (!overwrite) //and not overwriting
			controlTrial++; //increment
		initTrial(); //initTrial will handle a repeat or the next trial
	}
	// If we're done with control block
	else if (controlCondition && controlTrial==num_control_trials) {
		if (!overwrite) { // and not overwriting, change over
			plato_stop();
			exit(0);
			//controlCondition=false;
			//endControl=true;
		}
		else // if we ARE overwriting, initTrial again until we get it
			initTrial();
	}
	
	// If we're in the main block
	if( !trial[block].isEmpty() && !controlCondition)
	{
		if (!endControl) { // If it's NOT the v. first main block trial
			if (!overwrite) // and we're not overwriting due to a bad trial
				trialNumber++; // increment
			factors = trial[block].getNext(); // get the next trial and initialize it!
			initTrial();
		} else { // It IS the v. first main block trial (endControl)
			endControl=false; // turn this switch off so we know next time around that it's trial 1
			initTrial(); // initTrial WITHOUT getNext because we already gotNext at the end of calibration
		}
	}
	// Finally, if we're out of trials, increment block OR finish the experiment.
	else if (trial[block].isEmpty() && !controlCondition)
	{
		trialNumber++;
		block_num++;
		if (block_num < num_blocks) {
			block = block_order[block_num];
			factors = trial[block].getNext();
			initTrial();
		} else {
			controlCondition=true;
			initTrial();
		}
	}
}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
            
        case 'x':
        {
            
        }
            break;
            
        case 'i':
        {
            visibleInfo=!visibleInfo;
        }
            break;
            
        case 't':
        {
            training = !training;
        }
            break;
            
        case '*':
        {
            
        }
            break;
            
        case 'q':
        {
            
        }
            break;
            
        case 27: //ESC
        {
            exit(0);
        }
            break;
            
        case 'f':
        {
			if (!objectsForward)
			{
				beepOk(2);
				objectsForward = true;
				RoveretoMotorFunctions::moveObjectAbsolute(mountDisplayPosn, motorHomePosn, 3000);
				break;
			}
            if ( objectsForward && (startPosCalibration==0) && (fingerCalibrationDone==0) && allVisiblePlatform)
            {
                platformIndex=markers[16].p;
                platformThumb=markers[15].p;
                fingerCalibrationDone=1;
                beepOk(2);
                break;
            }
            if ( (startPosCalibration==0) && (fingerCalibrationDone==1) && allVisibleFingers )
            {
                indexCoords.init(platformIndex, markers.at(19).p, markers.at(23).p, markers.at(24).p );
                thumbCoords.init(platformThumb, markers.at(20).p, markers.at(21).p, markers.at(22).p );
                fingerCalibrationDone=2;
                beepOk(2);
				break;
            }
			if ( (startPosCalibration==0) && (fingerCalibrationDone==2) && allVisibleFingers) 
            {
                startPos_front = index.z()-20;
                startPos_right = index.x()+20;
                startPos_top = index.y()+20;
				startPos_rear = thumb.z()+20;
                startPos_left = thumb.x()-20;
                startPos_bottom = thumb.y()-20;
                startPosCalibration=1;
                beepOk(2);
                break;
            }
			if (  (startPosCalibration==1) && (fingerCalibrationDone==2)) 
			{
				startPosCalibration = 2;
				beepOk(2);
				RoveretoMotorFunctions::moveObjectAbsolute(showTopMidBoard,motorHomePosn,4000);
				break;
			}
            if ( (startPosCalibration==2) && (fingerCalibrationDone==2) ) 
            {
				objL_X = markers[10].p.x();
				objL_Y = markers[10].p.y();
				objL_Z = markers[10].p.z();
				startPosCalibration=3;
                beepOk(2);
				break;
			}
			if ( (startPosCalibration==3) && (fingerCalibrationDone==2) ) 
            {
				objR_X = markers[10].p.x();
				objR_Y = markers[10].p.y();
				objR_Z = markers[10].p.z();
                startPosCalibration=4;
				beepOk(2);
				break;
			}
			if ( (startPosCalibration==4) && (fingerCalibrationDone==2) && allVisibleFingers) 
            {
                fingerCalibrationDone=3;
                visibleInfo=false;
                training=false;
                factors = trial[block].getNext();
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
            cout << "Distance to Object: sqrt(" << x_dist << "^2 + " << y_dist << "^2 + " << z_dist << "^2) = " << grip_distanceToTarget << endl;
            cout << "Start Pos Corner: " << startPos_right << "\t" << startPos_top << "\t" << startPos_front << endl;
        }
            break;
            
        case '5': //down
        {
            
        }
            break;
            
        case '8': //up
        {
            
        }
            break;
            
        case '4': //left
        {
            
        }
            break;
            
        case '6': //right
        {
           
        }
            break;
            
        case '1': //left 2
        {  
           
        }
            break;
            
        case '3': //right 2
        {  
            
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

// Questa funzione definisce la distanza e l'orientazione dello schermo di proiezione virtuale, e' importante che la lasci cosi almeno che tu non voglia:
// 1) simulare uno schermo di proiezione che si muove o comunque con un orientamento diverso da quello standard cioe' il piano a z=focalDistance
// 2) spostare il piano di proiezione cambiando alignmentX ed alignmentY ma per quello ti consiglio di contattarmi o comunque utilizzare il file headCalibration.cpp che ha legato ai tasti 2,4,6,8 il movimento dello schermo per allineare mondo virtuale e mondo optotrak
void initProjectionScreen(double _focalDist, const Affine3d &_transformation, bool synchronous)
{
	cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,_focalDist));
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
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();

	// Coordinates picker //is this correct? (are the marker identities the same?)
	allVisiblePlatform = isVisible(markers[15].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[20].p) && isVisible(markers[21].p) && isVisible(markers[22].p);
	allVisibleIndex = isVisible(markers[22].p) && isVisible(markers[23].p) && isVisible(markers[24].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;
	if ( allVisibleFingers )
	{
		thumbCoords.update(markers[20].p, markers[21].p, markers[22].p );
		indexCoords.update(markers[19].p, markers[23].p, markers[24].p );
	}
	eyeRight = Vector3d(0,0,0);
	index = indexCoords.getP1();
	thumb = thumbCoords.getP1();

	//////////////////////////
	/// Real Stuff
	//////////////////////////

	if (fingerCalibrationDone==3) { // After we've calibrated
		
		if ( !allVisibleFingers ) { // Check for finger occlusion
			beepOk(0);

			if (started && !reachedObject) // Increment counter if hand is in flight
				numLostFrames += 1;
		}
		
		frameNumber++; // Advance frame number

		// Check that we're at the start position
		if( (index.y() < startPos_top) && // index below ceiling
            (index.y() > startPos_bottom) && // index above floor
            (index.x() < startPos_right) && // index left of right wall
			(index.x() > startPos_left) && // index right of left wall
            (index.z() < startPos_rear) && // index in front of rear wall
			(index.z() > startPos_front) ) { // index behind front wall

            // if so, keep resetting timer (need this outside the "fingers together" loop!)
            timer.start();
            
			// distance from index to thumb less than 1.5cm in each direction
			if ((abs(index.y() - thumb.y()) < 15) &&
				(abs(index.x() - thumb.x()) < 15) && 
				(abs(index.z() - thumb.z()) < 15) ) {

				// if so, we are in the start position
				handAtStart = true;
			}

		} else if (allVisibleFingers) { // we've moved from the start, begin counting 

			handAtStart = false;
			started = true;
			start_frame = frameNumber;

			if (!reachedObject) //(trial.getCurrent().at("OpenLoop") && !reachedObject)
				plato_write(3);
		}

		// Find component distances
		grip_X = (index.x()+thumb.x())/2;
		grip_Y = (index.y()+thumb.y())/2;
		grip_Z = (index.z()+thumb.z())/2;
		x_dist = abs(grip_X - target_X);
		y_dist = abs(grip_Y - target_Y);
		z_dist = abs(grip_Z - target_Z);

		plato_trigger = index.z() - (target_Z+20);

        // Distance formulas
		grip_aperture = sqrt(
			(abs(index.x() - thumb.x())*abs(index.x() - thumb.x())) + 
			(abs(index.y() - thumb.y())*abs(index.y() - thumb.y())) + 
			(abs(index.z() - thumb.z())*abs(index.z() - thumb.z())));
        
		grip_distanceToTarget = sqrt((x_dist*x_dist)+(y_dist*y_dist)+(z_dist*z_dist));

        // If hand is in flight
		if (!reachedObject && started && allVisibleFingers) {
			// PLATO Threshold
			if ( (plato_trigger < 0) && (plato_trigger > -1000) )
			{
				reachedObject = true;
				//if (trial.getCurrent().at("OpenLoop"))
				plato_write(0);
				TGA_frame = frameNumber - start_frame;
				TGA_time = timer.getElapsedTimeInMilliSec();
			}
		}

		if(handAtStart && started)
			advanceTrial();
	}

	// Write to trialFile
	if (fingerCalibrationDone==3 )
	{
		int TN;
		if (controlCondition)
			TN = controlTrial;
		else
			TN = trialNumber;

		trialFile << fixed <<
		parameters.find("SubjectName") << "\t" <<		//subjName
		TN << "\t" <<									//trialN
		timer.getElapsedTimeInMilliSec() << "\t" <<		//time
		frameNumber << "\t" <<							//frameN
		index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
		thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
		grip_distanceToTarget << "\t" <<
		allVisibleFingers << "\t" <<					//allVisibleFingers
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
void initMotorsOptotrak()
{
    // Move the monitor in the positions
	RoveretoMotorFunctions::homeMirror(3500);
	RoveretoMotorFunctions::homeScreen(3000);
	
	if (!quickStart)
	{
		RoveretoMotorFunctions::homeObjectAsynchronous(4500);
	}

    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile,24) != 0)
	{
        exit(0);
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
	parametersFile.open("C:/cncsvisiondata/parametersFiles/Evan/platoBlockGraspRovereto/parametersPlatoBlockGraspRovereto.c"); //is this correct?
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	
	// Output files directory
	string dirName  = "C:/Documents and Settings/Fulvio/My Documents/Google Drive/platoBlockGraspRovereto/" + subjectName;
	mkdir(dirName.c_str());

	// Response file
	string controlResponseFileName = "C:/Documents and Settings/Fulvio/My Documents/Google Drive/platoBlockGraspRovereto/" + subjectName + "/platoBlockGraspRovereto_control_" + subjectName + ".txt";
    string responseFileName =  "C:/Documents and Settings/Fulvio/My Documents/Google Drive/platoBlockGraspRovereto/" + subjectName + "/platoBlockGraspRovereto_" + subjectName + ".txt";

	// Check for output file existence 
	/// Response file
	if ( !fileExists((responseFileName)) ) {
        responseFile.open((responseFileName).c_str());
	}
	if ( !fileExists((controlResponseFileName)) ) {
        controlResponseFile.open((controlResponseFileName).c_str());
	}

	responseFile << fixed << "subjName\tblockN\ttrialN\ttrialDuration\ttargetObj\tsm_lg\tcon_inc" << endl;
	controlResponseFile << fixed << "subjName\ttrialN\ttrialDuration\ttargetObj\tsm_lg\tcon_inc" << endl;
	globalTimer.start();
}

int main(int argc, char*argv[])
{
	// PLATO Demo/Debug
	/*int plato_command = 1;
	plato_init();
	while (plato_command!=5){
		cout<<"Enter PLATO command index (0-3, 5 to stop): ";
		cin>>plato_command;
		switch (plato_command){
		case 0:
			plato_write(plato_command);
			break;
		case 1:
			plato_write(plato_command);
			break;
		case 2:
			plato_write(plato_command);
			break;
		case 3:
			plato_write(plato_command);
			break;
		}
	}
	plato_stop();
	pressanykey();
	return 0;
}*/

	for (int i=0; i<num_blocks; i++)
	{
		cout << "Enter block numbers in order (hit RETURN after EACH entry): " << endl;
		cin >> block_order[i];
	}
	block = block_order[block_num];

	mathcommon::randomizeStart();

	// Initializes the optotrak and starts the collection of points in background
	initMotorsOptotrak();
	plato_init();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize( 1024, 768);
	glutCreateWindow("EXP WEXLER");
	glutFullScreen();

	// Prima inizializza l'ambiente di rendering dopodiche' puoi inizializzare le variabili (questo 
	// ragionamento vale se una delle variabili che inizializzi e' del modulo GLViz cioe' richiede OpenGL attivo)
    initRendering();
	initGLVariables();
	// Inizializza gli stream di output o input. Consiglio di usare questo ordine perche' in initVariables
	// normalmente vorrai parsare un file di testo contenente i parametri
	initStreams();
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

    return 0;
}
