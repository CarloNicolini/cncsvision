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

// drawInfo
#include "GLText.h"

// trial matrix
#include "TrialGenerator.h"
#include "BalanceFactor.h"

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
StimulusDrawer stereocheckDrawer[4], fixCrossDrawer[2], stimDrawer[3];
BoxNoiseStimulus stereocheck[4], fixCross[2];
CylinderPointsStimulus cylinder[3];

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
ParametersLoader parameters;
BalanceFactor<double> trial;

int trialNumber = 0;
int frameN = 0;
int object = 0;
int direction_motion = 1;
int textureCounter = 0;
int num_adjustments = 0;
int sizedepth = 0;
#define max_points 600

double objdepth = 0.0, objdistance = 0.0, objheight = 0.0, objwidth = 0.0;
double depth_pedestal = 0.0, depth_stimulus = 0.0;
double motion = 0.0, motion_step = 0.0, motion_theta = 0.0;
double IOD = 0.0;
double current_depth = 0.0;
double stereocheckZ[4] = {25.0, 12.5, -12.5, -25.0};
double objdensity = 0.0, theta_sign = 0.0;
double direction_motion2 = 0.0;
double shuffleZ = 0.0;
double x_coord_p[max_points], y_coord_p[max_points];
double x_coord_s[max_points], y_coord_s[max_points];
double z[max_points], v[max_points];
double current_x_coord[max_points], current_y_coord[max_points];
double pointsT = 0.3;
double jitterZ[2] = {0.0, 0.0}, jitterX = 0.0, current_jitterX = 0.0;
double current_jitterZ = 0.0;
double flip_theta[2] = {0.0, 0.0}, current_theta = 0.0;

bool isStimulusDrawn = false;
bool checkglasses = true;
bool switch_object = true;
bool confirmAnswer=false;
bool rodsFlip=false;

string subjectName = "junk";
string fixed_object = "empty", adjusted_object = "empty", which_object = "empty";
string depth_cue_pedestal = "empty", depth_cue_stimulus = "empty";
string current_cue = "s", current_object = "empty";
string cues_object1 = "empty", cues_object2 = "empty";
string obj_type = "empty";

GLfloat glDarkRed[3] = {0.5,0,0};
GLfloat glConfirm[3] = {0.3,0.3,0};

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
void drawWedge(double x[max_points], double y[max_points], double height, double deltaz);
void build_stereocheck();
void buildThreeRods();

void draw_stereocheck();
void draw_fixation_rod();
void drawThreeRods(double deltaz);
void draw_all(string obj);
void drawStimulus();
void drawGLScene();

// trial
void initVariables();
void initTrial();
void advanceTrial();
void iSleep(int millisec);

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
string parametersFile_directory = experiment_directory + "parameters_3Dcues-paraboloids_adjustment.txt";

// response file name
string responseFile_name = "fall15-3Dcues-paraboloid_adjustment_";

// summary file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjheight\tobjtheta\tdepth_pedestal\tdepth_stimulus\tinitialDz\tnum_adjustments\tfixed_object\tadjusted_object\tdepth_cue_pedestal\tdepth_cue_stimulus\tDepthCue\ttrialDuration";

/*************************** FUNCTIONS ***********************************/

/*** Output files and streams ***/
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	cues_object1 = parameters.find("cues_object1");

	// Subject name
	subjectName = parameters.find("SubjectName") + "_" + cues_object1;

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
		// Remember to put double slash \\ to specify directories!!!
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\completed.wav", NULL, SND_FILENAME | SND_ASYNC);
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
		text.draw("pointsT: " +stringify<double>(pointsT));
		text.draw(" " );
		text.draw("depth pedestal: " + stringify<double>(depth_pedestal));
		text.draw("depth stimulus: " + stringify<double>(depth_stimulus));
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
void drawParaboloid(double z[max_points], double v[max_points], double size, double deltaz)
{
	if(confirmAnswer)
		glColor3fv(glConfirm);
	else
		glColor3fv(glRed);

	for(int i = 0; i < max_points; i++)
	{
		double x = size*sqrt(z[i]/deltaz)*cos(v[i]);
		double y = size*sqrt(z[i]/deltaz)*sin(v[i]);

		glPushMatrix();
		glTranslated(x, y, -z[i]);
		glutSolidSphere(pointsT,10,10);
		glPopMatrix();
	}
}

