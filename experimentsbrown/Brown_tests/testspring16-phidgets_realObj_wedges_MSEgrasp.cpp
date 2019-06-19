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

#include "Optotrak2.h"
#include "Marker.h"

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* VARIABLES OBJECTS  **********************/
Optotrak2 optotrak;
CoordinatesExtractor thumbCoords, indexCoords;

/********** VISUALIZATION AND STIMULI ***************/

// timers
Timer timer;

/************** PHIDGETS VARIABLES ********************/
CPhidgetAdvancedServoHandle wedgeMotors;

/********** EYES AND MARKERS **********************/
Vector3d thumb_marker(0.0,0.0,0.0), index_marker(0.0,0.0,0.0);
Vector3d ind(0,0,0), thu(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999), home_position(0,0,0), grip_position(0,0,0);

vector <Marker> markers;

// markers map
int indexcal = 6, thumbcal = 5;
int ind1 = 11, ind2 = 12, ind3 = 13;
int thu1 = 14, thu2 = 15, thu3 = 16;
int calibration1 = 1, calibration2 = 2;
int calibration3 = 3, calibration4 = 4;

/********* CALIBRATION VARIABLES *********/

int fingerCalibrationDone=0;

#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif

bool allVisibleIndex=markers_status;
bool allVisibleThumb=markers_status;
bool allVisibleFingers=markers_status;
bool allVisibleCalibrationMarkers=markers_status;
bool allVisibleReferenceMarkers=markers_status;

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
int fingersOccluded = 0, framesOccluded = 0, good_trial = 0, frames_at_start = 0, frames_after_grasp=0;
int which_object = 0;
int current_depth = 0;

// bool
bool visibleInfo = true;
bool expFinished = false;
bool expBegun = false;
bool showStimuli = false;
bool start_rotate = true;
bool iGrasped = false, isHandHome = true;
bool experiment=false, training=false;
bool pressingF = false;
bool system_calibrated = false;
bool motion = false;
bool start_motion = false;
bool handFarFromObject = true;
bool rocking_obj_in_position = false;

// double
double objdistance = 420.0;
double dist_to_home = 0.0;
double FGA = 0.0;
double theta = 0.0;
double current_theta = 0.0;
double motor_theta = 0.0;
double theta_30 = 0.0, theta_50 = 0.0;

// string
string task = "empty";
string subjectName = "junk";
string current_object = "empty";

/********** STREAMS **************/
ofstream responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/

//#define TEST

// output files and streams
void initStreams();

// sounds
void beepOk(int tone);

// info
void drawInfo();
void drawGLScene();

// grasp
void calibration_fingers(int phase);

// trial
void initCalibration();
void initVariables();
void initTrial();
void advanceTrial();

// keypresses
void handleKeypress(unsigned char key, int x, int y);

// online operations
void online_calibrate_markers();
void online_fingers();
void online_trial();

// idle
void idle();

// others
void cleanup();
void updateTheMarkers();
void handleResize(int w, int h);
void update(int value);
void initOptotrak();

/*************************** EXPERIMENT SPECS ****************************/

// experiment title
string experiment_title = "####### spring16-realObj_wedges_MSE #######";

// experiment directory
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2016/realObj/";

// parameters file directory and name
string parametersFile_directory = experiment_directory + "parameters_phidgets_realObj_wedges_MSEgrasp.txt";

// response file name
string responseFile_name = "spring16-realObj_wedges_";

// summary file headers
string responseFile_headers = "subjName\ttrialN\trepetitions\tCue\tFGA\ttrialDuration";

// markers file name
string markersFile_name = "realObj_wedges_MSEgrasp_trajectories_";

