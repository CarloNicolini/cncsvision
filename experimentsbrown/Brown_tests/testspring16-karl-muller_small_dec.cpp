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
#include <algorithm>

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

// openGL graphics
#include "GLUtils.h"

// optotrak
#include "VRCamera.h"
#include "CoordinatesExtractor.h"

// stimuli
#include "StimulusDrawer.h"
#include "BoxNoiseStimulus.h"
#include "CylinderPointsStimulus.h"

// drawInfo
#include "GLText.h"

// trial matrix
#include "BalanceFactor.h"

// staircase
#include "ParStaircase.h"

// miscellaneous
#include "ParametersLoader.h"
#include "Util.h"

/*********** NOISELIB LIBRARIES ***************/
#include <noise/noise.h>
#include "noiseutils.h"

/***** CALIBRATION THINGS *****/
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

//#define TEST

#ifdef TEST
	#include <boost/random/mersenne_twister.hpp>
	#include <boost/random/normal_distribution.hpp>
	#include <boost/random/variate_generator.hpp>
#endif

#ifndef SIMULATION
	#include <direct.h> // mkdir
#endif

#include "Optotrak2.h"
#include "Marker.h"
#include "BrownMotorFunctions.h"
#include "BrownPhidgets.h"
using namespace BrownMotorFunctions;
using namespace BrownPhidgets;

static const Vector3d center(0,0,focalDistance);
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0, phidgetsAlignment=0.0;
Screen screen;
Vector3d centercal(29.75,-133.94,-296.16); //updated 9/25/14

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
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// stimuli
StimulusDrawer fixCrossDrawer[2], stereocheckDrawer[4]; // , trainingstim
BoxNoiseStimulus fixCross[2], stereocheck[4];
//CylinderPointsStimulus cylinder;

// texture
#define textureHeight 512
#define textureWidth 512
static GLubyte textureImage[textureWidth][textureWidth][4];
static GLuint texName;
#ifndef SIMULATION
#include "../Brown_tests/texture_models.h"
#else
#include "texture_models.h"
#endif

/************** PHIDGETS VARIABLES ********************/
const int axisZ = 1;
CPhidgetStepperHandle rotTable;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft(0,0,0), eyeRight(0,0,0);
Vector3d ind(0,0,0), thu(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999), home_position(0,0,0), grip_position(0,0,0), upperPinMarker(0,0,0), lowerPinMarker(0,0,0);

vector <Marker> markers;
static double interoculardistance=0.0;

// markers map
int ind1 = 13, ind2 = 14, ind3 = 16;
int thu1 = 15, thu2 = 17, thu3 = 18;
int calibration1 = 1, calibration2 = 2;
int calibrationobj1 = 8, calibrationobj2 = 11, calibrationobj3 = 12;
int screen1 = 19, screen2 = 20, screen3 = 21;
int mirror1 = 6, mirror2 = 7;
int phidgets1 = 3, phidgets2 = 4;
int size1 = 9, size2 = 10, size3 = 23, size4 = 24;
int stepper = 22;

/********* CALIBRATION VARIABLES *********/

int fingerCalibrationDone=0;

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

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
BalanceFactor<double> trial;

// integer
int currBlock = 1;
int blocks = 0;
int frameN = 0;
int trialNumber = 0;
int textureCounter = 0;
int frameToGrasp = 0, frames_at_start = 0, frames_post_grasp = 0;
int fingersOccluded = 0, framesOccluded = 0, good_trial = 0;
int which_time = 0;
int repetitions = 0;
int max_repetitions = 0;
int base_repetitions = 0;
int curr_max_repetitions = 0;
int num_points = 0;
int obj_markers[3] = {3, 4, 24};
int obj_visible_marker = 1;
int objId = 0;
int yAdjust = 120;

#define max_points 450

// bool
bool isStimulusDrawn = false;
bool isTrialDone = false;
bool inPause = false;
bool iGrasped = false, isHandHome = true;
bool areFingersDrawn=false, isIndexDrawn=false;
bool index_behind_the_stimulus = false, index_inside_the_stimulus = false;
bool checkglasses = true;
bool visibleInfo=true;
bool expFinished=false;
bool experiment=false, training=false;
bool phidgetsKO = false;
bool cube = true;
bool fingers_calibrated = false;
bool goGrasp = false;
bool stimulus_cut = false;

// string
string subjectName = "junk";

