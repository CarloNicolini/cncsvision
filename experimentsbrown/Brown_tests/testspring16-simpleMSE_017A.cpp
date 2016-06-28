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

// integer
int frameN = 0;
int trialNumber = 1;
int fingersOccluded = 0, framesOccluded = 0, good_trial = 0, frames_at_start = 0, frames_after_grasp=0;
int total_trials = 0;

// bool
bool isTrialDone = false;
bool iGrasped = false, isHandHome = true;
bool visibleInfo=true;
bool expFinished=false;
bool experiment=false, training=false;
bool pressingF = false;
bool system_calibrated = false;

// string
string subjectName = "junk";
string student_directory = "empty";

// double
double dist_to_home = 0.0;
double MSE = 0.0;
double grip_aperture = 0.0;
double calibration_markers_distance = 0.0;

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

// stimuli
void drawGLScene();

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

// experiment directory
string experiment_directory = "/media/shapelab/Domini-ShapeLab/CLPS0050/";

// parameters file directory and name
string parametersFile_directory = experiment_directory + "parameters_MSE.txt";

// response file name
string responseFile_name = "MSE_results_";

// summary file headers
string responseFile_headers = "subjName\ttrialN\tMSE\ttrialDuration\tgood_trial";

// markers file name
string markersFile_name = "MSE_trajectories_";

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

	initVariables();

	// Subject name
	subjectName = parameters.find("SubjectName");

	string student_folder = experiment_directory + student_directory;

	mkdir(student_folder.c_str(), 777); // linux syntax

	// Subject folder
	string dirName  = student_folder + "/" + subjectName;

	mkdir(dirName.c_str(), 777); // linux syntax

	// Principal streams files

	// response file
	string responseFileName = student_folder + "/" + responseFile_name + subjectName + ".txt";

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
	/*
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
	}
	return;
#endif
*/
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
					text.draw("####### " + student_directory + "'s experiment #######");
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
				text.draw("Grip Aperture: " +stringify<double>(grip_aperture));

			}
		} else
		{
			text.draw("The experiment is finished.");
		}
		text.leaveTextInputMode();
	}
}

/*** STIMULI ***/
void drawGLScene()
{
	online_fingers();
	online_trial();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
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
	// student directory
	student_directory = parameters.find("student");
	total_trials = str2num<int>(parameters.find("total_trials"));
}

void initTrial()
{
	if(experiment)
		cerr << "\n--------- TRIAL #" << trialNumber << " ----------" << endl;
	else
		cerr << "\n--------- TRAINING ----------" << endl;

		// reset counters
		frameN = 0;
		framesOccluded = 0;
		frames_at_start = 0;
		frames_after_grasp=0;

		// subject did not grasp yet
		iGrasped = false;

		// refresh the scene
		drawGLScene();

		// draw the stimulus	
		beepOk(0);

		// markers file
		string markersFileName = experiment_directory + student_directory + "/" + subjectName + "/" + markersFile_name + subjectName + "_markers-trial_" + stringify(trialNumber) + ".txt";

		if(experiment)
		{
			markersFile.open(markersFileName.c_str());
			markersFile << fixed << markersFile_headers << endl;
		}

		timer.start();
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	double percent_occluded_frames = (double)framesOccluded/(frameN-frames_at_start-frames_after_grasp);
	bool not_enough_frames = percent_occluded_frames > 0.20;
	
	if(not_enough_frames)
	{
		beepOk(1);
		trialNumber--; // so that trialNumber does not increase eventually
		good_trial = 0;
		cerr << "Trial will be repeated (couldn't see the fingers for too long)" << endl;
	} else
	{
		//beepOk(0);
		good_trial = 1;
	}

	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			trialNumber << "\t" <<
			MSE << "\t" <<
			timeElapsed << "\t" << 
			good_trial
			<< endl;
 
	markersFile.close();

	trialNumber++;
	if(trialNumber <= total_trials)
	{
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
		case 'i': // show info
			visibleInfo=!visibleInfo;
			break;

		case 27: // press escape to quit
		{   
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
				initTrial();
			}
		} break;

		case 13:
		{
			if(allVisibleFingers && !isHandHome)
			{
				if(!iGrasped)
				{
					MSE = sqrt( pow((ind.x() - thu.x()), 2.0) + pow((ind.y() - thu.y()), 2.0) + pow((ind.z() - thu.z()), 2.0) );
					iGrasped = true;
					cerr << "MSE = " << MSE << endl;
				} else
					cerr << "Response already given." << endl;
			}
		} break;

		case ' ':
		{
			if(allVisibleFingers && iGrasped)
			{
				if(isHandHome)
				{
					if(experiment)
						advanceTrial();
					if(training)
						initTrial();
				} else
					cerr << "Hand is not at home." << endl;
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

		if(iGrasped)
		{
			frames_after_grasp++;
		}

		// middle point between index and thumb (center of grasp)
		grip_position = (ind + thu)/2;

		// euclidean distance to home
		dist_to_home = sqrt( pow((grip_position.x() - home_position.x()),2) + pow((grip_position.y() - home_position.y()),2) + pow((grip_position.z() - home_position.z()),2) );

		// euclidean distance to home
		grip_aperture = sqrt( pow((ind.x() - thu.x()), 2.0) + pow((ind.y() - thu.y()), 2.0) + pow((ind.z() - thu.z()), 2.0) );

		// euclidean distance to home
		calibration_markers_distance = sqrt( pow((markers.at(calibration2).p.x() - markers.at(calibration1).p.x()), 2.0) + pow((markers.at(calibration2).p.y() - markers.at(calibration1).p.y()), 2.0) + pow((markers.at(calibration2).p.z() - markers.at(calibration1).p.z()), 2.0) );

		// is the hand at start?
		if( dist_to_home < 50.0 )
			isHandHome = true;
		else
			isHandHome = false;

		if(!isHandHome && !allVisibleFingers && !iGrasped)
			framesOccluded++;
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

	// initializes optotrak and velmex motors
	initOptotrak();

	// initializing glut
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH-350, SCREEN_HEIGHT-250);
	glutCreateWindow("Experiment");

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