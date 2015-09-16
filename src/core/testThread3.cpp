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
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/shared_mutex.hpp>

using namespace std;
using namespace boost;


class background_task
{
private:
   std::vector<int> numbers;
   boost::shared_mutex myLock;

public:
   background_task()
   {  i=0;
      numbers.assign(6000,0);
   }
   void someLongComputation()
   {  while (++i < 1000 )
      {  boost::upgrade_lock<boost::shared_mutex> write_lock(myLock, boost::try_to_lock);

         cout << "Write= " << this->numbers.at(0) << endl;
         numbers.at(0)=i;
         boost::this_thread::sleep(boost::posix_time::milliseconds(10));

      }
   }

   int& getI()
   {  return i;
   }

   std::vector<int>& getNumbers()
   {  boost::shared_lock<boost::shared_mutex> lock(myLock, boost::try_to_lock);
      //if (lock)
      while ( !lock )
      {  return numbers;
      }

   }

   int getNumbers(size_t i)
   {  boost::shared_lock< boost::shared_mutex > readLock(myLock);
      return numbers.at(i);
   }
   int i;
};

background_task f;

void valuePicker()
{  int j=0;
   while ( (j++) < 20000 )
   {  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
      cout  << "\t" << "Read= "<< f.getNumbers().at(0) << " " << f.getNumbers(0) << endl;
   }
}

int main(void)
{

   boost::thread comp(boost::bind(&background_task::someLongComputation, &f));
   boost::thread value(valuePicker);

   comp.join();
   return 0;
}
