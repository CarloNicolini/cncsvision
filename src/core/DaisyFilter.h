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

#ifndef DAISY_FILTER_H_
#define DAISY_FILTER_H_

#include <Eigen/Core>
/**
 *
 * DaisyFilter
 * (C) 2009 Team 341
 *
 * This class implements a linear, digital filter.  All types of FIR and IIR filters are supported.  Static
 * factory methods are provided to create commonly used types of filters.
 *
 * Filters are of the form:
 *  y[n] = (b0*x[n] + b1*x[n-1] + ... + bP*x[n-P) - (a0*y[n-1] + a2*y[n-2] + ... + aQ*y[n-Q])
 *
 * Where:
 *  y[n] is the output at time "n"
 *  x[n] is the input at time "n"
 *  y[n-1] is the output from the LAST time step ("n-1")
 *  x[n-1] is the input from the LAST time step ("n-1")
 *  b0...bP are the "feedforward" (FIR) gains
 *  a0...aQ are the "feedback" (IIR) gains
 * IMPORTANT!  Note the "-" sign in front of the feedback term!  This is a common convention in signal processing.
 *
 * What can linear filters do?  Basically, they can filter - or diminish - the effects of undesirable input
 * frequencies.  High frequencies - or rapid changes - can be indicative of sensor noise or be otherwise undesirable.
 * A "low pass" filter smooths out the signal, reducing the impact of these high frequency components.  Likewise, a
 * "high pass" filter gets rid of slow-moving signal components, letting you detect large changes more easily.
 *
 * Example FRC applications of filters:
 *  - Getting rid of noise from an analog sensor input (note: the cRIO's FPGA can do this faster in hardware)
 *  - Smoothing out joystick input to prevent the wheels from slipping or the robot from tipping
 *  - Smoothing motor commands so that unnecessary strain isn't put on electrical or mechanical components
 *  - If you use clever gains, you can make a PID controller out of this class!
 *
 * For more on filters, I highly recommend the following articles:
 *  http://en.wikipedia.org/wiki/Linear_filter
 *  http://en.wikipedia.org/wiki/Iir_filter
 *  http://en.wikipedia.org/wiki/Fir_filter
 *
 * Note 1: Rather than enforce an interface for "filtered" components to implement, it was decided that this
 * should be a synchronous function.  That is, Calculate() should be called by the user on a known, regular period.
 * You can set up a Notifier to do this (look at the WPILib PIDController class), or do it "inline" with code in a
 * periodic function.
 *
 * Note 2: For ALL filters, gains are necessarily a function of frequency.  If you make a filter that works
 * well for you at, say, 100Hz, you will most definitely need to adjust the gains if you then want to run it at
 * 200Hz!  Combining this with Note 1 - the impetus is on YOU as a developer to make sure Calculate() gets
 * called at the desired, constant frequency!
 *
 * @author Jared Russell (jared@team341.com)
 */
class DaisyFilter
{
public:
   DaisyFilter(int ffOrder, const double *ffGains, int fbOrder, const double *fbGains);
   virtual ~DaisyFilter();

   // Static factory methods to create commonly used filters
   static DaisyFilter* SinglePoleIIRFilter(double gain);
   static DaisyFilter* MovingAverageFilter(int taps);
   static DaisyFilter* PIDFilter(double Kp, double Ki, double Kd);

   double Calculate(double value);

private:
   // This is a simple circular buffer so we don't need to "bucket brigade" copy old values
   // If we were so inclined, we could use a template here
   class DaisyCircularBuffer
   {
   public:
      DaisyCircularBuffer(int size)
      {  mSize = size;
         mData = new double[size];
         mFront = 0;
      }

      virtual ~DaisyCircularBuffer()
      {  if( mSize > 0 )
         {  delete[] mData;
         }
      }

      void Increment()
      {  mFront++;
         if( mFront >= mSize )
         {  mFront = 0;
         }
      }

      double& operator[] (int index)
      {  return mData[ (index+mFront) % mSize ];
      }

   private:
      int mSize;
      double* mData;
      int mFront;
   };

   int mInputOrder;
   int mOutputOrder;

   DaisyCircularBuffer mInputs;
   DaisyCircularBuffer mOutputs;
   double *mInputGains;
   double *mOutputGains;
};

#endif

