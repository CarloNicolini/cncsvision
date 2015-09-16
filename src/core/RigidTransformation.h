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

#ifndef RIGIDTRANSFORMATION_H
#define RIGIDTRANSFORMATION_H

#include <cassert>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Marker.h"

template <unsigned int N>
class RigidTransformation
{
public:
    // This is for a pointers alignment issue, read the following website for further informations:
    // http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    RigidTransformation();
    ~RigidTransformation();
    bool init( std::vector< Marker> &startPoints  );
    bool update(std::vector< Marker> &endPoints  , bool doScaling=false);

    const Affine3d & getFullTransformation() const;

    const Vector3d getAxis() const;
    const double getRoll() const;
    const double getPitch() const;
    const double getYaw() const;

private:
    std::vector < Marker > _startPoints, _endPoints;

    Eigen::Matrix<double,3,N> sourceMarkers;
    Eigen::Matrix3d destinationTriple;
    Eigen::Affine3d A;
};

/**
 * BEGIN IMPLEMENTATION
**/

template<unsigned int N>
RigidTransformation<N>::RigidTransformation()
{
    sourceMarkers.setZero();
    destinationTriple.setZero();
}

template<unsigned int N>
RigidTransformation<N>::~RigidTransformation()
{

}

template<unsigned int N>
bool  RigidTransformation<N>::init(std::vector<Marker> &startPoints)
{
    assert(startPoints.size()==N );

    // First check that all markers are visible
    bool allVisible=true;
    for (unsigned int i=0; i<N; i++)
    {
        allVisible = allVisible && startPoints.at(i).isVisible();
    }
    if (allVisible)
    {
        _startPoints = startPoints;
        return true;
    }
    else
        return false;
}

template<unsigned int N>
bool RigidTransformation<N>::update(std::vector<Marker> &endPoints, bool doScaling)
{
    assert(N==endPoints.size());
    assert(N==_startPoints.size());
    // The source triple MUST have all points visible
    Eigen::Matrix3d sourceTriple = Eigen::Matrix3d::Zero();
    int targets[]={0,0,0};

    int k=0;
    for (int i=0; i<N; i++)
    {
        if ( endPoints.at(i).isVisible() && k<3)
        {
            targets[k]=i;
            destinationTriple.col(k) << endPoints.at(i).p ; // Could be faster by mean of pointer copy...
            ++k;
        }
    }

    for (int i=0; i<3; i++)
    {
        sourceTriple.col(i) << _startPoints.at(targets[i]).p;
    }

    A.matrix() = Eigen::umeyama( sourceTriple, destinationTriple, false);
	return k==3;
}

template<unsigned int N>
const Affine3d & RigidTransformation<N>::getFullTransformation() const
{
    return A;
}

template<unsigned int N>
const double RigidTransformation<N>::getRoll() const
{
    return A.linear().eulerAngles(1,0,2).z();
}

template<unsigned int N>
const double RigidTransformation<N>::getPitch() const
{
    return A.linear().eulerAngles(1,0,2).y();
}

template<unsigned int N>
const double RigidTransformation<N>::getYaw() const
{
    return A.linear().eulerAngles(1,0,2).x();
}

template<unsigned int N>
const Vector3d RigidTransformation<N>::getAxis() const
{
    AngleAxis<double> aa(A.rotation());
    return aa.axis();
}

#endif // RIGIDTRANSFORMATION_H
