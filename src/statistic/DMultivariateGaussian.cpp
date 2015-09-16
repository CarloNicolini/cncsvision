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

#include <Eigen/Core>
#include <Eigen/LU>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Distribution.h"
#include "DMultivariateGaussian.h"


using namespace std;
using namespace Eigen;


DMultivariateGaussian::DMultivariateGaussian(unsigned int _dimension)
{
	if (dimension < 1 )
	{
		cerr << "WTF? Zero dimensional multivariate gaussian??? nDimensions should be >= 1 " << endl;
		exit(0);
	}
	dimension = _dimension;
	covar = MatrixXd::Identity(dimension,dimension);
	invCovar = covar;
	mean.setZero(dimension);
	
	isStandard=true;
				
}
void DMultivariateGaussian::setParameters(const VectorXd& _mean)
{
	mean=_mean;
	if (mean!=VectorXd::Zero(dimension) )
		isStandard=false;
}

void DMultivariateGaussian::setParameters(const VectorXd&  _mean, const MatrixXd &_covar, const MatrixXd &_invCovar)
{
	mean=_mean;
	covar=_covar;
	invCovar=_invCovar;
	
	if (covar==MatrixXd::Identity(dimension,dimension) && mean==VectorXd::Zero(dimension) && invCovar==MatrixXd::Identity(dimension,dimension) )
		isStandard=true;
	else
		isStandard=false;
}

void DMultivariateGaussian::body(const VectorXd &x )
{
	// Look http://en.wikipedia.org/wiki/Multinomial_distribution
	
	double n=x.size();
	double value=0.0;
	if (isStandard)
	{
		double factor1 = 1.0/pow(2.0*M_PI,n/2.0);
		double factor2 = 1.0/( sqrt(abs(covar.determinant()) ) );

		VectorXd dx = x-mean;
		RowVectorXd dxt =  dx.transpose();
	
		VectorXd x1 = invCovar*dx;

		double arg = -0.5* (x1.dot(dxt));
		value = factor1*factor2*exp(arg);
	}
	else
	{
		double factor1 = 1.0/pow(2.0*M_PI,n/2.0);
		double sigma=1.0;
		double factor2 = 1.0/sigma;
	
		VectorXd dx = x-mean;
		RowVectorXd dxt =  dx.transpose();
	
		VectorXd x1 = invCovar*dx;

		double arg = -0.5* (x1.dot(dxt));
		value = factor1*factor2*exp(arg);	
	}
	result = value;
}


DMultivariateGaussian::~DMultivariateGaussian()
{

}

