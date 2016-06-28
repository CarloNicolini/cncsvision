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
	static const bool stereo=false;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;

// timers
Timer timer;
Timer globalTimer;

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

// Display
double displayDepth = -400;

// Virtual target objects
double cylRad = 4;
double cylHeight = 40;
double cyl_x=45, cyl_y=cylHeight/2, cyl_z=displayDepth, cyl_pitch=90;

double spinCounter = 0;
bool spin = false;
double spinDelta = 5;

bool light = true;
GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 100.0f, -100.0f, 1.0f};

bool isStimulusDrawn = false;
int frameN = 0;
string subjectName = "junk";
double trial_duration = 0.0, trial_time = 0.0;

string first_object = "empty", second_object = "empty";
string current_object = "empty";
int current_illusion=0;
int object = 0;
int current_stair;

double current_size = 0.0, pedestal_size = 0.0, stimulus_size = 0.0;

int responsekey = 0;

bool isTrialDone = false;
double IOD = 0.0;
double custom_depth = 0.0;

const int numCylsInMask = 20;
double maskSizeAdd[numCylsInMask];
double maskXAdd[numCylsInMask];
double maskYAdd[numCylsInMask];
double maskZAdd[numCylsInMask];
bool maskBuilt = false;

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
void drawCylinder(double height,double cyl_x_shift,double cyl_y_shift, double cyl_z_shift);
void draw_mask();
void draw_all(string obj);
void drawTrial();
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
string experiment_directory = "S:/Domini-ShapeLab/evan/fall15-ponzoStaircase/";
//string experiment_directory = "C:/Users/visionlab/Desktop/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/evan/fall15-ponzoStaircase/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "fall15-ponzoStaircase_parameters.txt";

// response file name
string responseFile_name = "fall15-ponzoStaircase_";

// summary file headers
string responseFile_headers = "subjName\ttrialN\tfirstObj\tsecondObj\tpedestalSize\tstimulusSize\tstimulusIllusion\tresponsekey\tresponse\tstairID\tstairState\tstairInversions\tstairAscending\tstairStepsDone\ttimeElapsed";

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
		text.draw("####### fall15-ponzoStaircase #######");
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
		text.draw(" " );
		text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		text.draw(" " );

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
	// don't draw stimulus until not finished & short ISI
	if (!expFinished && isStimulusDrawn )
		drawTrial();
}

// this draws the actual stimulus
void drawTrial()
{
	//if (timer.getElapsedTimeInMilliSec() < 200)
		// draw nothing?

	if (timer.getElapsedTimeInMilliSec() >= 200 && timer.getElapsedTimeInMilliSec() < (trial_time+200) )
	{
		object = 1;
		current_object = first_object;
		draw_all(current_object); //string 'stimulus' or 'pedestal'
	}

	if (timer.getElapsedTimeInMilliSec() >= (trial_time + 200) && timer.getElapsedTimeInMilliSec() < (500 + trial_time + 200) )
	{
		draw_mask();
	}

	if (timer.getElapsedTimeInMilliSec() >= (500 + trial_time + 200) && timer.getElapsedTimeInMilliSec() < (500 + (2*trial_time) + 200))
	{
		object = 2;
		current_object = second_object;
		draw_all(current_object); //string 'stimulus' or 'pedestal'
	} 

	if(object == 2 && timer.getElapsedTimeInMilliSec() >= (500 + (2*trial_time) + 200))
	{
		object=0;
		isStimulusDrawn = false;
		isTrialDone = true;
	}
}

void draw_all(string obj)
{
	if (obj == "pedestal")
	{
		current_illusion = 0; //control background
		current_size = pedestal_size;
	} else
	{
		if (current_stair <= 5) // reducing illusion (0,1,2,3,4,5)
			current_illusion = 1;
		if (current_stair >= 6) // expanding illusion (6,7,8,9,10,11)
			current_illusion = 2;
		current_size = stimulus_size;
	}

	drawCylinder(current_size,0,0,0);

    int verticalDistance = 100;
    int horizontalDistance = 180;
    double planeAngle=0;
    
    // control background
    if(current_illusion==1){
    	planeAngle = 67;
    	horizontalDistance = 450;
    }else if(current_illusion==2){
    	planeAngle = -67;
    	horizontalDistance = 450;
    }

	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,-500);
	glRotated(planeAngle,0,1,0);
	if(current_illusion==1){
		glTranslated(75,0,0);
	}else if(current_illusion==2){
		glTranslated(-75,0,0);
	}
	glBegin(GL_LINE_LOOP);
	glVertex3f((-horizontalDistance/2),verticalDistance/2,0);
	glVertex3f((horizontalDistance/2),verticalDistance/2,0);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f((-horizontalDistance/2),-verticalDistance/2,0);
	glVertex3f((horizontalDistance/2),-verticalDistance/2,0);
	glEnd();
	double spacing = horizontalDistance/9;
	for(int i=0;i<10;i++){
		glBegin(GL_LINE_LOOP);
		glVertex3f((-horizontalDistance/2)+(i*spacing),verticalDistance/2,0);
		glVertex3f((-horizontalDistance/2)+(i*spacing),-verticalDistance/2,0);
		glEnd();
	}
	glPopMatrix();
}

void draw_mask()
{
	if(!maskBuilt){
		for (int i=0; i<numCylsInMask; i++){
			maskSizeAdd[i] = unifRand(-10,10);
			maskXAdd[i] = unifRand(-25,25);
			maskYAdd[i] = unifRand(-25,25);
			maskZAdd[i] = unifRand(-25,25);
		}
		maskBuilt=true;
	}
	for (int i=0; i<numCylsInMask; i++){
		drawCylinder(40+maskSizeAdd[i],maskXAdd[i],maskYAdd[i],maskZAdd[i]);
	}

}

