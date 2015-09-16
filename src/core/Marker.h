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

#ifndef _MARKER_H_
#define _MARKER_H_

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

/**
 * \ingroup Geometry
 * @brief The Marker class is needed to handle Optotrak markers with their coordinates, speed and accelerations in a simple way. All the members are public for a quicker access.
 */
class Marker
{
public:
   // http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   /**
    * \ingroup Geometry
    * @brief Marker default constructor, it set p,v,a to zero
    */
   Marker()
   {  p=v=a=Eigen::Matrix<double,3,1>::Zero();
   }
   /**
    * \ingroup Geometry
    * @brief Marker copy constructor
    * @param rhs
    */
   Marker(const Marker &rhs)       // copy constructor
   {  this->p = rhs.p;
      this->v = rhs.v;
      this->a = rhs.a;
   }
   /**
    * @brief operator =
    * @param rhs The source marker to copy
    * @return Marker copied
    */
   Marker & operator= (const Marker & rhs)
   {  this->p = rhs.p;
      this->v = rhs.v;
      this->a = rhs.a;
      return *this;
   }

   /**
    * @brief Marker destructor
    **/
   ~Marker()
   {
   }

   /**
    * @brief Marker fast constructor, it creates a marker with given x,y,z position, set everything else to zero
    * @param px position x
    * @param py position y
    * @param pz position z
    */
   Marker(double px, double py, double pz)
   {
       this->p << px,py,pz;
   }
   /**
    * @brief Marker constructor from Vector3d, assigns the position coordinates
    * @param _p
    */
   Marker(const Eigen::Matrix<double,3,1> &_p)
   {  this->p=_p;
      this->v= this->a = Eigen::Matrix<double,3,1>::Zero();
   }
   /**
    * @brief Marker  constructor from Vector3d, assigns the position coordinates  and velocities
    * @param _p coordinates
    * @param _v velocities
    */
   Marker(const Eigen::Matrix<double,3,1>_p,const Eigen::Matrix<double,3,1>&_v)
   {  this->p=_p;
      this->v=_v;
      this->a = Eigen::Matrix<double,3,1>::Zero();
   }

   /**
    * @brief Marker  constructor from Vector3d, assigns the position coordinates, velocities and accelerations
    * @param _p coordinates
    * @param _v velocities
    * @param _a accelerations (mostly useless)
    */
   Marker(const Eigen::Matrix<double,3,1>_p,const Eigen::Matrix<double,3,1>&_v,const Eigen::Matrix<double,3,1>_a)
   {  this->p=_p;
      this->v=_v;
      this->a=_a;
   }

   Eigen::Matrix<double,3,1> p,v,a;

   /**
    * @brief operator <<
    * @param output
    * @param v
    * @return
    */
   friend std::ostream &operator<<(std::ostream &output, const Marker &v)
   {  output  << v.p.transpose() << '\t' << v.v.transpose() << '\t' << v.a.transpose() ;
      return output;
   }

   /**
    * @brief operator * Get a new transformed marker with coordinates transformed as transformation
    * @param transformation affine transformation to use
    * @return
    */
   Marker operator*( const Eigen::Affine3d &transformation )
   {
       return Marker( transformation*(this->p) );
   }

   /**
    * @brief operator *= Inplace transform position coordinates of marker using given affine transformation
    * @param transformation affine transformation to use
    */
   void operator*=( const Eigen::Affine3d &transformation )
   {
        this->p  << ( transformation*(this->p) );
   }

   /**
    * @brief isVisible
    * @return true if the marker is visible from the Optotrak towers, false otherwise, it is based on a simple comparison
    */
   bool isVisible()
   {
       return ( abs(p.coeffRef(0))+abs(p.coeffRef(1))+abs(p.coeffRef(2)) < 1E10);
   }
};

#endif
