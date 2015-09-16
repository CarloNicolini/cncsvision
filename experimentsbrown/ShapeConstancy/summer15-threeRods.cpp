// three rods in a triangular arrangement appear in front of the observer
// the observer is asked to grasp the figure
// the observer sees their fingers
// the observer receives haptic feedback
// FACTORS:
// 1) egocentric distance of the object
// 2) relative depth of the object

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
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "Util.h"

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace noise;

//#define SIMULATION

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
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0, phidgetsAlignment=0.0;
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
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// stimuli
StimulusDrawer stimDrawer[3], trainingstim;
CylinderPointsStimulus cylinder[4];

/************** PHIDGETS VARIABLES ********************/
const int axisZ = 1;
CPhidgetStepperHandle rotTable;

/********** EYES AND MARKERS **********************/

Vector3d eyeLeft(0,0,0), eyeRight(0,0,0);
Vector3d ind(0,0,0), thu(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999), home_position(0,0,0), grip_position(0,0,0), upperPinMarker(0,0,0), lowerPinMarker(0,0,0);

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
int calibrationobj1 = 8, calibrationobj2 = 11, calibrationobj3 = 12;
int screen1 = 19, screen2 = 20, screen3 = 21;
int mirror1 = 6, mirror2 = 7;
int phidgets1 = 3, phidgets2 = 4;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int fingerCalibrationDone=0;

#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif

bool allVisibleHead=markers_status;
bool allVisiblePatch=markers_status;
bool allVisibleIndex=markers_status;
bool allVisibleThumb=markers_status;
bool allVisibleFingers=markers_status;
bool allVisibleObject=markers_status;
bool visibleInfo=true;
bool expFinished=false;
bool experiment=false, training=true;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
double objwidth = 0.0, objdepth = 0.0;
bool isStimulusDrawn = false;
bool iGrasped = false, isHandHome = true;
double dist_to_home = 0.0, dist_to_target = 0.0;
int fingersOccluded = 0, framesOccluded = 0, frameN = 0, good_trial = 0;
string subjectName = "junk";
Vector3d object_reset_position(0,0,0), object_position(0,0,0);
int frameToGrasp = 0, frames_at_start = 0, frames_post_grasp = 0;
double timeForGrasping = 15000.0;
bool areFingersDrawn=false, isIndexDrawn=false;
int trial_duration = 0;
int which_time = 0;
bool index_behind_the_stimulus = false, index_inside_the_stimulus = false;
double objtheta = 0.0;

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
void initGLVariables();
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial();
void initTrial();
void calibration_fingers(int phase);
void calibration_head(int phase);
void drawNoFingers();
void buildTrainingCylinder(double depth);
void drawFingers(bool draw);

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/SU2-threeRodsFB/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/SU2-threeRodsFB/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_threeRods.txt";

// markers file name
string markersFile_name = "summer15-threeRods_";

// response file headers
string markersFile_headers = "subjName\tIOD\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tframesOccluded\tAbsDepth\tRelDepthObj\tframesToGrasp";

// response file name
string responseFile_name = "summer15-threeRods_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjwidth\tRelDepthObj\ttrialDuration\tgood_trial";

/*************************** FUNCTIONS ***********************************/

/*** Output files and streams ***/
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	// Subject name
	subjectName = parameters.find("SubjectName");

	// Subject folder
	string dirName  = experiment_directory + subjectName;

#ifdef SIMULATION
	mkdir(dirName.c_str(), 777); // linux syntax
#else
	mkdir(dirName.c_str()); // windows syntax
