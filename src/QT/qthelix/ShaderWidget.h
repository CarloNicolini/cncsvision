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
#define GL_FRAGMENT_PRECISION_HIGH 1

#define PROJECTOR_WIDTH 608
#define PROJECTOR_HEIGHT 684

class ObjLoader2;
class QGLShaderProgram;
typedef Eigen::Matrix<GLfloat,3,1> VectorGL3f;
typedef Eigen::Transform<GLfloat,3,Eigen::Affine> AffineGL3f;
typedef Eigen::Transform<GLfloat,3,Eigen::Projective> ProjectiveGL3f;
typedef Eigen::AngleAxis <GLfloat> AngleAxisGLf;

#define DRAW_MODEL 0


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
    void saveFrame();

    // Drawing methods
    void draw();
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);

    // Variables
    QPointer<QGLShaderProgram> shaderProgram;
    ObjLoader2 *obj;
    GLfloat offsetY;
    GLfloat eyeZ;
    bool drawMesh;
    bool drawText;
    Eigen::MatrixXd Projection,ModelView;
    Eigen::MatrixXd VertexTransform;

    GLfloat  helicoidRotationAngle;
    GLfloat objectScale;

    VectorGL3f objectTranslation;

    QString folderName;
    QString imagesFileBaseName;
    QString imagesFilesFolderName;

    ParametersLoader projectorParameters;
    int projectorPixelWidth,projectorPixelHeight,viewsNumber,viewCount;
};

#endif
