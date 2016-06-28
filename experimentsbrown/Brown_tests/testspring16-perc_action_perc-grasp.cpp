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
#include "CylinderPointsStimulus.h"

// drawInfo
#include "GLText.h"

// trial matrix
#include "TrialGenerator.h"
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
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords, indexCoords, upperPin, lowerPin;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// stimuli
StimulusDrawer stereocheckDrawer[4], cylinderDrawer[3];
BoxNoiseStimulus stereocheck[4];
CylinderPointsStimulus cylinder[4];

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
TrialGenerator<double> perc_pre, perc_post;
BalanceFactor<double> grasp;

bool visibleInfo=true;
bool expFinished=false;
bool experiment=false, training=true;
bool isStimulusDrawn = false;
bool inPause = false, checkglasses = true;
bool iGrasped = false, isHandHome = true;
bool isIndexDrawn=true;
bool fingers_calibrated = false;
bool first_distance = true;
bool cube = true;
bool draw_the_fingers = true;

int trialNumber = 0;
int responsekey = 0;
int frameN = 0;
int which_time = 0;
int current_ID = 0, current_ascending = 0, current_stepsDone = 0, current_reversals = 0;
int frameToGrasp = 0, frames_at_start = 0, frames_post_grasp = 0;
int fingersOccluded = 0, framesOccluded = 0, good_trial = 0;

double objdistance = 0.0, objdistance_sequence[2] = {0.0, 0.0};
double trial_duration = 0.0, trial_time = 0.0;
double current_depth = 0.0;
double stereocheckZ[4] = {25.0, 12.5, -12.5, -25.0};
double timeForGrasping = 15000.0;
double dist_to_home = 0.0;
double pointing_motion = 0.0;
double previous_distance = 0.0;

string subjectName = "junk";
string block = "perc_pre_block";
string visual_feedback = "empty";

Vector3d object_reset_position(0,0,0), object_position(0,0,0), object_current_position(0,0,0);

/********** STREAMS **************/
ofstream perc_responseFile, grasp_responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/

// output files and streams
void initStreams();

// sounds
void beepOk(int tone);

// info
void drawInfo();

// grasp
void calibration_fingers(int phase);
void drawFingers(bool draw);

// stimuli
void build_stereocheck();
void build_three_cylinders();
void draw_three_cylinders(double deltaz);
void draw_stereocheck();
void draw_fixation_rod();
void fingers_not_visible();

void draw_all();
void drawStimulus();
void drawGLScene();

// trial
int training_time(int counter);
void initVariables();
void initTrial();
void advanceTrial(bool response);

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
void initial_checkglasses();

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2016/perc_action_perc/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2016/perc_action_perc/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_perc_action_perc-grasp.txt";

// response file name
string perc_responseFile_name = "spring16-percactperc_perc_";

// response file headers
string perc_responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tfirst_distance\tstairID\tRelDepthObj\tstairReversals\tstairAscending\tstairStepsDone\tresponsekey\tresp\tblock\ttrialDuration";

// response file name
string grasp_responseFile_name = "spring16-percactperc_grasp_";

// response file headers
string grasp_responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tRelDepthObj\ttrialDuration\tvisual_feedback\tgood_trial\ttraining";

// markers file name
string markersFile_name = "spring16-percactperc_";

// markers file headers
string markersFile_headers = "subjName\tIOD\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\tfingersOccluded\tframesOccluded\tAbsDepth\tRelDepthObj\tframesToGrasp";

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

	//---- perc response file
	string perc_responseFileName = experiment_directory + perc_responseFile_name + subjectName + ".txt";

	// Check for output file existence
	if (util::fileExists(perc_responseFileName))
	{
		string error_on_file_io = perc_responseFileName + string(" already exists");
		cerr << error_on_file_io << endl;
#ifndef SIMULATION
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
#endif
		exit(0);
	}
	else
	{
		perc_responseFile.open(perc_responseFileName.c_str());
		cerr << "File " << perc_responseFileName << " loaded successfully" << endl;
	}

	perc_responseFile << fixed << perc_responseFile_headers << endl;

	//---- grasp response file
	string grasp_responseFileName = experiment_directory + grasp_responseFile_name + subjectName + ".txt";

	// Check for output file existence
	if (util::fileExists(grasp_responseFileName))
	{
		string error_on_file_io = grasp_responseFileName + string(" already exists");
		cerr << error_on_file_io << endl;
#ifndef SIMULATION
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
#endif
		exit(0);
	}
	else
	{
		grasp_responseFile.open(grasp_responseFileName.c_str());
		cerr << "File " << grasp_responseFileName << " loaded successfully" << endl;
	}

	grasp_responseFile << fixed << grasp_responseFile_headers << endl;
}

