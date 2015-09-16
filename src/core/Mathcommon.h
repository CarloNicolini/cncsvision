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


#ifndef _MATHCOMMON_H_
#define _MATHCOMMON_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <Eigen/Core>
#include "MersenneTwister.h"
#include <stdexcept>
#include <typeinfo>

// For thread-safe random number generation
#include <boost/random.hpp>

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

#ifdef _WIN32
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#define fpu_error(x) (isinf(x) || isnan(x))
#endif

#ifndef SQR
#define SQR(c)  ((c) * (c))
#endif

using namespace std;
using namespace Eigen;

static boost::mt19937 rng ;  // a core engine class, set the seed to be somewhat random
/**
*
* \defgroup Mathcommon
* \brief This namespace contains various mathematical functions and utilities, such as random number generations functions, input/output functions, string utility, tokenizers and degree-radians conversion
*
* \namespace mathcommon
* \brief This namespace contains various mathematical functions and utilities, such as random number generations functions, input/output functions, string utility, tokenizers and degree-radians conversion
**/
namespace mathcommon
{
    double toDegrees(double );
    Eigen::Vector3d toDegrees( const Eigen::Vector3d &x );
    double toRadians(double );
    Eigen::Vector3d toRadians( const Eigen::Vector3d &x );

    double signum(double);
    double unifRand();
    double unifRand(double a, double b);
    int   unifRand(int a, int b);
    double unifRandExc(double);
    long   unifRand(long n);
    int unifPosOrNeg();
    double gaussRand();
    double gaussRand(double , double );
    void randomizeStart();
    double mod(double x, double y);
    bool isVisible(const Vector3d &v);
    double sawtooth(double x, double period);
    double trianglewave(double x, double period);
    template <typename T> T factorial(T value)
    {  T n=1;
       T result = 1;
       for ( n = 1; n <= value; n++ )
       {  result *= n;
       }
       return result;
    }

    /** \ingroup Mathcommon
    * \brief Returns the value "value" brought into the range [ Min, Max ]
    *          If the value is greater than "Max", return Max. If the value is
    *          less than "Min", return Min. Otherwise, return the original value.
    *
    * \param Value   The value to be clamped
    * \param Min    The low point of the range to be clamped to
    * \param Max    The high point of the range to be clamped to
    */
    template<typename T>
    T clamp(T Value, T Min, T Max)
    {  return (Value < Min)? Min : (Value > Max)? Max : Value;
    }

    /** \ingroup Geometry
    * \brief Mimic the MatLab/Octave meshgrid function
    * \param xMin Start x value
    * \param xMax End x value
    * \param xStep Step size for X domain
    * \param yMin Start y value
    * \param yMax End y value
    * \param yStep Step size for Y domain
    * \param X Matrix of X values
    * \param Y Matrix of Y values
    **/
    template < typename Derived, typename Scalar >
    void meshgrid( Scalar xMin, Scalar xMax, Scalar xStep, Scalar yMin, Scalar yMax, Scalar yStep, Eigen::MatrixBase<Derived> &X, Eigen::MatrixBase<Derived> &Y )
    {  assert(xMax > xMin);
       assert(yMax > yMin);
       assert(xStep > 0);
       assert(yStep > 0);

       int nX = (int)(abs(xMax-xMin)/xStep)+1;
       int nY = (int)(abs(yMax-yMin)/yStep)+1;

       Eigen::Matrix<Scalar,1,Dynamic> xVector = Eigen::Matrix<Scalar,1,Dynamic>::LinSpaced(Sequential,nX,xMin,xMax);
       Eigen::Matrix<Scalar,Dynamic,1> yVector = Eigen::Matrix<Scalar,Dynamic,1>::LinSpaced(Sequential,nY,yMin,yMax);

       X= xVector.replicate(nY,1);
       Y= yVector.replicate(1,nX);
    }


    // http://en.wikipedia.org/wiki/Ellipse
    double ellipseCircumferenceBetter(double a,double b);
	double getParaboloidArea(double radius, double curvature);
}   //end namespace

#endif  /* _Mathcommon_H */

