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
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <functional>
#include <limits>
#include <Eigen/Core>

#include "Mathcommon.h"
#include "Density.h"
#include "Util.h"
#include "Sampler.h"
#include "MersenneTwister.h"
#include "Timer.h"
#include "Inferencer.h"

#include "Distribution.h"
#include "DMultivariateGaussian.h"
#include "DExample.h"
#include "DBayesianModel.h"
#include "BayesNetwork.h"

using namespace std;
using namespace Eigen;
using namespace mathcommon;
using namespace density;
using namespace util;


int main(int argc, char*argv[])
{

	randomizeStart();
	srand(time(0));
	
	unsigned int nDimensions=2;
	double domainMin =-10;
	double domainMax = 10;
	double domainStep= 0.1;
	
	vector < pair<double, double> > domain;
	
	double dgranularity= (abs(domainMax-domainMin))/domainStep;
	unsigned int granularity = floor(dgranularity+0.5);
	unsigned int nGibbsBurnIn=0;
	unsigned int nGibbsValid=1000;
	
	domain.resize(nDimensions);
	for (unsigned int i=0; i<nDimensions; i++)
		domain[i] = pair<double,double>(domainMin,domainMax);
	
        DMultivariateGaussian model(nDimensions);
       
        Inferencer inferencer;
        inferencer.init( &model, nDimensions,granularity, domainStep, domain );
        /*
        for ( unsigned i=1000; i<=5000; i+=500)
        {
        inferencer.init( &model, nDimensions,granularity, domainStep, domain );
        cerr << i << " " << inferencer.gibbsSamplingIntegral(nGibbsBurnIn,i) << endl;        
        }
        */
        
        Sampler sampler;
        VectorXd cdf(2000),pdf(2000);
        
        double x=-6;
        double delta=abs(x)/1000.0;
        for ( int i=0; i<2000; i++)
        {
                pdf(i) = exp(-(0.5*(x*x) ));
                //cout << x << " " << exp(-(0.5*(x*x) )) << endl;
                x+=delta;
        }
        
        pdf*=1.0/(sqrt(2*M_PI));
        
        cout << sampler.fullIntegrate(pdf,delta) << endl;
        //cerr << cdf.sum()*delta << endl;
        sampler.discreteCumulativeDistribution(pdf,cdf);
        for ( int i=0; i<10; i++)
                sampler.inverseCumulativeSampling(cdf,delta);
                
        /*
        x=-5;
        double sum=cdf.sum();
        cdf/=(sum/(1000.0));
        for ( int i=0; i<2000; i++)
        {
                cout << x << " " << cdf(i) << endl;
                x+=delta;
        }

        for ( int i=0; i<100000; i++)
                cerr << sampler.inverseCumulativeSampling(cdf,delta) << endl;
        */
        return 0;
}

