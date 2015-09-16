#include "BrownPhidgetsSim.h"

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

//-------------------------------

namespace BrownPhidgetsSim
{
	/***** PHIDGETS *****/

	// common

	int CCONV AttachHandler(CPhidgetHandle MC, void *userptr)
	{

	}

	int CCONV DetachHandler(CPhidgetHandle MC, void *userptr)
	{

	}

	int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description)
	{
	}

	// linear actuator

	void reset_phidgets_linear()
	{
		cerr << "Phidgets axes are reset." << endl;	
	}

	int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value)
	{
		
	}

	int CCONV CurrentChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, int Value)
	{

	}

	void phidgets_linear_move(double desired_distance, int axis)
	{
		cerr << "Phidgets axis " << axis <<" is set to " << desired_distance << endl;
	}

	// stepper

	int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value)
	{

	}


	CPhidgetStepperHandle stepper_connect(void)
	{
		cerr << "Phidgets stepper is connected." << endl;
	}

	void stepper_rotate(CPhidgetStepperHandle phid, double final_theta)
	{
		cerr << "Phidgets stepper is set to " << final_theta << endl;
	}

	void stepper_close(CPhidgetStepperHandle phid)
	{
		cerr << "Phidgets stepper is disconnected." << endl;
	}

}
