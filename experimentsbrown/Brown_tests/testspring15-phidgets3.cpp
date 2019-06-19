///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

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

/**** INLCUDE PHIDGETS LIBRARY **********/
#include <stdio.h>
#include <phidget21.h>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "Util.h"
/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

#define SIMULATION

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
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0;
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
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[3];
CylinderPointsStimulus cylinder[3];
Timer timer;
Timer globalTimer;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, ind, thu, platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999);
vector <Marker> markers;
static double interoculardistance=0.0;
#ifdef SIMULATION
vector<Marker> simMarkers;
Timer simTimer;
#endif

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int fingerCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool expFinished=false;
bool expStarted=false;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
double objwidth = 0.0, adjDz = 0.0;
bool isStimulusDrawn = false;
Vector3d object_reset_position(0,0,0), object_position(0,0,0);

/****** PHIDGETS VARIABLES *******/

double start_time = 0.0, movement_time = 0.0, stop_time = 0.0;
double phidgets_linear_status = 0.0;
double phi_distance = 0.0;
double start_acceleration = 100.0, stop_acceleration = 0.0, vel = 20.0;
const int Z_AXIS = 0, Y_AXIS = 1;
double step_theta = 45;
bool first_rotation = true;
int which_axis = 0;

// stepper
int result_stepper;
__int64 curr_pos_stepper;
const char *err_stepper;
double maxAccel_stepper, maxVel_stepper;
int stopped_stepper;
CPhidgetStepperHandle rotTable;

/********** STREAMS **************/
ofstream responseFile, markersFile;


/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void cleanup();
void drawInfo();
void drawStimulus();
void drawGLScene();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void update(int value);
void idle();
void initMotors();
void initGLVariables();
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial();
void initTrial();
void calibration_fingers(int phase);
void calibration_head(int phase);
void drawSquare();
void mouseFunc(int button, int state, int x, int y);
// phidgets common
int CCONV AttachHandler(CPhidgetHandle MC, void *userptr);
int CCONV DetachHandler(CPhidgetHandle MC, void *userptr);
int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description);
// phidgets motors
void reset_phidgets_linear();
void phidgets_linear_move(double distance, int axis);
// phidgets stepper
int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value);
CPhidgetStepperHandle stepper_connect(void);
void stepper_rotate(CPhidgetStepperHandle phid, double final_theta);
void stepper_close(CPhidgetStepperHandle phid);
int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/armlength/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/armlength/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_armlength.txt";

// response file name
string responseFile_name = "spring15-armlength-adj_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjwidth\tadjDepth\ttime";

/*************************** FUNCTIONS ***********************************/

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

/***** PHIDGETS *****/

// common

int CCONV AttachHandler(CPhidgetHandle MC, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (MC, &name);
	CPhidget_getSerialNumber(MC, &serialNo);
	printf("%s %10d attached!\n", name, serialNo);

	return 0;
}

int CCONV DetachHandler(CPhidgetHandle MC, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (MC, &name);
	CPhidget_getSerialNumber(MC, &serialNo);
	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description)
{
	printf("Error handled. %d - %s\n", ErrorCode, Description);
	return 0;
}

// linear actuator

