// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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

   ofstream outputfile;
   string outputfilename = "response_" + params.find("SubjectName")+params.find("Condition")+".txt";
   outputfile.open(outputfilename.c_str());
   outputfile.precision(1);
   outputfile << fixed << "#trial" << "\t" <<
              "ID" << "\t" <<
              "Stimul" << "\t" << 
              "Orient" << "\t" <<
              "sStep" << "\t" <<
              "sInv" << "\t" <<
              "Dist" << "\t" <<
              "Type" << "\t" <<
              "Answer" << "\t" <<
              "Resp" << endl;

   map<int,string> id2string;
   id2string[0]="Hand";
   id2string[1]="Hand";
   id2string[2]="Arm";
   id2string[3]="Arm";
   
   map<int,string> id2HV;
   id2HV[0]="H";
   id2HV[1]="V";
   id2HV[2]="H";
   id2HV[3]="V";
   
   int finished=0;

   int totalTrial=0;
   while ( finished==0 )
   {  Staircase *s = parStairCase.getCurrentStaircase();
      bool isOneOrTwo = unifRand(0.0,1.0) >= 0.5;
      string stimtype;
      if ( isOneOrTwo )
         stimtype="Two";
      else
         stimtype="One";

      cerr << fixed << "Staircase" << s->getID() << ":\t" << id2string[s->getID()] << "\t" <<  id2HV[s->getID()] << "\t"  << stimtype << "\tDistance " << s->getState() << "\tResponse? (1 or 2) " ;
      char answer;
      do
      {  cin >> answer;
      }
      while ( tolower(answer)!='1' && tolower(answer)!='2' );

      bool response = (isOneOrTwo) == ( answer=='1');

      outputfile << fixed << totalTrial << "\t" <<
                 s->getID() << "\t" <<
                 id2string[s->getID()] << "\t" <<
                 id2HV[s->getID()] << "\t" <<
                 s->getStepsDone() << "\t" <<
                 s->getReversals() << "\t" <<
                 s->getState() << "\t" <<
                 stimtype << "\t" <<
                 answer << "\t" <<
                 response << endl;

      finished = parStairCase.step(response);
      totalTrial++;
   }

   return 0;
}
