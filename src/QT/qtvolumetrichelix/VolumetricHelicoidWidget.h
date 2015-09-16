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

#ifndef VOLUMETRIC_HELICOID_WIDGET_H
#define VOLUMETRIC_HELICOID_WIDGET_H


#include <QtGui>
#include <QGLWidget>
#include <Eigen/Core>

#include "Util.h"
#include "GLUtils.h"
#include "GLText.h"
#include "Arcball.h"
#include "ParametersLoader.h"

#define PROJECTOR_RESOLUTION_WIDTH 608
#define PROJECTOR_RESOLUTION_HEIGHT 684
#define PROJECTOR_SLICES_NUMBER 96

#define TEXTURE_RESOLUTION_X 768
#define TEXTURE_RESOLUTION_Y 768
#define TEXTURE_RESOLUTION_Z 768

#define GL_FRAGMENT_PRECISION_HIGH 1

class QGLShaderProgram;
typedef Eigen::Matrix<GLfloat,3,1> VectorGL3f;
typedef Eigen::Transform<GLfloat,3,Eigen::Affine> AffineGL3f;
typedef Eigen::Transform<GLfloat,3,Eigen::Projective> ProjectiveGL3f;
typedef Eigen::AngleAxis <GLfloat> AngleAxisGLf;

class VolumetricMeshIntersection;
class CameraDirectLinearTransformation;


class VolumetricHelicoidWidget : public QGLWidget
{
    Q_OBJECT
public:
    VolumetricHelicoidWidget(QWidget *parent = 0);
    ~VolumetricHelicoidWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setOutputFolderName(const QString &name);
    void setObjectOffset(double x, double y, double z);
    void startFramesGeneration();
    void setOffset(double x, double y, double z);
    void setObjectScale(double objScale);
    void setRotation(double angle);
    void setSlicesNumber(int nSlices);
    void setCurrentGLColor(Qt::GlobalColor val);
    void setHelicoidZeroColor(int value);
    void randomizeSpheres(int nSpheres,int minRadius, int maxRadius);
    void toggleStandardGL(bool val);
    void toggleUseCalibratedGLView();
    void setPoints2Dfilename(const QString &val);
    void setPoints3Dfilename(const QString &val);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    int frameNumber;

private:
    // Variables
    bool drawMesh;
    bool drawText;
    bool isSavingFrames;
    bool isDrawingFrustum;
    bool useCalibratedGLView;

    int projectorPixelWidth;
    int projectorPixelHeight;
    int viewsNumber;
    int viewCount;

    GLfloat offsetY;
    GLfloat eyeZ;
    Eigen::MatrixXd Projection,ModelView;
    Eigen::MatrixXd VertexTransform;

    // Volumetric mesh intersection instance
    VolumetricMeshIntersection*volume;
    CameraDirectLinearTransformation *cam;
    // Arcball control
    Arcball arcball;

    QString folderName;
    QString imagesFileBaseName;
    QString imagesFilesFolderName;

    QString points2Dfilename;
    QString points3Dfilename;
    // Background color
    Qt::GlobalColor currentGLColor;

    std::vector<unsigned char> allFrames;

    // Private methods
    void loadCameraSettings();
    void saveFrame(const QString &outputDirectory);
    void draw();
    void drawVisualizedCameraScene();
    void myDraw();
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
};

#endif