// double
double objdistance = 0.0;
double trial_duration = 0.0, trial_time = 0.0;
double motion = 0.0;
double IOD = 0.0;
double direction_motion = 1.0;
double current_height = 0.0;
double current_angle = 0.0;
double stereocheckZ[4] = {25.0, 12.5, -12.5, -25.0};
double timeForGrasping = 15000.0;
double dist_to_home = 0.0;
double dist_to_goal = 999.0;
double motion_step = 0.0, motion_theta = 0.0;
double objradius = 0.0;
double fixed_deltaz = 20.0, fixed_size = 15.0;
double objheight = 0.0, objwidth = 0.0;
double pointing_motion = 0.0;
double cube_z = 0.0;
double beta = 0.0;
double offsetX = 0.0;
double offsetY = 0.0;
double offsetZ = -4.0;
double targetMarkerX = 0.0;
double targetMarkerY = 0.0;
double targetMarkerZ = 0.0;


// pointers
double *z, *y, *x;

// openGL
GLfloat glDarkRed[3] = {0.3,0,0};
GLfloat glDarkGreen[3] = {0,0.3,0};

// vector3d
Vector3d object_reset_position(0,0,0), object_position(0,0,0);

/********** STREAMS **************/
ofstream responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/

// output files and streams
void initStreams();

// sounds
void beepOk(int tone);

// info
void drawInfo();

// grasp
void calibration_fingers(int phase);
void drawNoFingers();
void drawFingers(bool draw);

// stimuli
void build_stereocheck();

void drawMullerLyer(double body_width, double body_height, double finAngle);
void draw_stereocheck();
void draw_height();
void draw_all();
void drawStimulus();
void drawGLScene();

// trial
void initVariables();
void initTrial();
void advanceTrial();
int training_time(int counter);

// keypresses
void handleKeypress(unsigned char key, int x, int y);

// online operations
void online_apparatus_alignment();
void online_fingers();
void online_trial();

// idle
void idle();

// others
void cleanup();
void updateTheMarkers();
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void update(int value);
void initMotors();
void initOptotrak();
void initRendering();

//#include "./generate_trajectories.h"

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/karl/spring16-karl-muller/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/karl/spring16-karl-muller/";
#endif

// parameters file directory and name
string parametersFile_directory = experiment_directory + "parameters_spring16-karl-muller_x_1.txt";

// response file name
string responseFile_name = "spring16-karl-muller_x_1_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\trepetitions\tblock\tMLheight\tfinAngle\tAbsDepth\ttrialDuration\tgood_trial";

// markers file name
string markersFile_name = responseFile_name;

