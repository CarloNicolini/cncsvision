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
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/math/special_functions/erf.hpp>

#include "ParStaircase.h"
#include "ParStaircase.h"
#include "Util.h"
#include "Mathcommon.h"

using namespace util;

int main(int argc, char *argv[])
{
    ParStaircase parStairCase;
    parStairCase.init(1);

    parStairCase.setStartState(vlist_of<double>(5.0));
    parStairCase.setAscending(vlist_of<bool>(false));
    parStairCase.setMaxReversals(vlist_of<int>(20));
    parStairCase.setMaxTrials(vlist_of<int>(500));
    parStairCase.setCorrectAnswers(vlist_of<int>(2));
    parStairCase.setStairStep(0,vlist_of<double>(0.5)(0.5),0.5);
    parStairCase.setClamp(-1000,1000,1000.0);
    // Perform a staircase simulation
    int finished=0;
    double mean=0.0;
    double std=0.001;

        ParStaircase parStairCase2(parStairCase);

    while ( finished==0 )
    {
        Staircase *s = parStairCase2.getCurrentStaircase();
        double decision = mathcommon::unifRand(0.0,1.0);
        double observer = (boost::math::erf(((s->getState() - mean)/std)))/2+0.5;
        cout << fixed << s->getID() << "\t" << s->getState() << endl;
        bool response = (decision < observer);
        finished = parStairCase2.step(response);
    }

    return 0;
}
