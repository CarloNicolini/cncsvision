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

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QTimer>
#include <math.h>
#include "GLWidgetShader.h"
#include "GLWidgetGeometry.h"
#include "ExperimentalParameters.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//! [0]
GLWidgetShader::GLWidgetShader(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    glEnable(GL_MULTISAMPLE);

    // Important to enable the shaders
    QGLFormat rgbaformat;
    rgbaformat.setRgba(true);
    rgbaformat.setSampleBuffers(true);
    rgbaformat.setDoubleBuffer(true);
    rgbaformat.setSamples(4);
    rgbaformat.setOverlay(0);
    this->setFormat(rgbaformat);
    this->makeCurrent();
    // Very important
    this->setAutoFillBackground(false);

    QTimer *timer = new QTimer(this);
    timer->start(15);
    timer->setInterval(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));

    this->isDrawing=false;
    qglClearColor(Qt::white);
    this->nVertices = 0;
}

/**
 * @brief GLWidgetShader::loadShader
 * @param _vertexShaderFile
 * @param _fragmentShaderFile
 * @param textureFile
 */
void GLWidgetShader::loadShader(const QString &_vertexShaderFile, const QString &_fragmentShaderFile)
{
    if (shaderProgram != NULL )
    {
        delete shaderProgram;
    }
    else
        shaderProgram = new QGLShaderProgram(this);

    this->vertexShaderFile=_vertexShaderFile;
    this->fragmentShaderFile=_fragmentShaderFile;
    this->shaderProgram->addShaderFromSourceFile(QGLShader::Vertex,vertexShaderFile);
    this->shaderProgram->addShaderFromSourceFile(QGLShader::Fragment,fragmentShaderFile);
}

/**
 * @brief GLWidgetShader::~GLWidgetShader
 */
GLWidgetShader::~GLWidgetShader()
{

}

QSize GLWidgetShader::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidgetShader::sizeHint() const
{
    return QSize(200,200);
}

void GLWidgetShader::initializeGL()
{
    qglClearColor(Qt::white);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_MULTISAMPLE);
    qglClearColor(Qt::white);
#ifdef APPLE
    QString vertexShader("/Users/rs/workspace/cncsvisioncmake/data/glslshaders/RasterScanLineShader.vert");
    QString fragmentShader("/Users/rs/workspace/cncsvisioncmake/data/glslshaders/RasterScanLineShader.frag");
#else

    QString vertexShader( "/home/carlo/workspace/cncsvisioncmake/data/glslshaders/LightFieldVertexShader.vert");
    QString fragmentShader("/home/carlo/workspace/cncsvisioncmake/data/glslshaders/LightFieldFragmentShader.frag");
#endif
    this->makeCurrent();
    this->loadShader(vertexShader,fragmentShader);

}

void GLWidgetShader::paintGL()
{
    if (nVertices!=0)
    {
        Eigen::Matrix4d MVP;
        MVP <<  -0.0709951 ,  0.0997458 ,  -0.152342   , -0.15187,
                -0.0744123 ,-0.00746067,   0.0552333  ,  -2.31223,
                  0.263665   , 0.276252  ,  0.924317  ,  -17.4836,
                 -0.263639,   -0.276224 ,  -0.924224  ,   17.6819;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glLoadMatrixd(MVP.data());
        QVector3D norm(parameters->mirrorPlane.normal().x(), parameters->mirrorPlane.normal().y(),parameters->mirrorPlane.normal().z());
        QVector3D projCenter(parameters->projCenter.x(),parameters->projCenter.y(),parameters->projCenter.z());
        QVector3D reflectedProjector(parameters->reflectedProjector.x(),parameters->reflectedProjector.y(),parameters->reflectedProjector.z());

        QMatrix4x4 qm;
        for (int i=0; i<4;i++)
        {
            for (int j=0; j<4;j++)
            {
                qm(i,j) = this->m(i,j);
            }
        }

        this->beginShader();
        glPointSize(0.1);
        glEnableClientState(GL_VERTEX_ARRAY);
        shaderProgram->setUniformValue("viewerDistance",(GLfloat)parameters->viewerDistance);
        shaderProgram->setUniformValue("viewerHeight",(GLfloat)parameters->viewerHeight);
        shaderProgram->setUniformValue( "mirrorNormal",norm );
        shaderProgram->setUniformValue("projectorModelViewProjectionMatrix",qm);
        //shaderProgram->setUniformValue("ProjectorCenter", projCenter);
        shaderProgram->setUniformValue("ReflectedProjector",reflectedProjector);
        shaderProgram->setUniformValue("mirrorCenter",QVector3D(parameters->mirrorCenter.x(),parameters->mirrorCenter.y(),parameters->mirrorCenter.z()));

        glVertexPointer(3,GL_FLOAT, 0, this->verticesPtr );
        glDrawArrays(GL_POINTS, 0, this->nVertices);

        if (!this->shaderProgram->log().isEmpty())
            qDebug() << "Shader log " << this->shaderProgram->log();
        this->endShader();
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

/**
 * @brief GLWidgetShader::beginShader
 */
void GLWidgetShader::beginShader()
{
    this->shaderProgram->link();
    this->shaderProgram->bind();
    //qDebug() << this->shaderProgram->bind();
    //qDebug() << this->shaderProgram->log();
}

/**
 * @brief GLWidgetShader::endShader
 */
void GLWidgetShader::endShader()
{
    this->shaderProgram->release();
}

/**
 * @brief GLWidgetShader::resizeGL
 * @param width
 * @param height
 */
void GLWidgetShader::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float FOV=30;
    gluPerspective(FOV, (float)width / (float)height, 0.1, 20000.0);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief GLWidgetShader::settices
 * @param ptr
 * @param size
 */
void GLWidgetShader::setVertices(GLfloat *ptr, unsigned int size)
{
    this->verticesPtr = ptr;
    this->nVertices = size;
}

/**
 * @brief GLWidgetShader::setParameters
 * @param param
 */
void GLWidgetShader::setParameters(ExperimentalParameters *param)
{
    this->parameters = param;
}

/**
 * @brief GLWidgetShader::setModelviewProjectionMatrix
 * @param m
 */
void GLWidgetShader::setModelviewProjectionMatrix(const Eigen::Matrix4d &_m)
{
    this->m = _m;
}