// response file headers
string markersFile_headers = "subjName\tIOD\ttrialN\tattempt\ttime\tglobalTime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tframesOccluded\tAbsDepth\tMLheight\tfinAngle\tframesToGrasp";

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

	// response file
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
		case 4:
		// Remember to put double slash \\ to specify directories!!!
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\completed.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 5:
		// Remember to put double slash \\ to specify directories!!!
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\knock-out.wav", NULL, SND_FILENAME | SND_ASYNC);
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
		text.draw("####### testspring16-karl-muller_xy_1 #######");
		text.draw("");

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
		text.draw("# block: " +stringify<double>(currBlock));
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
		if ( abs(270 - abs(markers.at(phidgets1).p.z())) < 3.0 )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Front Rod Z = " +stringify<double>(markers.at(phidgets1).p.z()));

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

		text.draw(" ");
		text.draw("Stepper marker " + stringify< Eigen::Matrix<double,1,3> > (markers.at(stepper).p.transpose()));
		text.draw("Free marker " + stringify< Eigen::Matrix<double,1,3> > (markers.at(9).p.transpose()));
		
		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("Index = " +stringify< Eigen::Matrix<double,1,3> > (ind.transpose()));
		text.draw("Thumb = " +stringify< Eigen::Matrix<double,1,3> > (thu.transpose()));
		text.draw(" " );
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw("time for grasping = " +stringify<int> (timeForGrasping));
		text.draw("objdepth = " +stringify<double> (current_height));
		text.draw("objwidth = " +stringify<double> (objwidth*2.0));
		text.draw("currentReps = " +stringify<int> (curr_max_repetitions));
		text.draw("finAngle = " +stringify<double> (current_angle));
		text.draw(" " );
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[0]) + " " + stringify<double>(stereocheckZ[1]));
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[2]) + " " + stringify<double>(stereocheckZ[3]));

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
#define DEG2RAD 3.14159/180.0
void drawMullerLyer(double body_width, double body_height, double finAngle)
{
	glColor3fv(glRed);

	double teeth_width = 4.0;
	double teeth_height = 16.0;

	objId = (body_height-40) /5;

	if (isVisible(markers.at(obj_markers[objId]).p)){
		targetMarkerX = markers.at(obj_markers[objId]).p.x();
		targetMarkerY = markers.at(obj_markers[objId]).p.y();
		targetMarkerZ = markers.at(obj_markers[objId]).p.z();
	}

	// main body
	glLoadIdentity();
	glTranslated(0, 0, objdistance);


	double xCorr = sin(DEG2RAD*finAngle)*teeth_height/2 -
        fabs(cos(DEG2RAD*finAngle)*teeth_width/2);
    double yCorr = cos(DEG2RAD*finAngle)*teeth_height/2 -
        fabs(sin(DEG2RAD*finAngle)*teeth_width/2);
	double thetas[4] = {-1*finAngle, finAngle, finAngle, -1*finAngle};
	double latDisplacement[4] = {body_width/2.0+xCorr, body_width/2.0+xCorr, -xCorr-body_width/2.0, -xCorr-body_width/2.0};
	double vertDisplacement[4] = {body_height/2.0+yCorr, -yCorr-body_height/2.0, body_height/2.0+yCorr, -yCorr-body_height/2.0};

	// main body
	glLoadIdentity();
	glTranslated(targetMarkerX+offsetX, targetMarkerY+offsetY, objdistance);

	glBegin(GL_QUADS);
	glVertex2f(-body_width/2.0, -body_height/2.0);
	glVertex2f(-body_width/2.0, body_height/2.0);
	glVertex2f(body_width/2.0, body_height/2.0);
	glVertex2f(body_width/2.0, -body_height/2.0);
	glEnd();

	// teeth, if they are not disabled
	if(finAngle != 999) {
	for(int i = 0; i < 4; i++)
	{
		glLoadIdentity();
		glTranslated(latDisplacement[i]+targetMarkerX+offsetX, vertDisplacement[i]+targetMarkerY+offsetY, objdistance);
		glRotated(thetas[i], 0, 0, 1);

		glBegin(GL_QUADS);
		glVertex2f(-teeth_width/2.0, -teeth_height/2.0);
		glVertex2f(-teeth_width/2.0, teeth_height/2.0);
		glVertex2f(teeth_width/2.0, teeth_height/2.0);
		glVertex2f(teeth_width/2.0, -teeth_height/2.0);
		glEnd();
	}
	}
}

void build_stereocheck()
{
	for (int i = 0; i < 4; i++)
	{
		// BOX left
		stereocheck[i].setDimensions(50.0, 50.0, 0.1);
		stereocheck[i].setNpoints(500);
		stereocheck[i].compute();

		stereocheckDrawer[i].setStimulus(&stereocheck[i]);
		stereocheckDrawer[i].setSpheres(true);
		stereocheckDrawer[i].initList(&stereocheck[i], glRed);
	}
}

void draw_stereocheck()
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);
//	glRotated(90, 1, 0, 0);

	// Upper left
	glPushMatrix();
	glTranslated(-25.0, 25.0, stereocheckZ[0]);
	stereocheckDrawer[0].draw();
	glPopMatrix();

	// Upper right
	glPushMatrix();
	glTranslated(25.0, 25.0, stereocheckZ[1]);
	stereocheckDrawer[1].draw();
	glPopMatrix();

	// Lower left
	glPushMatrix();
	glTranslated(-25.0, -25.0, stereocheckZ[2]);
	stereocheckDrawer[2].draw();
	glPopMatrix();

	// Lower right
	glPushMatrix();
	glTranslated(25.0, -25.0, stereocheckZ[3]);
	stereocheckDrawer[3].draw();
	glPopMatrix();
}

void draw_all()
{

	IOD = interoculardistance;

	drawMullerLyer(12.5, current_height, current_angle);
}

void drawStimulus()
{
	if(checkglasses && !training)
	{
		glLoadIdentity();
		glTranslated(0, 0, objdistance);

		IOD = interoculardistance;
		//draw_stereocheck();
	}
		
	if(fingerCalibrationDone==4)
		if(!allVisibleFingers)
			drawNoFingers();

	drawFingers(false);

	if ( isStimulusDrawn && !iGrasped )
	{
		draw_all();
	}

	if (iGrasped && isHandHome)
			if(experiment)
				advanceTrial();
			else if(training)
				initTrial();
}