void drawWedge(double x[max_points], double y[max_points], double height, double deltaz)
{
	for(int i = 0; i < max_points; i++)
	{
		if(confirmAnswer)
			glColor3fv(glConfirm);
		else if(y[i] > 0)
			glColor3fv(glRed);
		else
			glColor3fv(glDarkRed);

		double z_coord = - abs(deltaz/height*y[i]) + deltaz/2.0;
		glPushMatrix();
		glTranslated(x[i], y[i], z_coord);
		glutSolidSphere(pointsT,10,10);
		glPopMatrix();
	}
}

void drawFrontCar(double x[max_points], double y[max_points], double height, double deltaz)
{
	for(int i = 0; i < max_points; i++)
	{
		if(confirmAnswer)
			glColor3fv(glConfirm);
		else 
			glColor3fv(glRed);

		double z_coord = 0.0;
		if(y[i] < -0.33*height)
			z_coord = deltaz/2.0;
		else if(y[i] > 0.33*height)
			z_coord = -deltaz/2.0;
		else
			z_coord = - deltaz/2.0*y[i]/(height*0.33);
		//z_coord = z_coord - deltaz/2.0;
		glPushMatrix();
		glTranslated(x[i], y[i], z_coord);
		glutSolidSphere(pointsT,10,10);
		glPopMatrix();
	}
}