/***** SOUNDS *****/
void beepOk(int tone)
{
#ifndef SIMULATION
	switch(tone)
	{
		case 0:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 1:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 2:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\calibrate.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 3:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-home.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 4:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\completed.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 5:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\keyboard.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
		case 6:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\spoken-grasp.wav", NULL, SND_FILENAME | SND_ASYNC);
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

		if ( isVisible(markers.at(phidgets1).p.transpose()) && isVisible(markers.at(phidgets2).p.transpose()) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Phidgets marker 1" + stringify< Eigen::Matrix<double,1,3> > (markers.at(phidgets1).p.transpose()));
		text.draw("Phidgets marker 2" + stringify< Eigen::Matrix<double,1,3> > (markers.at(phidgets2).p.transpose()));

		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("Index = " +stringify< Eigen::Matrix<double,1,3> > (ind.transpose()));
		text.draw("Thumb = " +stringify< Eigen::Matrix<double,1,3> > (thu.transpose()));
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw("time for grasping = " +stringify<int> (timeForGrasping));
		text.draw(" " );
		
		text.draw(" " );
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[0]) + " " + stringify<double>(stereocheckZ[1]));
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[2]) + " " + stringify<double>(stereocheckZ[3]));

		text.draw("current_depth: " +stringify<double>(current_depth));
		text.draw("objdistance: " +stringify<double>(objdistance));
		text.draw("iGrasped: " +stringify<int>(iGrasped));
		text.draw("isHandHome: " +stringify<int>(isHandHome));
		text.draw("block: " + block);
		text.draw("stereocheck: " + stringify<int>(checkglasses));
		text.draw("obj offset = " +stringify<double> (abs(object_current_position.z() - (double)objdistance)));

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

	if ( inPause )
	{
		GLText text3;	
		text3.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
		text3.enterTextInputMode();
		text3.draw("P A U S E");
		text3.leaveTextInputMode();
	}
}

/*** STIMULI ***/
void build_three_cylinders()
{
	// Initilaizes the three cylinders
	for (int i=0; i<3; i++)
	{
		cylinder[i].setNpoints(150);
		cylinder[i].setRadiusAndHeight(2.0,50.0);

		cylinder[i].compute();
		cylinderDrawer[i].setStimulus(&cylinder[i]);

		cylinderDrawer[i].setSpheres(true);
		cylinderDrawer[i].initList(&cylinder[i], glRed);
	}
}

