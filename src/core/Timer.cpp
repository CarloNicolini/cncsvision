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

//////////////////////////////////////////////////////////////////////////////
// Timer.cpp
// =========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include "Timer.h"

/**
* \ingroup Mathcommon
* Constructor
**/
Timer::Timer()
{
#ifdef WIN32
   int core=0;
   //SetThreadAffinityMask(GetCurrentThread(),(1<<core)-1);
   QueryPerformanceFrequency(&frequency);
   startCount.QuadPart = 0;
   endCount.QuadPart = 0;
#else
   startCount.tv_sec = startCount.tv_usec = 0;
   endCount.tv_sec = endCount.tv_usec = 0;
#endif

   stopped = 0;
   startTimeInMicroSec = 0;
   endTimeInMicroSec = 0;
}

/**
* \ingroup Mathcommon
* Destructor
**/
Timer::~Timer()
{
}

/**
* \ingroup Mathcommon
* Start timer.
* startCount will be set at this point.
**/
void Timer::start()
{  stopped = 0; // reset stop flag
#ifdef WIN32
   QueryPerformanceCounter(&startCount);
#else
   gettimeofday(&startCount, NULL);
#endif
}

/**
* \ingroup Mathcommon
* Stop the timer.
* endCount will be set at this point.
**/

void Timer::stop()
{  stopped = 1; // set timer stopped flag

#ifdef WIN32
   QueryPerformanceCounter(&endCount);
#else
   gettimeofday(&endCount, NULL);
#endif
}



/**
* \ingroup Mathcommon
* Compute elapsed time in micro-second resolution.
* other getElapsedTime will call this first, then convert to correspond resolution.
* \return Elapsed time in microseconds
**/
double Timer::getElapsedTimeInMicroSec()
{
#ifdef WIN32
   if(!stopped)
      QueryPerformanceCounter(&endCount);

   startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
   endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
   if(!stopped)
      gettimeofday(&endCount, NULL);

   startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
   endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

   return endTimeInMicroSec - startTimeInMicroSec;
}


/**
* \ingroup Mathcommon
* Compute elapsed time in millisecond resolution.
* other getElapsedTime will call this first, then convert to correspond resolution.
* \return Elapsed time in millisecond
**/
double Timer::getElapsedTimeInMilliSec()
{  return this->getElapsedTimeInMicroSec() * 0.001;
}



/**
* \ingroup Mathcommon
* Compute elapsed time in seconds resolution.
* other getElapsedTime will call this first, then convert to correspond resolution.
* \return Elapsed time in seconds
**/
double Timer::getElapsedTimeInSec()
{  return this->getElapsedTimeInMicroSec() * 0.000001;
}



/**
* \ingroup Mathcommon
* Same as getElapsedTimeInSec()
* \return Elapsed time in seconds
**/
double Timer::getElapsedTime()
{  return this->getElapsedTimeInSec();
}

/**
* Full the CPU for specified time (in milliSeconds)
* Not very beatiful
**/
void Timer::sleep(int milliSeconds)
{  
   if ( milliSeconds < 0 )
   {	
	    std::cerr << "logical error during sleep time, microseconds < 0" << std::endl;
		milliSeconds = - milliSeconds;
   }
   this->start();
   while ( this->getElapsedTimeInMilliSec() < milliSeconds );
   this->stop();
}

void Timer::sleepMicro(double microseconds)
{  
   if ( microseconds < 0 )
   {	
	    std::cerr << "logical error during sleep time, microseconds < 0" << std::endl;
		microseconds = -microseconds;
   }
   this->start();
   while ( this->getElapsedTimeInMicroSec() < microseconds );
   this->stop();
}

void Timer::reset()
{  this->startTimeInMicroSec=this->endTimeInMicroSec=0;
}

double Timer::getTimeIntervalInMilliSec()
{
#ifdef WIN32
   if(!stopped)
      QueryPerformanceCounter(&endCount);
   deltatime = endCount.QuadPart * (1000000.0 / frequency.QuadPart) - (endTimeInMicroSec - startTimeInMicroSec);
   endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
   if(!stopped)
      gettimeofday(&endCount, NULL);
   deltatime = endTimeInMicroSec - ((endCount.tv_sec * 1000000.0) + endCount.tv_usec);
   endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif
   return (deltatime)/1000;
}
