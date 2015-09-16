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

#ifndef GLWIDGETSHADER_H
#define GLWIDGETSHADER_H

#include <QGLWidget>
#include <QGLShader>
#include <Eigen/Core>
#include <QPointer>
#include "Arcball.h"


//#include "GLWidgetGeometry.h"
class GLWidgetGeometry;
class GLWidgetShader : public QGLWidget
{
    Q_OBJECT
public:
    GLWidgetShader(QWidget *parent = 0);
    ~GLWidgetShader();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool isDrawing;
public slots:
    void setVertices(GLfloat *p, unsigned int n);
    void setParameters(ExperimentalParameters *param);
    void setModelviewProjectionMatrix(const Eigen::Matrix4d &m);
signals:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void loadShader(const QString &_vertexShaderFile, const QString &_fragmentShaderFile);
    void beginShader();
    void endShader();

    GLfloat *verticesPtr;
    unsigned int nVertices;
    Eigen::Matrix4d m;

    ExperimentalParameters *parameters;
    // For use with OpenGL GLSL shaders programs
    QString vertexShaderFile, fragmentShaderFile;
    QPointer<QGLShaderProgram> shaderProgram;
};
//! [3]

#endif