void reset_phidgets_linear()
{
	int result_linear, result_sensors, numSensors, i;
	const char *err_linear, *err_sensors;
	int resp = 0;
	int index = 1;
	int current_position;

	//Declare a motor control handle
	CPhidgetMotorControlHandle motoControl = 0;
	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit = 0;

	//create the motor control object
	CPhidgetMotorControl_create(&motoControl);
	//create the InterfaceKit object
	CPhidgetInterfaceKit_create(&ifKit);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)motoControl, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)motoControl, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)motoControl, ErrorHandler, NULL);
	//interfacekit
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

	//open the motor control for device connections
	CPhidget_open((CPhidgetHandle)motoControl, -1);
	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)ifKit, -1);

	//get the program to wait for a motor control device to be attached
	cerr << "Waiting for MotorControl to be attached...." << endl;
	if((result_linear = CPhidget_waitForAttachment((CPhidgetHandle)motoControl, 10000)))
	{
		CPhidget_getErrorDescription(result_linear, &err_linear);
		cerr << "Problem waiting for attachment: " << err_linear << endl;
	}
	//get the program to wait for an interface kit device to be attached
	printf("Waiting for interface kit to be attached....");
	if((result_sensors = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
	{
		CPhidget_getErrorDescription(result_sensors, &err_sensors);
		printf("Problem waiting for attachment: %s\n", err_sensors);
	}

	//get the number of sensors available
	CPhidgetInterfaceKit_getSensorCount(ifKit, &numSensors);

	//Change the sensitivity trigger of the sensors
	for(i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_setSensorChangeTrigger(ifKit, i, 1);  //we'll just use 10 for fun
	}

	CPhidgetMotorControl_setAcceleration (motoControl, Y_AXIS, start_acceleration);	
	CPhidgetMotorControl_setAcceleration (motoControl, Z_AXIS, start_acceleration);

	// do three stroke, each with the following speeds
	double speed[3] = {100.0, 20.0, 20.0}; // first stroke is fast, the next two are slow for refinements
	for(int stroke = 0; stroke < 3; stroke++)
	{
		// where is the motor?
		CPhidgetInterfaceKit_getSensorValue(ifKit, Y_AXIS, &current_position);
		CPhidgetInterfaceKit_getSensorValue(ifKit, Z_AXIS, &current_position);

		double distance = 0 - current_position;

		int direction = 0;
	
		if(distance < 0)
			direction = -1;

		if(distance > 0)
			direction = 1;
		
		// motors move
		CPhidgetMotorControl_setVelocity (motoControl, Y_AXIS, direction*speed[stroke]);
		CPhidgetMotorControl_setVelocity (motoControl, Z_AXIS, direction*speed[stroke]);

		// when the motors have travelled the required distance, stop it
		while(direction*current_position < direction*phi_distance*10)
		{
			CPhidgetInterfaceKit_getSensorValue(ifKit, Y_AXIS, &current_position);
		}

		CPhidgetInterfaceKit_getSensorValue(ifKit, Z_AXIS, &current_position);

		while(direction*current_position < direction*phi_distance*10)
		{
			CPhidgetInterfaceKit_getSensorValue(ifKit, Z_AXIS, &current_position);
		}

		CPhidgetMotorControl_setVelocity (motoControl, Y_AXIS, 0.00);
		CPhidgetMotorControl_setVelocity (motoControl, Z_AXIS, 0.00);

		// where is the motor?
		CPhidgetInterfaceKit_getSensorValue(ifKit, Y_AXIS, &current_position);
		CPhidgetInterfaceKit_getSensorValue(ifKit, Z_AXIS, &current_position);
	}

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	CPhidget_close((CPhidgetHandle)motoControl);
	CPhidget_delete((CPhidgetHandle)motoControl);
	CPhidget_close((CPhidgetHandle)ifKit);
	CPhidget_delete((CPhidgetHandle)ifKit);
}

void stopMotor(CPhidgetMotorControlHandle motoControl)
{
	CPhidgetMotorControl_setVelocity (motoControl, 1, 0.00);
}

int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *phidgetPtr, int Index, int Value)
{
	CPhidgetMotorControlHandle motoControl = *(CPhidgetMotorControlHandle *)phidgetPtr;
	printf("Sensor: %d > Value: %d\n", Index, Value);
//	if(Value > 500)
//		CPhidgetMotorControl_setVelocity (motoControl, 0, 0.00);
	return 0;
}

