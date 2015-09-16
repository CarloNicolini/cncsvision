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
    stair.init();
    stair.setID(0);
    stair.setAscending(false);
    stair.setStartState(20);
    stair.setMaxNtrials(40);
    stair.setMaxReversals(10);
    stair.setCorrectAnswers(1);
    stair.setStairStep(util::vlist_of<double>(4),4);

    int finished = 0;
    while ( finished==0 )
    {
        cout << fixed << stair.getID() << "\t" << stair.getReversals() << "\t" << stair.getState() << "\t" << stair.getReversals() <<"\t" ;
        double x = stair.getState();
        bool resp;
        cin >> resp;

        finished = stair.step(resp);
    }

    cout << stair.getAverageReversals(3) << endl;
    return 0;
}
