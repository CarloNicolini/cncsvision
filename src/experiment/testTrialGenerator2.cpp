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
#include "Mathcommon.h"
#include "ParametersLoader.h"
#include "Staircase.h"
#include "ParStaircase.h"
#include "BalanceFactor.h"
#include "Util.h"
#include "TrialGenerator.h"

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/erf.hpp>

using namespace util;

int main(void)
{
    BalanceFactor<double> balance;
    balance.init(1,0);
    vector<double> factorAValues;
    factorAValues = vlist_of<double>(99);
    balance.addFactor( "FactorA",  factorAValues );
    //balance.addFactor( "FactorB",  factorBValues );

    ParStaircase parStairCase;
    parStairCase.init(1);

    parStairCase.setStartState(vlist_of<double>(5.0));
    parStairCase.setAscending(vlist_of<bool>(true));
    parStairCase.setMaxReversals(vlist_of<int>(20));
    parStairCase.setMaxTrials(vlist_of<int>(500));
    parStairCase.setCorrectAnswers(vlist_of<int>(4));
    parStairCase.setStairStep(0,vlist_of<double>(1.0)(1.0),2.5);
    //parStairCase.setClamp(0,-1000.0,1000.0,10);
    parStairCase.setClamp(-1000.0,1000.0,1000);

    TrialGenerator<double> trials(balance,&parStairCase);
    double mean=0.0;
    double std=0.001;

    while ( !trials.isEmpty() )
    {
        double decision = unifRand(0.0,1.0);
        // Simulate an observer with a sigmoidal detection function (psychometric)
        double observer = (boost::math::erf((( trials.getCurrent().second->getCurrentStaircase()->getState() - mean)/std)))/2.0+0.5;
        bool answer = decision<=observer;

        cout << fixed <<
                trials.getCurrent().first.at("FactorA") << "\t" <<
                //trials.getCurrent().first.at("FactorB") << "\t" <<
                trials.getCurrent().second->getCurrentStaircase()->getID() << "\t" <<
                trials.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
                trials.getCurrent().second->getCurrentStaircase()->getReversals() << "\t" <<
                trials.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
                answer <<
                endl;
        trials.next(answer);
    }

    //trials.next(true);   // will produce an assert because the set is now empty

    return 0;
}
