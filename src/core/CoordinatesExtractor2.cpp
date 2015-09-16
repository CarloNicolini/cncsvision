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

#include "CoordinatesExtractor2.h"


/**
* \ingroup Geometry
* Constructor
*
**/

CoordinatesExtractor2::CoordinatesExtractor2()
{  interOcularDistance=1;
   currentPoints.resize(2);
   prevpoints.resize(2);
   prevprevpoints.resize(2);
   eyeLeft.resize(3);
   eyeRight.resize(3);
   cyclopeanEye.resize(3);
   initialized=false;
   headMode=false;
   filterVelocity=false;
}


CoordinatesExtractor2::~CoordinatesExtractor2()
{  clear();
   clearFilter();
}

void CoordinatesExtractor2::setFilterVelocity(bool _filt,double filtergainval)
{  filterVelocity=_filt;
   filterGain=filtergainval;
}
void CoordinatesExtractor2::clearFilter()
{  for (unsigned int i=0; i<filters.size(); i++)
   {  for (unsigned int j=0; j<3; j++ )
      {  if ( filters[i][j] != NULL )
         {  delete filters[i][j];
            filters[i][j]=NULL;
         }
      }
   }
}

void CoordinatesExtractor2::initFilter(int size, double gain)
{  filters.resize(size);
   for (unsigned int i=0; i<filters.size(); i++)
   {  filters[i].resize(3);
      for (unsigned int j=0; j<3; j++ )
      {  filters[i][j] = DaisyFilter::SinglePoleIIRFilter(gain);
      }
   }
}

void CoordinatesExtractor2::clear()
{  startPoints.clear();
   currentPoints.clear();
   prevpoints.clear();
   prevprevpoints.clear();

}
/**
* \ingroup Geometry
* Set the rigid body transformation, this is like a photoshot. This method is thought for estimating the finger position.
* You must call init() only when finger relative distance to the 3 references is fixed.
*
**/
void CoordinatesExtractor2::init(const Vector3d &fingerStart, const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3 )
{  rigidStart.setRigidBody(ref1,ref2,ref3);

   Marker leftMaskMarker(fingerStart);

   clear();
   startPoints.resize(1);
   startPoints[0]=  leftMaskMarker ;


   currentPoints.resize(1);
   prevpoints.resize(1);
   prevprevpoints.resize(1);

   if ( filters.size() == 0 )
   {  filters.resize(1);
      initFilter(1,filterGain);
   }
   else
   {  clearFilter();
      initFilter(1,filterGain);
   }

   initialized=true;
}

/**
* \ingroup Geometry
* Set the rigid body transformation, this is like a photoshot
*
**/

void CoordinatesExtractor2::init(const Vector3d &leftMask, const Vector3d &rightMask,
                                 const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3, double iod )
{  interOcularDistance=iod;

   rigidStart.setRigidBody(ref1,ref2,ref3);

   Marker leftMaskMarker(leftMask);
   Marker rightMaskMarker(rightMask);

   clear();
   startPoints.resize(2);
   startPoints[0] = leftMaskMarker ;
   startPoints[1] = rightMaskMarker ;

   currentPoints.resize(2);
   prevpoints.resize(2);
   prevprevpoints.resize(2);

   if ( filters.size() == 0 )
   {  filters.resize(3);
      initFilter(3,filterGain);
   }
   else
   {  clearFilter();
      initFilter(3,filterGain);
   }

   initialized=true;
   headMode=true;
}

void CoordinatesExtractor2::init(const vector<Marker> &markers, const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3 )
{  rigidStart.setRigidBody(ref1,ref2,ref3);

   clear();
   unsigned int n=markers.size();
   for (unsigned int j=0; j<n; j++ )
      startPoints.push_back( markers[j] );


   currentPoints.resize(n);
   prevpoints.resize(n);
   prevprevpoints.resize(n);
   //cerr << startPoints.size() << "\t" << currentPoints.size() << "\t" << prevpoints.size() << "\t" << prevprevpoints.size() << endl;

   if ( filters.size() == 0 )
   {  filters.resize(n);
      initFilter(n,filterGain);
   }
   else
   {  clearFilter();
      initFilter(n,filterGain);
   }

   initialized=true;
   headMode=false;
}

