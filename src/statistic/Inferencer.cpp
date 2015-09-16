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
#include <cmath>
#include <iostream>
#include <functional>
#include <numeric>
#include <queue>
#include <Eigen/Core>
#include <Eigen/QR>

#include "Mathcommon.h"
#include "Sampler.h"
#include "Inferencer.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

#define RANDOMSTEP 0.001

Inferencer::Inferencer() 
{
	dist=NULL; 
}

void Inferencer::init(int _dimension, int _granularity, double _stepSize)
{	
	dimension=_dimension;
	granularity=_granularity;
	stepSize=_stepSize;
	fullConditionalPdf.setZero(granularity);
	fullConditionalCdf.setZero(granularity);
	
	randomizeState(_dimension);
		
}

void Inferencer::init(Distribution *_d, int _dimension, int _granularity, double _stepsize)
{
	dist = _d;
	init( _dimension,  _granularity,  _stepsize );
}


void Inferencer::init(Distribution *_d, int _dimension, int _granularity, double _stepsize, vector< pair<double, double> > &domain)
{
	dist = _d;
	integrationDomain=domain;
	init( _dimension,  _granularity,  _stepsize );

}

void Inferencer::randomizeState(int dimension)
{
	state.setZero(dimension);
	for ( int i=0; i<dimension; i++)
	{
	        double r = RANDOMSTEP*unifRand( integrationDomain[i].first, integrationDomain[i].second );
		state.coeffRef(i)=r;	
	}
	fullConditionalPdf.setZero(granularity);
	fullConditionalCdf.setZero(granularity);
}

void Inferencer::saveInfo(ostream &os)
{
	os << "Search space= R^" << dimension << "  Granularity= " << granularity << " stepSize= " << stepSize << endl;
	for (unsigned int i=0; i<dimension; i++)
	os << "Variable(" << i << ") = [ " << integrationDomain[i].first << ", " << integrationDomain[i].second << " ]\tΔ= " << stepSize << 
		"\tN= " << abs(integrationDomain[i].second - integrationDomain[i].first)/stepSize << endl;
}

const VectorXd& Inferencer::computeFullConditional( unsigned int actualVariableIndex  )
{
	double x=integrationDomain[actualVariableIndex].first;
	
	VectorXd tempvars(dimension);
	tempvars.setZero(dimension);

	tempvars=state;
	tempvars(actualVariableIndex)=x;
	for (unsigned int i=0; i<granularity; i++)
	{
		tempvars.coeffRef(actualVariableIndex)=x;
		fullConditionalPdf.coeffRef(i) = dist->eval(tempvars);
		x+=stepSize;
	}
	
	// Normalization is needed in order to have area 1 pdf and so that cdf(max)=1
	double sum=fullConditionalPdf.sum();
	//if ( sum > 1E-18 )
	fullConditionalPdf/=(sum);
		
	return fullConditionalPdf;
}

double Inferencer::sampleFromFullConditional(const VectorXd &fullConditional )
{
	//fullConditionalCdf = sampler.discreteCumulativeDistribution(fullConditional);
	// Compute the fullConditionalCdf (second argument given via reference)
	sampler.discreteCumulativeDistribution(fullConditional,fullConditionalCdf);
	double cdfSample = sampler.inverseCumulativeSampling(fullConditionalCdf,stepSize);
	
	return cdfSample;
}

const VectorXd& Inferencer::gibbsSteps( void )
{

        for (unsigned int k=0; k<dimension; k++)
	{
		fullConditionalPdf = computeFullConditional(k);		
		state.coeffRef(k) = sampleFromFullConditional(fullConditionalPdf);
	        //cout << state.transpose() << endl;
	}
	return state;
}

double Inferencer::gibbsSamplingIntegral(unsigned int burnInIterations, unsigned int numvalidIterations)
{
	// Starting conditions
	for ( unsigned int i=0; i<dimension; i++)
	{
	        double r = RANDOMSTEP*unifRand( integrationDomain[i].first, integrationDomain[i].second );
		state.coeffRef(i)=r;
	}

	fullConditionalPdf.setZero(granularity);
	fullConditionalCdf.setZero(granularity);
	
	double integral=0.0;
	double value=0.0;
	
	for (unsigned int t=0; t<burnInIterations; t++)
			gibbsSteps();
	
	for (unsigned int t=0;t<numvalidIterations; t++)
	{	
		gibbsSteps();
		value=dist->eval(state);
		integral+=value;
		//cout << state.transpose() << endl;
	}
	
	double integrationVolume = 1;
	for ( unsigned int i=0; i<dimension; i++)
	        integrationVolume*=(integrationDomain[i].first, integrationDomain[i].second);

        
	return integral/((double) numvalidIterations)*integrationVolume;	
}

