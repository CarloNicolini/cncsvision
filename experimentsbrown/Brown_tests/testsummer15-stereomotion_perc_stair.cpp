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

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

#define SIMULATION
//#define TEST

#ifdef TEST
	#include <boost/random/mersenne_twister.hpp>
	#include <boost/random/normal_distribution.hpp>
	#include <boost/random/variate_generator.hpp>
#endif

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
#ifdef SIMULATION
Optotrak2Sim *optotrak;
#else
Optotrak2 *optotrak;
#endif

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// stimuli
StimulusDrawer stepsFlatDrawer[4], stereocheckDrawer[4], rodMiddleDrawer, fixCrossDrawer[2];
BoxNoiseStimulus stepsFlat[4], stereocheck[4], rodMiddle, fixCross[2];

/********** EYES AND MARKERS **********************/

Vector3d eyeLeft(0,0,0), eyeRight(0,0,0);

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

#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif

bool visibleInfo=true;
bool expFinished=false;
bool experiment=false, training=true;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
TrialGenerator<double> trial;
double objwidth = 0.0, objdepth = 0.0, objdistance = 0.0;
bool isStimulusDrawn = false;
int frameN = 0;
string subjectName = "junk";
double trial_duration = 0.0, trial_time = 0.0;
int stimulus = 0;
double depth_pedestal = 0.0, depth_stimulus = 0.0;
string first_depth_cue = "empty", second_depth_cue = "empty";
string first_stimulus = "empty", second_stimulus = "empty";
string depth_cue_pedestal = "empty", depth_cue_stimulus = "empty";
bool isTrialDone = false;
int responsekey = 0;
double motion = 0.0, motion_step = 0.0, motion_theta = 0.0;
int direction_motion = 1;
string current_cue = "empty", current_stimulus = "empty";
double IOD = 0.0, IOD_motion = 0.0;
bool inPause = false, checkglasses = true;
double oblPlane_pedestal_length = 0.0, oblPlane_stimulus_length = 0.0;
double oblTheta_pedestal = 0.0, oblTheta_stimulus = 0.0;
double obl_length = 0.0;
int motion_mono = 0;

/********** STREAMS **************/
ofstream responseFile;

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
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial(bool response);
void initTrial();
void draw_stairway_stereo();
void draw_stairway_motion_mono();
void build_stereocheck();
void draw_stereocheck();
void build_stairway();
void build_fixation_cross();
void draw_fixation_cross();
void draw_stairway(double deltaz);

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/stereomotion_stair/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/stereomotion_stair/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_stereomotion_perc_stair.txt";

// response file name
string responseFile_name = "summer15-stereomotion_perc_stair_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tmotion_mono\tAbsDepth\tdepth_pedestal\tdepth_stimulus\tfirst_stimulus\tsecond_stimulus\tdepth_cue_pedestal\tdepth_cue_stimulus\tstairID\tstairState\tstairReversals\tstairAscending\tstairStepsDone\tresponsekey\tresp\tDepthCue\ttrialDuration";

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

		glColor3fv(glWhite);
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
		text.draw(" " );
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw(" " );
		if(depth_cue_pedestal == "stereo")
		{
			text.draw("Moving: = " +stringify<double> (depth_stimulus));
			text.draw("Stereo: = " +stringify<double> (depth_pedestal));
		} else
		{
			text.draw("Moving: = " +stringify<double> (depth_pedestal));
			text.draw("Stereo: = " +stringify<double> (depth_stimulus));
		}

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
void draw_stairway(double deltaz)
{
	// Leftmost
	glPushMatrix();
	glTranslated(-30.0, 0, deltaz/2);
	stepsFlatDrawer[0].draw();
	glPopMatrix();

	// Center Left
	glPushMatrix();
	glTranslated(-10.0, 0, deltaz/6);
	stepsFlatDrawer[1].draw();
	glPopMatrix();

	// Center Right
	glPushMatrix();
	glTranslated(10.0, 0, -deltaz/6);
	stepsFlatDrawer[2].draw();
	glPopMatrix();

	// Center Right
	glPushMatrix();
	glTranslated(30.0, 0, -deltaz/2);
	stepsFlatDrawer[3].draw();
	glPopMatrix();
}

void draw_stairway_stereo(double deltaz)
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);
//	glRotated(90, 1, 0, 0);

	draw_stairway(deltaz);
}

void draw_stairway_motion_mono(double deltaz)
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);
	glRotated(motion, 1, 0, 0);
	//glRotated(90, 1, 0, 0);

	draw_stairway(deltaz);
}

void draw_stereocheck()
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);
//	glRotated(90, 1, 0, 0);

	// Upper left
	glPushMatrix();
	glTranslated(-25.0, 25.0, 25.0);
	stereocheckDrawer[0].draw();
	glPopMatrix();

	// Upper right
	glPushMatrix();
	glTranslated(25.0, 25.0, 12.5);
	stereocheckDrawer[1].draw();
	glPopMatrix();

	// Lower left
	glPushMatrix();
	glTranslated(-25.0, -25.0, -12.5);
	stereocheckDrawer[2].draw();
	glPopMatrix();

	// Lower right
	glPushMatrix();
	glTranslated(25.0, -25.0, -25.0);
	stereocheckDrawer[3].draw();
	glPopMatrix();
}

