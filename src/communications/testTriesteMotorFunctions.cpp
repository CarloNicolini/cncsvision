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

//A serial class that appears as an iostream

#include <iostream>
#include <Eigen/Core>
#include "SerialStream.h"
#include "TriesteMotorFunctions.h"

using namespace std;
using namespace Eigen;
using namespace boost::posix_time;

int main(int argc, char* argv[])
{  double homeFocalDistance=-387;
   cerr << "Insert home focal distance" << endl;
   cin >> homeFocalDistance;

   cerr << "Select one of the following functions, press a number in [0,15] then enter" << endl;

   cerr << "0)     homeObject"<< endl;
   cerr <<"1)      homeObjectAsynchronous" << endl;
   cerr << "2)     homeMirror"<< endl;
   cerr <<"3)      homeMirrorAsynchronous" << endl;
   cerr <<"4)      homeMonitor" << endl;
   cerr <<"5)      homeMonitorAsynchronous" << endl;
   cerr <<"6)      homeEverything" << endl;
   cerr << "7)     homeEverythingAsynchronous" << endl;
   cerr <<"8)      moveObjectAbsolute" << endl;
   cerr <<"9)      moveObjectAbsoluteAsynchronous" << endl;
   cerr << "10)   moveMirrorAbsolute" << endl;
   cerr << "11)   moveMirrorAbsoluteAsynchronous" << endl;
   cerr << "12)   moveMonitorAbsolute" << endl;
   cerr << "13)   moveMonitorAbsoluteAsynchronous"<< endl;
   cerr << "14)   moveMirrorRelative" << endl;
   cerr << "15)   moveObject" << endl;


   int function=0;
   cin >> function;
   switch  (function )
   {  case 0:
      {  cerr << "homeObject"<< endl;
         cerr << "Dovrebbe spostare l'oggetto a home" << endl;
         TriesteMotorFunctions::homeObject(2500);
      }
      break;
      case 1:
      {  cerr <<"homeObjectAsynchronous" << endl;
         cerr << "Dovrebbe spostare l'oggetto a home (ASINCRONA)" << endl;
         TriesteMotorFunctions::homeObjectAsynchronous(2500);
      }
      break;
      case 2:
      {  cerr << "homeMirror"<< endl;
         cerr << "Dovrebbe spostare a home il mirror" << endl;
         TriesteMotorFunctions::homeMirror(2500);
      }
      break;
      case 3:
      {  cerr <<"homeMirrorAsynchronous" << endl;
         cerr << "Dovrebbe spostare a home il mirror (ASINCRONO)" << endl;
         TriesteMotorFunctions::homeMirrorAsynchronous(2500);
      }
      break;
      case 4:
      {  cerr <<"homeMonitor" << endl;
         cerr << "Dovrebbe spostare a home il mirror" << endl;
         TriesteMotorFunctions::homeMonitor(2500);
      }
      break;
      case 5:
      {  cerr <<"homeMonitorAsynchronous" << endl;
         cerr << "Dovrebbe spostare il monitor a home (ASINCRONO)" << endl;
         TriesteMotorFunctions::homeMonitorAsynchronous(2500);
      }
      break;
      case 6:
      {  cerr <<"homeEverything" << endl;
         TriesteMotorFunctions::homeEverything(2500,2500,2500);
      }
      break;
      case 7:
      {  cerr << "homeEverythingAsynchronous" << endl;
         TriesteMotorFunctions::homeEverythingAsynchronous(2500,2500,2500);
      }
      break;
      case 8:
      {  cerr <<"moveObjectAbsolute" << endl;
         TriesteMotorFunctions::moveObjectAbsolute(Vector3d(10,10,10), Vector3d(0,0,0), 2500);
      }
      break;
      case 9:
      {  cerr <<"moveObjectAbsoluteAsynchronous" << endl;
         TriesteMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(0,0,0), Vector3d(0,0,0), 2500);
      }
      break;
      case 10:
      {  cerr << "moveMirrorAbsolute" << endl;
         TriesteMotorFunctions::moveMirrorAbsolute(-4100,homeFocalDistance, 2500);

      }
      break;
      case 11:
      {  cerr << "moveMirrorAbsoluteAsynchronous" << endl;
         TriesteMotorFunctions::moveMirrorAbsoluteAsynchronous(-4100,homeFocalDistance, 2500);

      }
      break;
      case 12:
      {  cerr << "moveMonitorAbsolute" << endl;
         TriesteMotorFunctions::moveMonitorAbsolute(-400,homeFocalDistance,2500);
      }
      break;
      case 13:
      {  cerr << "moveMonitorAbsoluteAsynchronous"<< endl;
         TriesteMotorFunctions::moveMonitorAbsoluteAsynchronous(-400,homeFocalDistance,2500);
      }
      break;
      case 14:
      {  cerr << "moveMirrorRelative" << endl;
         TriesteMotorFunctions::moveMirror(100);
      }
      break;
      case 15:
      {  cerr << "moveObject" << endl;
         TriesteMotorFunctions::moveObject(Vector3d(10,10,10),2500);
      }
      break;
   }
   cerr << "::::::::::" << endl;
   // boost::thread::sleep(5);

   return 0;
}