void phidgets_linear_move(double desired_distance, int axis)
{

	int result_linear, result_sensors, numSensors, i;
	const char *err_linear, *err_sensors;
//	int resp = 0;
//	int index = 1;
	int current_position = 0, current_position2 = 0;
//	double current;

	//Declare a motor control handle
	CPhidgetMotorControlHandle motoControl = 0;
	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit = 0;

	//create the motor control object
	CPhidgetMotorControl_create(&motoControl);
	//create the InterfaceKit object
	CPhidgetInterfaceKit_create(&ifKit);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)motoControl, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)motoControl, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)motoControl, ErrorHandler, NULL);
	//interfacekit
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

	//open the motor control for device connections
	CPhidget_open((CPhidgetHandle)motoControl, -1);
	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)ifKit, -1);

	//get the program to wait for a motor control device to be attached
	cerr << "Waiting for MotorControl to be attached...." << endl;
	if((result_linear = CPhidget_waitForAttachment((CPhidgetHandle)motoControl, 10000)))
	{
		CPhidget_getErrorDescription(result_linear, &err_linear);
		cerr << "Problem waiting for attachment: " << err_linear << endl;
	}
	//get the program to wait for an interface kit device to be attached
	printf("Waiting for interface kit to be attached....");
	if((result_sensors = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
	{
		CPhidget_getErrorDescription(result_sensors, &err_sensors);
		printf("Problem waiting for attachment: %s\n", err_sensors);
	}

	//get the number of sensors available
	CPhidgetInterfaceKit_getSensorCount(ifKit, &numSensors);

	//Change the sensitivity trigger of the sensors
	for(i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_setSensorChangeTrigger(ifKit, i, 1);  //every 1 ms
	}

	CPhidgetMotorControl_setAcceleration (motoControl, 0, start_acceleration);	
	CPhidgetMotorControl_setAcceleration (motoControl, 1, start_acceleration);
	CPhidgetMotorControl_setBraking (motoControl, 0, 100.0);
	CPhidgetMotorControl_setBraking (motoControl, 1, 100.0);
	
	// interface kit callback
	CPhidgetInterfaceKit_set_OnSensorChange_Handler (ifKit, SensorChangeHandler, &motoControl);

	// where is the motor?
	CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);

	// how much does the motor have to travel?
	double distance = desired_distance*10 - current_position;

	// direction (positive = outwards; negative = inwards)
	int direction = 0;
	if(distance < 0)
		direction = -1;

	if(distance > 0)
		direction = 1;

	// set initial velocity	
	double velocity = direction*100.0;

	// set other motion variables
	bool should_travel = false, slow = false;

	double part_distance = 0.0, prop_to_travel = 0.0, vel_scaling = 100.0;
	
	// if the motor has to travel more than 2 mm (otherwise it's already there so don't move)
	if(abs(distance) > 20.0)
	{
		// where is the motor?
		CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);
//		cerr << "Initial current_position: " << current_position << endl;

		// to-be-traveled distance
		//distance = desired_distance*10 - current_position;

		// motor starts travelling
		CPhidgetMotorControl_setVelocity (motoControl, axis, velocity);

		// while the motor is traveling
		while(1)
		{
			// update the motor's position
			CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);

			// this is the partial position: the distance between the current position 
			// to the final position
			part_distance = abs(desired_distance*10 - current_position);

			// check whether it should continue travelling based on the partial position:
			// if this distance is greater than 2 mm then the motor should continue moving
			should_travel = part_distance > 20.0;			

/*			// if the partial distance is less than 15 mm slow down
			if(part_distance < 15.0 && !slow) // we use slow so that velocity changes only once
			{
				// reduce velocity to 1/4
				velocity = velocity*.25;
				// update velocity
				CPhidgetMotorControl_setVelocity (motoControl, axis, velocity * vel_scaling);
				slow = true;
			}
*/
/*			if(should_travel)
			{
				// proportion of distance still to be covered
				prop_to_travel = part_distance / abs(distance);

				// set a scaling factor for velocity
				//vel_scaling = pow(prop_to_travel, .5);
				//if(prop_to_travel > .75)
				//	vel_scaling = 100.0;
				if(prop_to_travel < .30)
				{
					vel_scaling = .25;
	
					// update velocity (reduce it gradually)
					CPhidgetMotorControl_setVelocity (motoControl, axis, velocity * vel_scaling);
				}

				cerr << vel_scaling << "\t" << part_distance << "\t" << current_position << endl;
			}
*/
//			if(direction*current_position > direction*desired_distance*10)
			if(!should_travel)
				break;
		}

		// motors stop
		CPhidgetMotorControl_setVelocity (motoControl, axis, 0.00);