void drawGLScene()
{
	online_apparatus_alignment();
	online_fingers();
	online_trial();

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

void drawFingers(bool draw)
{
	if(draw)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated(ind.x(),ind.y(),ind.z());
		glColor3fv(glRed);
		glutSolidSphere(2.5,10,10);
		glPopMatrix();
	
		glPushMatrix();
		glLoadIdentity();
		glTranslated(thu.x(),thu.y(),thu.z());
		glColor3fv(glRed);
		glutSolidSphere(2.5,10,10);
		glPopMatrix();
	}

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

/*** TRIAL ***/
void initVariables()
{
	trial.init(parameters);
	trial.print();

	interoculardistance = str2num<double>(parameters.find("IOD"));
	trial_duration = str2num<double>(parameters.find("trial_duration"));

	blocks = str2num<int>(parameters.find("blocks"));

	motion_step = str2num<double>(parameters.find("motion_step"));
	motion_theta = str2num<double>(parameters.find("motion_theta"));
	objheight = str2num<double>(parameters.find("objheight"));
	if(parameters.find("distance") == "n")
		objdistance = -300;
	else if(parameters.find("distance") == "f")
		objdistance = -450;

	max_repetitions = str2num<int>(parameters.find("max_repetitions"));
	base_repetitions = str2num<int>(parameters.find("base_repetitions"));
	if(currBlock==1){
		curr_max_repetitions = str2num<int>(parameters.find("curr_max_repetitions"));
	}
}

void initTrial()
{
		cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

		// reset counters
		frameN = 0;
		frameToGrasp = 0;
		framesOccluded = 0;
		frames_post_grasp = 0;
		frames_at_start = 0;
		motion = 0.0;

		#ifdef SIMULATION
			ind = Vector3d(0.0, 0.0, 0.0);
			goGrasp = false;
		#endif

		updateTheMarkers();
		if(training)
		{
			if(phidgetsAlignment < 85.0)
				phidgetsKO = true;
		}

		/*if(checkglasses)
			build_stereocheck();*/

		// subject did not grasp yet
		iGrasped = false;

		// draw an empty screen
		isStimulusDrawn=false;

		if(training)
		{
			if(unifRand(-1.0, 1.0) > 0) {
				current_angle  = 999.0;
				current_height = 40.0;
			}
			else {
				current_angle  = 999.0;
				current_height = 50.0;
			}
		}

		if(experiment)
		{
			timeForGrasping = trial_duration;

			// retrieve current object
			current_height = trial.getCurrent()["MLheight"];
			//retrieve fin angle, if we are in block 2 (adaptatiobn)
			if(currBlock==2) {
				current_angle  = trial.getCurrent()["AngleOfFins"];
			}
			else {
				current_angle = 999;
			}	
		}

		// refresh the scene
		drawGLScene();

		// set the monitor at the right ditance
		initProjectionScreen(objdistance);

		// calculate where the object has to go
//		Vector3d object_position(0.0,object_reset_position.y(),objdistance-depth_offset); // depth offset depends on the block
		//double depth_offset = 80.0;
		//Vector3d object_position(0.0,object_reset_position.y(),objdistance-depth_offset); // depth offset has to be measured on the carousel
		// move the object to position
		//moveObjectAbsolute(object_position, object_reset_position, 5000);

		// set the phidgets --> this needs to be updated based on the carousel!!!!
		double target_angle = 36.0;
		if(current_height == 45.0)
			target_angle = 126.0;
		if(current_height == 50.0)
			target_angle = 216.0;

		// rotate teh carousel
		if(unifRand(-1.0, 1.0) > 0) {
			stepper_rotate(rotTable, target_angle-40.0);
		}else {
			stepper_rotate(rotTable, target_angle+60.0);
		}
		stepper_rotate(rotTable, target_angle);

		// draw the stimulus	
		beepOk(0);
		isStimulusDrawn=true;
		frames_at_start = 0;

		// markers file
		string markersFileName = experiment_directory + subjectName + "/" + markersFile_name + subjectName + "_markers-trial_" + stringify(trialNumber) + ".txt";

		// if in experiment mode, record the optotrak data
		if(experiment)
		{
			markersFile.open(markersFileName.c_str());
			markersFile << fixed << markersFile_headers << endl;
		}

		timer.start();
		globalTimer.start();
}

void advanceTrial()
{
	double timeElapsed = globalTimer.getElapsedTimeInMilliSec();
	double percent_occluded_frames = (double)framesOccluded/(frameN-frames_at_start-frames_post_grasp);
	bool not_enough_frames = percent_occluded_frames > 0.20;

	updateTheMarkers();
	if(phidgetsAlignment < 85.0)
		phidgetsKO = true;
	
	cerr << "not_enough_frames = " << not_enough_frames << "\n" << endl;
	cerr << "phidgetsKO = " << phidgetsKO << "\n" << endl;
	cerr << "iGrasped = " << iGrasped << "\n" << endl;

	if(not_enough_frames || frames_at_start < 10)
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
			repetitions << "\t" <<
			currBlock << "\t" <<
			current_height << "\t" <<
			current_angle << "\t" <<
			objdistance << "\t" <<
			timeElapsed << "\t" << 
			good_trial
			<< endl;
 
	markersFile.close();

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else if (repetitions < curr_max_repetitions)
	{	
		repetitions++;
		initVariables();

		trial.next();
		initTrial();
	} else
	{
		//check if we are at the end
		if(currBlock==blocks) {
			responseFile.close();
			expFinished = true;
			beepOk(4);
		}else { //if not: go to next block and initialise!
			currBlock++;
			if(currBlock==2) {
				curr_max_repetitions = max_repetitions;
			}else {
				curr_max_repetitions = base_repetitions;
			}
			repetitions = 1;
			initVariables();
			trial.next();
			initTrial();
		}
	}
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


			case 'h':
			{
				ind.z() -= 10;
				thu.z() -= 10;
			} break;
			case 'j':
			{
				ind.z() += 10;
				thu.z() += 10;
			} break;

			case 'g':
			{
				goGrasp = true;
			}

		#endif

		if(training)
		{
			case '5':
				cube_z += 2.0;
				break;

			case '8':
				cube_z -= 2.0;
				break;
		}

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
			stepper_rotate(rotTable, 0.0);
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
				fingers_calibrated = true;
				fingerCalibrationDone=3;
				calibration_fingers(fingerCalibrationDone);
				beepOk(3);

				build_stereocheck();
				drawGLScene();
				initProjectionScreen(objdistance);

				break;
			}

			// Start the experiment
			if ( fingerCalibrationDone==3  && (allVisibleFingers || !fingers_calibrated) && isVisible(markers.at(stepper).p))
			{
				centercal[1] = markers[stepper].p.y();
				centercal[2] = markers[stepper].p.z();
				double depth_offset = 85.0;

				Vector3d moveTo(0.0, centercal[1]+yAdjust, objdistance-depth_offset);
				moveObjectAbsolute(moveTo,centercal,6000);

				
				fingerCalibrationDone=4;
				beepOk(0);
				home_position = ind;
				visibleInfo=false;
				drawGLScene(); // this is needed otherwise the next motor command freezes the screen

				// check where the object is
				//object_reset_position = markers.at(stepper).p.transpose();
				/*
				if(markers.at(phidgets2).p.x() - markers.at(phidgets1).p.x() > 0.0)
					stepper_rotate(rotTable, -(90.0-phidgetsAlignment));
				else
					stepper_rotate(rotTable, (90.0-phidgetsAlignment));
				stepper_set_angle(rotTable, 0);
				*/
				training = true;
				repetitions = 1;
				trial.next();
				initTrial();
			}

			if (fingerCalibrationDone==4 && training)
				areFingersDrawn = !areFingersDrawn;

		} break;

		case 'e':
		{
			if(training)
			{
				training = false;
				checkglasses = false;
				experiment = true;
				beepOk(0);
				initTrial();
			}
		} break;

		case 'a':
		case 'A':
			offsetX -= 0.5;
			break;
		case 'd':
		case 'D':
			offsetX += 0.5;
			break;
		case 's':
		case 'S':
			offsetY -= 0.5;
			break;
		case 'w':
		case 'W':
			offsetY += 0.5;
			break;

		case 'k':
		case 'K':
		{
			if(training){
				initTrial();
			}else if(!training && !fingers_calibrated) {
				advanceTrial();
			}
		} break;

		case 8: // backspace resets calibration
		{
			initMotors();
			fingerCalibrationDone=0;
		} break;

		case'o':
		{
			fingers_calibrated = false;
			fingerCalibrationDone=3;
		} break;


		case '+':
		{
			checkglasses = !checkglasses;
			random_shuffle(&stereocheckZ[0], &stereocheckZ[4]);

			if(training)
			{
				which_time ++;
				which_time = which_time%4;
				timeForGrasping = training_time(which_time);
			}			
		} break;

		case 'c':
			cube = !cube;
		break;
	}
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
			thu = thumbCoords.getP1();
	}
	/*
	#else
		if(goGrasp && timer.getElapsedTimeInMilliSec() < trial_duration)
			ind = index_trajectory(Vector3d(200.0, -150.0, 0.0), Vector3d(0.0, 0.0, objdistance), M_PI/2.0, 1000.0, timer.getElapsedTimeInMilliSec()/3000.0, current_height);
	*/
	#endif
}

