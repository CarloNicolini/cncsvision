#include "BrownPhidgets.h"

// linear actuators
double start_time = 0.0, movement_time = 0.0, stop_time = 0.0;
double phidgets_linear_status = 0.0;
double phi_distance = 0.0;
double start_acceleration = 80.0, stop_acceleration = 0.0, vel = 20.0;
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

// servo
int result_servo;
const char *err_servo;
double maxAccel_servo, maxVel_servo, minAccel_servo;

//-------------------------------

namespace BrownPhidgets
{
	/***** PHIDGETS *****/

	// common

	int CCONV AttachHandler(CPhidgetHandle MC, void *userptr)
	{
		#ifndef SIMULATION
			int serialNo;
			const char *name;

			CPhidget_getDeviceName (MC, &name);
			CPhidget_getSerialNumber(MC, &serialNo);
			printf("%s %10d attached!\n", name, serialNo);

			return 0;
		#endif
	}

	int CCONV DetachHandler(CPhidgetHandle MC, void *userptr)
	{
		#ifndef SIMULATION
			int serialNo;
			const char *name;

			CPhidget_getDeviceName (MC, &name);
			CPhidget_getSerialNumber(MC, &serialNo);
			printf("%s %10d detached!\n", name, serialNo);

			return 0;
		#endif
	}

	int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description)
	{
		#ifndef SIMULATION
			printf("Error handled. %d - %s\n", ErrorCode, Description);
			return 0;
		#endif
	}

	// linear actuator

	void reset_phidgets_linear()
	{
		#ifndef SIMULATION
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
		#else
			cerr << "Phidgets axes are reset." << endl;	
		#endif
	}

	int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value)
	{
		#ifndef SIMULATION
			printf("Sensor: %d > Value: %d\n", Index, Value);
			return 0;
		#endif
	}

	int CCONV CurrentChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, int Value)
	{
		#ifndef SIMULATION
	//		int re = *(int *)usrptr;
			printf("Motor: %d > Current Draw: %d\n", Index, Value);
	//		*(int *)usrptr = Value;
			return 0;
		#endif
	}

	void phidgets_linear_move(double desired_distance, int axis)
	{
		#ifndef SIMULATION
			int result_linear, result_sensors, numSensors, i;
			const char *err_linear, *err_sensors;
			int resp = 0;
			int index = 1;
			int current_position;
			//double current;
	//		int test = 99;

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
				CPhidgetMotorControl_setAcceleration (motoControl, i, start_acceleration);
				CPhidgetMotorControl_setBraking (motoControl, i, 100.0);
			}

			//CPhidgetMotorControl_setAcceleration (motoControl, 0, start_acceleration);	
			//CPhidgetMotorControl_setAcceleration (motoControl, 1, start_acceleration);

			//------ callbacks
			//CPhidgetMotorControl_set_OnCurrentChange_Handler (motoControl, CurrentChangeHandler, NULL);
			// interface kit
	//		CPhidgetInterfaceKit_set_OnSensorChange_Handler (ifKit, SensorChangeHandler, &test);


			// where is the motor?
			CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);

			// how much does the motor have to travel?
			double distance = desired_distance*10 - current_position;
		
			// if the motor has to travel more than 2 mm (otherwise it's already there so don't move)
			if(abs(distance) > 10.0)
			{
				// where is the motor?
				CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);

				// to-be-traveled distance
				distance = desired_distance*10 - current_position;

				// direction (positive = outwards; negative = inwards)
				int direction = 0;
				if(distance < 0)
					direction = -1;

				if(distance > 0)
					direction = 1;
			
				// set initial velocity	
				double velocity = direction*75.0;

				// motor moves
				CPhidgetMotorControl_setVelocity (motoControl, axis, velocity);

		//		bool travelling = false;

				double part_distance = 0.0, prop_to_travel = 0.0, vel_scaling = 100.0;
				
				bool reduced_speed = false;

				// while the motor is traveling
				while(1)
				{
					// update the motor's position
					CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);

		//			travelling = direction*current_position < direction*desired_distance*10;			

	//				if(travelling)
	//				{
						// partial position
						part_distance = abs(desired_distance*10 - current_position);
						// proportion of distance still to be covered
						prop_to_travel = part_distance / abs(distance);
	/*
						// set a scaling factor for velocity
						//vel_scaling = pow(prop_to_travel, .5);
*/
						if(prop_to_travel < .30 && !reduced_speed)
						{
							vel_scaling = .5;
						//cerr << vel_scaling << "\t" << travelling << endl;
		
						// update velocity (reduce it gradually)
							CPhidgetMotorControl_setVelocity (motoControl, axis, velocity * 
vel_scaling);
							reduced_speed = true;
						}
				//	}

					if(part_distance < 20.0)
						break;
				}

				// motors stop
				CPhidgetMotorControl_setVelocity (motoControl, axis, 0.00);

				CPhidgetInterfaceKit_getSensorValue(ifKit, axis, &current_position);
				cerr << "Final current_position: " << current_position << endl;
	//			cerr << "Test: " << test << endl;
			}
			//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
			CPhidget_close((CPhidgetHandle)motoControl);
			CPhidget_delete((CPhidgetHandle)motoControl);
			CPhidget_close((CPhidgetHandle)ifKit);
			CPhidget_delete((CPhidgetHandle)ifKit);
		#else
			cerr << "Phidgets axis " << axis <<" is set to " << desired_distance << endl;
		#endif
	}

	// stepper

	int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, double Value)
	{
		#ifndef SIMULATION
			printf("Motor: %d > Current Position: %f\n", Index, Value);
			return 0;
		#endif
	}


	CPhidgetStepperHandle stepper_connect(void)
	{
		#ifndef SIMULATION
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
			CPhidgetStepper_setAcceleration(stepper, 0, maxAccel_stepper/2.0);
			CPhidgetStepper_getVelocityMax(stepper, 0, &maxVel_stepper);
			CPhidgetStepper_setVelocityLimit(stepper, 0, maxVel_stepper/2.0);

			// callback
			//CPhidgetStepper_set_OnCurrentChange_Handler (stepper, PositionChangeHandler, NULL);

			//display current motor position if available
			if(CPhidgetStepper_getCurrentPosition(stepper, 0, &curr_pos_stepper) == EPHIDGET_OK)
				printf("Motor: 0 > Current Position: %lld\n", curr_pos_stepper);

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
		#else
			cerr << "Phidgets stepper is connected." << endl;
		#endif
	}

	void stepper_rotate(CPhidgetStepperHandle phid, double final_theta, double step_constant)
	{
		#ifndef SIMULATION
			// how many steps?
			int num_steps = (int)final_theta*step_constant;
			
			// rotate
			CPhidgetStepper_setTargetPosition (phid, 0, num_steps);

			// check if still rotating
			stopped_stepper = PFALSE;
			while(!stopped_stepper)
			{
				CPhidgetStepper_getStopped(phid, 0, &stopped_stepper);
				//usleep(100000);
			}
		#else
			cerr << "Phidgets stepper is set to " << final_theta << endl;
		#endif
	}
