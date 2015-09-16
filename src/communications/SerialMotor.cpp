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

#include "SerialStream.h"
#include "SerialMotor.h"

using namespace std;
using namespace boost;
using util::stringify;
using util::str2num;

/** \ingroup Communication
* Default constructor
**/
SerialMotor::SerialMotor(int _comport, int _baudrate)
{  port=_comport;
   baudrate=_baudrate;
   buffer="";
   init(_comport,_baudrate);
}
/** \ingroup Communication
* Default destructor
**/
SerialMotor::~SerialMotor()
{

}

/** \ingroup Communication
* Init the VmxMotor serial port, needed to start the communication, not to be called directly from the main
* \param serialportnumber
* \param baudrate
**/
void SerialMotor::init(int serialportnumber, int baudrate )
{  buffer="";

#ifdef _WIN32
   communicationPort = "COM"+stringify<int>(serialportnumber);
#endif

#ifdef __linux__
   communicationPort = "/dev/ttyS"+stringify<int>(serialportnumber);
#endif
#ifdef __APPLE__
   communicationPort = "/dev/tty"+stringify<int>(serialportnumber);
#endif
   if ( communicationPort=="" )
      throw std::logic_error(string("You must specify a correct serial port identifier i.e. (COM1 or /dev/ttyS4 for example)"));

}

/** \ingroup Communication
* Clear the current buffer to be send to via serial port
**/
void SerialMotor::clearBuffer()
{  buffer.clear();
}

/** \ingroup Communication
* Write the preformed string to the predefined port
**/
void SerialMotor::write()
{  SerialOptions options;
   options.setDevice(communicationPort);
   options.setBaudrate(9600);
   options.setTimeout(boost::posix_time::seconds(60000));   // a great time out is VITAL!!!!
   SerialStream serial(options);
#ifdef DEBUG
   cerr << "Sending: " << buffer << endl;
#endif
   serial << buffer << endl;

   char out=0;
   serial >> out;  // this line blocks this method until "^" is arrived
   if ( out!='^' )
      cerr << "Error in SerialMotor::write()" << endl;
}