// response file headers
string markersFile_headers = "subjName\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\tfingersOccluded\tframesOccluded";

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

	mkdir(dirName.c_str(), 777); // linux syntax

	// Principal streams files
	initVariables();
	// response file
	string responseFileName = experiment_directory + responseFile_name + subjectName + "_" + task + ".txt";

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
		text.init(SCREEN_WIDTH-350,SCREEN_HEIGHT-250,glWhite,GLUT_BITMAP_HELVETICA_18);
		text.enterTextInputMode();

		if(!expFinished)
		{
			if(!system_calibrated)
			{
				text.draw("");
				text.draw("");
				text.draw("Calibrate the coordinate system by pressing C");
				text.draw("");
				text.draw("");
				if ( allVisibleReferenceMarkers )
						glColor3fv(glGreen);
					else
						glColor3fv(glRed);
				text.draw("Marker "+ stringify<int>(calibration1)+":	"+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibration1).p.transpose())+ " [mm]" );
				text.draw("Marker "+ stringify<int>(calibration2)+":	"+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibration2).p.transpose())+ " [mm]" );
				text.draw("Marker "+ stringify<int>(calibration3)+":	"+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibration3).p.transpose())+ " [mm]" );
				text.draw("Marker "+ stringify<int>(calibration4)+":	"+stringify< Eigen::Matrix<double,1,3> > (markers.at(calibration4).p.transpose())+ " [mm]" );
			} else
			{
				switch (fingerCalibrationDone)
				{
					case 0:
						text.draw("Calibration markers are not visible.");		
						break;

					case 1:
						{
							glColor3fv(glGreen);
							text.draw("Calibration markers have been detected. Press F to record their positions.");	
						} break;

					case 2:
						{
							glColor3fv(glWhite);
							text.draw("Place index and thumb tips on the markers and press F to calibrate");	
						} break;

					case 3:
						text.draw("Set home position then press F to start.");		
						break;
				}
				glColor3fv(glWhite);
				text.draw(" ");

				if(fingerCalibrationDone == 4)
				{
					text.draw(experiment_title);
					text.draw("");

					if(experiment)
					{
						text.draw("####### EXPERIMENT #######");
					}
					if(training)
					{
						glColor3fv(glRed);
						text.draw("####### THIS IS JUST A TRAINING: press E to begin when ready #######");
					}
					glColor3fv(glWhite);
					text.draw("#");
					text.draw("# Name: " + subjectName);
					text.draw("#");
					text.draw("# trial: " +stringify<double>(trialNumber));
					text.draw("#");
					text.draw("#######################\n\n");
					text.draw(" ");
				}

				if(fingerCalibrationDone < 3)
				{
					glColor3fv(glWhite);
					text.draw("Index Marker: "+ stringify< Eigen::Matrix<double,1,3> > (index_marker.transpose()) + " [mm]" );
					text.draw("Thumb Marker: "+ stringify< Eigen::Matrix<double,1,3> > (thumb_marker.transpose()) + " [mm]" );
					text.draw("");

					if(fingerCalibrationDone < 2)
					{
						if ( allVisibleCalibrationMarkers )
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						
						text.draw("Index Calibration Marker "+ stringify<int>(indexcal)+stringify< Eigen::Matrix<double,1,3> > (markers.at(indexcal).p.transpose())+ " [mm]" );
						text.draw("Thumb Calibration Marker "+ stringify<int>(thumbcal)+stringify< Eigen::Matrix<double,1,3> > (markers.at(thumbcal).p.transpose())+ " [mm]" );
						text.draw(" ");
					}
				}

				glColor3fv(glWhite);
				text.draw(" " );
				text.draw("Index" );

				if ( allVisibleIndex )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Index = " +stringify< Eigen::Matrix<double,1,3> > (ind.transpose()));

				glColor3fv(glWhite); 
				text.draw(" " );
				text.draw("Thumb" );

				if ( allVisibleThumb )
					glColor3fv(glGreen);
				else
					glColor3fv(glRed);
				text.draw("Thumb = " +stringify< Eigen::Matrix<double,1,3> > (thu.transpose()));			

				glColor3fv(glWhite); 
				text.draw(" " );

				text.draw(" " );
				text.draw("Elapsed Time: " +stringify<int>(timer.getElapsedTimeInMilliSec()));
			}
		} else
		{
			text.draw("The experiment is finished.");
		}
		
		text.leaveTextInputMode();
	}
}

