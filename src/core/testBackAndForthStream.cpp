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
#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>
#include <list>
#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Mathcommon.h"
#include "RigidBody.h"
#include "EulerExtractor.h"
#include "Timer.h"

#include "CoordinatesExtractor.h"
#include "CoordinatesExtractor2.h"
#include "Screen.h"
#include "MatrixStream.h"
#include "Marker.h"
#include "DaisyFilter.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

bool readline(ifstream &is, string &_s, int linenumber )
{  if ( is.eof() )
      return true;
   int l = linenumber-1;
   string junk;
   while ( (l--) )
   {  getline(is,junk);
   }
   getline(is,_s);

   is.clear();
   is.seekg(0,ios::beg);

   return false;
}


int main(int argc, char *argv[])
{
   /*
   ifstream infile;
   infile.open("tmp.txt");
   string txt;

   for (int i=10; i<15; i++)
   {  readline(infile,txt,i);
      cout << txt << endl;
   }
   readline(infile,txt,10);

   for (int i=10; i<15; i++)
   {  readline(infile,txt,i);
      cout << txt << endl;
   }
   */
    ifstream infile;
    infile.open(argv[1]);


    infile.close();
}
