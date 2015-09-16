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
#include <math.h>

#include "Mathcommon.h"
#include "ParametersLoader.h"
#include "Staircase.h"
#include "ParStaircase.h"
#include "BalanceFactor.h"
#include "Util.h"

#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/math/special_functions/erf.hpp>

using namespace std;
using namespace mathcommon;
using namespace util;


int main(int argc, char *argv[])
{  randomizeStart();
   ifstream paramfile;
   paramfile.open(argv[1]);
   ParametersLoader params;
   params.loadParameterFile(paramfile);

   ParStaircase parStairCase;
   
   parStairCase.init(params);
   int finished=0;
   double mean=str2num<double>(params.find("Mu"));
   double std=str2num<double>(params.find("Sigma"));
   double guessRate = str2num<double>(params.find("GuessRate"));
   double lapseRate = str2num<double>(params.find("LapseRate"));
   ofstream outputfile;
   outputfile.open(string(params.find("SubjectName")+"_sim.txt").c_str() );

   outputfile.precision(3);
   cout.precision(3);
   while ( finished==0 )
   {
       Staircase *s = parStairCase.getCurrentStaircase();
      double decision = unifRand(0.0,1.0);
      double x = s->getState();
	  double observer  = guessRate+(1-guessRate-lapseRate)*(0.5*(1.0+boost::math::erf( (x-mean)/( sqrt(2.0)*std ))));
      bool response = decision < observer;
      //cout << fixed << s->getID() << "\t" << s->getReversals() << "\t" << s->getState() << "\t" << parStairCase.getCurrentStaircase()->getTrialsDone() << "\t" << response << endl;
      cout << fixed << s->getID() << "\t" << s->getReversals() << "\t" << s->getState() << "\t" << parStairCase.getCurrentStaircase()->getTrialsDone() << "\t" << response << endl;
      outputfile << fixed << s->getID() << "\t" << s->getReversals() << "\t" << s->getState() << "\t" << parStairCase.getCurrentStaircase()->getTrialsDone()<< "\t" << response << endl;
      finished = parStairCase.step(response);
   }

   parStairCase.save("xxx.dat");

   return 0;
}
