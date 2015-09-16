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

#include "Staircase.h"
#include "Util.h"
#include "Mathcommon.h"

int main(int argc, char *argv[])
{
    Staircase stair;
    stair.setID(0);
    stair.setStartState(15);
    stair.setAscending(true);
    stair.setMaxNtrials(200);
    stair.setMaxReversals(10);
    stair.setCorrectAnswers(1);
    stair.setStairStep(util::vlist_of<double>(4)(2),util::vlist_of<double>(0.5));

    double mean=20;
    double std=0.01;
    double guessRate = 0.0;
    double lapseRate = 0.0;
    int finished = 0;
    while ( finished==0 )
    {
       double decision = mathcommon::unifRand(0.0,1.0);
       //double observer = (erf(((s->getState() - mean)/(std))))/2+0.5;
       double x = stair.getState();
       double observer  = guessRate+(1-guessRate-lapseRate)*(0.5*(1.0+boost::math::erf( (x-mean)/( sqrt(2.0)*std ))));
       bool response = decision < observer;
       cout << fixed << stair.getID() << "\t" << stair.getReversals() << "\t" << stair.getState() << "\t" << stair.getTrialsDone() <<"\t" << response << endl;
       finished = stair.step(response);
    }

    ofstream os("xxx.dat");
    std::vector<std::pair<int,double> > revStates = stair.getReversalStates();
    for (int i=0; i<revStates.size();i++)
    {
        os << revStates.at(i).first << ":" << revStates.at(i).second << ", ";
    }
    os << endl;

    vector<double> states = stair.getStates();
    std::copy(states.begin(), states.end(), ostream_iterator<double>(os, ", "));
    os << endl;
    return 0;
}
