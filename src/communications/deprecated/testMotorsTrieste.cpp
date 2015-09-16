// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include "SerialStream.h"
#include "TriesteMotorFunctions.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace Eigen;

void exitWithHelp()
{  cerr << "Move motor in Trieste configuration via RS232 commands" << endl;
    cerr << "Usage: ./testMotorsTrieste" << endl;
    cerr << "(Carlo Nicolini, 7 November 2012) carlo.nicolini@iit.it" << endl;
    exit(1);
}

int main(int argc, char** argv)
{
    int c=3;
    cout << "Which port drive, COM3 or COM5? Press 3 or 5 then Enter" << endl;
    cin >> c;
    if (c!=3 && c!=5)
    {
        cout << "You must select 3 or 5!" << endl;
        cin.ignore(1E6,'\n');
        return 0;
    }

    int thing=0;
    cout << "Select 1 for monitor movement, 2 for mirror movement, 3 for object movement" << endl;
    cin >> thing;
    if (thing!=1 && thing!=2 && thing!=3)
    {
        cout << "Invalid option, select 1 or 2 or 3" << endl;
        cin.ignore(1E6,'\n');
        return 0;
    }



    switch (thing)
    {
    case 1: // Monitor
    {
        cout << "Moving 1 cm to the right" << endl;
        VmxLinearMotor motor;
        motor.moveSteps(3,1,10.0,3500,5E-3f);
        cout << "Moving 1 cm to the toward" << endl;
        motor.moveSteps(3,2,10.0,3500,5E-3f);
        return 0;
    }
        break;
    case 2: // Mirror
    {
        cout << "Moving mirror 1 cm to toward" << endl;
        VmxLinearMotor motor;
        motor.moveSteps(5,1,10.0,3500,5E-3f);
        return 0;
    }
        break;
    case 3: // Object
    {
        cout << "Moving object 1 cm right, 1 cm up, 1 cm toward" << endl;
        VmxLinearMotor motor;
        motor.moveSteps(5,2,-10.0,3500,5E-3f);// right
        motor.moveSteps(5,3,-10.0,3500,5E-3f);// up
        motor.moveSteps(5,4,10.0,3500,5E-3f);// lontano
        return 0;
    }
        break;
    }

    return 0;
}

