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

#ifndef _DISTRIBUTION_H
#define _DISTRIBUTION_H

#include <iostream>
#include <fstream>

#include <Eigen/Core>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace std;
using namespace Eigen;

/**
*
* \defgroup Statistic
* \ingroup Statistic
* \class Distribution
* \brief Distribution is a class needed to abstract the parameters of a distribution over a domain of \f$ \mathbf{x}_1, \mathbf{x}_2, \ldots , \mathbf{x}_n \f$
*       Overloaded operators permits the user to simply call distribution objects as normal functions
**/

class Distribution
{
	protected:
		mutable double result;
		virtual void body(const VectorXd &) = 0;
		
	public:
		virtual ~Distribution() {};
		inline double &value() { return result; };
		inline double &eval( const VectorXd &x )
		{
			body(x); return result;
		};
		
		double &operator() () { return result; };
		double &operator() (VectorXd &x) { return eval(x); };
		
		unsigned int dimension;
		double val;
};




#endif
