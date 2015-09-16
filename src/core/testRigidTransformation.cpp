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
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Util.h"
#include "Mathcommon.h"
#include "RigidTransformation.h"
#include "Marker.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;
static const unsigned int N = 4;

int main(int argc, char *argv[])
{

    std::vector< Marker> markers,markersBase;
    for (int i=0; i<N; i++)
        markers.push_back( Marker( mathcommon::unifRand(0.0,1.0) ,mathcommon::unifRand(0.0,1.0) ,mathcommon::unifRand(0.0,1.0) ) );
    markersBase=markers;
    vector<Marker> a;
    a= util::vlist_of< Marker>(markers[0])(Marker(1,2,3));
    RigidTransformation<N> rigidTransf;

    rigidTransf.init( markersBase );

    for (int c=0; c<10; c++)
    {
        AngleAxis<double> aa( ((double)c)*0.01,Vector3d::UnitY() );
        Affine3d m = Affine3d::Identity();
        //m.linear().matrix() = aa.toRotationMatrix();
        m.translation() << (double)c*c, 0,0;
        for (int i=0; i<N; i++)
            markers.at(i) = markersBase.at(i)*m;

        //if (c%2==0)
        //markers[c%4].p << 1E10,1E10,1E10;
        cout << "invisible marker " ;

        markers[0].p << 1E10,1E10,1E10;
        markers[1].p << 1E10,1E10,1E10;

        cout << endl;
        rigidTransf.update(markers);

        //cout << setw(6) << setprecision(3) << rigidTransf.getRoll() << "\t" << rigidTransf.getPitch() << "\t" << rigidTransf.getYaw() <<"\t" << m.translation().transpose() << endl;
        cout << setw(2) << setprecision(3) << rigidTransf.getFullTransformation().translation().transpose() << endl;
    }
    /*
    cout << coord.getFullTransformation().matrix() << endl;
    cout << "Axis = " << coord.getAxis().transpose() << endl;
    cout << "Angle " << coord.getFullTransformation().linear().eulerAngles(1,0,2).transpose()*180/3.1415 << endl;
*/
    return 0;
}

