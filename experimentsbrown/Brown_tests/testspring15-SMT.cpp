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

/*********** NIOSELIB LIBRARIES ***************/
#include <noise/noise.h>
#include "noiseutils.h"

/********* PHIDGETS LIBRARY **********/
#include <stdio.h>
#include <phidget21.h>

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
using namespace noise;

#define SIMULATION

#ifdef SIMULATION
	#include "Optotrak2Sim.h"
	#include "MarkerSim.h"
	#include "BrownMotorFunctionsSim.h"
	#include "BrownPhidgetsSim.h"
	using namespace BrownMotorFunctionsSim;
	using namespace BrownPhidgetsSim;
#else
	#include <direct.h>
	#include "Optotrak2.h"
	#include "Marker.h"
	#include "BrownMotorFunctions.h"
	#include "BrownPhidgets.h"
	using namespace BrownMotorFunctions;
	using namespace BrownPhidgets;
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
#ifndef SIMULATION
	static const bool gameMode=true;
	static const bool stereo=true;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// random dots
StimulusDrawer stimDrawer;
BoxNoiseStimulus cylinder;

// texture
#define textureHeight 512
#define textureWidth 512
static GLubyte textureImage[textureWidth][textureWidth][4];
static GLuint texName;
double stimulus_height = 150.0;
double direction_motion = 1.0;
double motion_theta = 0.0;

#include "texture_models.h"

/************** PHIDGETS VARIABLES ********************/
const int axisZ = 1;
CPhidgetStepperHandle rotTable;

/********** EYES AND MARKERS **********************/

Vector3d eyeLeft(0,0,0), eyeRight(0,0,0);
Vector3d ind(0,0,0), thu(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999);

vector <Marker> markers;
static double interoculardistance=0.0;

#ifdef SIMULATION
vector<Marker> simMarkers;
Timer simTimer;
#endif

// fingers markers numbers
int ind1 = 13, ind2 = 14, ind3 = 16;
int thu1 = 15, thu2 = 17, thu3 = 18;
int calibration1 = 1, calibration2 = 2;
int screen1 = 19, screen2 = 20, screen3 = 21;
int mirror1 = 6, mirror2 = 7;

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
bool expFinished=false;
bool expStarted=false;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
bool isStimulusDrawn = false;
Vector3d object_reset_position(0,0,0), object_position(0,0,0);
double objdistance = 0.0, objlength = 10.0, objtheta = 0.0;
float theta = 0.0, textdepth = 0.0, motion = 0.0, motion2 = 60.0;

/********** STREAMS **************/
ofstream responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void cleanup();
void updateTheMarkers();
void drawInfo();
void drawStimulus();
void drawGLScene();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void update(int value);
void idle();
void initMotors();
void buildDots();
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial();
void initTrial();
void calibration_fingers(int phase);

void buildTexture();
void renderTexture();
void drawTexture();
void drawDots();
void drawContactPoints();

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/SMT/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/SMT/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_SMT.txt";

// response file name
string responseFile_name = "spring15-SMT_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjelngth\tobjtheta\tstereo\tmotion\ttexture\t\ttottime";

/*************************** FUNCTIONS ***********************************/

/*** Output files and streams ***/
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	// Subject name
	string subjectName = parameters.find("SubjectName");

	// Principal streams files

	// response file (if haptic)
	string responseFileName = experiment_directory + responseFile_name + subjectName + ".txt";

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

}

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

/*** INFO ***/
void drawInfo()
{
	if ( visibleInfo )
	{
		GLText text;	
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text.enterTextInputMode();
		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
		text.draw("# IOD: " +stringify<double>(interoculardistance));
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
		text.draw("#######################");
		text.draw(" ");
		if ( isVisible(markers.at(3).p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers.at(3).p.transpose())+ " [mm]" );

		text.leaveTextInputMode();
	}

	if ( expFinished )
	{
		GLText text2;	
		text2.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text2.enterTextInputMode();
		text2.draw("The experiment is finished.");
		text2.leaveTextInputMode();
	}

}

/*** STIMULI ***/
void buildTexture()
{
//	voronoi_grid();
//	voronoi_patches();
 	checkerboard_messy();
}

void renderTexture()
{    
	glShadeModel(GL_SMOOTH);

	buildTexture();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		   GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		   GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, 
		textureWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
		textureImage);
}

