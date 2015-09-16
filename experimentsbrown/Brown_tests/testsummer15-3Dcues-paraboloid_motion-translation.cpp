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
#include "TrapezoidPointsStimulus.h"

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

#include "Optotrak2.h"
#include "Marker.h"
#include "BrownMotorFunctions.h"
using namespace BrownMotorFunctions;

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
/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;
Timer globalTimer;

// stimuli
StimulusDrawer stereocheckDrawer[4], fixCrossDrawer[2];
BoxNoiseStimulus stereocheck[4], fixCross[2];

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

/********** EYES AND MARKERS **********************/

Vector3d eyeLeft(0,0,0), eyeRight(0,0,0);

vector <Marker> markers;
static double interoculardistance=0.0;

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
double objdepth = 0.0, objdistance = 0.0, objheight = 0.0;
bool isStimulusDrawn = false;
int frameN = 0;
string subjectName = "junk";
double trial_duration = 0.0, trial_time = 0.0;
int object = 0;
double depth_pedestal = 0.0, depth_stimulus = 0.0;
string first_depth_cue = "empty", second_depth_cue = "empty";
string first_object = "empty", second_object = "empty";
string depth_cue_pedestal = "empty", depth_cue_stimulus = "empty";
bool isTrialDone = false;
int responsekey = 0;
double motion = 0.0, motion_step = 0.0, motion_theta = 0.0;
int direction_motion = 1;
string current_cue = "empty", current_object = "empty";
double IOD = 0.0;
bool inPause = false, checkglasses = true;
int textureCounter = 0;
bool haveBothObjectsTexture = false, hasOneObjectTexture = false;
string cues_object1 = "empty", cues_object2 = "empty";
double current_depth = 0.0;
double stereocheckZ[4] = {25.0, 12.5, -12.5, -25.0};
double objtheta = 0.0, objdensity = 0.0;

/********** STREAMS **************/
ofstream responseFile;

/********** FUNCTION PROTOTYPES *****/

// output files and streams
void initStreams();

// sounds
void beepOk(int tone);

// info
void drawInfo();

// stimuli
void build_stereocheck();
void drawParaboloid(double size, double deltaz);

void draw_stereocheck();
void draw_fixation_cross();
void draw_all(string obj);
void drawStimulus();
void drawGLScene();

// trial
void initVariables();
void initTrial();
void advanceTrial(bool response);

// keypresses
void handleKeypress(unsigned char key, int x, int y);

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

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/3Dcues-paraboloid/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/3Dcues-paraboloid/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_3Dcues-paraboloid_perc.txt";

// response file name
string responseFile_name = "summer15-3Dcues-paraboloid_perc_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjheight\tobjtheta\tdepth_pedestal\tdepth_stimulus\tfirst_object\tsecond_object\tdepth_cue_pedestal\tdepth_cue_stimulus\tstairID\tstairState\tstairReversals\tstairAscending\tstairStepsDone\tresponsekey\tresp\tDepthCue\ttrialDuration";

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
		text.draw("####### summer15-SMT-pyramid #######");
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

		glColor3fv(glWhite); 
		text.draw(" " );
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw(" " );
		text.draw("current depth: " +stringify<double>(current_depth));
		text.draw(" " );
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[0]) + " " + stringify<double>(stereocheckZ[1]));
		text.draw("stereocheck: " + stringify<double>(stereocheckZ[2]) + " " + stringify<double>(stereocheckZ[3]));

/*
		if(depth_cue_pedestal == "stereo")
		{
			text.draw("Moving: = " +stringify<double> (depth_stimulus));
			text.draw("Stereo: = " +stringify<double> (depth_pedestal));
		} else
		{
			text.draw("Moving: = " +stringify<double> (depth_pedestal));
			text.draw("Stereo: = " +stringify<double> (depth_stimulus));
		}
*/
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

