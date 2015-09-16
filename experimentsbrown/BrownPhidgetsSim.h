#ifndef _BROWN_PHIDGETS_H_
#define _BROWN_PHIDGETS_H_
#include <iostream>
#include <boost/thread/thread.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>
#include <stdio.h>
#include <phidget21.h>

using namespace std;

namespace BrownPhidgetsSim
{
	// phidgets common
	int CCONV AttachHandler(CPhidgetHandle MC, void *userptr);
	int CCONV DetachHandler(CPhidgetHandle MC, void *userptr);
	int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description);
	// phidgets motors
	void reset_phidgets_linear();
	void phidgets_linear_move(double distance, int axis);
	int CCONV CurrentChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, double Value);
	// phidgets stepper
	int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value);
	CPhidgetStepperHandle stepper_connect(void);
	void stepper_rotate(CPhidgetStepperHandle phid, double final_theta);
	void stepper_close(CPhidgetStepperHandle phid);
	// phidgets interaface kit
	int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);
}


#endif
