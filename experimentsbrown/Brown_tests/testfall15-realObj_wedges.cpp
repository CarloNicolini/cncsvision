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
#include "CoordinatesExtractor.h"

// drawInfo
#include "GLText.h"

// trial matrix
#include "BalanceFactor.h"

// miscellaneous
#include "ParametersLoader.h"
#include "Util.h"

/***** CALIBRATION THINGS *****/
#include "Calibration_017A.h"

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

#include "BrownPhidgets.h"
using namespace BrownPhidgets;

#include "BrownFtdi.h"
using namespace BrownFtdi;

double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0, phidgetsAlignment=0.0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;

/************** PHIDGETS VARIABLES ********************/
CPhidgetAdvancedServoHandle wedgeMotors;

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
BalanceFactor<double> trial;

// integer
int frameN = 0;
int trialNumber = 0;
int repetitions = 1;
int max_repetitions = 0;
int response = 0;
int demo_position = 0;
int switch_number[2] = {1,2};
// bool
bool visibleInfo = true;
bool experiment = false, training = true;
bool expFinished = false;
bool expBegun = false;
bool showStimuli = false;
bool start_rotate = true;

/********** STREAMS **************/
ofstream responseFile;

/********** FUNCTION PROTOTYPES *****/

//#define TEST

// output files and streams
void initStreams();

// sounds
void beepOk(int tone);

// info
void drawInfo();

void drawGLScene();

// trial
void initVariables();
void initTrial();
void advanceTrial();
//void switches_on();
//void switches_off();

// keypresses
void handleKeypress(unsigned char key, int x, int y);

// idle
void idle();

// others
void handleResize(int w, int h);
//void initRendering();
void update(int value);

/*************************** EXPERIMENT SPECS ****************************/

// experiment title
//string experiment_title = "####### fall15-realObj_wedges_adjustment #######";
string experiment_title = "####### spring16-realObj_wedges_adjustment #######";

// experiment directory
//string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/realObj/wedges/";
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2016/realObj/";

// parameters file directory and name
string parametersFile_directory = experiment_directory + "parameters_realObj_wedges.txt";

// response file name
//string responseFile_name = "fall15-3Dcues-realObj_wedges_";
string responseFile_name = "spring16-3Dcues-realObj_wedges_";

// summary file headers
string responseFile_headers = "subjName\ttrialN\trepetitions\tTheta\tTilt_sign\tresponse\ttrialDuration";

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

	// Subject folder
	string dirName  = experiment_directory + subjectName;

	mkdir(dirName.c_str(), 777); // linux syntax

	// Principal streams files

	// response file
	string responseFileName = experiment_directory + responseFile_name + subjectName + ".txt";

	// Check for output file existence
	if (util::fileExists(responseFileName))
	{
		string error_on_file_io = responseFileName + string(" already exists\n");
		cerr << error_on_file_io << endl;
		cerr << "FILE ALREADY EXISTS\n\nPlease check the subject's name in the parameters file.\n" << endl;

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
/*		case 0:
		// Remember to put double slash \\ to specify directories!!!
		PlaySound((LPCSTR) "/home/shapelab/workspace/cncsvisioncmake/data/sounds/thankyou.mp3", NULL, SND_FILENAME | SND_ASYNC);
		break;
/*		case 1:
		PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
/*		case 2:
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
*/	}
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
		text.draw(experiment_title);
		text.draw("");

		if(!expFinished)
		{
			text.draw("####### SUBJECT #######");
			text.draw("#");
			text.draw("# Name: " +parameters.find("SubjectName"));
			text.draw("#");
			text.draw("# trial: " +stringify<double>(trialNumber));
			text.draw("#");
			text.draw("#######################\n\n");
			text.draw(" ");

			text.draw("time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
		} else
		{
			text.draw("The experiment is finished.");
		}
		
		text.leaveTextInputMode();
	}
}

void drawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!showStimuli || expFinished)
	{
		timer.start();
    	glClearColor(0.0,0.0,0.0,1.0);
	}
    else
    	glClearColor(0.4,0.0,0.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //if(!expBegun || expFinished)
		drawInfo();
    glutSwapBuffers();
}

/*** TRIAL ***/
void initVariables()
{
	trial.init(parameters);
	trial.print();

	max_repetitions = str2num<int>(parameters.find("max_repetitions"));
}

