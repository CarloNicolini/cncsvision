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
#include <vector>
#include <list>
#include <algorithm>

#include "Mathcommon.h"
#include "Util.h"
#include "ParametersLoader.h"
#include "BalanceFactor.h"


using namespace std;
using namespace mathcommon;
using namespace util;



int main(void)
{  randomizeStart();

   vector <string> a = vlist_of<string>("A")("B")("C")("D");
   vector <string> b = vlist_of<string>("10");
   vector <string> c = vlist_of<string>("F")("G");

   BalanceFactor<string> balance;


   balance.init(4,true,true);
   balance.addFactor("factor1",a);
   balance.addFactor("factor2",b);
   balance.print(cout);

   /*
   ifstream infile;
   infile.open("prova.exp");
   ParametersLoader params;
   params.loadParameterFile(infile);
   balance.init(params,true);
   balance.print(cerr);
   */
   /*
   int i=0;
      while ( !balance.isEmpty() )
      {
         balance.next();
         map<string,string> factors = balance.getCurrent();

         for ( map<string,string>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
            cout << iter->second << "\t";

         if ( 0.5 >= (double)rand()/(double)RAND_MAX )
         //if ( i==5 )
         {

            balance.reinsert( factors );
            cerr << "reinsert" ;
            //cerr << "*****"  << endl;
            //balance.print(cout);
            //cerr << "*****" << endl;
    }
    cout << endl;
    //cout << endl;
      i++;
      }
      cout << i << endl;

   */
   return 0;
}
