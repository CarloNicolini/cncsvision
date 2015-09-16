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

#ifndef _VRCAMERA_H
#define _VRCAMERA_H

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Screen.h"
#include "IncludeGL.h"

#ifdef __WIN32
#define EIGEN_DONT_Eigen::VectorIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_ALIGN_STATICALLY
#endif

//using namespace Eigen;
/**
* \class VRCamera
* \ingroup GLVisualization
* \brief VRCamera is a class for define a monocular point of view in a OpenGL scene.
* VRCamera follows the approach to generalized perspective projection given in  http://aoeu.snth.net/?p=43 Robert Kooima's page.
*
* Basically you first have to compute the real-world screen coordinates by measuring them directly with a
* tracker (we use Optotrak Certus). Look http://aoeu.snth.net/static/gen-perspective.pdf {this PDF full
* explanation}.
*
**/

class VRCamera
{
    Eigen::Affine3d *modelview;
    Eigen::Affine3d *mview;
    Eigen::Projective3d *projectionMatrix;
    Eigen::Vector4i *viewport;

    Eigen::Vector3d pEye;
    Eigen::Vector3d pA, pB, pC;
    Eigen::Vector3d vA, vB, vC;
    Eigen::Vector3d vU, vN, vR;

    //the distance from the eye position pEye to the screen-space origin
    GLfloat d;
    // Frustrum coordinates left, right, bottom, top, near, far
    GLfloat l, r, b, t, n, f;
    // Modelview matrix as affine transformation
    bool mirrorMode;
    bool drySimulation;
    bool isOrthoGraphic;
    void project();
public:
    //EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    VRCamera();
    VRCamera(const VRCamera &rhs);
    VRCamera(const Screen &screen);
    ~VRCamera();
    void setNearFarPlanes(GLfloat near, GLfloat far);

    void init(const Eigen::Vector3d &, const Eigen::Vector3d &, const Eigen::Vector3d &);
    void init(const Eigen::Vector3d &, const Eigen::Vector3d &, const Eigen::Vector3d &, bool useMirror);
    void init(const Screen &, bool useMirror=true);

    void updateMonitorPoint(const Eigen::Vector3d &, const Eigen::Vector3d &, const Eigen::Vector3d &);
    void updateMonitorPoint(const Screen &);
    void setEye(const Eigen::Vector3d &);
    void setOrthoGraphicProjection(bool);
    void reset();
    void setDrySimulation(bool _dry);

    const Eigen::Affine3d& getModelViewMatrix() const;
    const Eigen::Projective3d& getProjectiveMatrix() const;
    const Eigen::Vector4i& getViewport() const;
    const Eigen::Vector3d& getEye() const;

    Eigen::Vector2d computeProjected(const Eigen::Vector3d &x);
};

#endif /* _VRCAMERA_H */