void drawTexture()
{
	glLoadIdentity();

	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);
	glRotatef(motion, 1, 0, 0);
	glRotatef(objtheta + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, stimulus_height/2, 0.0);

	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, stimulus_height/2, 0.0);

	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+50);

	glColor3fv(glBlack);
	glBegin(GL_QUADS);

	glVertex3f(-100.0f, -100.0f, 0.1f);		// Top Left
	glVertex3f( 100.0f, -100.0f, 0.1f);		// Top Right
	glVertex3f( 100.0f, -30.0f, 0.1f);		// Bottom Right
	glVertex3f(-100.0f, -30.0f, 0.1f);		// Bottom Left

	glVertex3f(-100.0f, 30.0f, 0.1f);		// Top Left
	glVertex3f( 100.0f, 30.0f, 0.1f);		// Top Right
	glVertex3f( 100.0f, 100.0f, 0.1f);		// Bottom Right
	glVertex3f(-100.0f, 100.0f, 0.1f);		// Bottom Left

	glEnd();
}

void buildDots()
{
	// fixed density of 10 dots per cm^2
	int numDots = floor(10.0 * (objlength/10.0*cos((objtheta+theta)*M_PI/180.0)*stimulus_height/10.0) );

	cylinder.setNpoints(numDots);
	cylinder.setDimensions(objlength, stimulus_height, 1.0);

	cylinder.compute();
	stimDrawer.setStimulus(&cylinder);
	cylinder.setFluffiness(1E-6);

	stimDrawer.setSpheres(true);
	stimDrawer.initList(&cylinder, glRed);
}

void drawDots()
{
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);
	glRotatef(motion, 1, 0, 0);
	glRotatef(objtheta + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);

	glPushMatrix();
	stimDrawer.draw();
	glPopMatrix();

	glLoadIdentity();

	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+50);

	glColor3fv(glBlack);
	glBegin(GL_QUADS);

	glVertex3f(-100.0f, -200.0f, 0.5f);		// Bottom Left
	glVertex3f( 100.0f, -200.0f, 0.5f);		// Bottom Right
	glVertex3f( 100.0f, -20.0f, 0.5f);		// Top Right
	glVertex3f(-100.0f, -20.0f, 0.5f);		// Top Left

	glVertex3f(-100.0f, 20.0f, 0.5f);		// Top Left
	glVertex3f( 100.0f, 20.0f, 0.5f);		// Top Right
	glVertex3f( 100.0f, 200.0f, 0.5f);		// Bottom Right
	glVertex3f(-100.0f, 200.0f, 0.5f);		// Bottom Left

	glEnd();

}

void drawStimulus()
{
	if ( isStimulusDrawn )
	{
		if(trial.getCurrent()["Texture"])
		{
			drawTexture();
		} else {
			drawDots();
		}
		//drawContactPoints();

	}
}

void drawContactPoints()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+0.5);
	glRotatef(objtheta + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();


	glPushMatrix();
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+0.5);
	glRotatef(objtheta + theta, 0, 1, 0);
	glTranslated(-objlength/2, 0, 0);
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
}

void drawGLScene()
{
    if (stereo)
    {   
	// back on both windows (left and right)
	glDrawBuffer(GL_BACK);

	// now start switching between presentations:
	// Draw left eye view first
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
	drawStimulus();
	drawInfo();

        // Draw right eye view afterwards
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
	if(trial.getCurrent()["Stereo"])
	{
		drawStimulus();
		drawInfo();
	}
	// swap between buffers
        glutSwapBuffers();
    }
    else
    {   
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
	drawStimulus();
	drawInfo();
        glutSwapBuffers();
    }
}

/*** TRIAL ***/
void initVariables()
{
	trial.init(parameters);
	interoculardistance = str2num<int>(parameters.find("IOD"));
	motion_theta = str2num<int>(parameters.find("motion_angle"));
}

void initTrial()
{
	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// no stimulus is shown
	isStimulusDrawn=false;

	// reset the motion indexes
	motion = 0.0; 
	motion2 = stimulus_height; 

	// retrieve the target's specs
	objtheta = trial.getCurrent()["Theta"];
	objlength = trial.getCurrent()["RelDepthObj"];
	objdistance = trial.getCurrent()["AbsDepth"];

	// build the stimuli based on the target's specs
	buildDots();
	renderTexture();	

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(objdistance);

	// set the phidgets
	stepper_rotate(rotTable, objtheta);
	phidgets_linear_move(objlength, axisZ);

	// start the timer
	timer.start();

	// draw the stimulus		
	isStimulusDrawn=true;
}

