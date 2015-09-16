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

#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <fstream>
#include <Eigen/Core>
#include <QtGui>
#include <QGLWidget>

#include "VRCamera.h"
#include "ObjLoader.h"
#define PROJECTOR_WIDTH 608
#define PROJECTOR_HEIGHT 684

class QGLShaderProgram;
using Eigen::Vector3d;
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

class CalibrationWidget : public QGLWidget
{
    Q_OBJECT

public:
    CalibrationWidget(QWidget *parent = 0);
    ~CalibrationWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void saveData();
    void toggleText();
	void moveCursor(int x,int y);
    void addPoint();
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QVector<QPoint> points2D;
    std::vector<Eigen::Vector3d> scaleAndTranslate( const std::vector<Eigen::Vector3d> &vertices,double scale);

    bool drawingText;
    QString folderName,pointsFileName;
    QPoint lastPoint;
    ofstream outputPoints;
};

#endif
