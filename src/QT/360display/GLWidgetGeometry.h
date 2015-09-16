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

#ifndef GLWIDGETGEOMETRY_H
#define GLWIDGETGEOMETRY_H

#include <QGLWidget>
#include <Eigen/Core>
#include "ExperimentalParameters.h"
#include "Screen.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "VRCamera.h"


//! [0]
class GLWidgetGeometry : public QGLWidget
{
        Q_OBJECT
    public:
        GLWidgetGeometry(QWidget *parent = 0);
        ~GLWidgetGeometry();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        void loadMesh(const QString &filename);
        ExperimentalParameters *getParameters();

        //! [1]
    public slots:
        void computeProjectionsBackward();
        void setProjectorPositionX(double x);
        void setProjectorPositionY(double y);
        void setProjectorPositionZ(double z);
        void setProjectorPitch(double pitch);
        void setProjectorRoll(double roll);
        void setProjectorFocal(double f);
        void setProjectorAperture(double aperture);
        void setProjectedAreaEdge(double edge);
        void setMirrorRotation(double angle);
        void setMirrorRotationSpeed(double freq);
        void setViewerDistance(double distance);
        void setViewerHeight(double h);
        void setAutorotation(bool autorot);
        void setViewRaysBackward(bool v);
        void setViewRaysForward(bool v);
        void setRayVertexIndex(int v);

    signals:
        void modelChanged();
        void angleChanged(double);
        void modelViewMatrixChanged( const Eigen::Matrix4d &);
        void projectionMatrixChanged(const Eigen::Matrix4d &);
        void modelViewProjectionMatrixChanged(const Eigen::Matrix4d &);
        void projPlanesChanged(const Screen &s);
        void verticesChanged(GLfloat *ptr, unsigned int n);

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        void keyPressEvent(QKeyEvent *);
        void keyReleaseEvent(QKeyEvent *);
        void mousePressEvent(QMouseEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

        // Draw methods
        void drawReferenceXYZ(double length);
        void drawCylinder(const Eigen::Vector3d &cylindercenter, double radius);
        void drawMirror(double mirrorEdge, double mirrorTilt, double mirrorAngleY);
        void drawReflectedProjector();
        void drawProjections();
        void scaleAndTranslate(std::vector<Vector3d> &vertices);

    private:
        double cameraZ;
        Arcball arcball;
        ObjLoader *obj;

        // Vertices and projections
        std::vector< Eigen::Matrix<GLfloat,3,1> > pixelFragments,objVerticesMirror,verticesIntersectionCylinder;
        // Vertices composing the object
        std::vector < Vector3d > objVertices;

        bool isRotating;
        bool viewRaysBackward,viewRaysForward;
        int rayVertexIndex;

        VRCamera projectorCamera;
        Vector3d reflectedProjector;

        // Projected coordinates, needed for painter to draw informations
        Vector2d projector2Dcoords;
        Vector2d reflectedProjector2Dcoords;
        Vector2d xAxis2DCoords;
        Vector2d yAxis2DCoords;
        Vector2d zAxis2DCoords;
        Vector2d vertexQ2DCoords,vertexV2DCoords,vertexM2DCoords;

        ExperimentalParameters *parameters;
        Screen projScreen;
};
//! [3]

#endif
