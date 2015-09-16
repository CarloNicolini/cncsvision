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
#include <boost/thread/thread.hpp>
#include "Timer.h"

using namespace std;
using namespace boost;

int i=0;
boost::mutex formutex;
void someLongComputation()
{  while (i<20)
   {  //    boost::mutex::scoped_lock(formutex);
      i++;
      cout << i << endl;
      boost::this_thread::sleep(boost::posix_time::milliseconds(250));
   }
}


void valuePicker()
{  int j=0;
   while ( ++j < 10 )
   {  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
      cerr << "Value of i= " << i << endl;
   }
}
int main(void)
{  int j=0;
   /*
      boost::thread comp(someLongComputation);
      boost::thread value(valuePicker);

      comp.join();
   */
   double t=0;
   int tt=10000;
   Timer time,time2;
   time.start();
   time2.sleep(10);
   //boost::this_thread::sleep(boost::posix_time::microseconds(tt));
   t = time.getElapsedTimeInMicroSec();
   cerr << abs(t - tt) << endl;
   return 0;
}
