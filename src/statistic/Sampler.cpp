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

#include <iostream>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <numeric>
#include <Eigen/Core>

#include "Mathcommon.h"
#include "Sampler.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

Sampler::Sampler()
{

}
//******* DISTRIBUTIONS ******//
// Density of the uniform distribution
double Sampler::duniform(double a, double b)
{
	return 1./(b-a);
}

// Density of the beta distribution
double Sampler::dbeta(double x, double a, double b, bool give_log )
{ 	
	return density::dbeta(x,a,b,give_log);
}

// Density of the binomial distribution.
double Sampler::dbinom(int k, int n, double p, bool give_log)
{ 	
	return density::dbinom( k,  n,  p,  give_log);
}
// Density of the cauchy distribution
double Sampler::dcauchy(double x, double l, double s, bool give_log)
{
	return density::dcauchy( x,  l,  s,  give_log);
}
// Density of the chi-squared distribution
double Sampler::dchisq(double x, double n, bool give_log)
{ 
	return density::dchisq( x,  n,  give_log);
}
// Density of the Dirichlet distribution
double Sampler::ddirch(const vector<double>& p, const vector<double>& a, const bool give_log , const bool include_const )
{
	return density::ddirch( p, a,  give_log, include_const);
}
// The density of the exponential distribution.
double Sampler::dexp(double x, double b, bool give_log)
{
	return density::dexp(x,  b,  give_log);
}
//	Density of the F distribution
double Sampler::df(double x, double m, double n, bool give_log)
{
	return density::df( x, m,  n,  give_log);
}
// Density of the geometric distribution
double Sampler::dgeom(unsigned x, double p, bool give_log)
{
	return density::dgeom( x,  p,  give_log);
}
// Density of the gamma distribution
double Sampler::dgamma(double x, double shape, double scale, bool give_log)
{
	return density::dgamma( x,  shape,  scale,  give_log);
}
// Density of the hypergeometric distribution.
double Sampler::dhyper(unsigned x, unsigned r, unsigned b, unsigned n, bool giveLog)
{
	return density::dhyper( x,  r,  b,  n,  giveLog);
}
// Density of the inverse gamma distribution
double Sampler::dinvgamma(double y, double shape, double scale, bool give_log)
{
	return density::dinvgamma( y,  shape,  scale,  give_log);
}
//    The density of the lognormal distribution.
double Sampler::dlnorm(double x, double mu, double sigma, bool give_log)
{
	return density::dlnorm( x,  mu,  sigma,  give_log);
}

// Density of the logistic distribution

double Sampler::dlogis(double x, double m, double s, bool give_log)
{
	return density::dlogis( x,  m,  s,  give_log);
}
// Density of the multinomial distribution
double Sampler::dmulti(const vector<int>& n, const vector<double>& p, bool give_log, bool include_factorial )
{
	return density::dmulti(  n, p,  give_log,  include_factorial );

}
// Density of the negative binomial distribution
double Sampler::dnbinom(unsigned x, double n, double p, bool give_log)
{
	return density::dnbinom( x,  n,  p,  give_log);
}
// Density of the normal distribution.
double Sampler::dnorm(double x_in, double mu, double sigma, bool give_log)
{
	return density::dnorm( x_in,  mu,  sigma,  give_log);
}
// Density of the Poisson distribution
double Sampler::dpois(unsigned x, double lambda, bool give_log)
{
	return density::dpois( x,  lambda,  give_log);
}
// Density of Student's t distribution
double Sampler::dt(double x, double n, bool give_log)
{
	return density::dt( x,  n,  give_log);
}
// Density of the Weibull distribution
double Sampler::dweibull(double x, double a, double b, bool give_log)
{
	return density::dweibull( x,  a,  b,  give_log);
}
// Entropy of a beta distribution with parameters a and b.
double Sampler::betaEntropy(double a, double b)
{
	return density::BetaEntropy( a,  b);
}
// Logarithm of n choose k.
double Sampler::logChoose(double n, double k)
{
	return density::logChoose( n,  k);
}

double Sampler::gamln(double x)
{
	return density::gamln( x);
}
// Logarithm of \f$\beta(a,b)\f$.
double Sampler::lbeta(double a, double b)
{
	return density::lbeta( a,  b);
}
//****** END DISTRIBUTIONS *******///



void Sampler::print(ostream &os, const VectorXd &x)
{
	unsigned int m=x.size();
	for (unsigned int i=0; i<m; i++)
		os << x(i) << endl;
}

VectorXd Sampler::discreteCumulativeDistribution( const VectorXd &pdf)
{
	unsigned int m=pdf.size();
	VectorXd cdf(m);
	cdf.setZero(m);
	
	cdf(0)=pdf(0);
	for (unsigned int i=1; i<m; i++)
		cdf.coeffRef(i)=cdf.coeff(i-1)+pdf.coeff(i);
		
	return cdf;
}

void Sampler::discreteCumulativeDistribution( const VectorXd &pdf, VectorXd &cdf )
{
	unsigned int m=pdf.size();
	//cdf.setZero(m);
	
	cdf(0)=pdf(0);
	for (unsigned int i=1; i<m; i++)
		cdf.coeffRef(i)=cdf.coeffRef(i-1)+pdf(i);
}

double Sampler::inverseCumulativeSampling( const VectorXd &cdf, double delta )
{
	int i=0;
	int m=cdf.size();
	double u = unifRand(0.0,1.0);
	while ( i<m && cdf.coeffRef(i)<u  ) ++i;
		
	double value = ( static_cast<double>(i) - static_cast<double>(m)*0.5 ) *delta;
	return value;
}

unsigned int Sampler::inverseCumulativeSampling( const VectorXd &cdf )
{
	int i=0;
	double m=cdf.size();
	double u = unifRand();
	
	while ( i<m && cdf.coeff(i)<=u  ) ++i;
		
	return i;
}

double Sampler::fullIntegrate( const VectorXd &distribution, double delta)
{
	return distribution.sum()*delta;

}


