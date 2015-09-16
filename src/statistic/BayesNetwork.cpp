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
#include <fstream>
#include <map>

#include "Util.h"
#include "Mathcommon.h"
#include "BayesNetwork.h"
#include "ParametersLoader.h"

using namespace std;
using namespace mathcommon;

BayesNetwork::BayesNetwork()
{
    reset();
}


void BayesNetwork::reset()
{
    nDimensions=0;
    nGibbsBurnIn=1;
    nGibbsValid=1;

    // Diagonal covariance matrices element
    sigmaOmega0x=1;
    sigmaOmega0y=1;
    sigmaOmega0z=1;

    sigmaOmegax=1;
    sigmaOmegay=1;
    sigmaOmegaz=1;

    sigmaGx=1;
    sigmaGy=1;

    // Mean value of gaussians
    ma1=ma2=ma3=ma4=ma5=ma6=0;
    mOmega0x=mOmega0y=mOmega0z=mOmegax=mOmegay=mOmegaz=0;

    domainMin=-1;
    domainMax=1;
    domainStep=0.1;

    gxMin=gyMin=-1;
    gxMax=gyMax=1;
    gStep=0.1;

    domain.clear();
}

void BayesNetwork::loadParameterFile(ifstream &is)
{

    ParametersLoader param;
    param.loadParameterFile(is);
    nDimensions=    util::str2num<int>(param.find("nDimensions"));
    nGibbsBurnIn=   util::str2num<int>(param.find("nGibbsBurnIn"));
    nGibbsValid =   util::str2num<int>(param.find("nGibbsValid"));
    sigmaOmega0x=   util::str2num<double>(param.find("sigmaOmega0x"));
    sigmaOmega0y=   util::str2num<double>(param.find("sigmaOmega0y"));
    sigmaOmega0z=   util::str2num<double>(param.find("sigmaOmega0z"));

    sigmaOmegax=   util::str2num<double>(param.find("sigmaOmegax"));
    sigmaOmegay=   util::str2num<double>(param.find("sigmaOmegay"));
    sigmaOmegaz=   util::str2num<double>(param.find("sigmaOmegaz"));

    mOmega0x=util::str2num<double>(param.find("mOmega0x"));
    mOmega0y=util::str2num<double>(param.find("mOmega0y"));
    mOmega0z=util::str2num<double>(param.find("mOmega0z"));

    mOmegax=util::str2num<double>(param.find("mOmegax"));
    mOmegay=util::str2num<double>(param.find("mOmegay"));
    mOmegaz=util::str2num<double>(param.find("mOmegaz"));

    sigmaGx=        util::str2num<double>(param.find("sigmaGx"));
    sigmaGy=        util::str2num<double>(param.find("sigmaGy"));

    ma1=            util::str2num<double>(param.find("ma1"));
    ma2=            util::str2num<double>(param.find("ma2"));
    ma3=            util::str2num<double>(param.find("ma3"));
    ma4=            util::str2num<double>(param.find("ma4"));
    ma5=            util::str2num<double>(param.find("ma5"));
    ma6=            util::str2num<double>(param.find("ma6"));

    epsilon1=       util::str2num<double>(param.find("epsilon1"));
    epsilon2=       util::str2num<double>(param.find("epsilon2"));
    epsilon3=       util::str2num<double>(param.find("epsilon3"));
    epsilon4=       util::str2num<double>(param.find("epsilon4"));
    epsilon5=       util::str2num<double>(param.find("epsilon5"));
    epsilon6=       util::str2num<double>(param.find("epsilon6"));

    domainMin=      util::str2num<double>(param.find("domainMin"));
    domainMax=      util::str2num<double>(param.find("domainMax"));
    domainStep=     util::str2num<double>(param.find("domainStep"));

    gxMin=gyMin=    util::str2num<double>(param.find("gMin"));
    gxMax=gyMax=    util::str2num<double>(param.find("gMax"));
    gStep=          util::str2num<double>(param.find("gStep"));

    domain.resize(nDimensions);
    for (unsigned int i=0; i<nDimensions; i++)
        domain[i] = pair<double,double>(domainMin,domainMax);
}