void draw_fixation_cross()
{
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

#define DEG2RAD 3.14159/180.0
void drawParaboloid(double size, double deltaz)
{
	glColor3fv(glRed);

	double step = size * objdensity;
	double C = deltaz / pow(size/3, 2);

	glBegin(GL_LINE_LOOP);
	for(double y=-size+motion; y<size+motion; y+=step)
	{
		for(double x=-size; x<size; x+=step)
		{
			double Z = C / 2.0 * (pow(x, 2) + pow(y, 2));
			glVertex3f(x, y, -Z+(deltaz/2));
		}
	}
	glEnd();

	// draw a mask
	glColor3fv(glBlack);
	glBegin(GL_QUADS);
	glVertex3f(-size*8, -size*8, -deltaz/2);
	glVertex3f(-size*8, size*8, -deltaz/2);
	glVertex3f(size*8, size*8, -deltaz/2);
	glVertex3f(size*8, -size*8, -deltaz/2);
	glEnd();

/*
	// this is a check
	glColor3fv(glWhite);
	glBegin(GL_QUADS);
	glVertex3f(-size/2, -size/2, -deltaz);
	glVertex3f(-size/2, size/2, -deltaz);
	glVertex3f(size/2, size/2, -deltaz);
	glVertex3f(size/2, -size/2, -deltaz);
	glEnd();
*/
}

void draw_all(string obj)
{
	if (obj == "pedestal")
	{
		current_cue = depth_cue_pedestal;
		current_depth = depth_pedestal;
	} else
	{
		current_cue = depth_cue_stimulus;
		current_depth = depth_stimulus;
	}

	if (current_cue.find('s') != std::string::npos)
		IOD = interoculardistance;
	else
		IOD = 0.0;

	if (current_cue.find('m') != std::string::npos)
		motion += motion_step*direction_motion;
	else
		motion = 0.0;

	glRotated(objtheta, 0, 0, 1);

	if (current_cue.find('t') != std::string::npos)
//		draw_rampwings_texture(current_depth, current_rampLength, current_rampTheta);
		cerr << "texture" << endl;
	else
		drawParaboloid(objheight, current_depth);
}

void drawStimulus()
{
	if(checkglasses)
	{
		IOD = interoculardistance;
		draw_stereocheck();
	} else if ( isStimulusDrawn )
	{
		glLoadIdentity();
		glTranslated(0, 0, objdistance);
//		glRotated(90, 0, 1, 0);

		if (timer.getElapsedTimeInMilliSec() < 200)
			draw_fixation_cross();

		if (timer.getElapsedTimeInMilliSec() > 200 && timer.getElapsedTimeInMilliSec() < trial_time+200)
		{
			current_object = first_object;
			draw_all(current_object);
		} 

		if (timer.getElapsedTimeInMilliSec() > (500 + trial_time + 200) && timer.getElapsedTimeInMilliSec() < (500 + (2*trial_time) + 200))
		{
			object = 2;
			current_object = second_object;
			draw_all(current_object);
		} 

		if(object == 2 && timer.getElapsedTimeInMilliSec() > (500 + (2*trial_time) + 200))
		{
			isStimulusDrawn = false;
			isTrialDone = true;
		}
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
	objheight = str2num<double>(parameters.find("objheight"));
	objtheta = str2num<double>(parameters.find("objtheta"));
	objdensity = str2num<double>(parameters.find("objdensity"));
	cues_object1 = parameters.find("cues_object1");
	cues_object2 = parameters.find("cues_object2");

	// is there at least one object with texture?
	hasOneObjectTexture = cues_object1.find('t') != std::string::npos || cues_object2.find('t') != std::string::npos;	
	// do both objects have texture?
	haveBothObjectsTexture = cues_object1.find('t') != std::string::npos && cues_object2.find('t') != std::string::npos;
}

void initTrial()
{
	// reset counters
	frameN = 0;
	object = 1;
	isTrialDone = false;
	motion = 0.0;

	if(checkglasses)
		build_stereocheck();

	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// assign the depth cues to pedestal and stimulus
	if(trial.getCurrent().first["DepthCue"] == 1)
	{
		depth_cue_pedestal = cues_object1;
		depth_cue_stimulus = cues_object2;
	}
	else
	{
		depth_cue_pedestal = cues_object2;
		depth_cue_stimulus = cues_object1;
	}

	// draw an empty screen
	isStimulusDrawn=false;

	// randomly assign the order of the objects
	if(unifRand(-1.0, 1.0) > 0.0)
	{
		first_object = "pedestal";
		second_object = "stimulus";
		direction_motion = 1.0;
	} else 
	{
		first_object = "stimulus";
		second_object = "pedestal";
		direction_motion = - 1.0;
	}

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

//		bool decision = ((depth_stimulus + JND) > (depth_pedestal + PSE) && first_object == "stimulus") || (depth_stimulus + JND) > (depth_pedestal + PSE) && second_object == "stimulus";

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

		objdistance << "\t" <<
		objheight << "\t" <<
		objtheta << "\t" <<

		depth_pedestal << "\t" << 
		depth_stimulus << "\t" << 
		first_object << "\t" << 
		second_object << "\t" << 
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
		{
			checkglasses = !checkglasses;
			random_shuffle(&stereocheckZ[0], &stereocheckZ[4]);
		} break;

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
							if(first_object == "stimulus")
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
							if(second_object == "stimulus")
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
					if(first_object == "stimulus")
						advanceTrial(true);
					else
						advanceTrial(false);
				}

				if(responsekey == 6)
				{
					if(second_object == "stimulus")
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

	// motion
	if(motion > motion_theta || motion < -motion_theta)
			direction_motion = direction_motion * -1;

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
