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

#ifndef _BAYES_NETWORK_H
#define _BAYES_NETWORK_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <string>

#include <Eigen/Core>
#include "DBayesianModel.h"
#include "Sampler.h"
#include "Inferencer.h"

using namespace std;
using namespace Eigen;

/**
*       \class BayesNetwork
*       \ingroup Statistic
*       \brief BayesNetwork is a class needed to solve the Domini vision model via MCMC integration over different values for optic-flow gradient
**/

class BayesNetwork
{
	private:
		// Gibbs sampler related
		unsigned int nDimensions;
		unsigned int nGibbsBurnIn;
		unsigned int nGibbsValid;
		
       
		// Diagonal covariance matrices element
		double sigmaOmega0x, sigmaOmega0y, sigmaOmega0z;
		double sigmaOmegax, sigmaOmegay, sigmaOmegaz;
		double sigmaGx, sigmaGy;
		double epsilon1,epsilon2,epsilon3,epsilon4,epsilon5,epsilon6;
		
		double mOmega0x,mOmega0y,mOmega0z,mOmegax,mOmegay,mOmegaz;
		
		// Mean value of gaussians
		double ma1, ma2,ma3, ma4,ma5, ma6;
		
		double domainMin, domainMax, domainStep;
		
		double gxMin, gxMax, gyMin, gyMax, gStep;
		vector < pair<double, double> > domain;
		
                MatrixXd Pgxgy;
		
	public:
		BayesNetwork();
		void init();
		void reset();
		        
		void loadParameterFile(ifstream &);
		void loadParameterFile(ifstream &, int);
		
		void checkAllParameters();
		
        void setParametersOut(ofstream &);
		
		void computeModel(ostream &);
		const MatrixXd& getMatrix(){ return Pgxgy;};
};

#endif
