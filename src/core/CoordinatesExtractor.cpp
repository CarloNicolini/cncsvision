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

#include "CoordinatesExtractor.h"

/**
* \ingroup Geometry
* Constructor
*
**/

CoordinatesExtractor::CoordinatesExtractor()
{  interOcularDistance=1;

}

/**
* \ingroup Geometry
* Set the rigid body transformation, this is like a photoshot
*
**/

void CoordinatesExtractor::init(const Vector3d &leftMask, const Vector3d &rightMask,
                                const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3, double iod )
{  interOcularDistance=iod;
   p1Start.p=leftMask;
   p2Start.p=rightMask;

   p1.p=p1Start.p;
   p2.p=p2Start.p;

   p0Start.p = Vector3d::Zero();

   rigidStart[0].setRigidBody(ref1,ref2,ref3);

}

/**
* \ingroup Geometry
* Set the rigid body transformation, this is like a photoshot. This method is thought for estimating the finger position.
* You must call init() only when finger relative distance to the 3 references is fixed.
*
**/
void CoordinatesExtractor::init(const Vector3d &fingerStart, const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3 )
{  p1Start.p=fingerStart.eval();
   p2.p = Vector3d::Zero();

   p1.p = p1Start.p;
   p0Start.p = Vector3d::Zero();

   rigidStart[0].setRigidBody(ref1,ref2,ref3);
}

/**
* \ingroup Geometry
*
* Set the rigid body transformation, this is like a photoshot, this version works for pure positional points
*
**/
void CoordinatesExtractor::update(const Vector3d &ref1, const Vector3d &ref2, const Vector3d &ref3 )
{  rigidEnd[0].setRigidBody(ref1,ref2,ref3);
   rigidStart[0].computeTransformation(rigidEnd[0]);
   p1.p=rigidStart[0].getFullTransformation()*p1Start.p;
   //cerr << p1Start.p.transpose() << "\t" << p1.p.transpose() << endl;
   p2.p=rigidStart[0].getFullTransformation()*p2Start.p;
   // Set the view direction
   p0.p=rigidStart[0].getFullTransformation()*p0Start.p;

}

/**
* \ingroup Geometry
*
* Set the rigid body transformation, this is like a photoshot, this version works for markers [ position, velocity ]
*
**/
void CoordinatesExtractor::update(const Marker &ref1, const Marker &ref2, const Marker &ref3 )
{  rigidEnd[0].setRigidBody(ref1.p,ref2.p,ref3.p);
   rigidStart[0].computeTransformation(rigidEnd[0]);

   // This part is needed to compute velocity for ghost points (see CNCSVision main article and axis-angle representation of rotation matrices on wikipedia)
   // Y is the rotation matrix that rotates the body from the position at time t to the position
   // at time t + delta t.
   Matrix3d Y = rigidStart[1].getFullTransformation().linear()*rigidStart[0].getFullTransformation().linear().transpose();
   //cout << Y << endl << endl;

   // Here we follow the axis angle representation of rotation matrices
   //double framerate=10;  //hertz
   double theta  = acos((Y.trace()-1.0)/2.0)*10;
   double denominator = 0.5/(sin(theta));
   double omegax = denominator*(Y.coeffRef(2,1) - Y.coeffRef(1,2));
   double omegay = denominator*(Y.coeffRef(0,2) - Y.coeffRef(2,0));
   double omegaz = denominator*(Y.coeffRef(1,0) - Y.coeffRef(0,1));
   double omeganorm = sqrt(omegax*omegax+omegay*omegay+omegaz*omegaz);
   omegax/=omeganorm;
   omegay/=omeganorm;
   omegaz/=omeganorm;
   //cout << fixed << omegax << " " << omegay << " " << omegaz << " " << toDegrees(theta) << endl;
   // Create the angular velocity rank 2 tensor
   Matrix3d Omega;
   Omega << 0, -omegaz, omegay,
         omegaz, 0, -omegax,
         -omegay, omegax, 0 ;
   // Another method by linear system solving
   //Vector3d omega;
   //Vector3d result = Y.lu().solve(omega);            // using partial-pivoting LU

   //cerr << Omega.determinant() << endl;
   // Compute the translational velocity
   Vector3d t0 = rigidStart[0].getFullTransformation().translation();
   Vector3d t1 = rigidStart[1].getFullTransformation().translation();

   Vector3d translationVel = (ref1.v+ref2.v+ref3.v)/3 - Omega*t0; //XXX non son molto sicuro di fare (v1+v2+v3)/3 per ottenere la velocità  di
   // traslazione del corpo anche se in effetti e' proprio la velocita'  del centro di massa
   //cerr << translationVel.transpose().cast<int>() << endl;

   p1.p=rigidStart[0].getFullTransformation()*p1Start.p;
   p2.p=rigidStart[0].getFullTransformation()*p2Start.p;
   // Set velocities
   p1.v = Omega*p1.p + translationVel;
   p2.v = Omega*p2Start.p + translationVel;

   // Set the view direction
   p0.p=rigidStart[0].getFullTransformation()*p0Start.p;

   rigidStart[1]=rigidStart[0]; // the previous starting rigidbody
   rigidEnd[1]=rigidEnd[0];     // the previous rigidbody
}

