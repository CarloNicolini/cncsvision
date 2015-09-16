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

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>
#include <list>
#include <memory>
#include <math.h>
//#include <boost\math\special_functions\erf.hpp>
#include "Mathcommon.h"
#include "ParametersLoader.h"
#include "Staircase.h"
#include "ParStaircase.h"
#include "BalanceFactor.h"
#include "Util.h"
#include "TrialGenerator.h"

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/erf.hpp>

int main(int argc, char *argv[])
{
    randomizeStart();
    ifstream infile;
    infile.open(argv[1]);
    ParametersLoader params;
    params.loadParameterFile(infile);

    TrialGenerator<double> trial2;

    trial2.init(params);

    pair< map<string,double>,ParStaircase* > factorStaircasePair = trial2.getCurrent();
    double mean=factorStaircasePair.first.at("CylOrientation");
    double std=0.001;

    while ( !trial2.isEmpty() )
    {   //cout << fixed << factorStaircasePair.first.at("Extraction") << "\t" << factorStaircasePair.first.at("Tilt") << "\t" << factorStaircasePair.first.at("Slant") << "\t" << factorStaircasePair.second->getCurrentStaircase()->getID() << "\t" << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;
        cout << fixed << factorStaircasePair.first["CylOrientation"]  << "\t" << factorStaircasePair.second->getCurrentStaircase()->getID() << "\t" << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;
        bool answer=true;
        double decision = unifRand(0.0,1.0);
        double observer = (boost::math::erf(((factorStaircasePair.second->getCurrentStaircase()->getState() - mean)/std)))/2+0.5;
        answer = decision<=observer;
        trial2.next(answer);
        factorStaircasePair = trial2.getCurrent();
    }
    return 0;
}
