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
#include <vector>
#include "Mathcommon.h"
#include "Util.h"

using namespace std;

int main(int argc, char *argv[])
{
    /*
    mathcommon::randomizeStart();
    std::vector<int> x;
    for (int i=0; i<12; i++)
        x.push_back(i);

    std::random_shuffle(x.begin(),x.end());
    for (int i=0; i<12; i++)
        cerr << x[i] << ", ";
    cerr << endl;

    cout << *util::max_element_nth(x.begin(),x.end(),3)<< endl;
    cout << *util::max_element_nth(x.begin()+1,x.end(),3)<< endl;
    cout << *util::max_element_nth(x.begin()+2,x.end(),3)<< endl;

    cout << *util::min_element_nth(x.begin(),x.end(),3)<< endl;
    cout << *util::min_element_nth(x.begin()+1,x.end(),3)<< endl;
    cout << *util::min_element_nth(x.begin()+2,x.end(),3)<< endl;
    */
    cout << mathcommon::unifRand(50.0,-50.0) << endl;
    return 0;
}