int CCONV PositionChangeHandlerServoRocking(CPhidgetAdvancedServoHandle ADVSERVO, void *usrptr, int Index, double Value)
{
	// first we check that the static object has reached its final position
	// the callback is updated based on the most recent call to any motor
	// the program moves the static object first, so the value of the variable Value
	// refers to motor 2 (static object) as long as it is moving
	// resp is a logic variable that checks that the position of motor 2 corresponds to 
	// its intended angle, according to the equation: pos = 40.0 + 8/9 * final_theta (found empirically)
	// specifically, when the absolute  difference between the current position (Value) and the intended position
	// drops below 0.5, the motor is considered in place
	bool resp = true;
	if(expBegun)
		resp = abs(Value - (40.0 + (90 + trial.getCurrent()["Tilt_sign"] * trial.getCurrent()["Theta"]) * 8/9)) < .5;
	else
		resp = abs(Value - demo_position) < .5;

	// when the static object has reached its final orientation, the other object starts rocking
	// otherwise the screen is black (so the objects are not illuminated)
	if(resp & start_rotate)
	{
		usleep(500000);
		showStimuli = true;

		switches_on();

		servo_rotate(wedgeMotors, 2, -7.5);
		start_rotate = false;
	}

	// rocking is controlled by the following loop
	if(Index == 2 && abs(Value - 33.5) < .5)
		CPhidgetAdvancedServo_setPosition (ADVSERVO, 2, 46.5);
	if(Index == 2 && abs(Value - 46.5) < .5)
		CPhidgetAdvancedServo_setPosition (ADVSERVO, 2, 33.5);

	return 0;
}

void servo_rock()
{
	CPhidgetAdvancedServo_set_OnPositionChange_Handler(wedgeMotors, PositionChangeHandlerServoRocking, NULL);
}

void initTrial()
{
	#ifndef TEST
		cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;
		cerr << trial.getCurrent()["Theta"] << endl;

		// reset counters
		frameN = 0;

		// hide the stimuli
		showStimuli = false;
		start_rotate = true;

		switches_off();

		// refresh the scene
		drawGLScene();
		// this second call is a hack that is needed since the phidgets freeze the screen otherwise
		drawGLScene();

		// wait a bit
		usleep(500000);

		// tilt the static object
		servo_rotate(wedgeMotors, 3, 90.0 + trial.getCurrent()["Tilt_sign"] * trial.getCurrent()["Theta"]);

		// swing the moving object
		servo_rock();

		timer.start();
	#else
		advanceTrial();
	#endif
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			trialNumber << "\t" <<
			repetitions << "\t" <<
			trial.getCurrent()["Theta"] << "\t" <<
			trial.getCurrent()["Tilt_sign"] << "\t" <<
			response << "\t" <<
			timeElapsed << "\t" << 
			endl;
 
	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else if (repetitions < max_repetitions)
	{	
		repetitions++;
		initVariables();
		trial.next();
		initTrial();
	} else
	{
		responseFile.close();
		expFinished = true;
		beepOk(4);
	}
}

/*** keypresses ***/
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		case 27: // press escape to quit
		{   
			servo_disengage(wedgeMotors, 2);
			servo_disengage(wedgeMotors, 3);
			usleep(500000);
			servo_rotate(wedgeMotors, 2, 0.0);
			servo_rotate(wedgeMotors, 3, 90.0);
			servo_close(wedgeMotors);
			switch_disconnect();
			exit(0);
		} break;

		case 13:
		{
			if(!expBegun)
			{
				expBegun = true;
				trial.next();
				initTrial();
			}
		} break;

		case ' ':
		{
			if(!expBegun)
			{
				double static_obj_position = 0.0;
				CPhidgetAdvancedServo_getPosition (wedgeMotors, 3, &static_obj_position);
				start_rotate = true;
				switches_off();
				drawGLScene();
				usleep(500000);
				if(static_obj_position == 120.0) // flat object
				{
					demo_position = 170;
					servo_rotate(wedgeMotors, 3, 180.0); // flip it vertical
				}
				if(static_obj_position > 190.0) // vertical object
				{
					demo_position = 120;
					servo_rotate(wedgeMotors, 3, 90.0); // flip it horizontal
				}
				servo_rock();
			}
		} break;

		case '4':
		{
			if(expBegun)
			{
				response = 4;
				servo_disengage(wedgeMotors, 2);
				servo_disengage(wedgeMotors, 3);
				advanceTrial();
			}
		} break;

		case '6':
		{
			if(expBegun)
			{
				response = 6;
				servo_disengage(wedgeMotors, 2);
				servo_disengage(wedgeMotors, 3);
				advanceTrial();
			}
		} break;
	}
}


/*** IDLE ***/
void idle()
{
	frameN++;
}

//######################################################## MAIN #####################//

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	wedgeMotors = servo_connect();

	switch_connect();
	switches_on();

	// initializing glut
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Experiment");


	// initializing experiment's parameters
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

	servo_close(wedgeMotors);
	switch_disconnect();

	return 0;
}

//############################################################
//############################################################
/****************** FIXED FUNCTIONS  ************************/
//############################################################
//############################################################

// Resize window
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

// Handle refresh of the screen
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}