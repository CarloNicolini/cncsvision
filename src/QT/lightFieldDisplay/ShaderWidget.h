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


#ifndef SHADERWIDGET_H
#define SHADERWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <Eigen/Core>
#include "ParametersLoader.h"

#define PROJECTOR_WIDTH 608
#define PROJECTOR_HEIGHT 684

class ObjLoader2;
class QGLShaderProgram;

#define DRAW_MODEL 0
#define DRAW_TEAPOT 1
#define DRAW_BALL 2

#define GL_FRAGMENT_PRECISION_HIGH 1

class ShaderWidget : public QGLWidget
{
    Q_OBJECT
    //EIGEN_MAKE_ALIGNED_OPERATOR_NEW
public:
    ShaderWidget(QWidget *parent = 0);
    ~ShaderWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void loadObjFile(const std::string& objFileName);
    void initializeShader();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
	int frameNumber;

    // Private methods
private:
    void updateView();
    void saveFrame();

	// Drawing methods
    void drawGpuComputedM();
	void drawSphere(int mode, int radius, int nSlices, int nStacks);
	void drawCylinder(int mode, double radiusX, double radiusZ, double height, int nSlices, int nStacks);
    void drawFunction();
    
    // Variables
    QPointer<QGLShaderProgram> shaderProgram;
    ObjLoader2 *obj;

    Eigen::Vector3d mirrorCenter;
    Eigen::Vector3d mirrorNormal;
    Eigen::Vector3d reflectedProjector;
    Eigen::Vector3d projector;
	Eigen::MatrixXd Projection,ModelView;
    Eigen::MatrixXd VertexTransform;

    double mirrorAngleRad;
    double projectorHeight;
    double viewerDistance;
    double viewerHeight;
    double objectScale;

    QString folderName;
    QString imagesFileBaseName;
    QString imagesFilesFolderName;
    Eigen::Vector3d getReflectedRay(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &normal);
/*
	Eigen::Vector3d getRayCylinderIntersection( const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, float cylinderRadius );
	Eigen::Vector3d getRayPlaneIntersection(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, const Eigen::Vector3d &planeNormal, const Eigen::Vector3d &planeOrigin);
*/
    ParametersLoader projectorParameters;
    int projectorPixelWidth,projectorPixelHeight,viewsNumber ;

};

#endif
