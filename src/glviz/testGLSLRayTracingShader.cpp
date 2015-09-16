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
#include <Eigen/Geometry>
#include "GLUtils.h"
#include "Mathcommon.h"
#include "Util.h"
#include "GLSL.h"
#include "Arcball.h"
#include "GLText.h"
#include "ObjLoader.h"

using namespace Eigen;

using namespace std;
int width=1024,height=1024;

double eyeDistance=0;
double FOV = 45;
double mirrorTilt=M_PI/4;
double viewAngle=0,frequency=0.01;
bool rotationActive=false;
double viewerDistance=1000.0,viewerHeight=0.0;
Eigen::Matrix<GLfloat,1,3> projCenter(0.0,-791.0,0.0);
Eigen::Matrix<GLfloat,1,3> baseMirrorNormal(0.0, cos(mirrorTilt),sin(mirrorTilt));
Arcball arcball;
ObjLoader obj;

#define SIMPLE_CENTERED_POINT 0
#define SIMPLE_CENTERED_PLANE 1
#define SIMPLE_CENTERED_CUBE 2
#define SIMPLE_RAYS_ORIGINATIN_FROM_CENTER 3
#define SIMPLE_CENTERED_CYLINDER 4

int shape=SIMPLE_CENTERED_POINT;
glsl::glShaderManager *SM;
glsl::glShader *shader;

void handleKeypress(unsigned char key, int x, int y)
{  switch (key)
    {
    case 'q': //Escape key
    {
        delete shader;
        exit(0);
        break;
    }
    case '4':
    {
        projCenter.x()+=25;
        break;
    }
    case '6':
    {
        projCenter.x()-=25;
        break;
    }
    case '2':
    {
        projCenter.y()+=25;
        break;
    }
    case '8':
    {
        projCenter.y()-=25;
        break;
    }
    case '7':
    {
        projCenter.z()+=25;
        break;
    }
    case ' ':
    {
        shape++;
        shape=shape%4;
        break;
    }
    case '9':
    {
        projCenter.z()-=25;
        break;
    }
    case 's':
    {
        viewerDistance+=25;
        break;
    }
    case 'x':
    {
        viewerDistance-=25;
        break;
    }
    case 'd':
    {
        viewerHeight+=1;
        break;
    }
    case'c':
    {
        viewerHeight-=1;
        break;
    }
    case '+':
    {
        eyeDistance+=25;
        break;
    }
    case '-':
    {
        eyeDistance-=25;
        break;
    }
    case 'a':
    {
        viewAngle+=1;
        break;

    }
    case 'z':
    {
        viewAngle-=1;
        break;
    }
    }
}

void initRendering()
{
    //glShadeModel(GL_SMOOTH);
    //
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_COLOR_MATERIAL);
    //glEnable (GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_NORMALIZE);
    //glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);
    /*
    GLfloat ambientLight[] = {1.0,1.0,1.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    GLfloat lightColor[] = {1.0f,1.0f,1.0f,1.0f};

    GLfloat lightPos[] = {200,100, -100.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, standardSpecular);
*/
    // Object loading
    //obj.load("../data/objmodels/angel.obj");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,5,eyeDistance);
}

void handleResize(int w, int h)
{  arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 2000.0);
}

Eigen::Vector3d getRayCylinderIntersection( const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, float cylinderRadius )
{
    double a = rayDirection.x()*rayDirection.x()+rayDirection.z()*rayDirection.z();
    double b = 2*(rayDirection.x()*rayOrigin.x()+rayOrigin.z()*rayDirection.z());
    double c = rayOrigin.x()*rayOrigin.x()+rayOrigin.z()*rayOrigin.z()-cylinderRadius*cylinderRadius;
    double t = (-b+sqrt(b*b-4*a*c))/(2*a);

    return rayDirection*t+rayOrigin;
}

Eigen::Vector3d getRayPlaneIntersection(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, const Eigen::Vector3d &planeNormal, const Eigen::Vector3d &planeOrigin)
{
    double offset = -planeNormal.dot(planeOrigin);
    double t = -(offset+planeNormal.dot(rayOrigin))/ planeNormal.dot(rayDirection);
    return rayDirection*t+rayOrigin;
}


