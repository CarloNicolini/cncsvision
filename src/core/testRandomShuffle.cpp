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
#include "Util.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

#define NPOINTS 1E2

void randomizeSeed(void)
{  int stime;
   long ltime;
   ltime = time(NULL);
   stime = (unsigned) ltime / 2;
   srand(stime);
}


int main(int argc, char *argv[])
{  /*
   deque <int> x;
   for (int i=0; i<10; i++)
           for (int j=0; j<1; j++)
                   x.push_back(j);

          random_shuffle( x.begin(), x.end() );

          deque<int>::iterator iter2;
          while ( adjacent_find(x.begin(), x.end())!=x.end() )
          {
          cerr << "Randomize" << endl;
          random_shuffle( x.begin(), x.end() );
          }
          for ( deque<int>::iterator iter = x.begin(); iter!=x.end(); ++iter )
           cout << *iter << endl;
           */
   string a("1_1_2_3");
   vector<int> ai = util::str2num<int>(a,"_");
   for (int i=0; i<ai.size(); i++)
      //cout << ai[i] << endl;

      cout << factorial<int>(3) << " " << factorial<int>(5) << " " << factorial<int>(0) <<  " " << factorial<int>(1) << endl;
}
