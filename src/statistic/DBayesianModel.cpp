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

#define _USE_MATH_DEFINES
#include <math.h>

#include "Distribution.h"
#include "DBayesianModel.h"


using namespace std;
using namespace Eigen;

#ifndef SQR
#define SQR(c)  ((c) * (c))
#endif

/**
* Set the dimension of this bayesian model. This method must always be called!
* @param d Number of parameters of the model
* 
**/
DBayesianModel::DBayesianModel(unsigned int d)
{
	ready=false;
	if (d!=6)
	{
		cerr << "This model is designed to work only with 6 dimensions, i.e. [ omegax, omegay, omegaz, omega0x, omega0y, omega0z ] " << endl;
		exit(0);
	}
	dimension = d;
	
}

void DBayesianModel::setSigmaG(double _sigmaGx, double _sigmaGy )
{
        sigmaGx=_sigmaGx;
        sigmaGy=_sigmaGy;
        ready=false;
}

/** 
* \ingroup Statistic
* Set the sigma and omega of the model
* @param Standard deviation of rotation on x  gaussian p.d.f
* @param _sigmaOmega0y Standard deviation of rotation on y  gaussian p.d.f
* @ _sigmaOmega0z  Standard deviation of rotation on z  gaussian p.d.f
**/
void DBayesianModel::setSigmaOmega(double _sigmaOmegax,double _sigmaOmegay,double _sigmaOmegaz)
{
        sigmaOmegax=_sigmaOmegax;
        sigmaOmegay=_sigmaOmegay;
        sigmaOmegaz=_sigmaOmegaz;
        ready=false;
}

/** 
* \ingroup Statistic
* Set the sigma and omega of the model
* @param Standard deviation of translation on x  gaussian p.d.f
* @param _sigmaOmega0y Standard deviation of translation on y  gaussian p.d.f
* @ _sigmaOmega0z  Standard deviation of translation on z  gaussian p.d.f
**/
void DBayesianModel::setSigmaOmega0(double _sigmaOmega0x, double _sigmaOmega0y,double _sigmaOmega0z)
{
        sigmaOmega0x=_sigmaOmega0x;
        sigmaOmega0y=_sigmaOmega0y;
        sigmaOmega0z=_sigmaOmega0z;
        ready=false;
}
/** 
* \ingroup Statistic
* Set the gx,gy of the model. This method must always be called!
* @param gx \f[ g_x \f]
* @param gy \f[ g_y \f]
**/
void DBayesianModel::setGxGy(double _gx, double _gy)
{
	        gx=_gx;
	        gy=_gy;
	        ready=false;
}
/** \ingroup Statistic
* set the \f$ \mathbf{a} \f$ average coefficients
**/
void DBayesianModel::setAMean(double ma1, double ma2, double ma3, double ma4, double ma5 , double ma6 )
{
	ma[0]=ma1;
	ma[1]=ma2;
	ma[2]=ma3;
	ma[3]=ma4;
	ma[4]=ma5;
	ma[5]=ma6;
	ready=false;
}

void DBayesianModel::setOmega0Mean(double _mOmega0x,double _mOmega0y,double _mOmega0z)
{

        mOmega0x=_mOmega0x;
        mOmega0y=_mOmega0y;
        mOmega0z=_mOmega0z;
        ready=false;
        
}

void DBayesianModel::setOmegaMean(double _mOmegax, double _mOmegay, double _mOmegaz)
{
        mOmegax=_mOmegax;
        mOmegay=_mOmegay;
        mOmegaz=_mOmegaz;
        ready=false;
}

void DBayesianModel::setEpsilons(double _e1,double _e2,double _e3,double _e4,double _e5,double _e6)
{
        e[0]=_e1;
        e[1]=_e2;
        e[2]=_e3;
        e[3]=_e4;
        e[4]=_e5;
        e[5]=_e6;
        ready=false;
}

void DBayesianModel::finalize()
{
        denominator=  2.0*M_PI*sigmaGx*sigmaGy;
        denominator*= 2.0*pow(M_PI,3.0/2.0)*sigmaOmega0x*sigmaOmega0y*sigmaOmega0z;
        denominator*= 2.0*pow(M_PI,3.0/2.0)*sigmaOmegax*sigmaOmegay*sigmaOmegaz;
        denominator*= 2.0*pow(M_PI,3)*sqrt(e[0]*e[1]*e[2]*e[3]*e[4]*e[5]);
        ready=true;
}
/** \ingroup Statistic
* Body of the function, compute the value of the integrand on x vector
* @param x \f$ \mathbf \f$ vector on which calculate \f$ f(\mathbf{x} ) \f$
**/
void DBayesianModel::body(const VectorXd &x )
{
	double arg1, arg2, arg3, arg4;
	
	omegax=x.coeffRef(0);
	omegay=x.coeffRef(1);
	omegaz=x.coeffRef(2);
	
	omega0x=x.coeffRef(3);
	omega0y=x.coeffRef(4);
	omega0z=x.coeffRef(5);
	
	a[0]=  omega0y+omegay 	;
	a[1]=  -omega0y*gx-omega0z 	;
	a[2]=  -omega0y*gy-omegaz	;
	a[3]=  omega0x-omegax		;
	a[4]=  -omega0x*gx+omegaz	;
	a[5]=  -omega0x*gy-omega0z	;
	
	arg1 = -0.5 * ( gx*gx/(sigmaGx*sigmaGx) + gy*gy/(sigmaGy*sigmaGy) );
	arg2 = -0.5 * ( SQR(omegax-mOmegax)/(sigmaOmegax*sigmaOmegax) + SQR(omegay-mOmegay)/(sigmaOmegay*sigmaOmegay) + SQR(omegaz-mOmegaz)/(sigmaOmegaz*sigmaOmegaz) );
	arg3 = -0.5 * ( SQR(omega0x-mOmega0x)/(sigmaOmega0x*sigmaOmega0x) + SQR(omega0y-mOmega0y)/(sigmaOmega0y*sigmaOmega0y) + SQR(omega0z-mOmega0z)/(sigmaOmega0z*sigmaOmega0z) );
	
	arg4 = -0.5 * (	SQR(a[0]-ma[0] )/SQR(e[0])	
			+ (SQR(a[1]-ma[1]))/SQR(e[1]) 	
			+ (SQR(a[2]-ma[2]))/SQR(e[2])
			+ (SQR(a[3]-ma[3]))/SQR(e[3])
			+ (SQR(a[4]-ma[4]))/SQR(e[4])
			+ (SQR(a[5]-ma[5]))/SQR(e[5])
			);

	result = exp(arg1+arg2+arg3+arg4)/denominator;
	
	
}

/** Destructor **/
DBayesianModel::~DBayesianModel()
{

}