void draw_three_cylinders(double deltaz)
{
	// Left rear 
	glPushMatrix();
	glTranslated(-20.0, 0, -deltaz);
	cylinderDrawer[0].draw();
	glPopMatrix();

	// Right rear rod
	glPushMatrix();
	glTranslated(20.0, 0, -deltaz);
	cylinderDrawer[1].draw();
	glPopMatrix();

	// Front rod
	glPushMatrix();
	glTranslated(0, 0, 0);
	cylinderDrawer[2].draw();
	glPopMatrix();
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

void initial_checkglasses()
{
	build_stereocheck();
	objdistance = -375.0;
	initProjectionScreen(objdistance);
	previous_distance = objdistance;
	drawGLScene();
}

void draw_fixation_rod()
{
		// upper left 
		glColor3fv(glRed);
		glBegin(GL_QUADS);
		glVertex3f(-0.3, 0, 0);
		glVertex3f(-0.3, 4.0, 0);
		glVertex3f(0.3, 4.0, 0);
		glVertex3f(0.3, 0, 0);
		glEnd();

		// horizontal left
		glBegin(GL_QUADS);
		glVertex3f(0, -0.3, 0);
		glVertex3f(-4.0, -0.3, 0);
		glVertex3f(-4.0, 0.3, 0);
		glVertex3f(0, 0.3, 0);
		glEnd();

		// lower right
		glColor3fv(glRed);
		glBegin(GL_QUADS);
		glVertex3f(0.3, 0, 0);
		glVertex3f(0.3, -4.0, 0);
		glVertex3f(-0.3, -4.0, 0);
		glVertex3f(-0.3, 0, 0);
		glEnd();

		// horizontal right
		glBegin(GL_QUADS);
		glVertex3f(0, -0.3, 0);
		glVertex3f(4.0, -0.3, 0);
		glVertex3f(4.0, 0.3, 0);
		glVertex3f(0, 0.3, 0);
		glEnd();
}

void drawCube()
{
		double edge = 10.0;

		glLoadIdentity();
		glTranslated(0.0, 0, objdistance);
		glTranslated(80.0, 0.0, 0.0);
		glRotated(pointing_motion, 0, 1, 0);

		if(abs(ind.x()-80.0) <= edge &&
			abs(ind.y()) <= edge &&
			abs(ind.z()-objdistance) <= edge &&
			abs(thu.x()-80.0) <= edge &&
			abs(thu.y()) <= edge &&
			abs(thu.z()-objdistance) <= edge)
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);

		//front surface
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,edge);
		glVertex3d(edge,-edge,edge);
		glVertex3d(-edge,-edge,edge);
		glVertex3d(-edge,edge,edge);
		glEnd();
		//upper surface
		glBegin(GL_LINE_LOOP);
		glVertex3d(-edge,edge,-edge);
		glVertex3d(edge,edge,-edge);
		glVertex3d(edge,edge,edge);
		glVertex3d(-edge,edge,edge);
		glEnd();
		//lower surface
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,-edge,-edge);
		glVertex3d(-edge,-edge,-edge);
		glVertex3d(-edge,-edge,edge);
		glVertex3d(edge,-edge,edge);
		glEnd();
		//back surface
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,-edge);
		glVertex3d(edge,-edge,-edge);
		glVertex3d(-edge,-edge,-edge);
		glVertex3d(-edge,edge,-edge);
		glEnd();
}

void draw_all()
{
	if(block == "grasp_block" && training && cube)
	{
		pointing_motion += 1.0;
		drawCube();
	}

	glLoadIdentity();
	glTranslated(0, 0, objdistance);
	//glRotated(90, 0, 1, 0);
	draw_three_cylinders(current_depth);
}

void fingers_not_visible()
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
	if(block == "grasp_block")
	{
		if(checkglasses && fingerCalibrationDone == 3)
		{
			glLoadIdentity();
			glTranslated(0, 0, objdistance);
			draw_stereocheck();
		} 

		if(fingerCalibrationDone==4)
		{
			if(!allVisibleFingers)
				fingers_not_visible();

			if(draw_the_fingers)
				drawFingers(isIndexDrawn);

			if ( isStimulusDrawn && !iGrasped )
			{
				draw_all();
			}

			if (iGrasped && isHandHome)
			{
				advanceTrial(true);
			}
		}
	} else
	{
		if(checkglasses)
		{
			draw_stereocheck();
		}
		else
		{
			if (isStimulusDrawn && !expFinished)
			{
				glLoadIdentity();
				glTranslated(0, 0, objdistance);
				if(timer.getElapsedTimeInMilliSec() < 200)
					draw_fixation_rod();

				if (timer.getElapsedTimeInMilliSec() > 200)
					draw_all();
			}
		}
	}
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
	}
		glPushMatrix();
		glLoadIdentity();
		glTranslated(thu.x(),thu.y(),thu.z());
		glColor3fv(glRed);
		glutSolidSphere(2.5,10,10);
		glPopMatrix();

}

/*** TRIAL ***/
void initVariables()
{
	perc_pre.init(parameters);
	grasp.init(parameters);
	grasp.addFactor("RelDepthObj", vlist_of<double>(30.0)(50.0));
	grasp.addFactor("AbsDepth", vlist_of<double>(-350.0)(-450.0));
	perc_post.init(parameters);

	string start_with = parameters.find("start_with");
	if(start_with == "g")
		block = "grasp_block";

	interoculardistance = str2num<double>(parameters.find("IOD"));
	trial_duration = str2num<double>(parameters.find("trial_duration"));
	visual_feedback = parameters.find("visual_feedback");

	if(parameters.find("distance") == "n")
	{
		objdistance_sequence[0] = -350.0;
		objdistance_sequence[1] = -450.0;
	}
	else if(parameters.find("distance") == "f")
	{
		objdistance_sequence[0] = -450.0;
		objdistance_sequence[1] = -350.0;
	}
}