void online_trial()
{
	if(fingerCalibrationDone==4)
	{

		if(training) // in training mode the object rocks with a wider angle
		{
			if(motion > motion_theta*2 || motion < -motion_theta*2)
				direction_motion = direction_motion * -1.0;
		} else // in experiment mode the object rocks with the correct angle of rotation
		{
			if(motion > motion_theta || motion < -motion_theta)
				direction_motion = direction_motion * -1.0;
		}

		if(training)
			pointing_motion += 1.0;

		if( (ind.z() > objdistance+2.0 && 
			 thu.z() > objdistance+2.0) || 
			(abs(ind.x()) >= objwidth &&
			 abs(thu.x()) >= objwidth)	) // object will stop moving when both fingers are almost touching the surface
		{
			stimulus_cut = false;
			motion += motion_step*direction_motion;
		} else
		{
			stimulus_cut = true;
			motion = 0.0;
		}

		// fingersOccluded
		if ( allVisibleFingers )
			fingersOccluded = 0;
		else
		{
			fingersOccluded=1;
		}
		
		if(isStimulusDrawn && timer.getElapsedTimeInMilliSec() > timeForGrasping)
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
		dist_to_goal = sqrt( pow((grip_position.x() - (targetMarkerX+offsetX)),2) +
			pow((grip_position.y() - (targetMarkerY+offsetY)),2) +
			pow((grip_position.z() - (targetMarkerZ+offsetZ)),2) );

		if(!isHandHome && dist_to_goal < 20.0)
			iGrasped = true;


		if(!isHandHome && !allVisibleFingers && !iGrasped)
			framesOccluded++;
	}
}

