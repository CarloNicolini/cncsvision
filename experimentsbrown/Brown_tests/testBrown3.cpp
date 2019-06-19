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

//#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "StimulusDrawer.h"
#include "CylinderPointsStimulus.h"

#include "BrownMotorFunctions.h"
#include "Optotrak2.h"
#include "Marker.h"

using namespace std;
using namespace Eigen;
using namespace BrownMotorFunctions;

Arcball arcball;
ObjLoader model;
int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;

GLUquadricObj* myReusableQuadric;
double frame=0.0;

CylinderPointsStimulus cylinder;
StimulusDrawer stimDrawer;

//#define SIMULATION
static const Vector3d calibration(160,179,-75);
Optotrak2 *optotrak;
vector <Marker> markers;
VRCamera cam;

/* ############################# */

void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    int numMarkers=22;
    float frameRate=85.0f;
    float markerFreq=4600.0f;
    float dutyCycle=0.4f;
    float voltage = 7.0f;
#ifdef SIMULATION
    if ( optotrak->init("C:/cncsvisiondata/camerafiles/Aligned20111014",numMarkers, frameRate, markerFreq, dutyCycle,voltage) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
#endif
    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        optotrak->updateMarkers();
        markers = optotrak->getAllMarkers();
    }
}

void avoidunknownnvidiabugwhichshowsupifnobrownmotorfunctionisevercalled()
{
    homeEverythingAsynchronous(0,0);
}

void drawSphereAt(float x, float y, float z, float radius, int quality, bool wired)
{
    /*
            glPushMatrix();
            glTranslatef(x, y, z);

            if(wired)
                glutWireSphere(radius, quality, quality);
            else
                glutSolidSphere(radius, quality, quality);
            glPopMatrix();
    */
    //frame+=0.1;

/*
    GLLight light2;
    light2.setPosition(1,1,0);
    light2.setDiffuse(0.6,0.5,0.5,1.0);

    light2.setSpecular(0.3,0,0.5,1);
    light2.setAmbient(0.6,0.5,0.1,0.1);
    light2.on();
    light2.apply();
*/
    glPushMatrix();
    glScaled(1.0,1.0,1.0);
    model.draw();
    glPopMatrix();

/*
    GLLight light;
    light.setPosition(3,1,0);
    light.setDirection(2,sin(frame),0);
    light2.setAmbient(0.6,1.0,0.1,0.8);
    light.on();
    light.apply();


    glPushMatrix();
    glTranslated(2.0,0,0.5);
    model.draw();
    glPopMatrix();
*/
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
    /*
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    */
    glClearColor(0.0,0.0,0.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

}

void initVariables()
{
    model.load("../experimentsbrown/Brown_tests/objmodels/parabolic_cylinder_50.obj");
    //model.load("../data/objmodels/bunny.obj");
    cylinder.setNpoints(750);  //XXX controllare densita di distribuzione dei punti
    cylinder.setAperture(0,2*M_PI);
    cylinder.setRadiusAndHeight(1,1);
    cylinder.setFluffiness(0.0);
    cylinder.compute();

    stimDrawer.setSpheres(true);

    stimDrawer.setStimulus(&cylinder);
    stimDrawer.setRadius(0.01);
    stimDrawer.initList(&cylinder);
}

void handleResize(int w, int h) {

    arcball.setWidthHeight(w, h);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}



void drawReference()
{
    double length=4;
    glPushMatrix();
    //glTranslatef(length/2,0,0);
    glPushMatrix();
    //glColor3f(0,0,1);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(-90,1,0,0);
    //glColor3f(0,1,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(90,0,1,0);
    //glColor3f(1,0,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPopMatrix();
}

void drawThings()
{
	// This is a light that is attached to the teapot
    GLLight l1;
    l1.setDiffuse(0.1,0.1,0.1,1);
    l1.setAmbient(0.2,0.3,0.2,1);
    l1.setSpecular(1,1,0,1);
    l1.setDiffuse(1,0,1,1);
    l1.setPosition(0,-10,0);
    l1.setDirection(0,1,0);
    l1.on();


	glPushMatrix();
	glLoadIdentity();
    glTranslated(0,0,eyeZ-5);
    arcball.applyRotationMatrix();
    stimDrawer.draw();
    glPopMatrix();


	// Draw inner cylinder
	GLUquadric *quad = gluNewQuadric();
    gluQuadricDrawStyle(quad,GLU_LINE);
	glColor3fv(glWhite);
	glPushMatrix();
	glLoadIdentity();
    glTranslated(0,0,eyeZ-5);
    arcball.applyRotationMatrix();
	glRotated(90,1,0,0);
	glTranslated(0.0,0.0,-0.5);
    gluCylinder(quad,4,1,1,20,2);
	glPopMatrix();

   
	gluDeleteQuadric(quad);
    
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();

    drawSphereAt(0,0,0,2.0,20,1);
    /*
     GLLight light2;
     light2.setDiffuse(1,sin(frame),0.4,1);
     light2.apply();
     glPushMatrix();
     glTranslated(2,0,0);
     model.draw();
     glPopMatrix();
    */
    //drawThings();
    //drawReference();
    glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {

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
        eyeZ+=1;
    if (state ==4)
        eyeZ-=1;

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
    initVariables();
    homeScreen(0);
#ifdef SIMULATION
    initOptotrak();
#endif
    //myReusableQuadric =  gluNewQuadric();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

