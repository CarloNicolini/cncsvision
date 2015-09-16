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
#include "VRCamera.h"

using namespace std;
//using namespace Eigen;
/*
Real screen coordinates meaning (frontal view)
             ----------------------
            /                    / |
           /                    /  |
        (pc)--------------------   |
         |                      |  |
         |                      |  |
         |                      |  |
         |                      |  |
         |                      | /
        (pa)-------------------(pb)

If you use the mirror the correct points positions are (imagine them to stay in front of you)
             ----------------------
            /                    / |
           /                    /  |
          -------------------- (pc)|
         |                      |  |
         |                      |  |
         |                      |  |
         |                      |  |
         |                      | /
        (pb)-------------------(pa)

and don't forget to set the variable useMirror=true, this is to ensure that the points are correctly computed
*/


/**
* \ingroup GLVisualization
* Standard constructor, it sets near projection plane distance to 0.1 and far projection plane to 1000 by default, you can change these values with void \ref setNearFarPlanes(GLfloat _n, GLfloat _f)
**/
VRCamera::VRCamera()
{  n=100;
   f=1800;
   isOrthoGraphic=false;
   drySimulation=false;
   //modelview=Eigen::Affine3d::Identity();
   this->modelview = new Eigen::Affine3d();
   this->mview = new Eigen::Affine3d();
   this->projectionMatrix = new Eigen::Projective3d();;
   this->viewport = new Eigen::Vector4i();

   this->modelview->setIdentity();
   this->mview->setIdentity();
   this->projectionMatrix->setIdentity();
   this->viewport->setZero();
}

/** \ingroup GLVisualization
* Copy constructor
* \param rhs VRCamera object to copy from
**/

VRCamera::VRCamera(const VRCamera &rhs)
{  pA=rhs.pA;
   pB=rhs.pB;
   pC=rhs.pC;
   pEye=rhs.pEye;
   mirrorMode=rhs.mirrorMode;
   isOrthoGraphic=rhs.isOrthoGraphic;
   n=rhs.n;
   f=rhs.f;
   *modelview = *(rhs.modelview);
   *mview = *(rhs.mview);
   *projectionMatrix=*(rhs.projectionMatrix);
   *viewport=*(rhs.viewport);
   this->init(pA,pB,pC);
}

VRCamera::~VRCamera()
{
    delete modelview;
    delete projectionMatrix;
    delete mview;
    delete viewport;
}

/**
 * @brief VRCamera::VRCamera
 * @param screen
 */
VRCamera::VRCamera(const Screen &screen)
{
    this->init(screen.pa,screen.pb,screen.pc);
}

/**
* \ingroup GLVisualization
* Set the distances of near and far plane, 100 and 800 are typical good values. They affect the z-buffer depth, so take core of choosing them correctly
* \param _n is the near plane distance
* \param _f is the far plane distance
*
* Remember that these values affect the z-buffer precision. In particular try to keep the ratio f/n
* as small as possible. Nor n or f affect the z-compression, image-plane depth or other things involved in
* the geometry of the visualization.
**/
void VRCamera::setNearFarPlanes(GLfloat nearPlane, GLfloat farPlane)
{
	this->n = nearPlane;
	this->f = farPlane;
}

/**
* \ingroup GLVisualization
* This is needed to initialize the real-world screen coordinates
* The order of the arguments is important, remember that:
* \f$ \mathbf{p}_a \f$ is the lower left point of the screen
*\f$ \mathbf{p}_c \f$ is the upper left point of the screen
* \f$ \mathbf{p}_b \f$ is the lower right point of * the screen
* Here we assume that you stay in the front of the monitor, your right arm points
* toward the positive x axis, your eye points toward the negative z axis,
* the screen lie at negative z coordinates and you eye is the (0,0,0)
*
**/
void VRCamera::init(const Eigen::Vector3d &pa, const Eigen::Vector3d &pb, const Eigen::Vector3d &pc )
{  // Initialize the internal variables
   pA=pa ;
   pB=pb ;
   pC=pc ;

   // Compute an orthonormal basis for the screen

   if (mirrorMode)
      vR = (pA - pB).normalized();
   else
      vR = (pB - pA).normalized();

   vU = (pC - pA).normalized();
   vN = (vR.cross(vU)).normalized();
}