void initTrial()
{
	// reset counters
	frameN = 0;
	frameToGrasp = 0;
	framesOccluded = 0;
	frames_post_grasp = 0;
	frames_at_start = 0;

	iGrasped = false;

	if(checkglasses)
		build_stereocheck();
	else
		build_three_cylinders();

	updateTheMarkers();

	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// draw an empty screen
	isStimulusDrawn=false;

#ifdef TEST
		double PSE = str2num<double>(parameters.find("PSE"));
		double JND = unifGauss(0, str2num<double>(parameters.find("JND")));

		cerr << "PSE = " << PSE << endl;
		cerr << "JND = " << JND << endl;

		// set the depth of the pedestal
		depth_pedestal = trial.getCurrent().first["RelDepthObj"];

		// set the depth of the stimulus
		depth_stimulus = depth_pedestal + trial.getCurrent().second->getCurrentStaircase()->getState();

		objdistance = trial.getCurrent().first["AbsDepth"];

		bool decision = ((depth_stimulus + JND) > (depth_pedestal + PSE) || (depth_stimulus + JND) > (depth_pedestal + PSE));

		advanceTrial(decision);
#else

	if(block == "grasp_block") // grasp task
	{
		// in training mode the current depth is either 20.0 or 40.0
		if(training)
		{	
			draw_the_fingers = true;

			if(unifRand(-1.0, 1.0) > 0)
				current_depth = 20.0;
			else
				current_depth = 40.0;

			objdistance = objdistance_sequence[trialNumber%2];
			
		} else if(experiment)
		{
			if(visual_feedback == "y")
				draw_the_fingers = true;
			else
				draw_the_fingers = false;

			// subject did not grasp yet
			timeForGrasping = trial_duration;
			
			current_depth = grasp.getCurrent()["RelDepthObj"];
			objdistance = grasp.getCurrent()["AbsDepth"];
		}
	
		// detect where the object is currently
		object_current_position = markers.at(phidgets1).p.transpose();
		// determine the next position of the object
		Vector3d object_next_position(0.0,object_current_position.y(),objdistance);
		// update the focal distance of the screen (required when moving the monitor - usually done in intiprojectionscreen)
		screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
		screen.setOffset(alignmentX,alignmentY);
		screen.setFocalDistance(objdistance);
		screen.transform(Affine3d::Identity());
		cam.init(screen);
		// move object and screen
		moveObjectAndScreen(object_next_position-object_current_position, Vector3d(objdistance, 0.0, 0.0)-Vector3d(previous_distance, 0.0, 0.0), 5000, 4500);
		// mode the phidgets
		phidgets_linear_move(current_depth-10.0, axisZ);

		// markers file
		string condition = "empty";
		if(training)
			condition = "training";
		else
			condition = "experiment";
		string markersFileName = experiment_directory + subjectName + "/" + markersFile_name + subjectName + "_markers-trial_" + stringify(trialNumber) + "_" + condition + ".txt";

		markersFile.open(markersFileName.c_str());
		markersFile << fixed << markersFile_headers << endl;
	}

	// what block are we in?
	if(block == "perc_pre_block") // first perceptual task
	{
		if(first_distance)
			objdistance = objdistance_sequence[0];
		else
			objdistance = objdistance_sequence[1];
		
		current_ID = perc_pre.getCurrent().second->getCurrentStaircase()->getID();
		current_depth = perc_pre.getCurrent().second->getCurrentStaircase()->getState();
		current_reversals = perc_pre.getCurrent().second->getCurrentStaircase()->getInversions();
		current_ascending = perc_pre.getCurrent().second->getCurrentStaircase()->getAscending();
		current_stepsDone = perc_pre.getCurrent().second->getCurrentStaircase()->getStepsDone();
	} else if(block == "perc_post_block") // second perceptual task
	{
		if(first_distance)
			objdistance = objdistance_sequence[0];
		else
			objdistance = objdistance_sequence[1];

		current_ID = perc_post.getCurrent().second->getCurrentStaircase()->getID();
		current_depth = perc_post.getCurrent().second->getCurrentStaircase()->getState();
		current_reversals = perc_post.getCurrent().second->getCurrentStaircase()->getInversions();
		current_ascending = perc_post.getCurrent().second->getCurrentStaircase()->getAscending();
		current_stepsDone = perc_post.getCurrent().second->getCurrentStaircase()->getStepsDone();
	}

	// refresh the scene
	drawGLScene();
	
	// set the monitor at the right ditance in perceptual tasks
	if(block != "grasp_block")
		initProjectionScreen(objdistance);
	// record the current distance as "previous distance" for the next trial
	previous_distance = objdistance;

	// draw the stimulus	
	if(block == "perc_post_block" && trialNumber == 0 && first_distance)
		beepOk(5);
	else
		beepOk(0);

	#ifdef SIMULATION
		usleep(3000);
	#else
		Sleep(300);
	#endif

	isStimulusDrawn=true;

	timer.start();
#endif
}