Vector3d& CoordinatesExtractor::getP2Vel()
{  return p2.v;
}

Vector3d& CoordinatesExtractor::getP1Vel()
{  return p1.v;
}

void CoordinatesExtractor::reset()
{  p1.p = p2.p = p1Start.p = p2Start.p = p0Start.p = Vector3d(0.0,0.0,0.0);
}
/**
* \ingroup Geometry
*
* Set the interOcularDistance for the subject. Default 60.
*
**/
void CoordinatesExtractor::setInterOcularDistance(double iod)
{  interOcularDistance=iod;
}

/**
* \ingroup Geometry
*
* Get a reference to left eye coordinates
*
**/

Vector3d& CoordinatesExtractor::getLeftEye()
{  eyeLeft.p  = (p1.p+p2.p)/2.0 - (p2.p-p1.p).normalized()*(interOcularDistance/2.0);
   return eyeLeft.p;
}

/**
* \ingroup Geometry
*
* Get a reference to right eye coordinates
*
**/
Vector3d& CoordinatesExtractor::getRightEye()
{  eyeRight.p  = (p1.p+p2.p)/2.0 + (p2.p-p1.p).normalized()*(interOcularDistance/2.0);
   return eyeRight.p;
}

/**
* \ingroup Geometry
*
* Get a reference to right eye coordinates
*
**/
Vector3d& CoordinatesExtractor::getCyclopeanEye()
{  cyclopeanEye.p = (p1.p+p2.p)/2;
   return cyclopeanEye.p;
}


/**
* \ingroup Geometry
* get p1
**/
Vector3d& CoordinatesExtractor::getP1()
{  return p1.p;
}

/**
* \ingroup Geometry
* get p1
**/
Vector3d& CoordinatesExtractor::getP2()
{  return p2.p;
}


/**
* \ingroup Geometry
*
* Set the point behind the eyes needed to compute the viewing direction
*
**/
void CoordinatesExtractor::setEyesPlane(const Vector3d &v)
{  p0Start.p=v;
}

/**
* \ingroup Geometry
*
* Get the viewing direction vector
*
**/
Vector3d& CoordinatesExtractor::getViewingDirection(double scale)
{  viewDirection.p = rigidStart[0].getFullTransformation().linear()*Vector3d(0,0,-1);
   return viewDirection.p;
}

RigidBody & CoordinatesExtractor::getRigidStart()
{  return rigidStart[0];
}

/**
* \ingroup Geometry
* Return the finger position if previously the finger relative distance has been fixed
*
**/
Vector3d& CoordinatesExtractor::getFinger()
{  return p1.p;
}
