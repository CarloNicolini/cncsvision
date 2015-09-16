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
#include <vector>

using namespace std;
using namespace boost;

class background_task
{
private:
   std::vector<int> numbers;
public:
   background_task()
   {  i=0;
      numbers.assign(6000,0);
   }
   void someLongComputation()
   {  while (++i )
      {  boost::mutex::scoped_lock(formutex);
         // cout << "Thread inside: i= " << this->i << endl;
         numbers.at(0)=i;
         //cout << "Thread inside numbers= " << numbers.at(0) << endl;
         //boost::this_thread::sleep(boost::posix_time::milliseconds(1));
      }
   }
   int& getI()
   {  return i;
   }

   std::vector<int>& getNumbers()
   {  boost::mutex::scoped_lock(formutex);
      return numbers;
   }
   int i;
};

background_task f;

void valuePicker()
{  int j=0;
   while ( (j++) < 20 )
   {  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
      //    cerr << "Value of i= " << f.getI() << endl;
      cout  << "First number= " << f.getNumbers().at(0) << endl;
   }
}

int main(void)
{

   boost::thread comp(boost::bind(&background_task::someLongComputation, &f));
   boost::thread value(valuePicker);

   comp.join();
   return 0;
}