void drawThings()
{
    Vector3d mirrorNormal;
    AngleAxis<double> aa(viewAngle,Vector3d::UnitY());
    mirrorNormal = aa.toRotationMatrix()*mirrorNormal;

    //glColor3fv(glWhite);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(0.1);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance);
    //arcball.applyRotationMatrix();
    GLfloat m[16]={2, 0, 0, 0, 0, 0, -2, 0, 0, 1.12, 0, -435, 0, 1, 0, -199};
    shader->begin();
    // Here me must set the uniforms
    shader->setUniform3f((GLcharARB*)"ProjectorCenter",projCenter.x(),projCenter.y(),projCenter.z());
    shader->setUniform1f((GLcharARB*)"viewerDistance",(GLfloat)viewerDistance);
    shader->setUniform1f((GLcharARB*)"viewerHeight",(GLfloat)viewerHeight);
    shader->setUniform3f((GLcharARB*)"mirrorNormal",(GLfloat)baseMirrorNormal.x(),(GLfloat)0.0,(GLfloat)baseMirrorNormal.z());
    shader->setUniform4fv((GLcharARB*)"projectorModelViewProjectionMatrix",16,m);
    glBegin(GL_POINTS);
    switch(shape)
    {
    case SIMPLE_CENTERED_POINT:
    {
        glVertex3d(0,0,0);
        break;
    }
    case SIMPLE_CENTERED_CUBE:
    {
        for (double x=-4; x<=4.0;x+=0.5)
        {
            for (double y=-4; y<=4.0;y+=0.5)
            {
                for (double z=-4;z<=4.0; z+=0.5)
                    glVertex3d(x,y,z);
            }
        }
        break;
    }
    case SIMPLE_CENTERED_PLANE:
    {
        for (double x=-4; x<=4.0;x+=0.5)
        {
            for (double y=-4; y<=4.0;y+=0.5)
            {
                glVertex3d(x,y,0);
            }
        }
        break;
    }
    }
    glEnd();
    shader->end();

    glPopMatrix();
    getGLerrors();
    glPopAttrib();
}

void drawText()
{
    GLText text(width,height,glRed);
    text.enterTextInputMode();
    text.draw("EyeDistance= "+util::stringify<double>(eyeDistance));
    text.draw("Frequency"+util::stringify<double>(frequency));
    text.draw(string("Degree° ")+util::stringify<int>(viewAngle) );
    text.draw("ViewDistance="+util::stringify<double>(viewerDistance));
    text.draw("ViewHeight="+util::stringify<double>(viewerHeight));
    text.draw("ProjectorCenter"+util::stringify<double >(projCenter.z()));

    switch ( shape )
    {
    case SIMPLE_CENTERED_POINT:
    {
        text.draw("POINT");
        break;
    }
    case SIMPLE_CENTERED_CUBE:
    {
        text.draw("CUBE");
        break;
    }
    case SIMPLE_CENTERED_PLANE:
    {
        text.draw("PLANE");
        break;
    }
    case SIMPLE_RAYS_ORIGINATIN_FROM_CENTER:
    {
        text.draw("Rays from center");
        break;
    }
    case SIMPLE_CENTERED_CYLINDER:
    {
        text.draw("CYLINDER");
        break;
    }
    }

    text.leaveTextInputMode();
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawText();
    drawThings();
    glutSwapBuffers();
}

void mouseFunc(int state, int button, int _x , int _y)
{  if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}

//Called every 15 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

int main(int argc, char* argv[])
{
    if (argc<2)
    {
        cerr << "Usage: ./testGLSLRayTracingShader [VertexShaderFile] [FragmentShaderFile]" << endl;
        cerr << "will load VertexShaderFile and FragmentShaderFile" << endl;
        exit(0);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("test GLSL raytracing shader");
    initRendering();

    // Load (and compile, link) from file
    string shadervertfile = string(argv[1]);
    string shaderfragfile = string(argv[2]);

    SM = new glsl::glShaderManager();
    shader = SM->loadfromFile((char*)shadervertfile.c_str(), (char*)shaderfragfile.c_str());
    delete SM;
    
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