/*
		// inertia check
		CPhidgetMotorControl_getCurrent (motoControl, axis, &current);
		if(axis == Y_AXIS)
		{
			while(current > 0.3)
			{
				CPhidgetMotorControl_getCurrent (motoControl, axis, &current);
				CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);
//					cerr << "current: " << current << endl;
			}
		}
		if(axis == Z_AXIS)
		{
			while(current > 0.1)
			{
				CPhidgetMotorControl_getCurrent (motoControl, axis, &current);
				CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);
//					cerr << "current: " << current << endl;
			}
		}			// which loop
//		cerr << "Loop #: " << stroke << endl;
*/
		CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);
		cerr << "Final current_position: " << current_position << endl;
	}
	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	CPhidget_close((CPhidgetHandle)motoControl);
	CPhidget_delete((CPhidgetHandle)motoControl);
	CPhidget_close((CPhidgetHandle)ifKit);
	CPhidget_delete((CPhidgetHandle)ifKit);

}

// stepper

int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value)
{
	printf("Motor: %d > Current Position: %lld\n", Index, Value);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////

CPhidgetStepperHandle stepper_connect(void)
{
	//Declare an stepper handle
	CPhidgetStepperHandle stepper = 0;

	//create the stepper object
	CPhidgetStepper_create(&stepper);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)stepper, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)stepper, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)stepper, ErrorHandler, NULL);

	//open the device for connections
	CPhidget_open((CPhidgetHandle)stepper, -1);

	//get the program to wait for an stepper device to be attached
	printf("Waiting for Phidget to be attached....");
	if((result_stepper = CPhidget_waitForAttachment((CPhidgetHandle)stepper, 10000)))
	{
		CPhidget_getErrorDescription(result_stepper, &err_stepper);
		printf("Problem waiting for attachment: %s\n", err_stepper);
	}

	//Set up some initial acceleration and velocity values
	CPhidgetStepper_getAccelerationMax(stepper, 0, &maxAccel_stepper);
	CPhidgetStepper_setAcceleration(stepper, 0, maxAccel_stepper/100.0);
	CPhidgetStepper_getVelocityMax(stepper, 0, &maxVel_stepper);
	CPhidgetStepper_setVelocityLimit(stepper, 0, maxVel_stepper/100.0);

	//display current motor position if available
	if(CPhidgetStepper_getCurrentPosition(stepper, 0, &curr_pos_stepper) == EPHIDGET_OK)
		printf("Motor: 0 > Current Position: %lld\n", curr_pos_stepper);

	//change the motor position
	//we'll set it to a few random positions to move it around

	//Step 1: Position 0
	if(first_rotation)
	{
		printf("Set to position 0 and engage.\n");
		CPhidgetStepper_setCurrentPosition(stepper, 0, 0);
		first_rotation=false;
	}

	// Engage the stepper
	CPhidgetStepper_setEngaged(stepper, 0, 1);

	return stepper;
}

void stepper_rotate(CPhidgetStepperHandle phid, double final_theta)
{
	// how many steps?
	int num_steps = final_theta / 0.1125;
	
	// rotate
	CPhidgetStepper_setTargetPosition (phid, 0, num_steps);

	// check if still rotating
	stopped_stepper = PFALSE;
	while(!stopped_stepper)
	{
		CPhidgetStepper_getStopped(phid, 0, &stopped_stepper);
		//usleep(100000);
	}
}

void stepper_close(CPhidgetStepperHandle phid)
{
	// disengage
	CPhidgetStepper_setEngaged(phid, 0, 0);

	// close
	CPhidget_close((CPhidgetHandle)phid);
	CPhidget_delete((CPhidgetHandle)phid);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/***** OTHERS *****/

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

void calibration_fingers(int phase)
{
	switch (phase)
	{
	case 1:
		{
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
		}
		break;
	case 2:
		{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
		}
		break;
	}
}

void calibration_head(int phase)
{
	switch (phase)
	{
	case 1:
		{
			headEyeCoords.init(markers[3].p-Vector3d(70,0,0),markers[3].p, markers[10].p,markers[11].p,markers[12].p,interoculardistance );

		}
		break;
	case 2:
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
		}
		break;
	}
}

bool motor = false;