/*
* \ingroup GLVisualization
* Init the \ref VRCamera object starting from a predefined screen
*
*/
void VRCamera::init(const Screen &screen, bool useMirror)
{  init(screen.pa,screen.pb,screen.pc,useMirror);
}

/**
* \ingroup GLVisualization
* This is needed to initialize the real-world screen coordinates
* The order of the arguments is important, remember that:
* \f$ \mathbf{p}_a \f$ is the lower left point of the screen
*\f$ \mathbf{p}_c \f$ is the upper left point of the screen
* \f$ \mathbf{p}_b \f$ is the lower right point of * the screen
* Here we assume that you stay in the front of the monitor, your right arm points
* toward the positive x axis, your eye points toward the negative z axis,
* the screen lie at negative z coordinates and you eye is the (0,0,0)
*
**/
void VRCamera::init(const Eigen::Vector3d &pa, const Eigen::Vector3d &pb, const Eigen::Vector3d &pc, bool useMirror )
{  mirrorMode=useMirror;

   init(pa,pb,pc);
   glGetIntegerv(GL_VIEWPORT, viewport->data());
}

/**
* \ingroup GLVisualization
* Update the monitor points pA, pB, pC
**/
void VRCamera::updateMonitorPoint(const Eigen::Vector3d &_pa, const Eigen::Vector3d &_pb, const Eigen::Vector3d &_pc)
{  pA=_pa;
   pB=_pb;
   pC=_pc;
}

/**
 * @brief VRCamera::updateMonitorPoint
 * @param screen
 */
void VRCamera::updateMonitorPoint(const Screen &screen)
{  init(screen);
}

/**
 * @brief VRCamera::setOrthoGraphicProjection
 * @param val
 */
void VRCamera::setOrthoGraphicProjection(bool val)
{  isOrthoGraphic=val;
}

/**
* \ingroup GLVisualization
* Set the eye positions, this method has to be called continously to update the view point (usually call it in the drawGLScene function )
*
**/
void VRCamera::setEye(const Eigen::Vector3d &_eye)
{  pEye=_eye;
   //  Compute the screen corner vectors
   vA = pA - pEye;
   vB = pB - pEye;
   vC = pC - pEye;

   // Find the distance from the eye to screen plane
   d = -vA.dot(vN);
   // Find the extent of the perpendicular projection
   l = (GLfloat) vR.dot(vA)*n/d;
   r = (GLfloat) vR.dot(vB)*n/d;
   b = (GLfloat) vU.dot(vA)*n/d;
   t = (GLfloat) vU.dot(vC)*n/d;

   project();
}

/**
 * @brief VRCamera::reset
 */
void VRCamera::reset()
{  n=100;
   f=1800;
   *modelview=Eigen::Affine3d::Identity();
   *projectionMatrix=Eigen::Projective3d::Identity();
}

