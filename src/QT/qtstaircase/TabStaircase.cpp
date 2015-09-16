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

#include "TabStaircase.h"
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

TabStaircase::TabStaircase(QWidget *parent) :
    QTabWidget(parent)
{

}

TabStaircase::~TabStaircase()
{
    for (unsigned int i=0; i<stairSettings.size(); i++)
        delete stairSettings.at(i);
    stairSettings.clear();
    //cerr << "TabStaircase destructed" << endl;
}

StaircaseParamSetter *TabStaircase::getStaircaseParamsSetter(int i)
{
    return stairSettings.at(i);
}

void TabStaircase::reset()
{
    for (unsigned int i=0; i<stairSettings.size();i++)
        delete stairSettings.at(i);
    stairSettings.clear();

    //cerr << "TabStaircase::reset() size=" << stairSettings.size() << endl;
}

void TabStaircase::setNumberOfTabs(int n)
{
    int m = this->count();
    if ( n> this->count() )
    {
        for ( int i=m; i< n; i++ )
        {
            StaircaseParamSetter *s = new StaircaseParamSetter(this);
            stairSettings.push_back(s);
            this->addTab(stairSettings.back(),QString("Staircase ") + QString::number(i));
        }
    }
    else
        for (int i=m; i>n; i--)
        {
            delete stairSettings.back();
            stairSettings.pop_back();
            delete this->widget(i);
        }

    //cerr << "TabStaircase::count= " << count() << " nstair= " << stairSettings.size() << endl;
}


int TabStaircase::getNumberOfTabs()
{
    return (int)stairSettings.size();
}