void drawGLScene()
{
	online_fingers();
	online_trial();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//if (!showStimuli || expFinished)
	if (fingersOccluded == 0 || !handFarFromObject)
    	glClearColor(0.0,0.0,0.0,1.0);
    else
    	glClearColor(1.0,1.0,1.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	drawInfo();
    glutSwapBuffers();
}

/*** GRASP ***/
void calibration_fingers(int phase)
{
	switch (phase)
	{
		case 1:
		{
			index_marker = markers.at(indexcal).p;
			thumb_marker = markers.at(thumbcal).p;
		}

		case 2:
		{
			indexCoords.init(index_marker, markers.at(ind1).p, markers.at(ind2).p, markers.at(ind3).p);
			thumbCoords.init(thumb_marker, markers.at(thu1).p, markers.at(thu2).p, markers.at(thu3).p);
		} break;
	}
}

/*** TRIAL ***/
void initVariables()
{
	trial.init(parameters);
	trial.print();
	task = parameters.find("task");
	theta_30 = str2num<double>(parameters.find("theta_30"));
	theta_50 = str2num<double>(parameters.find("theta_50"));
	max_repetitions = str2num<int>(parameters.find("max_repetitions"));
}
bool lights_on = false;
int CCONV PositionChangeHandlerServoRocking(CPhidgetAdvancedServoHandle ADVSERVO, void *usrptr, int Index, double Value)
{
	bool resp = true;
	resp = abs(Value - (40.0 + (current_theta * 8/9))) < .5;

	if(resp & start_rotate)
	{
		sleep(.5);
		showStimuli = true;

		servo_rotate(wedgeMotors, 1, motor_theta, 250.0);

		start_rotate = false;
	}


	if(Index == 1 && abs(Value - (40.0 + (motor_theta * 8/9))) < .5 && !rocking_obj_in_position)
	{
		rocking_obj_in_position = true;
		if(motion && !start_motion)
		{
			sleep(.5);
			servo_rotate(wedgeMotors, 1, motor_theta+7.5);
			start_motion = true;
			switches_on();
		} else
			switches_on();
	}

	if(motion && start_motion && handFarFromObject)
	{
			if(Index == 1 && abs(Value - (40.0 + (motor_theta * 8/9))) < .5)
				servo_rotate(wedgeMotors, 1, motor_theta + 15.0);
			if(Index == 1 && abs(Value - (40.0 + ((motor_theta +15.0) * 8/9))) < .5)
				servo_rotate(wedgeMotors, 1, motor_theta);
	}

	return 0;
}

void servo_rock()
{
	if(current_depth == 30)
		if(motion)
			motor_theta = -7.5;
		else
			motor_theta = 0.0;
	else if (current_depth == 50)
		if(motion)
			motor_theta = 172.5;
		else
			motor_theta = 180.0;
	CPhidgetAdvancedServo_set_OnPositionChange_Handler(wedgeMotors, PositionChangeHandlerServoRocking, NULL);
}

void initTrial()
{
	#ifndef TEST
		if(experiment)
			cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;
		else
			cerr << "\n--------- TRAINING ----------" << endl;

		// reset counters
		frameN = 0;
		framesOccluded = 0;
		frames_at_start = 0;
		frames_after_grasp=0;
		start_motion = false;
		rocking_obj_in_position = false;

		// subject did not grasp yet
		iGrasped = false;

		// hide the stimuli
		showStimuli = false;
		start_rotate = true;

		switches_off();

		// refresh the scene
		drawGLScene();

		if(training)
		{
			double random_depth = unifRand(-1.0, 1.0);

			if(random_depth > 0.0)
				current_depth = 30;
			else
				current_depth = 50;

			double random_cue = unifRand(0.0, 3.0);
			if(random_cue < 1.0)
			{
				current_object = "Stereo";
			} else if(random_cue > 1.0 && random_cue < 2.0)
			{
				current_object = "Motion";
			} else if(random_cue > 2.0 && random_cue < 3.0)
			{
				current_object = "StereoMotion";
			}
		} else
		{
			current_depth = trial.getCurrent()["RelDepthObj"];

			// check what object to present
			if(trial.getCurrent()["Cue"] == 1)
			{
				current_object = "Stereo";
			} else if(trial.getCurrent()["Cue"] == 2)
			{
				current_object = "Motion";
			} else if(trial.getCurrent()["Cue"] == 3)
			{
				current_object = "StereoMotion";
			}
		}

		if(current_depth == 30)
			theta = theta_30;
		else
			theta = theta_50;

		if(current_object == "Stereo")
		{
			current_theta = 90.0 + theta;
			motion = false;
			servo_rotate(wedgeMotors, 3, current_theta);
			servo_rock();
		} else if(current_object == "Motion")
		{
			current_theta = 90.0;
			motion = true;
			servo_rotate(wedgeMotors, 3, current_theta);
			servo_rock();
		} else if(current_object == "StereoMotion")
		{
			current_theta = 90.0 + theta;
			motion = true;
			servo_rotate(wedgeMotors, 3, current_theta);
			servo_rock();
		}

		cerr << current_object << " | " << current_depth << endl;
		
		//phidgets_linear_move(40.0-10.0, axisZ);
		
		// markers file
		string markersFileName = experiment_directory + subjectName + "/" + markersFile_name + subjectName + "_" + task + "_" + "_markers-trial_" + stringify(trialNumber) + ".txt";

		if(experiment)
		{
			markersFile.open(markersFileName.c_str());
			markersFile << fixed << markersFile_headers << endl;
		}

		timer.start();
	#else
		advanceTrial();
	#endif
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	double percent_occluded_frames = (double)framesOccluded/(frameN-frames_at_start-frames_after_grasp);
	bool not_enough_frames = percent_occluded_frames > 0.20;
	
	if(not_enough_frames)
	{
		beepOk(1);
		trial.reinsert(trial.getCurrent());
		good_trial = 0;
		cerr << "Trial will be repeated (couldn't see the fingers for too long)" << endl;
	} else
	{
		good_trial = 1;
	}

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			subjectName << "\t" <<
			trialNumber << "\t" <<
			repetitions << "\t" <<
			trial.getCurrent()["Cue"] << "\t" <<
			FGA << "\t" <<
			timeElapsed << "\t" << 
			endl;
 
 	markersFile.close();

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
			servo_disengage(wedgeMotors, 1);
			servo_disengage(wedgeMotors, 3);
			sleep(1);
			servo_rotate(wedgeMotors, 1, 0.0);
			servo_rotate(wedgeMotors, 3, 90.0);
			servo_close(wedgeMotors);
			switch_disconnect();
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		// fingers calibration
		case 'f':
		case 'F':
		{
			// Record the calibration markers positions
			if ( fingerCalibrationDone==1 && allVisibleCalibrationMarkers && system_calibrated)
			{
				calibration_fingers(fingerCalibrationDone);
				pressingF = true;
				fingerCalibrationDone=2;
				beepOk(3);
			}

			// Triangulate the fingers
			if ( fingerCalibrationDone==2 && allVisibleFingers && !pressingF)
			{
				calibration_fingers(fingerCalibrationDone);
				pressingF = true;
				fingerCalibrationDone=3;
				beepOk(3);
			}

			// Start the experiment
			if ( fingerCalibrationDone==3  && allVisibleFingers && !pressingF)
			{
				fingerCalibrationDone=4;
				pressingF = true;
				beepOk(0);
				home_position = ind;
				training = true;
				initTrial();
			}
		} break;

		case 'e':
		{
			if(training)
			{
				training = false;
				experiment = true;
				beepOk(0);
				trial.next();
				initTrial();
			}
		} break;

		case 13:
		{
				if(allVisibleFingers && !isHandHome)
				{
					if(!iGrasped)
					{
						switches_off();
						FGA = sqrt( pow((ind.x() - thu.x()), 2.0) + pow((ind.y() - thu.y()), 2.0) + pow((ind.z() - thu.z()), 2.0) );
						iGrasped = true;
						cerr << "FGA = " << FGA << endl;
					} else
						cerr << "Response already given." << endl;
				}
		} break;

		case 'c':
		{
			if(allVisibleReferenceMarkers && !experiment)
			{
				system_calibrated = !system_calibrated;
				initCalibration(); 
			}
		} break;

		case 9: // tab
		{
			if(!system_calibrated)
			{
				system_calibrated = true;
				fingerCalibrationDone = 4;
				training = true;
				initTrial();
			} else
			{
				if(training)
				{
					servo_disengage(wedgeMotors, 1);
					servo_disengage(wedgeMotors, 3);
					initTrial();
				} else
				{
					servo_disengage(wedgeMotors, 1);
					servo_disengage(wedgeMotors, 3);
					advanceTrial();
				}
			}
		} break;

		case ' ':
		{
			errorHandlingFunction();
			switches_on();
		} break;
	}
}