void drawInfo()
{
	GLText text;	
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
	text.enterTextInputMode();

	text.draw("-----------LINEAR ACTUATORS-----------");
	text.draw("Press 7 to toggle motors");
	text.draw("Motor: " + stringify<int>(which_axis%2));		
	text.draw(" ");
	text.draw("Press w to increase desired position, or s to decrease desired position");
	text.draw("Then press 8 to move");
	text.draw(" ");
	text.draw("desired position = " + stringify<double>(phi_distance));
	text.draw(" ");
	text.draw("-----------STEPPER-----------");
	text.draw("Press q to increase desired theta, or a to decrease desired theta");
	text.draw("Then press 5 to move");
	text.draw(" ");
	text.draw("desired theta = " + stringify<double>(step_theta));
	text.draw(" ");

	text.leaveTextInputMode();
}

void drawStimulus()
{
	if ( isStimulusDrawn )
	{
		glLoadIdentity();
		glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);

		// Left rear 
		glPushMatrix();
		glTranslated(-objwidth/2, 0, -adjDz/2);
		stimDrawer[0].draw();
		glPopMatrix();

		// Right rear rod
		glPushMatrix();
		glTranslated(objwidth/2, 0, -adjDz/2);
		stimDrawer[1].draw();
		glPopMatrix();

		// Front rod
		glPushMatrix();
		glTranslated(0, 0, adjDz/2);
		stimDrawer[2].draw();
		glPopMatrix();
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
//        drawStimulus();
	drawInfo();

        // Draw right eye view afterwards
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
//        drawStimulus();
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
//        drawStimulus();
	drawInfo();
        glutSwapBuffers();
    }
}

void initTrial()
{
	// draw an empty screen
	isStimulusDrawn=false;

	// set the width of the object to be randomly between 30 and 70 mm
	objwidth = unifRand(30.0, 70.0);

	// set the depth of the object to be randomly between 30 and 70 mm
	adjDz = unifRand(25.0, 75.0);

	// refresh the scene
	drawGLScene();

	// set the monitor at the right ditance
	initProjectionScreen(trial.getCurrent()["AbsDepth"]);

	timer.start();

	// draw the stimulus		
	isStimulusDrawn=true;
}

void advanceTrial()
{
	beepOk(0);
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			trial.getCurrent()["AbsDepth"] << "\t" << 
			objwidth << "\t" << 
			adjDz << "\t" << 
			timeElapsed
			<< endl;

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		isStimulusDrawn=false;
		drawGLScene();
		responseFile.close();
		expFinished = true;
	}

}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		case '7':
		{
			which_axis++;
		} break;
		case '8':
		{
			phidgets_linear_move(phi_distance, which_axis%2);
		} break;
		case 27: // press escape to quit
		{   
			//cleanup(); // clean the optotrak buffer
			stepper_close(rotTable);
			exit(0);
		} break;
		case 'w':
			phi_distance++;
			break;
		case 's':
			phi_distance--;
			break;
		case 'q':
			step_theta++;
			break;
		case 'a':
			step_theta--;
			break;
		case '5':
			stepper_rotate(rotTable, step_theta);
			break;
	}
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

void idle()
{

	updateTheMarkers();

	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	// screen Y alignment check
	screenAlignmentY = asin(
			abs((markers[19].p.y()-markers[21].p.y()))/
			sqrt(
			pow(markers[19].p.x()-markers[21].p.x(), 2) +
			pow(markers[19].p.y()-markers[21].p.y(), 2)
			)
			)*180/M_PI;

	// screen Z alignment check
	screenAlignmentZ = asin(
			abs(markers[19].p.z()-markers[20].p.z())/
			sqrt(
			pow(markers[19].p.x()-markers[20].p.x(), 2) +
			pow(markers[19].p.z()-markers[20].p.z(), 2)
			)
			)*180/M_PI*
			abs(markers[19].p.x()-markers[20].p.x())/
			(markers[19].p.x()-markers[20].p.x());

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

}

void initOptotrak()
{
    optotrak=new Optotrak2();
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

void initVariables()
{

}

void initGLVariables()
{
	// Ora inizializza i 3 rods (stimoli)
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

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{

}

// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(4051,3500);
}

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	// initializes optotrak and velmex motors
	initOptotrak();

	initMotors();
	reset_phidgets_linear();
	rotTable = stepper_connect();

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
	initGLVariables();
//	initStreams();
//	initVariables();

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

	stepper_close(rotTable);
	cleanup();

	return 0;
}
