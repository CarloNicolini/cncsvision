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

#ifndef _DEXAMPLE_H
#define _DEXAMPLE_H

#include "Distribution.h"
using namespace std;

/**
* \class DExample
* \ingroup Statistic
* DExample is a simple model to help user to build its own distribution. Look DBayesianModel to see a real implementation.
*
**/

class DExample : public Distribution
{
	private:
		MatrixXd covar;
		MatrixXd invCovar;
		VectorXd mean;
		bool isStandard;
	
	public:
		DExample(unsigned int);		// standard multivariate normal distribution
		DExample(const VectorXd &mean);	
		DExample(const VectorXd &mean, const MatrixXd &covar);
		DExample(const VectorXd &mean, const MatrixXd &covar, const MatrixXd &invCovar );
		
		~DExample();
		
		
		void setParameters(const VectorXd& mean, const MatrixXd &, const MatrixXd &);
		void setParameters(const VectorXd& mean);
	protected:
		void body(const VectorXd &x) const;

};

#endif