/*** IDLE ***/
void idle()
{

	updateTheMarkers();

	// what the program checks online during the grasp
	if (fingerCalibrationDone==4 )
	{
		frameN++;

		if(experiment)
		{
			// Write to responseFile
			markersFile << fixed <<
				parameters.find("SubjectName") << "\t" <<
				interoculardistance << "\t" <<
				trialNumber << "\t" <<
				currBlock << "\t" <<
				timer.getElapsedTimeInMilliSec() << "\t" <<				//time
				globalTimer.getElapsedTimeInMilliSec() << "\t" <<		//globalTime
				frameN << "\t" <<										//frameN
				ind.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
				thu.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
				eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
				eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
				fingersOccluded	<< "\t" <<						//fingersOccluded
				framesOccluded	<< "\t" <<						//framesOccluded
				objdistance	<< "\t" <<
				current_height << "\t" <<
				current_angle << "\t" <<
				frameToGrasp << "\t" <<
				endl;
		}
	}
	
	// eye coordinates
	eyeRight = Vector3d(IOD/2,0,0);
	eyeLeft = Vector3d(-IOD/2,0,0);
}

//######################################################## MAIN #####################//

int main(int argc, char*argv[])
{
#ifdef TEST
	mathcommon::randomizeStart();

	// initializing experiment's parameters
	initStreams();
	initVariables();
	initTrial();

	return 0;
#else
	mathcommon::randomizeStart();

	// initializes optotrak and velmex motors
	initOptotrak();

	initMotors();
	//reset_phidgets_linear();
	rotTable = stepper_connect();

	// initializing glut
	glutInit(&argc, argv);
	#if SIMULATION
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutCreateWindow("Simulation test");
	#else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
		glutGameModeString(GAME_MODE_STRING);
		glutEnterGameMode();
		glutFullScreen();
	#endif

	// initializing experiment's parameters
	initRendering();
	initStreams();
	initVariables();
//	renderTexture();

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

	stepper_rotate(rotTable, 0.0);
	stepper_close(rotTable);
	cleanup();

	return 0;
#endif
}

//############################################################
//############################################################
/****************** FIXED FUNCTIONS  ************************/
//############################################################
//############################################################

void updateTheMarkers()
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
}

void cleanup()
{
	// Stop the optotrak
	optotrak.stopCollection();
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