#endif

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
		case 2:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\calibrate.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 3:
	PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-home.wav", 
		NULL, SND_FILENAME | SND_ASYNC);
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

		switch (fingerCalibrationDone)
		{
			case 0:
				text.draw("Calibration object and index marker are not visible.");		
				break;

			case 1:
				{
					glColor3fv(glGreen);
					text.draw("Index marker has been detected and calibrated. Waiting for Thumb marker to be visible...");	
				} break;

			case 2:
				{
					glColor3fv(glGreen);
					text.draw("Index marker has been detected and calibrated.");	
					text.draw("Thumb marker has been detected and calibrated.");	
					glColor3fv(glWhite);
					text.draw("Touch the two outer markers with the fingerpads then press F (fingers and calibration object visible)");
				} break;

			case 3:
				text.draw("Set home position then press F to start the demo");		
				break;
		}
		glColor3fv(glWhite);
		text.draw("FingerCalibration= " + stringify<int>(fingerCalibrationDone) );
		text.draw(" ");
		if(experiment)
			text.draw("####### SUBJECT #######");
		if(training)
			text.draw("####### THIS IS JUST A TRAINING #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
		text.draw("# IOD: " +stringify<double>(interoculardistance));
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
		text.draw("#######################\n\n");
		text.draw(" ");

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

		if ( abs(screenAlignmentZ) < 89.4 )
			glColor3fv(glRed);
		else
			glColor3fv(glGreen);
		text.draw("# Screen Alignment Z = " +stringify<double>(screenAlignmentZ));

		glColor3fv(glWhite);
		// X and Z coords of simulated fixation
		text.draw("# Fixation Z = " +stringify<double>(markers.at(screen1).p.x()-120.0)+ " [mm]");
		text.draw("# Fixation X = " +stringify<double>(markers.at(screen1).p.z()+363.0)+ " [mm]");
		text.draw(" ");

		if(fingerCalibrationDone < 3)
		{
			glColor3fv(glWhite);
			text.draw("Calibration Platform" );
			
			if ( allVisibleObject )
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			
			text.draw("Marker "+ stringify<int>(calibrationobj1)+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibrationobj1).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(calibrationobj2)+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibrationobj2).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(calibrationobj3)+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibrationobj3).p.transpose())+ " [mm]" );

			text.draw(" ");
		}

		glColor3fv(glWhite);
		text.draw("Rotary table marker" );
		if ( phidgetsAlignment > 88.0 )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Phidgets Alignment = " +stringify<double>(phidgetsAlignment));

		glColor3fv(glWhite);
		text.draw(" " );
		text.draw("Index" );

		if ( isVisible(markers.at(ind1).p) && isVisible(markers.at(ind2).p) && isVisible(markers.at(ind3).p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(ind1)+stringify< Eigen::Matrix<double,1,3> > (markers.at(ind1).p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(ind2)+stringify< Eigen::Matrix<double,1,3> > (markers.at(ind2).p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(ind3)+stringify< Eigen::Matrix<double,1,3> > (markers.at(ind3).p.transpose())+ " [mm]" );

		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("Thumb" );

		if ( isVisible(markers.at(thu1).p) && isVisible(markers.at(thu2).p) && isVisible(markers.at(thu3).p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(thu1)+stringify< Eigen::Matrix<double,1,3> > (markers.at(thu1).p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(thu2)+stringify< Eigen::Matrix<double,1,3> > (markers.at(thu2).p.transpose())+ " [mm]" );
		text.draw("Marker "+ stringify<int>(thu3)+stringify< Eigen::Matrix<double,1,3> > (markers.at(thu3).p.transpose())+ " [mm]" );
		
		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw("Index = " +stringify< Eigen::Matrix<double,1,3> > (ind.transpose()));
		text.draw("Thumb = " +stringify< Eigen::Matrix<double,1,3> > (thu.transpose()));
		text.draw("Home = " +stringify< Eigen::Matrix<double,1,3> > (home_position.transpose()));
		text.draw("Dist To Home = " +stringify<double> (dist_to_home));
		text.draw("experiment = " +stringify<int> (experiment));
		text.draw("training = " +stringify<int> (training));
		text.draw("time for grasping = " +stringify<int> (timeForGrasping));

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
void drawStimulus()
{
	if(fingerCalibrationDone==4)
		if(!allVisibleFingers)
			drawNoFingers();

	if(experiment)
	{
		if ( isStimulusDrawn && !isHandHome && !iGrasped )
		{
			drawFingers(isIndexDrawn);

			glLoadIdentity();
			glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);

			// Left rear 
			glPushMatrix();
			glTranslated(-objwidth/2, 0, -objdepth);
			stimDrawer[0].draw();
			glPopMatrix();

			// Right rear rod
			glPushMatrix();
			glTranslated(objwidth/2, 0, -objdepth);
			stimDrawer[1].draw();
			glPopMatrix();

			// Front rod
			glPushMatrix();
			glTranslated(0, 0, 0);
			stimDrawer[2].draw();
			glPopMatrix();
		}
	
		if (iGrasped && isHandHome)
			advanceTrial();
	}

	if(training)
	{
		drawFingers(areFingersDrawn);

		if ( isStimulusDrawn && !isHandHome && !iGrasped )
		{
			glLoadIdentity();
			glTranslated(0, 0, -450-(objdepth/2));

			// Left rear 
			glPushMatrix();
			trainingstim.draw();
			glPopMatrix();
		}
	
		if (iGrasped && isHandHome)
			initTrial();
	}
}

void drawFingers(bool draw)
{
	if(draw)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated(ind.x(),ind.y(),ind.z());
		glColor3fv(glRed);
		glutSolidSphere(1,10,10);
		glPopMatrix();
	}
		glPushMatrix();
		glLoadIdentity();
		glTranslated(thu.x(),thu.y(),thu.z());
		glColor3fv(glRed);
		glutSolidSphere(1,10,10);
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

void initGLVariables()
{
	// Ora inizializza i 3 rods (stimoli)
	for (int i=0; i<3; i++)
	{
		cylinder[i].setNpoints(150);
		cylinder[i].setRadiusAndHeight(2,50); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[i].compute();
		stimDrawer[i].setStimulus(&cylinder[i]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[i].setSpheres(true);
		stimDrawer[i].initList(&cylinder[i], glRed);
	}
}

void buildTrainingCylinder(double depth)
{
	// stimolo training
	cylinder[3].setNpoints(250);
	cylinder[3].setRadiusAndHeight(depth/2,50); // raggio (mm) altezza (mm)
	// Dispone i punti random sulla superficie cilindrica 
	cylinder[3].compute();
	trainingstim.setStimulus(&cylinder[3]);
	// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
	trainingstim.setSpheres(true);
	trainingstim.initList(&cylinder[3], glRed);
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
        drawStimulus();
	drawInfo();

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
	interoculardistance = str2num<double>(parameters.find("IOD"));
	trial_duration = str2num<int>(parameters.find("trial_duration"));
}

int training_time(int counter)
{
	int grasp_time = 0;

	switch(counter)
	{
		case 0:
			grasp_time = 15000;
			break;

		case 1:
			grasp_time = 5000;
			break;

		case 2:
			grasp_time = 3000;
			break;

		case 3:
			grasp_time = trial_duration;
			break;
	}

	return grasp_time;
}

void initTrial()
{

	if(training)
	{
		// reset counters
		frameN = 0;
		frameToGrasp = 0;
		framesOccluded = 0;
		frames_post_grasp = 0;
		frames_at_start = 0;

		// subject did not grasp yet
		iGrasped = false;

		// draw an empty screen
		isStimulusDrawn=false;

		// set the depth of the training object
		objdepth = unifRand(30.0, 70.0);

		// build the training object
		buildTrainingCylinder(objdepth);

		// refresh the scene
		drawGLScene();

		// set the monitor at the right ditance
		initProjectionScreen(-450);

		// calculate where the object has to go
		Vector3d object_position(0.0,object_reset_position.y(),-450);
		// move the object to position
		moveObjectAbsolute(object_position, object_reset_position, 5000);

		// set the phidgets
		stepper_rotate(rotTable, 0.0);
		phidgets_linear_move(objdepth-30.0, axisZ);

		// draw the stimulus	
		beepOk(0);
		isStimulusDrawn=true;

		timer.start();

	}

	if(experiment)
	{
		cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

		// reset counters
		frameN = 0;
		frameToGrasp = 0;
		framesOccluded = 0;
		frames_post_grasp = 0;
		frames_at_start = 0;
		timeForGrasping = trial_duration;

		// subject did not grasp yet
		iGrasped = false;

		// draw an empty screen
		isStimulusDrawn=false;

		// set the width of the object to be randomly between 30 and 70 mm
		objwidth = unifRand(30.0, 50.0);

		// set the depth of the object to be randomly between 30 and 70 mm
		objdepth = trial.getCurrent()["RelDepthObj"];

		// initialize the stimuli
		initGLVariables();

		// refresh the scene
		drawGLScene();

		// set the monitor at the right ditance
		initProjectionScreen(trial.getCurrent()["AbsDepth"]);

		// calculate where the object has to go
		Vector3d object_position(0.0,object_reset_position.y(),trial.getCurrent()["AbsDepth"]);
		// move the object to position
		moveObjectAbsolute(object_position, object_reset_position, 5000);

		// calculate the semi-angle at the vertex of the triangle
		objtheta = atan((objwidth/2)/objdepth);
		// calculate the side of the triangle
//		double objside = objdepth / cos(objtheta);

		// set the phidgets
//		stepper_rotate(rotTable, objtheta * 180.0 / M_PI);
		phidgets_linear_move(objdepth-30.0, axisZ);

		// draw the stimulus	
		beepOk(0);
		isStimulusDrawn=true;

		// markers file (if haptic)
		string markersFileName = experiment_directory + subjectName + "/" + markersFile_name + subjectName + "_markers-trial_" + stringify(trialNumber) + ".txt";

		markersFile.open(markersFileName.c_str());
		markersFile << fixed << markersFile_headers << endl;

		timer.start();
	}

}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	double percent_occluded_frames = (double)framesOccluded/(frameN-frames_at_start-frames_post_grasp);
	bool not_enough_frames = percent_occluded_frames > 0.20;
	
	if(not_enough_frames)
	{
		beepOk(1);
		trial.reinsert(trial.getCurrent());
		good_trial = 0;
	} else
	{
		//beepOk(0);
		good_trial = 1;
	}

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			trial.getCurrent()["AbsDepth"] << "\t" << 
			objwidth << "\t" << 
			objdepth << "\t" << 
			timeElapsed << "\t" << 
			good_trial
			<< endl;

	markersFile.close();

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		responseFile.close();
		expFinished = true;
	}

}

/*** keypresses ***/
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		#ifdef SIMULATION
		case '0':
		{
			good_trial = 1;
			advanceTrial();
		} break;

		case '1':
			allVisibleFingers = !allVisibleFingers;
			break;

		case '2':
		{
			trial.reinsert(trial.getCurrent());
			good_trial = 0;
			advanceTrial();
		} break;

		#endif

		case 'i': // show info
			visibleInfo=!visibleInfo;
			break;

		case 'm': // increase IOD
			interoculardistance += 0.5;
			break;

		case 'n': // decrease IOD
			interoculardistance -= 0.5;
			break;

		case 27: // press escape to quit
		{   
			stepper_close(rotTable);
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		// fingers calibration
		case 'f':
		case 'F':
		{
			// Interpolate the fingertip (fourth virtual marker)
			if ( fingerCalibrationDone==2 && allVisibleObject && allVisibleFingers )
			{
				fingerCalibrationDone=3;
				calibration_fingers(fingerCalibrationDone);
				beepOk(3);
				break;
			}

			// Start the experiment
			if ( fingerCalibrationDone==3  && allVisibleFingers && isVisible(markers.at(3).p) )
			{
				fingerCalibrationDone=4;
				beepOk(0);
				home_position = ind;
				visibleInfo=false;
				drawGLScene(); // this is needed otherwise the next motor command freezes the screen

				// check where the object is
				object_reset_position = markers[3].p.transpose();

				trial.next();
				initTrial();
			}

			if (fingerCalibrationDone==4 && training)
				areFingersDrawn = !areFingersDrawn;

		} break;

		case ' ': // confirm that you grasped
		{
			if(allVisibleFingers)
			{
				iGrasped = true;
				isStimulusDrawn = false;
			}
		} break;

		case 'e':
		{
			if(training)
			{
				training = false;
				experiment = true;
				beepOk(0);
				initTrial();
			}
		} break;

		case 's':
		{
			if(training)
				initTrial();
		} break;

		case 8: // backspace resets calibration
		{
			fingerCalibrationDone=0;
		} break;

		case '+':
		{
			if(training)
			{
				which_time ++;
				which_time = which_time%4;
				timeForGrasping = training_time(which_time);
			}
		} break;
	}
}

/*** IDLE ***/
void idle()
{

	updateTheMarkers();

	// Visibility check
	allVisibleIndex = isVisible(markers.at(ind1).p) && isVisible(markers.at(ind2).p) && isVisible(markers.at(ind3).p);
	allVisibleThumb = isVisible(markers.at(thu1).p) && isVisible(markers.at(thu2).p) && isVisible(markers.at(thu3).p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisibleObject = isVisible(markers.at(calibrationobj1).p) && isVisible(markers.at(calibrationobj2).p) && isVisible(markers.at(calibrationobj3).p);

	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers.at(mirror1).p.z()-markers.at(mirror2).p.z()))/
			sqrt(
			pow(markers.at(mirror1).p.x()-markers.at(mirror2).p.x(), 2) +
			pow(markers.at(mirror1).p.z()-markers.at(mirror2).p.z(), 2)
			)
			)*180/M_PI;

	// mirror alignment check
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

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

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
		thu = thumbCoords.getP1();
	#endif

	// what the program checks online during the grasp
	if (fingerCalibrationDone==4 )
	{
		// frames counting
		frameN++;

		// fingersOccluded
		if ( allVisibleFingers )
			fingersOccluded = 0;
		else
		{
			fingersOccluded=1;
		}
		
		if(timer.getElapsedTimeInMilliSec() > timeForGrasping)
			iGrasped = true;

		if(isHandHome && !iGrasped)
		{
			timer.start();
			frames_at_start++;
		}

		if(iGrasped)
			frames_post_grasp++;

		// middle point between index and thumb (center of grasp)
		grip_position = (ind + thu)/2;

		// euclidean distance to home
		dist_to_home = sqrt( pow((grip_position.x() - home_position.x()),2) + pow((grip_position.y() - home_position.y()),2) + pow((grip_position.z() - home_position.z()),2) );

		// is the hand at start?
		if( dist_to_home < 50.0 )
			isHandHome = true;
		else
			isHandHome = false;

		// euclidean distance to target
		dist_to_target = sqrt( pow((grip_position.x() - 0.0),2) + pow((grip_position.y() - 0.0),2) + pow((grip_position.z() - trial.getCurrent()["AbsDepth"]),2) );

		if(!isHandHome && !allVisibleFingers && !iGrasped)
			framesOccluded++;

		if(experiment)
		{
			index_behind_the_stimulus = (abs(ind.x()) < objwidth/2 && ind.z() < (trial.getCurrent()["AbsDepth"]-(objdepth/2)));
			index_inside_the_stimulus = (atan(ind.x()/abs(ind.z()-trial.getCurrent()["AbsDepth"])) < objtheta && ind.z() < trial.getCurrent()["AbsDepth"] && ind.z() > (trial.getCurrent()["AbsDepth"]-(objdepth/2)));
			isIndexDrawn = !(index_behind_the_stimulus || index_inside_the_stimulus);
		}

		if(experiment)
		{
			// Write to responseFile
			markersFile << fixed <<
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
				trial.getCurrent()["AbsDepth"]	<< "\t" <<
				trial.getCurrent()["RelDepthObj"] << "\t" <<
				frameToGrasp <<
				endl;
		}
	}

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
	initStreams();
	initVariables();

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

