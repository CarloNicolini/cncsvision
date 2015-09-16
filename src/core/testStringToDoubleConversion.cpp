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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include "Timer.h"
#include "Cycle.h"
#include "Util.h"
#include "Timer.h"

using namespace std;
int main()
{
    vector<double> vals(1E6);
    std::string x("45252.62362523");
    Timer timer;
    double end;
    timer.start();
    for (int i=0 ; i<1E6;i++)
    {
        vals[i]=boost::lexical_cast<double>(x);
    }
    end = timer.getElapsedTimeInMicroSec();
    cerr << end << endl;

    timer.start();
    for (int i=0 ; i<1E6;i++)
    {
        vals[i]=util::str2num<double>(x);
        //vals[i] = atof(x.c_str());
    }
    end = timer.getElapsedTimeInMicroSec();
    cerr << end << endl;
}
