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
#include "VmxLinearMotor.h"
#include "VmxRotationMotor.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace Eigen;

void exitWithHelp()
{  cerr << "Move motor via RS232 commands" << endl;
   cerr << "Usage: ./moveMotor -m [COM] -x [dx] -y [dy] -z [dz]" << endl;
   cerr << "Example: translate the object 10 mm right, 20 mm up, 30 mm far" << endl;
   cerr << "./moveMotor -m 3 -x 10 -y 20 -z -30" << endl;
   cerr << "(Carlo Nicolini, 21 August 2011) carlo.nicolini@iit.it" << endl;
   exit(1);
}

struct parameters
{  int motor;
   int comport;
   int speed;
   bool home;
   double dx,dy,dz;
   double angle;

} param;


void parseCommandLine(int argc, char **argv )
{  // default values
   param.motor=1;
   param.dx=0;
   param.dy=0;
   param.dz=0;
   param.comport=3;
   param.speed=1000;
   param.home=false;

   // parse options
   for(int i=1; i<argc; i++)
   {  if(argv[i][0] != '-') break;
      if(++i>=argc)
         exitWithHelp();
      switch(argv[i-1][1])
      {  case 'm':
         {  param.motor=atoi(argv[i]);
            if ( param.motor < 1 || param.motor > 4 )
            {  cerr << "Invalid motor" << endl;
               exitWithHelp();
            }
            if ( param.motor==4 )
            {  param.angle=atof(argv[i+1]);
            }
            cerr << "Selected motor " << param.motor << endl;

         }
         break;
         case 'x':
         {  param.dx=atof(argv[i]);
         }
         break;
         case 'y':
         {  param.dy=atof(argv[i]);
         }
         break;
         case 'z':
         {  param.dz=atof(argv[i]);
         }
         break;
         case 's':
         {  param.speed=atoi(argv[i]);
            break;
         }
         case 'r':
         {  cerr << "Bringing all motors to their limit switch" << endl;
            param.home=true;
         }
         break;
         default:
            fprintf(stderr,"Unknown option: -%c\n", argv[i-1][1]);
            exitWithHelp();
      }
   }
}


int main(int argc, char** argv)
{

   VmxLinearMotor linearMotor;
   VmxRotationMotor rotMotor;

   parseCommandLine(argc,argv);

   if ( param.home )
   {  linearMotor.home3d(VmxLinearMotor::OBJECTCOMPORT,1000);
      linearMotor.home3d(VmxLinearMotor::MONITORCOMPORT,1000);
      linearMotor.home3d(VmxLinearMotor::MIRRORCOMPORT,1000);
   }
   if ( param.motor==4)
      rotMotor.rotate(param.angle, param.speed);
   else


      /*
        if ( argc == 2 )
        {
          VmxRotationMotor rotMotor;
          rotMotor.rotate(atof(argv[1]) , 4000  );
          return 0;
        }

        if ( argc == 1 )
        {
      exitWithHelp();
        }
        if ( string(argv[1])=="HOME" )
        {
      cerr << "Moving ALL motors to their limit switch" << endl;
      motor.home3d(VmxLinearMotor::OBJECTCOMPORT,1000);
      motor.home3d(VmxLinearMotor::MONITORCOMPORT,1000);
      motor.home3d(VmxLinearMotor::MIRRORCOMPORT,1000);
        }
        else
        {
        if ( argc != 5 && argc!=1 )
        {
      exitWithHelp();
        }
        else
        {
      Vector3d delta( atof(argv[2]),atof(argv[3]),atof(argv[4]));
      motor.move3d( atoi(argv[1]),2000,delta);
        }
        }

        */
      return 0;
}