/**
* \ingroup Geometry
*
* Set the rigid body transformation, this is like a photoshot, this version works for pure positional points
*
**/
void CoordinatesExtractor2::update(const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3, double deltaTInMilliSeconds )
{  try
   {  if ( initialized )
      {  rigidEnd.setRigidBody(ref1,ref2,ref3);
         rigidStart.computeTransformation(rigidEnd);

         // Backward difference differentiation scheme
         for ( unsigned int i=0; i<currentPoints.size(); i++ )
         {  
		    #ifdef COMPUTE_VELOCITIES
		    prevprevpoints[i].p = prevpoints[i].p;
            prevpoints[i].p = currentPoints[i].p;
			#endif
            currentPoints[i].p = rigidStart.getFullTransformation()*startPoints[i].p;
         }
         double denominator = 0.5/(1e-3*deltaTInMilliSeconds);

         // velocities are measured in mm/s
         if ( headMode )
         {  eyeRight[2].p=eyeRight[1].p;
            eyeRight[1].p=eyeRight[0].p;
            eyeRight[0].p = (currentPoints[0].p+currentPoints[1].p)*0.5 + (currentPoints[1].p-currentPoints[0].p).normalized()*(interOcularDistance*0.5);

            eyeLeft[2].p=eyeLeft[1].p;
            eyeLeft[1].p=eyeLeft[0].p;
            eyeLeft[0].p = (currentPoints[0].p+currentPoints[1].p)*0.5 - (currentPoints[1].p-currentPoints[0].p).normalized()*(interOcularDistance*0.5);

            cyclopeanEye[2].p = cyclopeanEye[1].p;
            cyclopeanEye[1].p = cyclopeanEye[0].p;
            cyclopeanEye[0].p = (currentPoints[0].p+currentPoints[1].p)*0.5;
         }

         #ifdef COMPUTE_VELOCITIES
         Vector3d vtmp = Vector3d::Zero();
         Vector3d atmp = Vector3d::Zero();

         for ( unsigned int i=0; i<currentPoints.size(); i++ )
         {  prevprevpoints[i].v = prevpoints[i].v;
            prevpoints[i].v = currentPoints[i].v;
            currentPoints[i].v = (3*currentPoints[i].p - 4*prevpoints[i].p + prevprevpoints[i].p )*denominator ;
            currentPoints[i].a = (3*currentPoints[i].v - 4*prevpoints[i].v + prevprevpoints[i].v )*denominator ;

            if ( !headMode && filterVelocity )
            {  for ( int j=0; j<3; j++ )
                  currentPoints[i].v.coeffRef(j) = filters[i][j]->Calculate( currentPoints[i].v.coeffRef(j) );   // velocity
            }
         }
		 
         if ( headMode )
         {  // Backward difference schema for special points
            eyeRight[2].v = eyeRight[1].v;
            eyeRight[1].v = eyeRight[0].v;
            eyeRight[0].v = (3*eyeRight[0].p - 4*eyeRight[1].p + eyeRight[2].p)*denominator;
            eyeRight[0].a = (3*eyeRight[0].v - 4*eyeRight[1].v + eyeRight[2].v)*denominator;

            eyeLeft[2].v = eyeLeft[1].v;
            eyeLeft[1].v = eyeLeft[0].v;
            eyeLeft[0].v = (3*eyeLeft[0].p - 4*eyeLeft[1].p + eyeLeft[2].p)*denominator;
            eyeLeft[0].a = (3*eyeLeft[0].v - 4*eyeLeft[1].v + eyeLeft[2].v)*denominator;

            cyclopeanEye[2].v = cyclopeanEye[1].v;
            cyclopeanEye[1].v = cyclopeanEye[0].v;
            cyclopeanEye[0].v = (3*cyclopeanEye[0].p - 4*cyclopeanEye[1].p + cyclopeanEye[2].p)*denominator;
            cyclopeanEye[0].a = (3*cyclopeanEye[0].v - 4*cyclopeanEye[1].v + cyclopeanEye[2].v)*denominator;

            if ( filterVelocity )
            {  for (int j=0; j<3; j++ )
               {  eyeRight[0].v.coeffRef(j) = filters[0][j]->Calculate( eyeRight[0].v.coeffRef(j) );
                  eyeLeft[0].v.coeffRef(j) = filters[1][j]->Calculate( eyeLeft[0].v.coeffRef(j) );
                  cyclopeanEye[0].v.coeffRef(j) = filters[2][j]->Calculate( cyclopeanEye[0].v.coeffRef(j) );
               }
            }
         }
         #endif
      }
   }
   catch ( std::exception const& e )
   {  cerr << e.what() << endl;
   }
}

