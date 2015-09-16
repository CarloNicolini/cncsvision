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
#include <stdlib.h>
#include <stdio.h>
#include <Eigen/Core>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef _WIN32_
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <fstream>
#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "GLUtils.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
ObjLoader obj;

int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double znear=0.1;
double zfar=1000.0;

double frame=0.0;

Eigen::Vector3d getHelicoidCoordinates(double r, double theta)
{
    return Vector3d( r*cos(theta),theta,r*sin(theta));
}

Eigen::Vector3d getHelicoidTangent(double r, double theta)
{
    return Vector3d( cos(theta),0,sin(theta));
}

Eigen::Vector3d getHelicoidBitangent(double r, double theta)
{
    return Vector3d( -r*sin(theta),1,r*cos(theta) );
}
Eigen::Vector3d getHelicoidNormal(double r, double theta)
{
    return  getHelicoidTangent(r,theta).cross(getHelicoidBitangent(r,theta)).normalized();
}


void drawHelicoid(double rMin, double rMax, double radiusSteps, double thetaSteps,double baseTheta)
{
    glColor3f(1,1,1);
    for (double r=rMin; r<rMax; r+=(rMax-rMin)/radiusSteps)
    {
        glBegin(GL_POINTS);
        for  (double theta=-M_PI+baseTheta; theta<=M_PI+baseTheta; theta+=2*M_PI/thetaSteps)
        {
            Vector3d  p = getHelicoidCoordinates(r,theta);
            glVertex3dv(p.data());
        }
        glEnd();
    }
}

void drawHelicoidTangentPlane(double r, double theta, double edgeLength)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(2);
    // Find the "center" of the tangent plane quadrilateral
    Vector3d p = getHelicoidCoordinates(r,theta);
    // Get the tangent plane tangent and bitangent as well as its normal vector
    Vector3d b = getHelicoidBitangent(r,theta)/10;
    Vector3d t = getHelicoidTangent(r,theta)/10;
    Vector3d n = getHelicoidNormal(r,theta)/10;

    glPushMatrix();
    glTranslated(p.x(),p.y(),p.z());
    Affine3d R =  Affine3d::Identity();
    R.matrix().col(0) << b.normalized(),0;
    R.matrix().col(1) << t.normalized(),0;
    R.matrix().col(2) << n.normalized(),0;
    glMultMatrixd(R.data());

    // Draw the normal
    glColor3d(1,0,0);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(0,0,1);
    glEnd();
    // Draw the plane
    glBegin(GL_LINE_LOOP);
    glVertex3d(edgeLength,edgeLength,0.0);
    glVertex3d(edgeLength,-edgeLength,0.0);
    glVertex3d(-edgeLength,-edgeLength,0.0);
    glVertex3d(-edgeLength,edgeLength,0.0);
    glEnd();
    glPopMatrix();

    glPopAttrib();
}

vector<Vector3d> rayIntersection( double rMin, double rMax, int radiusSteps, int thetaSteps, double tolerance, double baseTheta,const Vector3d &displacement, const ObjLoader &obj)
{

    vector<Vector3d> vertices;// XXX = obj.getAllVertices();
    vector<Vector3d> intersections;
    for (double r=rMin; r<rMax; r+=(rMax-rMin)/radiusSteps)
    {
        for  (double theta=-M_PI+baseTheta; theta<=M_PI+baseTheta; theta+=2*M_PI/thetaSteps)
        {
            Vector3d helicoidVertex = getHelicoidCoordinates(r,theta);
            for (int i=0; i<vertices.size();i++)
            {
                if ( ( vertices.at(i)-displacement - helicoidVertex ).norm() < tolerance )
                    intersections.push_back(vertices.at(i));
            }
        }
    }
    return intersections;
}

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    }
}

void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

    obj.load("/home/carlo/workspace/cncsvisioncmake/data/objmodels/sphere.obj");
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, znear, zfar);
}

void drawAll()
{
    double rMin=1,rMax=4;
    int nStepsR=10;
    int nStepsTheta=100;
    static  double baseTheta=0;
    //baseTheta+=0.1;
    if (baseTheta>M_PI)
        baseTheta=0;
    double tol=0.1;
    Vector3d disp(0.1,0,0);
    glColor3fv(glWhite);
    drawHelicoid(rMin,rMax,nStepsR,nStepsTheta,baseTheta);

    vector<Vector3d> v = rayIntersection(rMin,rMax,nStepsR,nStepsTheta,tol,baseTheta,disp,obj);
    cerr << v.size() << endl;
    glColor3fv(glRed);
    glBegin(GL_POINTS);
    for (int i=0; i<v.size();i++)
        glVertex3dv(v[i].data());
    glEnd();
    /*
    glEnableClientState(GL_VERTEX_ARRAY);                 // Enable vertex arrays
    glVertexPointer(3,GL_FLOAT,   0,&v[0]);     // Vertex Pointer to triangle array
    glDrawArrays(GL_POINTS, 0, v.size());     // Draw the triangles
    glDisableClientState(GL_VERTEX_ARRAY);                // Disable vertex arrays
*/
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslated(0,0,-25);
    glRotated(90,1,0,0);
    arcball.applyRotationMatrix();
    glPushMatrix();
    drawAll();
    glPopMatrix();
    glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();
    if ( state == 3 )
        eyeZ+=0.1;
    if (state ==4)
        eyeZ-=0.1;
    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("Carloball");
    initRendering();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

