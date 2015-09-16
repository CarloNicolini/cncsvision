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
#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QTimer>
#include <math.h>
#include "GLWidgetGeometry.h"
#include "ExperimentalParameters.h"
#include "GeometricFunctions.h"


using std::cout;
using std::cerr;
using std::endl;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static int view = 0;

//! [0]
GLWidgetGeometry::GLWidgetGeometry(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    glEnable(GL_MULTISAMPLE);
    QTimer *timer = new QTimer(this);
    timer->start(15);
    timer->setInterval(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));

    // Important to enable the shaders
    QGLFormat rgbaformat;
    rgbaformat.setRgba(true);
    rgbaformat.setSampleBuffers(true);
    rgbaformat.setDoubleBuffer(true);
    rgbaformat.setSamples(4);
    rgbaformat.setOverlay(0);
    this->setFormat(rgbaformat);
    // Very important
    this->setAutoFillBackground(false);

    this->obj=NULL;
    this->isRotating=false;
    this->cameraZ=1000;
    this->parameters = new ExperimentalParameters();

    // Use of a projective camera to get the resulting points
    this->projectorCamera.setDrySimulation(true);
    this->viewRaysBackward=false;
    this->viewRaysForward=false;
    this->rayVertexIndex=0;
    this->isRotating=false;
}

/**
 * @brief GLWidgetGeometry::getParameters
 * @return
 */
ExperimentalParameters* GLWidgetGeometry::getParameters()
{
    return parameters;
}

/**
 * @brief GLWidgetGeometry::loadMesh
 * @param filename
 */
void GLWidgetGeometry::loadMesh(const QString &filename)
{
    if (!filename.isEmpty())
    {
        if (obj==NULL)
        {
            obj = new ObjLoader(const_cast<char*>(filename.toStdString().c_str()));
        }
        else
        {
            delete this->obj;
            obj=NULL;
            obj = new ObjLoader(const_cast<char*>(filename.toStdString().c_str()));
        }

        //this->objVertices =  obj->getAllVertices();
        this->scaleAndTranslate(this->objVertices);
        this->pixelFragments.resize(objVertices.size());
        this->objVerticesMirror.resize(objVertices.size());
        this->verticesIntersectionCylinder.resize(objVertices.size());
        emit modelChanged();
    }
    this->parameters->meshVertices=this->objVertices.size();
    QObject::connect(this,SIGNAL(modelChanged()),this,SLOT(computeProjectionsBackward()));
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::scaleAndTranslate
 * @param vertices
 */
void GLWidgetGeometry::scaleAndTranslate( std::vector<Vector3d> &vertices)
{
    // Get max, min, x,y,z coordinates
    long int n = vertices.size();
    double xmax=std::numeric_limits<double>::min(), xmin=std::numeric_limits<double>::max();
    double ymax=xmax, ymin=xmin,zmax=xmax,zmin=xmin;

    Vector3d barycenter(0.0,0.0,0.0);
    for (long i=0; i<n; i++)
    {
        xmax = std::max(vertices.at(i).x(),xmax);
        ymax = std::max(vertices.at(i).y(),ymax);
        zmax = std::max(vertices.at(i).z(),zmax);
        xmin= std::min(vertices.at(i).x(),xmin);
        ymin = std::min(vertices.at(i).y(),ymin);
        zmin = std::min(vertices.at(i).z(),zmin);
        barycenter+=vertices.at(i);
    }
    barycenter/=(double)n;

    double maxDimension = std::max(std::max(xmax,ymax),zmax);
    double minDimension = std::max(std::min(xmin,ymin),zmin);

    double scale = parameters->objectScale/(maxDimension-minDimension);

    for (long i=0; i<n; i++)
    {
        vertices.at(i)-=barycenter;
        vertices.at(i)*=scale;
    }
}

/**
 * @brief GLWidgetGeometry::~GLWidgetGeometry
 */
GLWidgetGeometry::~GLWidgetGeometry()
{
    delete obj;
    delete parameters;
}

/**
 * @brief GLWidgetGeometry::minimumSizeHint
 * @return
 */
QSize GLWidgetGeometry::minimumSizeHint() const
{
    return QSize(200, 200);
}

/**
 * @brief GLWidgetGeometry::sizeHint
 * @return
 */
QSize GLWidgetGeometry::sizeHint() const
{
    return QSize(200, 200);
}

/**
 * @brief GLWidgetGeometry::initializeGL
 */
void GLWidgetGeometry::initializeGL()
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
    arcball.setWidthHeight(this->width(),this->height());
}