void draw_fixation_cross()
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);

	// Horizontal
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	fixCrossDrawer[0].draw();
	glPopMatrix();

	// Horizontal
	glPushMatrix();
	fixCrossDrawer[1].draw();
	glPopMatrix();
}

void drawStimulus()
{
	if(checkglasses)
	{
		IOD = interoculardistance;
		draw_stereocheck();
	} else if ( isStimulusDrawn )
	{
		if (timer.getElapsedTimeInMilliSec() < 200)
			draw_fixation_cross();

		if (timer.getElapsedTimeInMilliSec() > 200 && timer.getElapsedTimeInMilliSec() < trial_time+200)
		{
			current_stimulus = first_stimulus;

			if (first_stimulus == "pedestal")
			{
				current_cue = depth_cue_pedestal;

				if (depth_cue_pedestal == "stereo")
				{
					IOD = interoculardistance;
					draw_stairway_stereo(depth_pedestal);
				}
				else
				{
					IOD = IOD_motion;
					draw_stairway_motion_mono(depth_pedestal);
				}

			} else
			{
				current_cue = depth_cue_stimulus;

				if (depth_cue_stimulus == "stereo")
				{
					IOD = interoculardistance;
					draw_stairway_stereo(depth_stimulus);
				}
				else
				{
					IOD = IOD_motion;
					draw_stairway_motion_mono(depth_stimulus);		
				}
			}
		} 

		if (timer.getElapsedTimeInMilliSec() > (500 + trial_time + 200) && timer.getElapsedTimeInMilliSec() < (500 + (2*trial_time) + 200))
		{
			stimulus = 2;
			current_stimulus = second_stimulus;

			if (second_stimulus == "pedestal")
			{
				current_cue = depth_cue_pedestal;

				if (depth_cue_pedestal == "stereo")
				{
					IOD = interoculardistance;
					draw_stairway_stereo(depth_pedestal);
				}
				else
				{
					IOD = IOD_motion;
					draw_stairway_motion_mono(depth_pedestal);
				}

			} else 
			{
				current_cue = depth_cue_stimulus;

				if (depth_cue_stimulus == "stereo")
				{
					IOD = interoculardistance;
					draw_stairway_stereo(depth_stimulus);
				}
				else
				{
					IOD = IOD_motion;
					draw_stairway_motion_mono(depth_stimulus);		
				}
			}		
		} 

		if(stimulus == 2 && timer.getElapsedTimeInMilliSec() > (500 + (2*trial_time) + 200))
		{
			isStimulusDrawn = false;
			isTrialDone = true;
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

void build_stairway()
{
	for ( int i = 0; i < 4; i++ ) // frontoparallel planes
	{
		stepsFlat[i].setDimensions(20.0, 50.0, 0.1);;
		stepsFlat[i].setNpoints(20.0*50.0*0.225);
		stepsFlat[i].compute();

		stepsFlatDrawer[i].setStimulus(&stepsFlat[i]);
		stepsFlatDrawer[i].setSpheres(true);
		stepsFlatDrawer[i].initList(&stepsFlat[i], glRed);
	}
}

void build_fixation_cross()
{
	for ( int i = 0; i < 2; i++ ) // frontoparallel planes
	{
		fixCross[i].setDimensions(2.0, 15.0, 0.1);;
		fixCross[i].setNpoints(100);
		fixCross[i].compute();

		fixCrossDrawer[i].setStimulus(&fixCross[i]);
		fixCrossDrawer[i].setSpheres(true);
		fixCrossDrawer[i].initList(&fixCross[i], glWhite);
	}
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
	trial_duration = str2num<double>(parameters.find("trial_duration"));
	motion_step = str2num<double>(parameters.find("motion_step"));
	motion_theta = str2num<double>(parameters.find("motion_theta"));
	obl_length = str2num<double>(parameters.find("obl_length"));
	motion_mono = str2num<int>(parameters.find("motion_mono"));
	
	if(motion_mono == 1)
		IOD_motion = 0.0;
	else
		IOD_motion = interoculardistance;
}

void initTrial()
{
	// reset counters
	frameN = 0;
	stimulus = 1;
	isTrialDone = false;
	motion = 0.0;

	if(checkglasses)
		build_stereocheck();

	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// set the width of the object to 20mm
	objwidth = 20.0;

	// retrieve the depth cue
	if(trial.getCurrent().first["DepthCue"] == 1)
	{
		depth_cue_pedestal = "stereo";
		depth_cue_stimulus = "motion_mono";
	}
	else
	{
		depth_cue_pedestal = "motion_mono";
		depth_cue_stimulus = "stereo";
	}
	// draw an empty screen
	isStimulusDrawn=false;

	// assign the stimuli
	if(unifRand(-1.0, 1.0) > 0.0)
	{
		first_stimulus = "pedestal";
		second_stimulus = "stimulus";
	} else 
	{
		first_stimulus = "stimulus";
		second_stimulus = "pedestal";
	}
#ifdef TEST
		double PSE = str2num<double>(parameters.find("PSE"));
		double JND = unifRand(-str2num<double>(parameters.find("JND")), str2num<double>(parameters.find("JND")));

		cerr << "PSE = " << PSE << endl;
		cerr << "JND = " << JND << endl;

		// set the depth of the pedestal
		depth_pedestal = trial.getCurrent().first["RelDepthObj"];

		// set the depth of the stimulus
		depth_stimulus = depth_pedestal + trial.getCurrent().second->getCurrentStaircase()->getState();

		objdistance = trial.getCurrent().first["AbsDepth"];

//		bool decision = ((depth_stimulus + JND) > (depth_pedestal + PSE) && first_stimulus == "stimulus") || (depth_stimulus + JND) > (depth_pedestal + PSE) && second_stimulus == "stimulus";

		bool decision = ((depth_stimulus + JND) > (depth_pedestal + PSE) || (depth_stimulus + JND) > (depth_pedestal + PSE));

		advanceTrial(decision);
#else
	if(training)
	{	
		trial_time = 2500;

		// set the depth of the pedestal
		depth_pedestal = 40.0;

		// set the depth of the stimulus to be either clearly smaller or clearly larger than the pedestal
		if(unifRand(-1.0, 1.0) > 0.0)
			depth_stimulus = 20.0;
		else
			depth_stimulus = 60.0;

		objdistance = -360;
	}

	if(experiment)
	{
		trial_time = trial_duration;

		// set the depth of the pedestal
		depth_pedestal = trial.getCurrent().first["RelDepthObj"];

		// set the depth of the stimulus
		depth_stimulus = depth_pedestal + trial.getCurrent().second->getCurrentStaircase()->getState();

		objdistance = trial.getCurrent().first["AbsDepth"];
	}

	// build fixation cross
	build_fixation_cross();
	
	// build the stairway
	build_stairway();

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(objdistance);

	// draw the stimulus	
	beepOk(0);
	isStimulusDrawn=true;

	timer.start();
#endif
}

void advanceTrial(bool response)
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
		parameters.find("SubjectName") << "\t" <<
		interoculardistance << "\t" <<
		trialNumber << "\t" <<
		motion_mono << "\t" <<
		objdistance << "\t" << 
		depth_pedestal << "\t" << 
		depth_stimulus << "\t" << 
		first_stimulus << "\t" << 
		second_stimulus << "\t" << 
		depth_cue_pedestal << "\t" << 
		depth_cue_stimulus << "\t" << 

		trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" << 
		trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getAscending()<< "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<

		responsekey << "\t" <<
		response << "\t" <<

		trial.getCurrent().first["DepthCue"] << "\t" <<
		timeElapsed 
		<< endl;

	trial.next(response);
	trialNumber++;

	if(!trial.isEmpty())
	{		
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
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		case 13: // press enter to begin the training
			initTrial();
			break;

		case '+': // toggle checkglasses
			checkglasses = !checkglasses;
			break;

		case 'e':
		{
			if(training)
			{
				visibleInfo = false;
				training = false;
				experiment = true;
				beepOk(0);
				initTrial();
			}
		} break;

		case '4': // stimulus shallower than pedestal
		{
			if(isTrialDone)
				{
					if(training)
						initTrial();
					else
					{
						if((1 + trialNumber) % 90 != 0)
						{
							responsekey=4;
							if(first_stimulus == "stimulus")
								advanceTrial(true);
							else
								advanceTrial(false);
						} else
						{
							responsekey=4;
							inPause = true;
						}
					}
				}
		} break;

		case '6': // stimulus deeper than pedestal
		{
			if(isTrialDone)
				{
					if(training)
						initTrial();
					else
					{
						if((1 + trialNumber) % 90 != 0)
						{
							responsekey=6;
							if(second_stimulus == "stimulus")
								advanceTrial(true);
							else
								advanceTrial(false);
						} else
						{
							responsekey=6;
							inPause = true;
						}
					}
				}
		} break;

		case ' ':
		{
			if(inPause)
			{
				if(responsekey == 4)
				{
					if(first_stimulus == "stimulus")
						advanceTrial(true);
					else
						advanceTrial(false);
				}

				if(responsekey == 6)
				{
					if(second_stimulus == "stimulus")
						advanceTrial(true);
					else
						advanceTrial(false);
				}
				inPause = false;
			}
		} break;
	}
}

/*** IDLE ***/
void idle()
{

	updateTheMarkers();

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

	// frames counting
	frameN++;
	
	if(training) // in training mode the object rocks with a wider angle
	{
		if(motion > motion_theta*2.5 || motion < -motion_theta*2.5)
			direction_motion = direction_motion * -1;
	} else // in experiment mode the object rocks with the correct angle of rotation
	{
		if(motion > motion_theta || motion < -motion_theta)
			direction_motion = direction_motion * -1;
	}

	motion += motion_step*direction_motion;

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
#ifndef SIMULATION
    optotrak=new Optotrak2();
#else
    optotrak=new Optotrak2Sim();
#endif
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
