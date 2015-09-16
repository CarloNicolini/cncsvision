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
#include "BalanceFactor.h"
#include "Timer.h"

using namespace std;
using namespace mathcommon;

void randomizeSeed(void)
{  int stime;
    long ltime;
    ltime = time(NULL);
    stime = (unsigned) ltime / 2;
    srand(stime);
}


int main(int argc, char *argv[])
{

    /*
    mathcommon::randomizeStart();
    ParametersLoader params;
    //params.loadParameterFile("/home/carlo/Desktop/test.txt");
    BalanceFactor<int> balance;

    params.addParameter("Repetitions","2");
    params.addParameter("Randomize","0");
    params.addParameter("fDistances","0 1");
    params.addParameter("fRelDepth","20 40");

    balance.init(params);

    while ( !balance.isEmpty() )
    {
        map<string,int> vals = balance.getNext();
        for ( map<string,int>::iterator iter = vals.begin(); iter!=vals.end(); ++iter )
        {
            cout << iter->first << " " << iter->second << " ";
        }
        cout <<  endl;
    }

    balance.print();

    while (balance.hasNext())
    {
        balance.next();
        map<string,int> vals = balance.getCurrent();
        cout << "( " << vals["fDistances"] << " " << vals["fRelDepth"] << " " << endl;
    }
    */
    mathcommon::randomizeStart();

    //ParametersLoader params;
    //params.loadParameterFile("/Users/rs/Desktop/test.txt");
    /*
    params.addParameter("Repetitions","2");
    params.addParameter("Randomize","0");
    params.addParameter("fDistances","0");
    params.addParameter("fRelDepth","rd, RD");
*/

    BalanceFactor<int> balance;
    vector<int> xx(4),yy(2);
    xx[0]=420;
    xx[1]=450;
    xx[2]=480;
    xx[3]=510;
    //xx[2]=480;
    //xx[3]=510;
    //xx[2]=2;
    yy[0]=2;
    yy[1]=3;
    balance.init(1,BalanceFactor<int>::DEBRUJIN_PSEUDORANDOM);
    balance.addFactor("x",xx);
    balance.addFactor("y",yy);
    balance.debrujinize(2,2,true);
    balance.print(cout);

    return 0;
}

