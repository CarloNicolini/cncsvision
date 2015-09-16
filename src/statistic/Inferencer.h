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

#ifndef _INFERENCER_H
#define _INFERENCER_H

#include <iostream>
#include <vector>
#include <Eigen/Core>

#include "MersenneTwister.h"
#include "Density.h"
#include "Util.h"

#include "Distribution.h"
#include "DMultivariateGaussian.h"
#include "Mathcommon.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

/**
* \class Inferencer
* \ingroup Statistic
* \brief Inferencer is a class intended to help the user to make inference on bayesian models by meaning of kernel integrals.
**/

class Inferencer
{
	public:
		Inferencer();
		void init(int dimension, int granularity, double stepsize);
		void init(Distribution *, int dimension, int granularity, double stepsize);
		void init(Distribution *, int dimension, int granularity, double stepsize, vector< pair<double, double> > &);
		
		// Set an initial random state
		void randomizeState(int dimension);

		double gibbsSamplingIntegral(unsigned int burnInIterations, unsigned int numvalidIterations);

		void saveInfo(ostream &);
		
	private:
		// Gibbs sampling related methods
		inline double sampleFromFullConditional(const VectorXd &);
		const VectorXd& gibbsSteps(void);
		const VectorXd& computeFullConditional( unsigned int variable );
		
		// The state of the Markov chain
		VectorXd state;
		// The full-conditionals pdf
		VectorXd fullConditionalPdf;
		// The full conditions cumulative density function
		VectorXd fullConditionalCdf;
		
		// An istance of the sampler (used to sample from arbitrary distributions and sample random numbers
		Sampler sampler;
		// The instance of the distribution object 
		Distribution* dist;
		// Dimension of the search space
		unsigned int dimension;
		// Granularity, i.e. the number of discretizations of the dimension
		unsigned int granularity;
		// Stepsize controls the dimension of the Δx 
		double stepSize;
		// Integrations domain of the variables, .first is the minimum, .second is the maximum
		vector < pair<double, double> > integrationDomain;
		
};
#endif