/*** Online operations ***/
void online_fingers()
{
	allVisibleReferenceMarkers = isVisible(markers.at(calibration1).p) && isVisible(markers.at(calibration2).p) && isVisible(markers.at(calibration3).p) && isVisible(markers.at(calibration4).p);
	
	allVisibleIndex = isVisible(markers.at(ind1).p) && isVisible(markers.at(ind2).p) && isVisible(markers.at(ind3).p);
	allVisibleThumb = isVisible(markers.at(thu1).p) && isVisible(markers.at(thu2).p) && isVisible(markers.at(thu3).p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisibleCalibrationMarkers = isVisible(markers.at(indexcal).p) && isVisible(markers.at(thumbcal).p);

	if(allVisibleCalibrationMarkers && fingerCalibrationDone == 0)
		fingerCalibrationDone = 1;

		// fingers coordinates, fingersOccluded and framesOccluded
	if ( fingerCalibrationDone >= 3)
	{
		if(allVisibleIndex)
		{
			indexCoords.update(markers.at(ind1).p, markers.at(ind2).p, markers.at(ind3).p );
			#ifndef SIMULATION
				ind = indexCoords.getP1();
			#endif
		}

		if(allVisibleThumb)
		{
			thumbCoords.update(markers.at(thu1).p, markers.at(thu2).p, markers.at(thu3).p );
			#ifndef SIMULATION
				thu = thumbCoords.getP1();
			#endif
		}
	}

		// what the program checks online during the grasp
	if (fingerCalibrationDone==4 )
	{
		if(experiment)
		{
			// Write to responseFile
			markersFile << fixed <<
				parameters.find("SubjectName") << "\t" <<
				trialNumber << "\t" <<
				timer.getElapsedTimeInMilliSec() << "\t" <<				//time
				frameN << "\t" <<										//frameN
				ind.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
				thu.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
				fingersOccluded	<< "\t" <<						//fingersOccluded
				framesOccluded	<<						//framesOccluded
				endl;
		}
	}
}

void online_trial()
{
	pressingF = false;

	if(fingerCalibrationDone==4)
	{
		// fingersOccluded
		if ( allVisibleFingers )
			fingersOccluded = 0;
		else
		{
			fingersOccluded=1;
		}

		if(isHandHome && !iGrasped)
		{
			timer.start();
			frames_at_start++;
		}

		if(!start_motion)
			timer.start();

		if(iGrasped)
		{
			frames_after_grasp++;
		}

		// middle point between index and thumb (center of grasp)
		grip_position = (ind + thu)/2;

		// euclidean distance to home
		dist_to_home = sqrt( pow((ind.x() - home_position.x()),2) + pow((ind.y() - home_position.y()),2) + pow((ind.z() - home_position.z()),2) );

		// is the hand at start?
		if( dist_to_home < 50.0 )
			isHandHome = true;
		else
			isHandHome = false;

		if(!isHandHome && !allVisibleFingers && !iGrasped && handFarFromObject)
			framesOccluded++;

		if(task == "grasp")
		{
			if(objdistance - dist_to_home < 45.0)
			{
				handFarFromObject = false;
				iGrasped = true;
			}
			else
				handFarFromObject = true;
		} else
			handFarFromObject = true;

		if(allVisibleFingers && iGrasped)
		{
			if(isHandHome)
			{
				if(experiment)
				{
					sleep(.5);
					servo_disengage(wedgeMotors, 1);
					servo_disengage(wedgeMotors, 3);
					advanceTrial();
				}
				if(training)
				{
					sleep(.5);
					servo_disengage(wedgeMotors, 1);
					servo_disengage(wedgeMotors, 3);
					initTrial();
				}
			}
		} 
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

	wedgeMotors = servo_connect();

	switch_connect();
	switches_on();

	// initializing glut
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(SCREEN_WIDTH-350, SCREEN_HEIGHT-250);
	glutCreateWindow("Experiment");

//	glutInitWindowPosition(250, 100);
//	glutInitWindowSize(1024, 768); 
//	glutCreateWindow("Window 2");

	// initializing experiment's parameters
	initStreams();

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
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
	if(system_calibrated)
		online_calibrate_markers();
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
    glViewport(0,0,SCREEN_WIDTH-350, SCREEN_HEIGHT-250);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
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
    chdir("/usr/NDIoapi/ndigital/realtime/");
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

void initCalibration()
{
	initReferenceM();

	MatrixXd temp_calibrationM(3,3);
	for (int i=0;i<3;i++) // markers index
	{
		Vector3d temp_marker = markers.at(i+2).p; // markers 2,3,4 correspond to x,y,z
	    for (int j=0;j<3;j++) // coordinates index
	    	temp_calibrationM(i,j) = temp_marker(j); // this inputs the markers vectors rowwise!
  	}

  	calibrationM = temp_calibrationM.transpose(); // transpose so that markers vectors are ordered columnwise
  	origin_raw = markers.at(1).p;

	// translate calibrationM to become calibrationM_tr
	MatrixXd origin_rep(3,3);
	for(int row=0; row<3; row++)
		for(int col=0; col<3; col++)
			origin_rep(row,col) = origin_raw(row);
	
	calibrationM_tr = calibrationM - origin_rep;

	// 5) rotationM = referenceM %*% calibrationM_tr_inv
	rotationM = referenceM * calibrationM_tr.inverse();
}

void online_calibrate_markers()
{
	for ( int i=1; i<=OPTO_NUM_MARKERS; i++)
    {  
        markers.at(i).p -= origin_raw;
        markers.at(i).p = rotationM * markers.at(i).p;
    }
}