void BayesNetwork::checkAllParameters()
{	
    cerr << "===== MODEL PARAMETERS =====" << endl << endl;
    cerr << "nDimensions= " << nDimensions << endl;
    cerr << "nGibbsBurnIn= " << nGibbsBurnIn << endl;
    cerr << "nGibbsValid= " << nGibbsValid << endl << endl;
    cerr << "Σ ω0x= " << sigmaOmega0x << endl;
    cerr << "Σ ω0y= " << sigmaOmega0y << endl;
    cerr << "Σ ω0z= " << sigmaOmega0z << endl << endl;
    cerr << "Σ ωx= " << sigmaOmegax << endl;
    cerr << "Σ ωy= " << sigmaOmegay << endl;
    cerr << "Σ ωz= " << sigmaOmegaz << endl << endl;
    cerr << "Σ gx= " << sigmaGx << endl;
    cerr << "Σ gy= " << sigmaGy << endl << endl;
    cerr << "ma1= " << ma1 << endl;
    cerr << "ma2= " << ma2 << endl;
    cerr << "ma3= " << ma3 << endl;
    cerr << "ma4= " << ma4 << endl;
    cerr << "ma5= " << ma5 << endl;
    cerr << "ma6= " << ma6 << endl << endl;
    cerr << "ε1= " << epsilon1 << endl ;
    cerr << "ε2= " << epsilon2 << endl;
    cerr << "ε3= " << epsilon3 << endl;
    cerr << "ε4= " << epsilon4 << endl;
    cerr << "ε5= " << epsilon5 << endl;
    cerr << "ε6= " << epsilon6 << endl;
    cerr << "domainMin= " << domainMin << endl;
    cerr << "domainMax= " << domainMax << endl;
    cerr << "domainStep= " << domainStep << endl << endl;
    cerr << "gMin= " << gxMin << endl;
    cerr << "gMax= " << gxMax << endl;
    cerr << "gStep= " << gStep << endl;
    cerr << "mOmega0x= " << mOmega0x << endl;
    cerr << "mOmega0y= " << mOmega0y << endl;
    cerr << "mOmega0z= " << mOmega0z << endl;
    cerr << "mOmegax= " << mOmegax << endl;
    cerr << "mOmegay= " << mOmegay << endl;
    cerr << "mOmegaz= " << mOmegaz << endl;
}


void BayesNetwork::computeModel(ostream &os)
{

    double dgranularity= (abs(domainMax-domainMin))/domainStep;
    unsigned int granularity = floor(dgranularity+0.5);

    double dgranularityGx=abs(gxMax-gxMin)/gStep;
    double dgranularityGy=abs(gyMax-gyMin)/gStep;
    int granularityGx = floor(dgranularityGx+0.5);
    int granularityGy = floor(dgranularityGy+0.5);

    Pgxgy.resize( granularityGx+1,granularityGy+1 );
    Pgxgy.setZero (granularityGx+1, granularityGy+1);

    // This is the correct way to do parallel loops with openmp, keep the gx, gy local inside the inner loop!!!
#pragma omp parallel for ordered schedule(dynamic)
    for (int i=0; i<granularityGx; i++)
    {
        for (int j=0; j<granularityGy; j++)
        {
            // A new instance of a multidimensional probability density function to integrate
            DBayesianModel dbayes(nDimensions);
            // Set the parameters of the pdf (see Fulvio for details)
            dbayes.setSigmaG(sigmaGx,sigmaGy);
            dbayes.setSigmaOmega(sigmaOmegax,sigmaOmegay,sigmaOmegaz);
            dbayes.setSigmaOmega0(sigmaOmega0x,sigmaOmega0y,sigmaOmega0z);
            dbayes.setAMean(ma1,ma2,ma3,ma4,ma5,ma6);
            dbayes.setOmega0Mean( mOmega0x,mOmega0y,mOmega0z);
            dbayes.setOmegaMean( mOmegax,mOmegay,mOmegaz);
            dbayes.setEpsilons(epsilon1,epsilon2,epsilon3,epsilon4,epsilon5,epsilon6);

            double gx = i*gStep+gxMin;
            double gy = j*gStep+gyMin;
            dbayes.setGxGy(gx,gy);
            dbayes.finalize();
            // Inferencer for Gibbs Sampling
            Inferencer inferencer;
            inferencer.init( &dbayes, nDimensions,granularity, domainStep, domain );
            Pgxgy(i,j)=inferencer.gibbsSamplingIntegral(nGibbsBurnIn,nGibbsValid);
        }
#pragma omp critical
        cerr << "Percent " << (double)i/(granularityGx)*100.0 << "\%" << endl;
    }

    os << Pgxgy << endl;
}