/**
 * @brief GLWidgetGeometry::drawProjections
 */
void GLWidgetGeometry::drawProjections()
{
    if (objVertices.empty())
        return;
    double *objVerticesArrayPointer = &(objVertices.at(0).coeffRef(0));
    GLfloat *vertexArrayPointer = &(pixelFragments.at(0).coeffRef(0));
    GLfloat *projectedVerticesArrayPointer = &(objVerticesMirror.at(0).coeffRef(0));
    GLfloat* cylinderVerticesPointer = &(verticesIntersectionCylinder.at(0).coeffRef(0));

    glPushMatrix();
    //arcball.applyRotationMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(1.0);
    this->vertexQ2DCoords = GeometricFunctions::project(this->objVertices.at(rayVertexIndex));
    this->vertexV2DCoords = GeometricFunctions::project(this->verticesIntersectionCylinder.at(rayVertexIndex).cast<double>());
    this->vertexM2DCoords = GeometricFunctions::project(this->objVerticesMirror.at(rayVertexIndex).cast<double>());

    glColor3d(0.0,0.0,0.0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3d(0.0,0.0,0.0);
    glVertexPointer(3,GL_DOUBLE, 0,objVerticesArrayPointer);
    glDrawArrays(GL_POINTS, 0, this->objVertices.size());

    glColor3d(0.0,0.0,0.0);
    glVertexPointer(3,GL_FLOAT, 0,cylinderVerticesPointer);
    glDrawArrays(GL_POINTS, 0, this->objVertices.size());

    glColor4d(1.0,0.0,0.0,0.8);
    glVertexPointer(3,GL_FLOAT, 0,projectedVerticesArrayPointer);
    glDrawArrays(GL_POINTS, 0, this->objVertices.size());
    glDisableClientState(GL_VERTEX_ARRAY);

    // Draw a ray
    if (viewRaysBackward)
    {
        glColor3d(0.0,0.0,0.0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex3dv(this->reflectedProjector.data());
        glVertex3dv(this->objVertices.at(rayVertexIndex).data());
        glVertex3fv(this->verticesIntersectionCylinder.at(rayVertexIndex).data());
        glVertex3fv(this->objVerticesMirror.at(rayVertexIndex).data());
        glVertex3fv(this->pixelFragments.at(rayVertexIndex).data());
        glEnd();

        glPointSize(10);
        glColor3d(1.0,0.0,0.0);
        glBegin(GL_POINTS);
        glVertex3dv(this->reflectedProjector.data());
        glVertex3dv(this->parameters->projCenter.data());
        glVertex3dv(this->objVertices.at(rayVertexIndex).data());
        glVertex3fv(this->verticesIntersectionCylinder.at(rayVertexIndex).data());
        glVertex3fv(this->objVerticesMirror.at(rayVertexIndex).data());
        glVertex3fv(this->pixelFragments.at(rayVertexIndex).data());
        glEnd();
    }
    if (viewRaysForward)
    {
        glColor3d(0.0,1.0,0.0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex3dv(this->parameters->projCenter.data());
        glVertex3fv(this->objVerticesMirror.at(rayVertexIndex).data());
        glVertex3fv(this->pixelFragments.at(rayVertexIndex).data());
        glVertex3fv(this->verticesIntersectionCylinder.at(rayVertexIndex).data());
        glEnd();
    }
    glPopAttrib();
    glPopMatrix();
}

/**
 * @brief GLWidgetGeometry::paintGL
 */
void GLWidgetGeometry::paintGL()
{
    if (isRotating)
    {
        parameters->mirrorRotationDegree+=parameters->mirrorAngularSpeedDegForSec;
        if (parameters->mirrorRotationDegree>360)
            parameters->mirrorRotationDegree-=360;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qglClearColor(Qt::white);
    glLoadIdentity();
    // Viewer distance is 1000 millimeters away from the center of the mirror
    glTranslatef(0.0, 0.0, -this->cameraZ);
    arcball.applyRotationMatrix();
    this->drawReferenceXYZ(10);
    this->drawMirror(parameters->mirrorEdgeLenght,parameters->mirrorTilt,parameters->mirrorRotationDegree);
    this->drawReflectedProjector();
    this->drawCylinder(parameters->mirrorCenter,parameters->viewerDistance);

    if (!objVertices.empty())
        this->drawProjections();

    // QPainter, now is drawing in orthographic coordinates!
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Important to make possible overpainting!!!
    // See the example Overpainting in Qt Creator examples
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.save();
    painter.setPen(Qt::black);
    painter.drawText(5,20,"Press 'space 'to reset view");
    painter.drawText(5,40,"Projector focal= "+ QString::number(parameters->projectorFocal));
    painter.drawText(5,60,"Angle="+ QString::number(parameters->mirrorRotationDegree));

    painter.drawText(5,80,"MirrorNormal="+
                     QString::number(parameters->mirrorPlane.normal().x())+","+
                     QString::number(parameters->mirrorPlane.normal().y())+","+
                     QString::number(parameters->mirrorPlane.normal().z()));
    cout << parameters->mirrorPlane.normal().transpose() << endl;
    painter.drawText(5,100,"ReflectedProjector="+
                     QString::number(parameters->reflectedProjector.x())+","+
                     QString::number(parameters->reflectedProjector.y())+","+
                     QString::number(parameters->reflectedProjector.z()));
    painter.drawText(5,120,"Current vertices to render= " + QString::number(this->objVertices.size()) );
    painter.setPen(Qt::black);
    painter.drawText(this->xAxis2DCoords.x(),this->xAxis2DCoords.y(),"x");
    painter.drawText(this->yAxis2DCoords.x(),this->yAxis2DCoords.y(),"y");
    painter.drawText(this->zAxis2DCoords.x(),this->zAxis2DCoords.y(),"z");
    painter.setPen(Qt::black);
    //painter.drawText(5,160,"Viewpoints= " + QString::number(parameters->totalViewPoints) + " CurrentViewIndex="+QString::number(parameters->currentViewIndex));
    painter.drawText(this->projector2Dcoords.x(),this->projector2Dcoords.y(),"P");
    painter.drawText(this->reflectedProjector2Dcoords.x(),this->reflectedProjector2Dcoords.y(),"P'");
    if (this->viewRaysBackward)
    {
        painter.setPen(Qt::black);
        QFont f = painter.font();
        f.setPixelSize(25);
        painter.setFont(f);

        painter.drawText(this->vertexM2DCoords.x(),this->vertexM2DCoords.y(),"M");
        painter.drawText(this->vertexQ2DCoords.x(),this->vertexQ2DCoords.y(),"Q");
        painter.drawText(this->vertexV2DCoords.x(),this->vertexV2DCoords.y(),"V");
    }
    painter.restore();
    painter.end();
    glPopAttrib();


    if (isRotating)
    {
        emit angleChanged(parameters->mirrorRotationDegree);
        emit modelChanged();
    }
}

/**
 * @brief GLWidgetGeometry::drawReferenceXYZ
 * @param length
 */
void GLWidgetGeometry::drawReferenceXYZ(double length)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    //arcball.applyRotationMatrix();
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0); // red x
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(length,0.0,0.0);

    glColor3f(0.0,1.0,0.0); // green y
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(0.0,length,0.0);

    glColor3f(0.0,0.0,1.0); //blue z
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(0.0,0.0,length);
    glEnd();

    // Compute the extremal points of axis triplet
    this->xAxis2DCoords = GeometricFunctions::project(Vector3d(length,0,0));
    this->yAxis2DCoords = GeometricFunctions::project(Vector3d(0,length,0));
    this->zAxis2DCoords = GeometricFunctions::project(Vector3d(0,0,length));

    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief GLWidgetGeometry::drawCylinder
 * @param cylindercenter
 * @param radius
 */
void GLWidgetGeometry::drawCylinder(const Vector3d &cylindercenter, double radius)
{
    GLUquadric *cylQuadric = gluNewQuadric();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_CULL_FACE);
    glColor4f(0.0,0.0,0.0,0.2);
    glPointSize(5);
    glPushMatrix();
    //arcball.applyRotationMatrix();
    glRotated(45,0,1,0);
    glTranslated(cylindercenter.x(),cylindercenter.y(),cylindercenter.z());
    glRotated(90,1,0,0);
    glTranslated(0,0,-parameters->cylinderHeight/2);
    gluQuadricDrawStyle(cylQuadric,GLU_LINE);
    gluCylinder(cylQuadric,radius,radius,parameters->cylinderHeight,parameters->totalViewPoints,20);
    glPopMatrix();
    gluDeleteQuadric(cylQuadric);
    glPopAttrib();
}

/**
 * @brief GLWidgetGeometry::drawMirror
 * @param mirrorEdge
 * @param mirrorTilt
 * @param mirrorAngleY
 */
void GLWidgetGeometry::drawMirror(double mirrorEdge, double mirrorTilt, double mirrorAngleY)
{
    std::vector<Vector3d >mirrorVertices(4);
    mirrorVertices[0]<<-mirrorEdge,-mirrorEdge,0;
    mirrorVertices[1]<<-mirrorEdge,mirrorEdge,0;
    mirrorVertices[2]<<mirrorEdge,mirrorEdge,0;
    mirrorVertices[3]<<mirrorEdge,-mirrorEdge,0;

    Eigen::AngleAxis<double> tiltRotation(-mirrorTilt,Eigen::Vector3d::UnitX());
    for (int i=0; i<4; i++)
        mirrorVertices.at(i)=tiltRotation.toRotationMatrix() *mirrorVertices.at(i);

    Vector3d mirrorCenter(0,0,0);

    Eigen::AngleAxis<double> angleRotation(mirrorAngleY*M_PI/180.0,Eigen::Vector3d::UnitY());
    for (int i=0; i<4; i++)
    {   mirrorVertices.at(i)=angleRotation.toRotationMatrix() *mirrorVertices.at(i);
        mirrorCenter+=mirrorVertices.at(i);
    }
    mirrorCenter/=4.0;

    parameters->mirrorPlane = Eigen::Hyperplane<double,3>::Through(mirrorVertices[0],mirrorVertices[1],mirrorVertices[2]);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    //    arcball.applyRotationMatrix();
    glColor4d(1.0,0.5,0.5,0.5);
    glLineWidth(1);
    glBegin(GL_QUADS);
    for (int i=0; i<4; i++)
        glVertex3dv(mirrorVertices.at(i).data());
    glEnd();

    // Draw the mirror normal in red
    /*
    Vector3d exitingNormal = mirrorCenter + parameters->mirrorPlane.normal().normalized()*25.0;
    glColor3d(1.0,0.0,0.0);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3dv(mirrorCenter.data());
    glVertex3dv(exitingNormal.data());
    glEnd();
    */
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief GLWidgetGeometry::drawReflectedProjector
 */
#include "Grid.h"
void GLWidgetGeometry::drawReflectedProjector()
{
    // Draw the rays from projector focal center to the projected points
    std::vector< Vector3d >projectorFocalSurface(4),projectorFocalSurfaceReflected(4);
    projectorFocalSurface[0]<< projScreen.pa;
    projectorFocalSurface[1]<< projScreen.pb;
    projectorFocalSurface[2]<<projScreen.pc;
    projectorFocalSurface[3]<<projScreen.pd;

    /*
    AngleAxis<double> aa(parameters->mirrorRotationDegree*M_PI/180,Vector3d::UnitY());
    for (int i=0; i<4;i++)
        projectorFocalSurfaceReflected.at(i)=aa.toRotationMatrix()*projectorFocalSurface.at(i);
*/
    /*
    AngleAxis<double> rot(M_PI/2,Vector3d::UnitX());
    for (int i=0; i<4;i++)
    {
        projectorFocalSurface.at(i)=rot.toRotationMatrix()*projectorFocalSurface.at(i);
        projectorFocalSurface.at(i).y()=parameters->projCenter.y()-parameters->projectorFocal-1;
    }
*/
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(0.5);
    glColor4f(0.0,0.0,0.0,0.8);
    glPointSize(1);
    glPushMatrix();
    //arcball.applyRotationMatrix();
    glLineWidth(2);

    glColor4f(0.0,0.0,0.0,0.8);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<4;i++)
        glVertex3dv(projectorFocalSurface.at(i).data());
    glEnd();
    glPointSize(0.5);
    // Lines from reflected projector to surface
    glColor4f(0.0,0.0,0.0,0.8);
    for (int i=0;i<4;i++)
    {
        glBegin(GL_LINES);
        glVertex3dv(parameters->projCenter.data());
        glVertex3dv(projectorFocalSurface.at(i).data());
        glEnd();
    }
    this->projector2Dcoords = GeometricFunctions::project(parameters->projCenter);
    this->reflectedProjector2Dcoords= GeometricFunctions::project(reflectedProjector);
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief GLWidgetGeometry::computeProjectionsBackward
 */
void GLWidgetGeometry::computeProjectionsBackward()
{
    AngleAxis<double> mirrorRotationTransformation(parameters->mirrorRotationDegree*M_PI/180,Vector3d::UnitY());
    this->reflectedProjector = mirrorRotationTransformation.toRotationMatrix()*Vector3d(parameters->projCenter.x(),parameters->projCenter.z(),parameters->projCenter.y());
    parameters->reflectedProjector << reflectedProjector;

    //2*tan(parameters->projectorApertureDegrees*M_PI/180)*(parameters->projCenter.y()-parameters->projectorFocal);
    double edge= parameters->projectorAreaEdge;

    //Vector3d rotAxis = parameters->projDirection.cross(Vector3d(0,1,0));
    //double rotAngle = parameters->projDirection.normalized().dot(Vector3d(0,1,0));
    // here the order is important
    projScreen.pa << Vector3d(edge,0,-edge);
    projScreen.pb << Vector3d(-edge,0,-edge);
    projScreen.pc << Vector3d(edge,0,edge);
    projScreen.pd << Vector3d(-edge,0,edge);

    //projScreen.rotate(AngleAxis<double>(M_PI/2,Vector3d::UnitX()).toRotationMatrix());

    this->projectorCamera.init(projScreen.pa,projScreen.pb,projScreen.pc);
    this->projectorCamera.setNearFarPlanes(10,3000);
    this->projectorCamera.setEye(this->parameters->projCenter);

    emit modelViewMatrixChanged( this->projectorCamera.getModelViewMatrix().matrix() );
    emit projectionMatrixChanged(this->projectorCamera.getProjectiveMatrix().matrix() );
    emit modelViewProjectionMatrixChanged( (this->projectorCamera.getProjectiveMatrix()*this->projectorCamera.getModelViewMatrix()).matrix() );
    emit projPlanesChanged(projScreen);

    for (unsigned int i=0; i<this->objVertices.size(); i++)
    {
        // Compute intersection of ray PQ with vertical cylinder with radius d to find view position V’
        Vector3d ray = objVertices.at(i)-reflectedProjector;
        Vector3d V = GeometricFunctions::getRayCylinderIntersection(ray,reflectedProjector,parameters->viewerDistance);
        V.y() = parameters->viewerHeight;
        Vector3d VQ = objVertices.at(i)-V;
        Vector3d M = GeometricFunctions::getRayPlaneIntersection(VQ,objVertices.at(i),parameters->mirrorPlane.normal(),parameters->mirrorCenter);

        int viewportX=640,viewportY=640;
        Vector4i viewport(0,0,viewportX,viewportY);
        Vector2d winProj = GeometricFunctions::project(M,projectorCamera.getProjectiveMatrix(),projectorCamera.getModelViewMatrix(),viewport);
        Vector3d mm(winProj.x()-viewportX/2, parameters->projCenter.y()-parameters->projectorFocal,winProj.y()-viewportY/2);
        this->pixelFragments.at(i) << mm.cast<GLfloat>();
        this->objVerticesMirror.at(i)  << M.cast<GLfloat>();
        this->verticesIntersectionCylinder.at(i) << V.cast<GLfloat>();
    }
    //GLfloat *v = &(objVerticesMirror.at(0).coeffRef(0));
    //GLfloat *v = const_cast<GLfloat*> (&(obj->getAllVerticesGLfloat().at(0).coeffRef(0))); //deprecated
    //emit verticesChanged(v,this->objVertices.size());
}

/**
 * @brief GLWidgetGeometry::resizeGL
 * @param width
 * @param height
 */
void GLWidgetGeometry::resizeGL(int width, int height)
{
    arcball.setWidthHeight(width,height);
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float FOV=30;
    if(parameters->perspectiveView)
        gluPerspective(FOV, (float)width / (float)height, 0.1, 20000.0);
    else
        glOrtho(-250, 250, -250, 250, -500, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief GLWidgetGeometry::keyPressEvent
 * @param event
 */
void GLWidgetGeometry::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Space )
    {
        arcball.reset();
        glLoadIdentity();
        glTranslated(0,0,-this->cameraZ);
    }

    if (event->key() == Qt::Key_P)
    {
        parameters->perspectiveView=!parameters->perspectiveView;
        resizeGL(this->width(),this->height());
    }
}

/**
 * @brief GLWidgetGeometry::keyReleaseEvent
 * @param event
 */
void GLWidgetGeometry::keyReleaseEvent(QKeyEvent *event)
{

}

/**
 * @brief GLWidgetGeometry::mousePressEvent
 * @param event
 */
void GLWidgetGeometry::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();
    if (event->button() == Qt::LeftButton)
    {
        arcball.startRotation(event->x(),event->y());
    }
}

/**
 * @brief GLWidgetGeometry::mouseDoubleClickEvent
 */

void GLWidgetGeometry::mouseDoubleClickEvent(QMouseEvent *)
{
    arcball.reset();
    view++;
}

/**
 * @brief GLWidgetGeometry::mouseReleaseEvent
 * @param event
 */
void GLWidgetGeometry::mouseReleaseEvent(QMouseEvent *event)
{
    arcball.stopRotation();
}

/**
 * @brief GLWidgetGeometry::mouseMoveEvent
 * @param event
 */
void GLWidgetGeometry::mouseMoveEvent(QMouseEvent *event)
{
    arcball.updateRotation(event->x(),event->y());
}

/**
 * @brief GLWidgetGeometry::wheelEvent
 * @param event
 */
void GLWidgetGeometry::wheelEvent(QWheelEvent *event)
{
    this->cameraZ += event->delta()/2.0;
}

/**
 * @brief GLWidgetGeometry::setProjectorPositionX
 * @param x
 */
void GLWidgetGeometry::setProjectorPositionX(double x)
{
    parameters->projCenter.x() = x;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorPositionY
 * @param y
 */
void GLWidgetGeometry::setProjectorPositionY(double y)
{
    parameters->projCenter.y() = y;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorPositionZ
 * @param z
 */
void GLWidgetGeometry::setProjectorPositionZ(double z)
{
    parameters->projCenter.z() = z;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorDirectionTilt
 * @param tilt
 */
void GLWidgetGeometry::setProjectorPitch(double pitch)
{
    parameters->projectorPitch=pitch;
    double projectorPitch = parameters->projectorPitch*M_PI/180;
    double projectorRoll= parameters->projectorRoll*M_PI/180;
    this->parameters->projDirection << sin(projectorPitch)*cos(projectorRoll),
            sin(projectorPitch)*sin(projectorRoll),
            cos(projectorPitch);
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorDirectionSlant
 * @param slant
 */
void GLWidgetGeometry::setProjectorRoll(double roll)
{
    parameters->projectorRoll=roll;
    double projectorPitch = parameters->projectorPitch*M_PI/180;
    double projectorRoll= parameters->projectorRoll*M_PI/180;
    this->parameters->projDirection <<
                                       sin(projectorPitch)*cos(projectorRoll),
            sin(projectorPitch)*sin(projectorRoll),
            cos(projectorPitch);
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorFocal
 * @param f
 */
void GLWidgetGeometry::setProjectorFocal(double f)
{
    parameters->projectorFocal=f;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectorAperture
 * @param aperture
 */
void GLWidgetGeometry::setProjectorAperture(double aperture)
{
    this->parameters->projectorApertureDegrees = aperture;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setProjectedAreaEdge
 * @param edge
 */
void GLWidgetGeometry::setProjectedAreaEdge(double edge)
{
    this->parameters->projectorAreaEdge = edge;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setMirrorRotation
 * @param angle
 */
void GLWidgetGeometry::setMirrorRotation(double angle)
{
    parameters->mirrorRotationDegree = angle;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setMirrorRotationSpeed
 * @param freq
 */
void GLWidgetGeometry::setMirrorRotationSpeed(double freq)
{
    parameters->mirrorAngularSpeedDegForSec = freq;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setViewerDistance
 * @param distance
 */
void GLWidgetGeometry::setViewerDistance(double distance)
{
    parameters->viewerDistance = distance;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setViewerHeight
 * @param h
 */
void GLWidgetGeometry::setViewerHeight(double h)
{
    parameters->viewerHeight=h;
    emit modelChanged();
}


/**
 * @brief GLWidgetGeometry::setAutorotation
 * @param autorot
 */
void GLWidgetGeometry::setAutorotation(bool autorot)
{
    this->isRotating=autorot;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setViewRays
 * @param v
 */
void GLWidgetGeometry::setViewRaysBackward(bool v)
{
    this->viewRaysBackward=v;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setViewRaysForward
 * @param v
 */
void GLWidgetGeometry::setViewRaysForward(bool v)
{
    this->viewRaysForward=v;
    emit modelChanged();
}

/**
 * @brief GLWidgetGeometry::setViewRays
 * @param v
 */
void GLWidgetGeometry::setRayVertexIndex(int v)
{
    this->rayVertexIndex = v;
    emit modelChanged();
}
