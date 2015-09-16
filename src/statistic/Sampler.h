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

#ifndef _SAMPLER_
#define _SAMPLER_

#include <iostream>
#include <vector>
#include <Eigen/Core>
#include "MersenneTwister.h"
#include "Density.h"
#include "Util.h"

using namespace std;
using namespace Eigen;

class Sampler
{
public:
  Sampler();


  // Distributions
  inline double duniform(double a, double b);
  inline double dbeta(double x, double a, double b, bool give_log = false);
  inline double dbinom(int k, int n, double p, bool give_log);
  inline double dcauchy(double x, double l, double s, bool give_log);
  inline double dchisq(double x, double n, bool give_log);
  inline double ddirch(const vector<double>& p, const vector<double>& a, const bool give_log = false, const bool include_const = true);
  inline double dexp(double x, double b, bool give_log);
  inline double df(double x, double m, double n, bool give_log);
  inline double dgeom(unsigned x, double p, bool give_log);
  inline double dgamma(double x, double shape, double scale, bool give_log);
  inline double dhyper(unsigned x, unsigned r, unsigned b, unsigned n, bool giveLog);
  inline double dinvgamma(double y, double shape, double scale, bool give_log);
  inline double dlnorm(double x, double mu, double sigma, bool give_log);
  inline double dlogis(double x, double m, double s, bool give_log);
  inline double dmulti(const vector<int>& n, const vector<double>& p, bool give_log = false, bool include_factorial = false);
  inline double dnbinom(unsigned x, double n, double p, bool give_log);
  inline double dnorm(double x_in, double mu, double sigma, bool give_log);
  inline double dpois(unsigned x, double lambda, bool give_log);
  inline double dt(double x, double n, bool give_log);
  inline double dweibull(double x, double a, double b, bool give_log);
  inline double betaEntropy(double a, double b);
  inline double logChoose(double n, double k);
  inline double gamln(double x);
  inline double lbeta(double a, double b);

  VectorXd discreteCumulativeDistribution( const VectorXd &pdf );
  void discreteCumulativeDistribution( const VectorXd &pdf, VectorXd &cdf );

  double inverseCumulativeSampling( const VectorXd &cdf, double delta );
  unsigned int inverseCumulativeSampling( const VectorXd &cdf );
  double fullIntegrate( const VectorXd &distribution, double delta);
  
  void print(ostream &, const VectorXd & );
};

#endif
