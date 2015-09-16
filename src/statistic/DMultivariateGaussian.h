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

#ifndef _MULTIVARIATEGAUSSIAN_H
#define _MULTIVARIATEGAUSSIAN_H

#include "Distribution.h"
using namespace std;

/**
* \class DMultivariateGaussian
* \ingroup Statistic
* DMultivariateGaussian models a multivariate gaussian with given dimension, mean vector and covariance matrix.
* The model is:
* \f[
* p(\mathbf{x},\mathbf{y}) = \int  \limits_{\Omega , \Omega_0 } \exp \left( -\frac{1}{2}(x-m)^T C^{-1} (x-m) \right) d \mathbf{x}
* \f]
* XXX CORREGGERE!!!
**/

class DMultivariateGaussian : public Distribution
{
	private:
		MatrixXd covar;
		MatrixXd invCovar;
		VectorXd mean;
		bool isStandard;
	
	public:
		DMultivariateGaussian(unsigned int);		// standard multivariate normal distribution
		DMultivariateGaussian(const VectorXd &mean);	
		DMultivariateGaussian(const VectorXd &mean, const MatrixXd &covar);
		DMultivariateGaussian(const VectorXd &mean, const MatrixXd &covar, const MatrixXd &invCovar );
		
		~DMultivariateGaussian();
		
		
		void setParameters(const VectorXd& mean, const MatrixXd &, const MatrixXd &);
		void setParameters(const VectorXd& mean);
	protected:
		void body(const VectorXd &x) ;

};

#endif
