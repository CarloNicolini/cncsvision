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

#ifndef EXPERIMENT_GL_WIDGET_H
#define EXPERIMENT_GL_WIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QGLFramebufferObject>
#include <Eigen/Core>

#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "Screen.h"
#include "BalanceFactor.h"

//#define EXPERIMENT_WINDOW_WIDTH 1024.0
//#define EXPERIMENT_WINDOW_HEIGHT 768.0

class ExperimentGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    ExperimentGLWidget(QWidget *parent = 0);
    ~ExperimentGLWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void initializeExperiment();
    void setCameraParameters(GLfloat _eyeZ, GLfloat _FOV, GLfloat _zNear, GLfloat _zFar);
    void initMonitor(int resX, int resY, int sizeX_mm, int sizeY_mm);

signals:
    void trialAdvanced(double);
    void getExperimentInfo(const QString &);
	void experimentFinished();

    // Inherited QGLWidget methods for GUI control
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *e);
    void applyOpenGLCameraFOV();

    void drawSideParabola(double xRange, double curvature);
    int trialNumber;
    void advanceTrial();

    GLfloat FOV;
    GLfloat zNear;
    GLfloat zFar;
    GLfloat eyeZ;
    GLint experimentWindowResolutionX;
    GLint experimentWindowResolutionY;
    VRCamera cam;

public:
    BalanceFactor<double> bal;
    double displayedCurvature;
    double xRange;

};

#endif
