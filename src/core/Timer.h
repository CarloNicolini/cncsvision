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

/*
High Resolution Timer.
This timer is able to measure the elapsed time with 1 micro-second accuracy
in both Windows, Linux and Unix system

AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
CREATED: 2003-01-13
UPDATED: 2006-01-13

Copyright (c) 2003 Song Ho Ahn
*/

#ifndef TIMER_H_DEF
#define TIMER_H_DEF

#ifdef _WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

/**
* \class Timer
* \ingroup Mathcommon
* \brief High Resolution Timer.
* This timer is able to measure the elapsed time with 1 micro-second accuracy
* in both Windows, Linux and Unix system

* AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
* CREATED: 2003-01-13
* UPDATED: 2006-01-13
* Copyright (c) 2003 Song Ho Ahn
**/

class Timer
{
public:
   Timer();                                    // default constructor
   ~Timer();                                   // default destructor

   void   start();                             // start timer
   void   stop();                              // stop the timer
   double getElapsedTime();                    // get elapsed time in second
   double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
   double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
   double getElapsedTimeInMicroSec();          // get elapsed time in micro-second
   void sleep(int milliSeconds);
   void sleepMicro(double microseconds);
   void reset();
   double getTimeIntervalInMilliSec();

private:
   double startTimeInMicroSec;                 // starting time in micro-second
   double endTimeInMicroSec;                   // ending time in micro-second
   int    stopped;                             // stop flag
   double deltatime;
#ifdef WIN32
   LARGE_INTEGER frequency;                    // ticks per second
   LARGE_INTEGER startCount;                   //
   LARGE_INTEGER endCount;                     //
#else
   timeval startCount;                         //
   timeval endCount;                           //
#endif
};

#endif