/**
* \ingroup Geometry
*
* Set the rigid body transformation, this is like a photoshot, this version works for markers [ position, velocity ]
*
**/
void CoordinatesExtractor2::update(const Marker &ref1, const Marker &ref2, const Marker &ref3, double deltaTInMilliSeconds )
{  update(ref1.p, ref2.p, ref3.p, deltaTInMilliSeconds);
}

/**
* \ingroup Geometry
*
* Set the interOcularDistance for the subject. Default 60.
*
**/
void CoordinatesExtractor2::setInterOcularDistance(double iod)
{  interOcularDistance=iod;
}

/**
* \ingroup Geometry
*
* Get a reference to left eye coordinates
*
**/

const Marker& CoordinatesExtractor2::getLeftEye()
{  
	if (!headMode && initialized )
         throw std::runtime_error("***** This rigidbody is not a head, so you can't pick the left eye! *****");
   return eyeLeft[0];
}

/**
* \ingroup Geometry
*
* Get a reference to right eye coordinates
*
**/
const Marker& CoordinatesExtractor2::getRightEye()
{  if (!headMode && initialized )
         throw std::runtime_error("***** This rigidbody is not a head, so you can't pick the left eye! *****");
   
   return eyeRight[0];
}

/**
* \ingroup Geometry
*
* Get a reference to right eye coordinates
*
**/
const Marker& CoordinatesExtractor2::getCyclopeanEye()
{  if (!headMode && initialized )
         throw std::runtime_error("***** This rigidbody is not a head, so you can't pick the left eye! *****");
   return cyclopeanEye[0];
}

/**
* \ingroup Geometry
* get p1
**/
const Marker& CoordinatesExtractor2::getP1()
{  return currentPoints[0];
}

/**
* \ingroup Geometry
* get p1
**/
const Marker& CoordinatesExtractor2::getP2()
{  return currentPoints[1];

}

const vector<Marker> &CoordinatesExtractor2::getTransformedMarkers()
{  return currentPoints;
}

RigidBody & CoordinatesExtractor2::getRigidStart()
{  return rigidStart;
}

const Vector3d CoordinatesExtractor2::getRotationAxis() const
{
    AngleAxis<double> aa(rigidStart.getFullTransformation().rotation());
    return aa.axis();
}

const Matrix3d CoordinatesExtractor2::getRotationMatrix() const
{
	return rigidStart.getFullTransformation().rotation();
}

const double CoordinatesExtractor2::getYaw() const
{
	return rigidStart.getFullTransformation().linear().eulerAngles(1,0,2).x();
}

const double CoordinatesExtractor2::getPitch() const
{
	return rigidStart.getFullTransformation().linear().eulerAngles(1,0,2).y();
}

const double CoordinatesExtractor2::getRoll() const
{
	return rigidStart.getFullTransformation().linear().eulerAngles(1,0,2).z();
}