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
#include "Optotrak2.h"
#include "Marker.h"

#ifndef SIMULATION
	#include <direct.h> // mkdir
#endif

// drawInfo
#include "GLText.h"

// trial matrix
#include "BalanceFactor.h"

// miscellaneous
#include "ParametersLoader.h"
#include "Util.h"

/***** CALIBRATION THINGS *****/
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

#include "BrownPhidgets.h"
using namespace BrownPhidgets;

double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0, phidgetsAlignment=0.0;

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

/************** PHIDGETS VARIABLES ********************/
CPhidgetAdvancedServoHandle wedgeMotors;

/********** EYES AND MARKERS **********************/
vector <Marker> markers;

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

// bool
bool visibleInfo = true;
bool experiment = false, training = true;
bool expFinished = false;
bool expBegun = false;
bool showStimuli = false;
bool translate_markers = false;

double objdistance = 0.0;

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

// keypresses
void handleKeypress(unsigned char key, int x, int y);

// idle
void idle();

// others
void handleResize(int w, int h);
void updateTheMarkers();
//void initRendering();
void update(int value);

/*************************** EXPERIMENT SPECS ****************************/

// experiment title
string experiment_title = "####### fall15-realObj_wedges_adjustment #######";

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/realObj/wedges/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/realObj/wedges/";
#endif

// parameters file directory and name
string parametersFile_directory = experiment_directory + "parameters_realObj_wedges.txt";

// response file name
string responseFile_name = "fall15-3Dcues-realObj_wedges_";

// summary file headers
string responseFile_headers = "subjName\ttrialN\trepetitions\tTheta\tTilt_sign\tresponse\ttrialDuration";

/*************************** FUNCTIONS ***********************************/

void updateTheMarkers()
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
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
			text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers.at(1).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers.at(2).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers.at(3).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers.at(4).p.transpose())+ " [mm]" );
			
			text.draw("Marker "+ stringify<int>(11)+stringify< Eigen::Matrix<double,1,3> > (markers.at(11).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(12)+stringify< Eigen::Matrix<double,1,3> > (markers.at(12).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(13)+stringify< Eigen::Matrix<double,1,3> > (markers.at(13).p.transpose())+ " [mm]" );

			text.draw("Marker "+ stringify<int>(14)+stringify< Eigen::Matrix<double,1,3> > (markers.at(14).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(15)+stringify< Eigen::Matrix<double,1,3> > (markers.at(15).p.transpose())+ " [mm]" );
			text.draw("Marker "+ stringify<int>(16)+stringify< Eigen::Matrix<double,1,3> > (markers.at(16).p.transpose())+ " [mm]" );
		} else
		{
			text.draw("The experiment is finished.");
		}
		
		text.leaveTextInputMode();
	}
}

void calibrate_markers()
{
	for ( int i=1; i<=OPTO_NUM_MARKERS; i++)
    {  
    	Vector3d origin(1000.0,1000.0,1000.0);
        markers[i].p -= origin;
    }
}

void drawGLScene()
{

	if(translate_markers)
		calibrate_markers();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!showStimuli || expFinished)
    	glClearColor(0.0,0.0,0.0,1.0);
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

	objdistance = str2num<double>(parameters.find("distance"));

	max_repetitions = str2num<int>(parameters.find("max_repetitions"));
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

		timer.start();
	#else
		advanceTrial();
	#endif
}

void advanceTrial()
{
	double timeElapsed = globalTimer.getElapsedTimeInMilliSec();

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
			translate_markers = !translate_markers;
		} break;

	}
}


/*** IDLE ***/
void idle()
{
	frameN++;
	updateTheMarkers();
}

//######################################################## MAIN #####################//

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	initOptotrak();
	
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