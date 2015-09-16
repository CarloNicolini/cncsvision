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

	//randomizeStart();
	
	if ( !argc )
	{
		cerr << "Inferencer for bayesian modeling\nCarlo Nicolini, July 2011" << endl;
		cerr << "Usage: ./test \"[parameters file]\" \"[matrix output]\" \n" << endl;
		exit(0);	
	}

	if (argv[1]==NULL)
	{
		cerr << "Inferencer for bayesian modeling\nCarlo Nicolini, July 2011" << endl;
		cerr << "Usage: ./test \"[parameters file]\" \"[matrix output]\" \n" << endl;
                exit(0);
	}
	
	ifstream params;
	
	BayesNetwork bayes;
	
	params.open(argv[1]);
        bayes.loadParameterFile(params);

        if ( argv[2] != NULL )
        {
	ofstream matrixoutput;
	matrixoutput.open(argv[2]);
	bayes.computeModel(matrixoutput);
	}
	else
	{
	bayes.computeModel(cout);
	}

	return 0;
}