void advanceTrial(bool response)
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();

	if(block == "perc_pre_block" || block == "perc_post_block")
	{
		perc_responseFile.precision(3); // max three decimal positions

		perc_responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<

			objdistance << "\t" <<
			first_distance << "\t" <<

			current_ID << "\t" <<
			current_depth << "\t" <<
			current_reversals << "\t" <<
			current_ascending << "\t" <<
			current_stepsDone << "\t" <<

			responsekey << "\t" <<
			response << "\t" <<

			block << "\t" <<

			timeElapsed
			<< endl;

	} else if(block == "grasp_block")
	{
		double percent_occluded_frames = (double)framesOccluded/(frameN-frames_at_start-frames_post_grasp);
		bool not_enough_frames = percent_occluded_frames > 0.20;
	
		updateTheMarkers();

		if(experiment && not_enough_frames)
		{
			beepOk(1);
			grasp.reinsert(grasp.getCurrent());
			good_trial = 0;
		} else
		{
			good_trial = 1;
		}

		grasp_responseFile.precision(3);

		grasp_responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<

			objdistance << "\t" <<
			current_depth << "\t" <<

			timeElapsed << "\t" <<
			visual_feedback << "\t" <<
			good_trial << "\t" <<
			training
			<< endl;

		markersFile.close();
	}

	if(block == "perc_pre_block")
	{
		perc_pre.next(response);
		trialNumber++;

		if(!perc_pre.isEmpty())
			initTrial();
		else if(first_distance)
		{
			first_distance = false;
			objdistance = objdistance_sequence[1];
			perc_pre.init(parameters);
			trialNumber = 0;
			initTrial();
		} else
		{
			updateTheMarkers();
			// check where the object is
			object_reset_position = markers.at(phidgets1).p.transpose();
			visibleInfo = true;
			training = true;
			experiment = false;
			trialNumber = 0;
			block = "grasp_block";
			beepOk(6);
		}
	} else if(block == "grasp_block")
	{
		trialNumber++;
		if(experiment)
		{
			if(grasp.hasNext() || trialNumber == 0)
			{
				grasp.next();
				initTrial();
			} else
			{
				first_distance = true;
				trialNumber = 0;
				block = "perc_post_block";
				initTrial();
			}
		} else
			initTrial();
	} else if(block == "perc_post_block")
	{
		perc_post.next(response);
		trialNumber++;
		
		if(!perc_post.isEmpty())
			initTrial();
		else if(first_distance)
		{
			first_distance = false;
			perc_post.init(parameters);
			trialNumber = 0;
			initTrial();
		} else
		{	
			perc_responseFile.close();
			expFinished = true;
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

			if(block == "grasp_block")
			{
				case '0':
				{
					good_trial = 1;
					advanceTrial(true);
				} break;

				case '1':
					allVisibleFingers = !allVisibleFingers;
					break;

				case '2':
				{
					grasp.reinsert(grasp.getCurrent());
					good_trial = 0;
					advanceTrial(true);
				} break;

				case 'h':
				ind.z() -= 10;
				break;
				case 'j':
					ind.z() += 10;
				break;
			}

		#endif

		case 'i':
		case 'I':
			visibleInfo=!visibleInfo;
			break;

		case 27: // press escape to quit
		{   
			stepper_close(rotTable);
			cleanup(); // clean the optotrak buffer
			initMotors();
			exit(0);
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
				break;
			}

			// Start the experiment
			if ( fingerCalibrationDone==3  && (allVisibleFingers || !fingers_calibrated) && isVisible(markers.at(phidgets1).p) && isVisible(markers.at(phidgets2).p))
			{
				fingerCalibrationDone=4;
				beepOk(0);
				home_position = ind;
				visibleInfo=false;
				checkglasses = false;
				drawGLScene(); // this is needed otherwise the next motor command freezes the screen

				if(markers.at(phidgets2).p.x() - markers.at(phidgets1).p.x() > 0.0)
					stepper_rotate(rotTable, -(90.0-phidgetsAlignment));
				else
					stepper_rotate(rotTable, (90.0-phidgetsAlignment));
				CPhidgetStepper_setCurrentPosition(rotTable, 0, 0);

				if(unifRand(-5.0, 5.0) > 0)
					objdistance = objdistance_sequence[0];
				else
					objdistance = objdistance_sequence[1];
				training = true;
				initTrial();
			}

		} break;

		case ' ': // press enter to begin the training
		{
			if(checkglasses)
			{
				initVariables();
				checkglasses = false;
			}

			if(block == "perc_pre_block")
			{
				checkglasses = false;
				training = false;
				experiment = true;
				visibleInfo = false;
				initTrial();
				break;
			}
		}

		case 'e':
		case 'E':
		{
			if(fingerCalibrationDone == 4 && training && block == "grasp_block" && isVisible(markers.at(phidgets1).p))
			{
				visibleInfo = false;
				training = false;
				checkglasses = false;
				experiment = true;
				trialNumber = -1;
				beepOk(0);
				advanceTrial(true);
			}
		} break;

		case 's':
		{
			if(training && isVisible(markers.at(phidgets1).p))
				advanceTrial(true);;
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

		case '8': // stimulus deeper than equilateral
		{
			if(block == "perc_pre_block" || block == "perc_post_block")
			{
				responsekey=8;
				if((1 + trialNumber) % 90 != 0)
					advanceTrial(true);
				else
					inPause = true;
			}
		} break;

		case '5': // stimulus shallower than equilateral
		{
			if(block == "perc_pre_block" || block == "perc_post_block")
			{
				responsekey=5;
				if((1 + trialNumber) % 90 != 0)
					advanceTrial(false);
				else
					inPause = true;
			}
		} break;

		case 13:
		{
			if(inPause)
			{
				if(responsekey == 8)
					advanceTrial(true);

				if(responsekey == 5)
					advanceTrial(false);

				inPause = false;
			}
		} break;

		case 'c':
			cube = !cube;
		break;

		case 'v':
		{
			if(training && visual_feedback == "n" && fingerCalibrationDone == 4)
				draw_the_fingers = !draw_the_fingers;
		} break;
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
		if(block == "grasp_block")
		{
			fingerCalibrationDone=1;
			calibration_fingers(fingerCalibrationDone);
		}
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
	#endif
}

void online_trial()
{
	if(fingerCalibrationDone==4)
	{
		// fingersOccluded
		if ( allVisibleFingers )
			fingersOccluded = 0;
		else
		{
			fingersOccluded=1;
		}
		
		if(isStimulusDrawn && timer.getElapsedTimeInMilliSec() > timeForGrasping)
			iGrasped = true;

		if(block == "grasp_block" && isHandHome && !iGrasped)
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

		if(!isHandHome && !allVisibleFingers && !iGrasped)
			framesOccluded++;

		bool index_behind_the_stimulus = abs(ind.x()) < 20.0 && ind.z() < objdistance;
		isIndexDrawn = !(index_behind_the_stimulus);
	}
}

/*** IDLE ***/
void idle()
{

	updateTheMarkers();

	frameN++;

	// what the program checks online during the grasp
	if (fingerCalibrationDone==4 && block=="grasp_block" )
	{
		// Write to responseFile
		markersFile << fixed <<
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			timer.getElapsedTimeInMilliSec() << "\t" <<		//time
			frameN << "\t" <<								//frameN
			ind.transpose() << "\t" <<						//indexXraw, indexYraw, indexZraw
			thu.transpose() << "\t" <<						//thumbXraw, thumbYraw, thumbZraw
			fingersOccluded	<< "\t" <<						//fingersOccluded
			framesOccluded	<< "\t" <<						//framesOccluded
			objdistance	<< "\t" <<
			current_depth << "\t" <<
			frameToGrasp << "\t" <<
			endl;
	}
	
	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2.0,0,0);
	eyeLeft = Vector3d(-interoculardistance/2.0,0,0);
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
	reset_phidgets_linear();
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
	initial_checkglasses();

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

	//cleanup();

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
