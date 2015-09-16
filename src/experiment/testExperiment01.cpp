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

#include "Mathcommon.h"
#include "ParametersLoader.h"
#include "Staircase.h"
#include "ParStaircase.h"
#include "BalanceFactor.h"

using namespace std;
using namespace mathcommon;

void randomizeSeed(void)
{  int stime;
   long ltime;
   ltime = time(NULL);
   stime = (unsigned) ltime / 2;
   srand(stime);
}

int main(void)
{  randomizeSeed();
   /*
   BalanceFactor<int> balance;
   map<string,int> factors;
   balance.init(1,true);
   vector<int> f;
   for (int i=0; i<10; i++)
   f.push_back(i);

   balance.addFactor( "Slant",  f );
   while ( !balance.isEmpty() )
   {
       factors = balance.getNext();
       cout << factors["Slant"] << endl;
   }
   balance.init(1,true);
   cerr << "==========" << endl;
   f.clear();
   for (int i=0; i<10; i++)
   f.push_back(i);

   balance.addFactor( "Slant",  f );
   while ( !balance.isEmpty() )
   {
       factors = balance.getNext();
       cout << factors["Slant"] << endl;
   }
   */
   /*
   balance.addFactor( "Tilt", vlist_of<int>(90) );
   balance.addFactor( "Anchored", vlist_of<int>(0)(1)(2)(3));

   //balance.print();

   while ( !balance.isEmpty() )
   {
         map<string, int> tmp2 = balance.getNext() ;
         for ( map<string, int>::iterator iter = tmp2.begin(); iter!=tmp2.end(); ++iter)
         cout << iter->first << " " << iter->second << " ";
         cout << endl;
   }

   cerr << isnan(1.0/0.0) << " " << isinf(1.0/0.0) << endl;
    return 0;
    */
   map<string,int> factors;
   ifstream paramFile;
   paramFile.open("prova.exp");

   ParametersLoader params;
   params.loadParameterFile(paramFile);
   BalanceFactor<int> bfact;
   bfact.init(params);
   while (!bfact.isEmpty())
   {  factors = bfact.getNext();
      cout << factors["StimSpeed"] << endl;
   }
   //bfact.print();

   bfact.init(params);

   while (!bfact.isEmpty())
   {  factors = bfact.getNext();
      cout << factors["StimSpeed"] << endl;
   }
   /*
   BalanceFactor<int> balance;
   balance.init(params);
   balance.print();

   while ( !balance.isEmpty() )
   {
        map<string,int> vals = balance.getNext();
        for ( map<string,int>::iterator iter = vals.begin(); iter!=vals.end(); ++iter )
        {
        cout << iter->first << " " << iter->second << endl;
        }
        cout << endl << endl;

   }
   */

   return 0;
}

/* ======  STAIRCASE CODE ======
   Staircase staircase;

   staircase.init();
   staircase.setStartState(0.5);

   staircase.run();
   staircase.print( );

   ParStaircase *parStairCase;
   parStairCase = new ParStaircase();
   parStairCase->init(1);
   parStairCase->run();
   parStairCase->print();
   delete parStairCase;
*/