/**
* \ingroup GLVisualization
* Project the actual scene using the generalized perspective projection paradigm
* ( we call the glFrustum with 6 parameters computed in the setEye().
*
* Beginning with constant screen corners \f$ \mathbf{p}_a , \mathbf{p}_b ,\mathbf{p}_c \f$ and varying eye position \f$ \mathbf{p}_e \f$
* we can straightforwardly produce a projection matrix that will work under all circumstances.
* Projection can also be computed in orthographic projection.
**/
void VRCamera::project()
{  // For a detailed introduction to the motivation of perspective and orthographic matrix, see
   // 3D Computer graphics: A mathematical introduction, Chapter II
   projectionMatrix->setIdentity();
   if ( isOrthoGraphic )
   {  // equivalent to glOrtho(l,r,b,t,n,f);
      projectionMatrix->matrix().coeffRef(0,0)=  2.0/(r-l);
      projectionMatrix->matrix().coeffRef(0,3)=  -(r+l)/(r-l);
      projectionMatrix->matrix().coeffRef(1,1)=  2.0/(t-b);
      projectionMatrix->matrix().coeffRef(1,3)=  -(t+b)/(t-b);
      projectionMatrix->matrix().coeffRef(2,2)=  -2.0/(f-n);
      projectionMatrix->matrix().coeffRef(2,3)=  -(f+n)/(f-n);
      projectionMatrix->matrix().coeffRef(3,3)=  1;
      // Scale the x and y components to have the same size at focal plane of perspective projection (equivalent to glScaled(n/d,n/d,1); )
      projectionMatrix->matrix().coeffRef(0,0)*=n/d;
      projectionMatrix->matrix().coeffRef(1,1)*=n/d;
   }
   else
   {  //glFrustum(l,r,b,t,n,f);   //here instead of glfrustum we put directly the projection matrix value inside (greater control)
      projectionMatrix->matrix().coeffRef(0,0)=  2*n/(r-l);
      projectionMatrix->matrix().coeffRef(0,2)=  (r+l)/(r-l);
      projectionMatrix->matrix().coeffRef(1,1)=  2*n/(t-b);
      projectionMatrix->matrix().coeffRef(1,2)=  (t+b)/(t-b);
      projectionMatrix->matrix().coeffRef(2,2)=  -(f+n)/(f-n);
      projectionMatrix->matrix().coeffRef(2,3)=  -2*f*n/(f-n);
      projectionMatrix->matrix().coeffRef(3,2)=  -1;
   }
   // Rotate the projection to be non-perpendicular
   // P' = P*M^T*T
   modelview->setIdentity();
   modelview->linear().col(0)=vR;
   modelview->linear().col(1)=vU;
   modelview->linear().col(2)=vN;
   modelview->matrix().transposeInPlace();
   (*modelview)*=Eigen::Translation3d( -pEye );
   *mview=*modelview;

   if ( !drySimulation )
   {  // Load the perpendicular projection
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glLoadMatrixd(((*projectionMatrix)*(*modelview)).data());

      glMatrixMode(GL_MODELVIEW);
   }
}

/**
 * @brief VRCamera::setDrySimulation
 * @param _dry
 */
void VRCamera::setDrySimulation(bool _dry)
{  drySimulation=_dry;
}

/**
 * @brief VRCamera::getModelViewMatrix
 * @return
 */
const Eigen::Affine3d &VRCamera::getModelViewMatrix() const
{  return *mview;
}

/**
 * @brief VRCamera::getProjectiveMatrix
 * @return
 */
const Eigen::Projective3d& VRCamera::getProjectiveMatrix() const
{  return *projectionMatrix;
}

/**
 * @brief VRCamera::getViewport
 * @return
 */
const Eigen::Vector4i& VRCamera::getViewport() const
{  return *viewport;
}

/**
 * @brief VRCamera::getEye
 * @return
 */
const Eigen::Vector3d & VRCamera::getEye() const
{  return pEye;
}

/**
 * @brief VRCamera::computeProjected
 * @param x
 * @return
 */
Eigen::Vector2d VRCamera::computeProjected(const Eigen::Vector3d &x)
{
    Eigen::Projective3d P = *projectionMatrix;
    Eigen::Affine3d M = *mview;
    Eigen::Vector3d y = ( ( (*projectionMatrix)* ((*mview)*   ( x )   ) .homogeneous() ).eval().hnormalized() );

    return Eigen::Vector2d( (*viewport)(0) + (*viewport)(2)*(y(0)+1)/2, (*viewport)(1) + (*viewport)(3)*(y(1)+1)/2);
}