void advanceTrial()
{
	beepOk(0);
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			trial.getCurrent()["AbsDepth"] << "\t" <<
			trial.getCurrent()["Theta"] << "\t" <<
			trial.getCurrent()["RelDepthObj"] << "\t" <<
			trial.getCurrent()["Motion"] << "\t" <<
			trial.getCurrent()["Stereo"] << "\t" <<
			trial.getCurrent()["Texture"] << "\t" <<
			timeElapsed
			<< endl;

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		isStimulusDrawn=false;
		drawGLScene();
		responseFile.close();
		expFinished = true;
	}

}

/*** keypresses ***/
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		case 'x': // test for final screen
			expFinished=true;
			break;

		case 'i': // show info
			visibleInfo=!visibleInfo;
			break;

		case 'm': // increase IOD
			interoculardistance += 0.5;
			break;

		case 'n': // decrease IOD
			interoculardistance -= 0.5;
			break;

		case '8': // increase object's depth
			textdepth += .25;
			break;

		case '5': // decrease object's depth
			textdepth -= .25;
			break;

		case '6': // increase object's depth
			theta -= 1.5;
			break;

		case '4': // decrease object's depth
			theta += 1.5;
			break;

		case '0': // go to the next trial
		{
			if(expStarted)
				advanceTrial();
		} break;

		case 27: // press escape to quit
		{   
			stepper_close(rotTable);
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		case 13: // press enter to start
		{   
			// turn off info
			visibleInfo = false;

			// check where the object is
			object_reset_position = markers.at(3).p.transpose();
			// calculate where the object has to go
			Vector3d object_position(0.0,object_reset_position.y(),-330.0);
			// move the object to position from where it is
			moveObjectAbsolute(object_position, object_reset_position, 5000);

			// start the experiment
			expStarted = true;
			trial.next();
			initTrial();
		} break;
	}
}

/*** IDLE ***/
void idle()
{

	updateTheMarkers();
	
	if(motion > motion_theta || motion < 0)
		direction_motion = direction_motion * -1;

	motion += trial.getCurrent()["Motion"]*direction_motion;

	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers.at(mirror1).p.z()-markers.at(mirror2).p.z()))/
			sqrt(
			pow(markers.at(mirror1).p.x()-markers.at(mirror2).p.x(), 2) +
			pow(markers.at(mirror1).p.z()-markers.at(mirror2).p.z(), 2)
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

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

}

//######################################################## MAIN #####################//

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	// initializes optotrak and velmex motors
	initOptotrak();

	initMotors();
	reset_phidgets_linear();
	rotTable = stepper_connect();

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
	buildDots();
	initStreams();
	initVariables();
	renderTexture();
	// glut callback
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutIdleFunc(idle);
	glutTimerFunc(TIMER_MS, update, 0);
	glutSetCursor(GLUT_CURSOR_NONE);

	boost::thread initVariablesThread(&initVariables);

	// Application main loop
	glutMainLoop();

	stepper_close(rotTable);
	cleanup();

	return 0;
}

//############################################################
//############################################################
/****************** FIXED FUNCTIONS  ************************/
//############################################################
//############################################################

void updateTheMarkers()
{
#ifdef SIMULATION
	optotrak->updateMarkers(simMarkers);
	markers = optotrak->getAllMarkers();
#else
	optotrak->updateMarkers();
	markers = optotrak->getAllMarkers();
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
			platformIndex=markers.at(calibration1).p;
			platformThumb=markers.at(calibration2).p;
		}
		break;
	case 2:
		{
			indexCoords.init(platformIndex, markers.at(ind1).p, markers.at(ind2).p, markers.at(ind3).p );
			thumbCoords.init(platformThumb, markers.at(thu1).p, markers.at(thu2).p, markers.at(thu3).p );
		}
		break;
	}
}

// Resize window
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

// move monitor to specific distance
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

// Handle refresh of the screen
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
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
        updateTheMarkers();
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

// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(4051,3500);
}