// draw a cylinder
void drawCylinder(double height, double cyl_x_shift, double cyl_y_shift, double cyl_z_shift)
{
    //glMatrixMode(GL_MODELVIEW);

    cyl_y = height/2;
    
	glPushMatrix();
    glLoadIdentity();
    glColor3f(0.1f, 0.1f, 0.1f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, LightAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, LightDiffuse);
	glTranslated(cyl_x+cyl_x_shift, cyl_y+cyl_y_shift, cyl_z+cyl_z_shift);
	glRotated(cyl_pitch,1,0,0);

	// Cylinder
	GLUquadricObj *cyl1;
	cyl1 = gluNewQuadric();
	gluQuadricDrawStyle(cyl1, GLU_LINE);
	gluCylinder(cyl1, cylRad, cylRad, height, 16, 1);
	// The object should change color to indicate that it's been "reached"
	//if (!reachedObject) {
	glColor3f(0.7f, 0.0f, 0.2f);
	//} else {
		//glColor3f(0.0f, 0.2f, 0.7f);
	//}
	gluQuadricDrawStyle(cyl1, GLU_FILL);
	gluQuadricNormals(cyl1, GLU_SMOOTH);
	gluCylinder(cyl1, cylRad, cylRad, height, 16, 1);
	// End-disk 1
	gluQuadricOrientation(cyl1, GLU_INSIDE);
	glTranslated(0.0, 0.0, 0.0);
	gluDisk(cyl1,0,cylRad,16,1);
	// End-disk 2
	gluQuadricOrientation(cyl1, GLU_OUTSIDE);
	glTranslated(0.0, 0.0, height);
	gluDisk(cyl1,0,cylRad,16,1);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

/*** TRIAL ***/
void initVariables()
{
	trial.init(parameters);
	trial_duration = str2num<double>(parameters.find("trial_duration"));
}

void initTrial()
{
	// reset counters
	frameN = 0;
	object = 1;
	isTrialDone = false;
	maskBuilt = false;

	cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;

	// draw an empty screen
	isStimulusDrawn = false;

	// randomly assign the order of the objects
	if(unifRand(-1.0, 1.0) > 0.0)
	{
		first_object = "pedestal";
		second_object = "stimulus";
	} else 
	{
		first_object = "stimulus";
		second_object = "pedestal";
	}

	current_stair = trial.getCurrent().second->getCurrentStaircase()->getID();

#ifdef TEST
		double PSE = str2num<double>(parameters.find("PSE"));
		double JND = unifRand(-(str2num<double>(parameters.find("JND"))), str2num<double>(parameters.find("JND")));

		if (current_stair <= 5)
			PSE = -PSE;

		cerr << "PSE = " << PSE << endl;
		cerr << "JND = " << JND << endl;

		// set the depth of the pedestal
		if(current_stair==0 || current_stair==1 || current_stair==6 || current_stair==7)
			pedestal_size = 40.0;
		if(current_stair==2 || current_stair==3 || current_stair==8 || current_stair==9)
			pedestal_size = 42.0;
		if(current_stair==4 || current_stair==5 || current_stair==10 || current_stair==11)
			pedestal_size = 44.0;

		// set the depth of the stimulus
		stimulus_size = pedestal_size + trial.getCurrent().second->getCurrentStaircase()->getState();

		//objdistance = trial.getCurrent().first["AbsDepth"];

		bool decision = ((stimulus_size + JND) > (pedestal_size + PSE) || (stimulus_size + JND) > (pedestal_size + PSE));

		advanceTrial(decision);
#else
	if(training)
	{	
		trial_time = trial_duration;

		// set the depth of the pedestal
		pedestal_size = 30.0;

		// set the depth of the stimulus to be either clearly smaller or clearly larger than the pedestal
		if(unifRand(-1.0, 1.0) > 0.0)
			stimulus_size = 20.0;
		else
			stimulus_size = 40.0;
	}

	if(experiment)
	{
		trial_time = trial_duration;

		// set the depth of the pedestal
		if(current_stair==0 || current_stair==1 || current_stair==6 || current_stair==7)
			pedestal_size = 40.0;
		if(current_stair==2 || current_stair==3 || current_stair==8 || current_stair==9)
			pedestal_size = 42.0;
		if(current_stair==4 || current_stair==5 || current_stair==10 || current_stair==11)
			pedestal_size = 44.0;

		// set the depth of the stimulus
		stimulus_size = pedestal_size + trial.getCurrent().second->getCurrentStaircase()->getState();
	}

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(displayDepth);

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
		trialNumber << "\t" <<
		first_object << "\t" << 
		second_object << "\t" << 
		pedestal_size << "\t" << 
		stimulus_size << "\t" << 
		current_illusion << "\t" <<

		responsekey << "\t" <<
		response << "\t" <<

		trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" << 
		trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getAscending()<< "\t" <<
		trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<

		timeElapsed << endl;

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
			//interoculardistance += 0.5;
			break;

		case 'n': // decrease IOD
			//interoculardistance -= 0.5;
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
			//checkglasses = !checkglasses;
			//random_shuffle(&stereocheckZ[0], &stereocheckZ[4]);
		} break;

		case 't':
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
						responsekey=4;
						if(first_object == "stimulus")
							advanceTrial(true);
						else
							advanceTrial(false);

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
						responsekey=6;
						if(second_object == "stimulus")
							advanceTrial(true);
						else
							advanceTrial(false);
					}
				}
		} break;

		case ' ':
		{

		} break;

		case '1':
		{
			
		} break;

		case '3':
		{
		
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