void buildThreeRods()
{
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

void drawThreeRods(double deltaz)
{
			// Left rear 
			glPushMatrix();
			glTranslated(-(current_jitterX + objwidth/2.0), 0, -deltaz);
			stimDrawer[0].draw();
			glPopMatrix();
/*
			// Right rear rod
			glPushMatrix();
			glTranslated(current_jitterX + objwidth/2, 0, -deltaz);
			stimDrawer[1].draw();
			glPopMatrix();

			// Front rod
			glPushMatrix();
			glTranslated(0, 0, 0);
			stimDrawer[2].draw();
			glPopMatrix();
			*/
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

void draw_fixation_rod()
{
	glLoadIdentity();
	glTranslated(0, 0, objdistance);

	if (which_object == "stimulus")
		glRotated(45, 0, 0, 1);

	// upper left 
	glColor3fv(glDarkRed);
	glBegin(GL_QUADS);
	glVertex3f(-0.3, 0, 0);
	glVertex3f(-0.3, 4.0, 0);
	glVertex3f(0.3, 4.0, 0);
	glVertex3f(0.3, 0, 0);
	glEnd();

	// horizontal left
	glColor3fv(glDarkRed);
	glBegin(GL_QUADS);
	glVertex3f(0, -0.3, 0);
	glVertex3f(-4.0, -0.3, 0);
	glVertex3f(-4.0, 0.3, 0);
	glVertex3f(0, 0.3, 0);
	glEnd();

	// lower right
	glColor3fv(glDarkRed);
	glBegin(GL_QUADS);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, -4.0, 0);
	glVertex3f(-0.3, -4.0, 0);
	glVertex3f(-0.3, 0, 0);
	glEnd();

	// horizontal right
	glColor3fv(glDarkRed);
	glBegin(GL_QUADS);
	glVertex3f(0, -0.3, 0);
	glVertex3f(4.0, -0.3, 0);
	glVertex3f(4.0, 0.3, 0);
	glVertex3f(0, 0.3, 0);
	glEnd();
}

void draw_all(string obj)
{	
	if (obj == "pedestal")
	{
		current_cue = depth_cue_pedestal;
		current_depth = depth_pedestal;
		theta_sign = 1.0;
		current_jitterZ = jitterZ[0];
		current_jitterX = jitterX;
		current_theta = flip_theta[0];
	} else
	{
		current_cue = depth_cue_stimulus;
		current_depth = depth_stimulus;
		theta_sign = -1.0;
		current_jitterZ = jitterZ[1];
		current_jitterX = 0.0;
		current_theta = flip_theta[1];
	}

	//draw_fixation_rod();

	glLoadIdentity();
	glTranslated(0, 0, objdistance);
	glTranslated(0,0, - current_jitterZ);
	glRotated(current_theta, 0, 0, 1);
	//glRotated(90, 1, 0, 0);

	//glRotated(objtheta * direction_motion2, 0, 0, 1);

	if (current_cue.find('s') != std::string::npos)
	{
		IOD = interoculardistance;
	}
	else
		IOD = 0.0;

	if (current_cue.find('m') != std::string::npos)
	{
		glRotated(motion, 0, 1, 0);
	}

	if (current_cue.find('t') != std::string::npos)
//		draw_rampwings_texture(current_depth, current_rampLength, current_rampTheta);
		cerr << "texture" << endl;
	else
	{
		if(obj == "pedestal")
		{
			//drawThreeRods(current_depth);
			//drawFrontCar(x_coord_p, y_coord_p, objheight, current_depth);
			drawParaboloid(z, v, depth_pedestal, current_depth);
		}
		else
		{
			//drawThreeRods(current_depth);
			//drawFrontCar(x_coord_s, y_coord_s, objheight, current_depth);
			drawParaboloid(z, v, depth_pedestal, current_depth);
		}
	}
}

void drawStimulus()
{
	
	if(checkglasses)
	{
		glLoadIdentity();
		glTranslated(0, 0, objdistance);

		IOD = interoculardistance;
		draw_stereocheck();
	} else if ( isStimulusDrawn && !expFinished )
	{
		draw_all(which_object);
	}
}

void drawGLScene()
{

	if(training) // in training mode the object rocks with a wider angle
	{
		if(motion > motion_theta*2 || motion < -motion_theta*2)
			direction_motion = direction_motion * -1;
	} else // in experiment mode the object rocks with the correct angle of rotation
	{
		if(motion > motion_theta || motion < -motion_theta)
			direction_motion = direction_motion * -1;
	}

	motion += motion_step*direction_motion;

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

		if(current_cue.find('s') != std::string::npos)
		{
			drawStimulus();
			drawInfo();
		}

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
	trial.next();

	for(int i = 0; i < max_points; i++)
	{
		z[i] = 0.0;
		v[i] = 0.0;
	}

	interoculardistance = str2num<double>(parameters.find("IOD"));
	motion_step = str2num<double>(parameters.find("motion_step"));
	motion_theta = str2num<double>(parameters.find("motion_theta"));
	objwidth = str2num<double>(parameters.find("objwidth"));
	objheight = str2num<double>(parameters.find("objheight"));
	objdensity = str2num<double>(parameters.find("objdensity"));
	cues_object2 = parameters.find("cues_object2");
	obj_type = parameters.find("obj_type");
	sizedepth = str2num<int>(parameters.find("sizedepth"));
}

void iSleep(int millisec)
{
	#ifdef SIMULATION
		usleep(millisec * 10);
	#else
		Sleep(millisec);
	#endif
}

void initTrial()
{
	// reset counters
	frameN = 0;
	object = 1;
	motion = 0.0;
	shuffleZ = unifRand(0.0, 40.0);
	if(sizedepth == 1)
		fixed_object = "stimulus";
	else
		fixed_object = "pedestal";
	adjusted_object = "stimulus";
	which_object = fixed_object;
	confirmAnswer = false;
	num_adjustments = 0;
	jitterX = unifRand(0.0, 7.5);

	if(checkglasses)
		build_stereocheck();

	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// assign the depth cues to pedestal and stimulus
	if(trial.getCurrent()["DepthCue"] == 1)
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
		direction_motion = 1.0;
		direction_motion2 = 1.0;
		jitterZ[0] = unifRand(-8.0, 8.0);
		jitterZ[1] = unifRand(-8.0, 8.0);
	} else 
	{
		direction_motion = - 1.0;
		direction_motion2 = - 1.0;
		jitterZ[0] = unifRand(-8.0, 8.0);
		jitterZ[1] = unifRand(-8.0, 8.0);
	}

	if(training)
	{
		depth_pedestal = unifRand(20.0, 50.0);

		objdistance = -450;

		if(unifRand(-1.0, 1.0) > 0.0)
		{
			flip_theta[0] = 0;
			flip_theta[1] = 270;
			depth_stimulus = 5.0;
		}
		else
		{
			flip_theta[0] = 270;
			flip_theta[1] = 0;
			depth_stimulus = 40.0;
		}
	}

	if(experiment)
	{
		if(trial.getCurrent()["Objtheta"] == 1)
		{
			flip_theta[0] = 0;
			flip_theta[1] = 0;
		}

		if(trial.getCurrent()["Objtheta"] == 2)
		{
			flip_theta[0] = 0;
			flip_theta[1] = 270;
		}

		if(trial.getCurrent()["Objtheta"] == 3)
		{
			flip_theta[0] = 270;
			flip_theta[1] = 0;
		}

		if(trial.getCurrent()["Objtheta"] == 4)
		{
			flip_theta[0] = 270;
			flip_theta[1] = 270;
		}
		// set the depth of the pedestal
		depth_pedestal = trial.getCurrent()["RelDepthObj"];

		objdistance = trial.getCurrent()["AbsDepth"];

		// set the depth of the stimulus
		depth_stimulus = shuffleZ;
	}

	// calculate the x and y coords of the points on the two paraboloids
	for(int i = 0; i < max_points; i++)
	{
		v[i] = unifRand(0.0, 2*M_PI);
		z[i] = unifRand(0.0, depth_stimulus);
	}

	buildThreeRods();

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(objdistance);

	// draw the stimulus	
	beepOk(0);

	iSleep(300);

	isStimulusDrawn=true;

	timer.start();
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
		parameters.find("SubjectName") << "\t" <<
		interoculardistance << "\t" <<
		trialNumber << "\t" <<

		objdistance << "\t" <<
		objheight << "\t" <<
		trial.getCurrent()["Objtheta"] << "\t" <<

		depth_pedestal << "\t" << 
		depth_stimulus << "\t" << 
		shuffleZ << "\t" << 
		num_adjustments << "\t" << 
		fixed_object << "\t" << 
		adjusted_object << "\t" << 
		depth_cue_pedestal << "\t" << 
		depth_cue_stimulus << "\t" << 

		trial.getCurrent()["DepthCue"] << "\t" <<
		timeElapsed 
		<< endl;

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		responseFile.close();
		expFinished = true;
		beepOk(2);
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
		{
			//if(trialNumber<1)
			//	trial.next();
			initTrial();
		} break;

		case '+': // toggle checkglasses
		{
			checkglasses = !checkglasses;
			random_shuffle(&stereocheckZ[0], &stereocheckZ[4]);
		} break;

		case '0':
		{
			/*
			buildThreeRods();
			if (switch_object)
			{
				isStimulusDrawn = false;
				drawGLScene();
				switch_object = !switch_object;
				which_object = fixed_object;
				iSleep(100);
				isStimulusDrawn = true;
			}
			else
			{
				isStimulusDrawn = false;
				drawGLScene();
				switch_object = !switch_object;
				which_object = adjusted_object;
				iSleep(100);
				isStimulusDrawn = true;
			}
			*/
		} break;

		case '4':
		{
			confirmAnswer=false;
			if(which_object == "stimulus")
			{
				num_adjustments++;
				isStimulusDrawn = false;
				drawGLScene();
				depth_stimulus += 5.0;
				for(int i = 0; i < max_points; i++)
				{
					z[i] = unifRand(0.0, depth_stimulus);
				}
				iSleep(200);
				isStimulusDrawn = true;
			}
		} break;

		case '1':
		{
			confirmAnswer=false;
			if(which_object == "stimulus")
			{
				num_adjustments++;
				isStimulusDrawn = false;
				drawGLScene();
				if(depth_stimulus > 0.0)
					depth_stimulus -= 5.0;
				else
					depth_stimulus = 0.0;
				for(int i = 0; i < max_points; i++)
				{
					z[i] = unifRand(0.0, depth_stimulus);
				}
				iSleep(200);
				isStimulusDrawn = true;
			}
		} break;

		case '5':
		{
			confirmAnswer=false;
			if(which_object == "stimulus")
			{
				num_adjustments++;
				isStimulusDrawn = false;
				drawGLScene();
				depth_stimulus += 1.0;
				for(int i = 0; i < max_points; i++)
				{
					z[i] = unifRand(0.0, depth_stimulus);
				}
				iSleep(200);
				isStimulusDrawn = true;
			}
		} break;

		case '2':
		{
			confirmAnswer=false;
			if(which_object == "stimulus")
			{
				num_adjustments++;
				isStimulusDrawn = false;
				drawGLScene();
				if(depth_stimulus > 0.0)
					depth_stimulus -= 1.0;
				else
					depth_stimulus = 0.0;
				for(int i = 0; i < max_points; i++)
				{
					z[i] = unifRand(0.0, depth_stimulus);
				}
				iSleep(200);
				isStimulusDrawn = true;
			}
		} break;

		case ' ':
		{
			if(!confirmAnswer)
				confirmAnswer=true;
			else if(training)
				initTrial();
			else if(experiment)
				advanceTrial();
		} break;

		case 'e':
		{
			visibleInfo = false;
			training = false;
			experiment = true;
			initTrial();
		}
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
	
	// eye coordinates
	eyeRight = Vector3d(IOD/2,0,0);
	eyeLeft = -eyeRight;
}

//######################################################## MAIN #####################//

int main(int argc, char*argv[])
{
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
