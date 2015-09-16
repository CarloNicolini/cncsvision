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
#include <boost/thread.hpp>
#include "BufferedAsyncSerial.h"
#include "AsyncSerial.h"

#include "SerialMotor.h"

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{  /*
   try {
       BufferedAsyncSerial serial("/dev/ttyS0",115200);

       //Return immediately. String is written *after* the function returns,
       //in a separate thread.
       serial.writeString("Hello world\n");

       //Simulate doing something else while the serial device replies.
       //When the serial device replies, the second thread stores the received
       //data in a buffer.
       this_thread::sleep(posix_time::seconds(2));

       //Always returns immediately. If the terminator \r\n has not yet
       //arrived, returns an empty string.
       cout<<serial.readStringUntil("\r\n")<<endl;

       serial.close();

   } catch(boost::system::system_error& e)
   {
       cout<<"Error: "<<e.what()<<endl;
       return 1;
   }
   */
   SerialMotor motor(3,9600);
   motor.write();
   motor.write();
   this_thread::sleep(posix_time::seconds(2));
}