/*
	void stepper_rotate(CPhidgetStepperHandle phid, double final_theta)
	{
		#ifndef SIMULATION
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
		#else
			cerr << "Phidgets stepper is set to " << final_theta << endl;
		#endif
	}
*/
	void stepper_set_angle(CPhidgetStepperHandle phid, double desired_theta, double step_constant)
	{
		CPhidgetStepper_setCurrentPosition(phid, 0, (int)desired_theta*step_constant);
	}

	void stepper_close(CPhidgetStepperHandle phid)
	{
		#ifndef SIMULATION
			// disengage
			CPhidgetStepper_setEngaged(phid, 0, 0);

			// close
			CPhidget_close((CPhidgetHandle)phid);
			CPhidget_delete((CPhidgetHandle)phid);
		#else
			cerr << "Phidgets stepper is disconnected." << endl;
		#endif
	}

	// servo
	CPhidgetAdvancedServoHandle servo_connect()
	{
		#ifndef SIMULATION

			//Declare an advanced servo handle
			CPhidgetAdvancedServoHandle servo = 0;

			//create the advanced servo object
			CPhidgetAdvancedServo_create(&servo);

			//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
			CPhidget_set_OnAttach_Handler((CPhidgetHandle)servo, AttachHandler, NULL);
			CPhidget_set_OnDetach_Handler((CPhidgetHandle)servo, DetachHandler, NULL);
			CPhidget_set_OnError_Handler((CPhidgetHandle)servo, ErrorHandler, NULL);

			//Registers a callback that will run when the motor position is changed.
			//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
			//CPhidgetAdvancedServo_set_OnPositionChange_Handler(servo, PositionChangeHandlerServo, NULL);

			//open the device for connections
			CPhidget_open((CPhidgetHandle)servo, -1);

			//get the program to wait for an advanced servo device to be attached
			printf("Waiting for Phidget to be attached....");
			if((result_servo = CPhidget_waitForAttachment((CPhidgetHandle)servo, 10000)))
			{
				CPhidget_getErrorDescription(result_servo, &err_servo);
				printf("Problem waiting for attachment: %s\n", err_servo);
			}

			return servo;
		#else
			cerr << "Phidgets servo is connected." << endl;
		#endif
	}

	void servo_rotate(CPhidgetAdvancedServoHandle phid, int motor, double theta, double velocity)
	{
		#ifndef SIMULATION
			//Set up some initial acceleration and velocity values
			CPhidgetAdvancedServo_getAccelerationMax(phid, motor, &maxAccel_servo);
			CPhidgetAdvancedServo_setAcceleration(phid, motor, maxAccel_servo/2.0);
			CPhidgetAdvancedServo_setVelocityLimit(phid, motor, velocity);

			CPhidgetAdvancedServo_setEngaged(phid, motor, 1);

			// conversion from positions to theta
			double position = 40.0 + theta * 8/9;

			// rotate
			CPhidgetAdvancedServo_setPosition (phid, motor, position);

		#else
			cerr << "Phidgets servo is set to " << theta << endl;
		#endif
	}

	void servo_disengage(CPhidgetAdvancedServoHandle phid, int motor)
	{
		#ifndef SIMULATION
			CPhidgetAdvancedServo_setEngaged(phid, motor, 0);
		#else
			cerr << "Phidget servo is disengaged." << endl;
		#endif
	}

	void servo_close(CPhidgetAdvancedServoHandle phid)
	{
		#ifndef SIMULATION
			CPhidget_close((CPhidgetHandle)phid);
			CPhidget_delete((CPhidgetHandle)phid);
		#else
			cerr << "Phidget servo is disconnected." << endl;
		#endif
	}

}
