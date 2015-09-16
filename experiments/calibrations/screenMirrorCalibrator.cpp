// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include "GLMaterial.h"
#include "GLUtils.h"
#include "Arcball.h"
#include "Marker.h"
#include "Grid.h"
#include "GLText.h"
#include "Util.h"
#include "Mathcommon.h"
#include "Optotrak2.h"
#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
Grid grid;
Optotrak2 optotrak;
vector<Marker> markers;
int width=1024, height=768;
double eyeZ=0;
double eyeShift=0;
double eyeDistance=-200;
double FOV = 45;

#define TIMER_MS 10
GLUquadricObj* myReusableQuadric;
double frame=0.0;

double mirrorYaw,mirrorPitch,mirrorRoll,savedMirrorYaw;

int calibrationPhase=0;

double screenYaw,screenPitch,screenRoll;
Vector3d screenNormal;

ofstream coordinateFile;
Vector3d mirror[4],mirrorSaved[4],screen[4],screenSaved[4],mirrorNormal, mirrorSavedCenter, mirrorCenter , screenCenter, screenProjectionOnMirror, planesIntersectionBase, planesIntersectionDirection, planesIntersecEnd;

Vector3d pMirror[4],pScreen[4];
Vector3d mirrorSavedCoordinates[4];


void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {
    case 'f':
    {
        glutFullScreen();
    }
    break;
    case 27:
    case 'q': //Escape key
        exit(0);
        break;
    case '2':
    {
        eyeZ-=10;
    }
    break;
    case '8':
    {
        eyeZ+=10;
    }
    break;
    case '+':
        eyeShift+=10;
        break;
    case '-':
        eyeShift-=10;
        break;
    case ' ':
    {
		for (int i=0; i<4;i++)
			mirrorSaved[i]=mirror[i];
        savedMirrorYaw = mirrorYaw;
		mirrorSavedCenter = (mirror[0]+mirror[1]+mirror[2]+mirror[3])/4.0;
        coordinateFile << "Mirror coordinates" << endl;
        coordinateFile << mirror[0].transpose() << endl << mirror[1].transpose() << endl << mirror[2].transpose() << endl;
        coordinateFile << "Screen coordinates" << endl;
        coordinateFile << screen[0].transpose() << endl << screen[1].transpose() << endl << screen[2].transpose() << endl;
        calibrationPhase=1;
    }
    break;
    }
}

void initRendering()
{   glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);

    GLfloat ambientLight[] = {0.9,0.9,0.9, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
//GLfloat lightColor[] = {0.3f, 0.3f, 0.3f, 0.3f};

//GLfloat lightPos[] = {200,100, -100.0f};
//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//glLightfv(GL_LIGHT0, GL_SPECULAR, standardSpecular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);
}

void initVariables()
{
    coordinateFile.open("calibrationCoordinates.txt");
    grid.setRowsAndCols(20,20); //here we set the number of columns and rows per edge
    // here we measure in millimeters
    grid.init(Vector3d(-500, -500, 0.0),Vector3d(500, -500, 0.0),Vector3d(-500, 500, 0.0),Vector3d(500, 500, 0.0));

    optotrak.setTranslation(calibration);
    optotrak.init(LastAlignedFile);
}

void handleResize(int w, int h)
{

    arcball.setWidthHeight(w, h);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.001, 1200.0);
    //glOrtho(-800.0, 800.0, -800,800, -800, 800);
}



void drawReference()
{   double length=10;
    glPushMatrix();
//glTranslatef(length/2,0,0);
    glPushMatrix();
    glColor3f(0,0,1);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(-90,1,0,0);
    glColor3f(0,1,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(90,0,1,0);
    glColor3f(1,0,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPopMatrix();
}

Vector3d getPlaneIntersection( const Hyperplane<double,3> &plane1, const Hyperplane<double,3> &plane2, double lambda)
{
    double dot = plane1.normal().dot(plane2.normal());
    Vector3d cross = plane1.normal().cross(plane2.normal());
    double c1 = (plane1.offset() - plane2.offset()*dot )/(1.0-dot*dot);
    double c2 = (plane2.offset() - plane1.offset()*dot )/(1.0-dot*dot);
    Vector3d r = (c1*plane1.normal() + c2*plane2.normal() ) + lambda*(cross);

    return r;
}

void drawGeneratedPlanes()
{
    Hyperplane<double,3> screenPlane,mirrorPlane,savedMirrorPlane;

    //mirror[3] = Vector3d(mirror[1].x(),mirror[2].y(),mirror[1].z());
    screen[3] = Vector3d(screen[1].x(), screen[2].y(),screen[1].z());

    screenPlane = Hyperplane<double,3>::Through( screen[0], screen[1],screen[2] );
    mirrorPlane = Hyperplane<double,3>::Through(mirror[1], mirror[0], mirror[2] );
	savedMirrorPlane = Hyperplane<double,3>::Through(mirrorSaved[1], mirrorSaved[0], mirrorSaved[2] );
    screenNormal = screenPlane.normal();
    mirrorNormal = mirrorPlane.normal();
	
    Eigen::ParametrizedLine<double,3> line = Eigen::ParametrizedLine<double,3>::Through( screenCenter , screenCenter + screenNormal*10);
    // Calcola il punto proiezione della normale uscente dallo schermo sullo specchio
    double p = line.intersection(mirrorPlane);
    screenProjectionOnMirror = p*((screenNormal).normalized()) + screenCenter;

    planesIntersectionBase = -getPlaneIntersection(mirrorPlane,screenPlane,0);
    planesIntersectionDirection = mirrorPlane.normal().cross(screenPlane.normal());

    planesIntersecEnd = planesIntersectionBase - 200*planesIntersectionDirection;

    // Draw the planes intersection
    glPushMatrix();
    glColor3fv(glBlue);
    glBegin(GL_LINES);
    glVertex3dv(planesIntersectionBase.data());
    glVertex3dv(planesIntersecEnd.data());
    glEnd();
    glPopMatrix();

    // Draw the mirror plane (only if calibrationPhase==0)
    if ( calibrationPhase==0 )
    {
        glPushMatrix();
        Grid gridMirror;
        gridMirror.setRowsAndCols(20,20);
        gridMirror.init(mirror[1],mirror[0],mirror[2],mirror[3]);	// invert the order to have 45 degrees yaw and 90 degree pitch when correct
        glColor3fv(glRed);
        gridMirror.draw();
        glPopMatrix();

        // Draw the mirror normal
        mirrorCenter = (mirror[0]+mirror[1]+mirror[2]+mirror[3])/4.0;
        Vector3d mirrorCenterDirection = mirrorCenter + 20*mirrorNormal;
        glPushMatrix();
        glColor3fv(glRed);
        glBegin(GL_LINES);
        glVertex3dv(mirrorCenter.data());
        glVertex3dv(mirrorCenterDirection.data());
        glEnd();
        glPopMatrix();
    }

    // Draw the screen plane
    if ( calibrationPhase!=0)
    {
        glPushMatrix();
        Grid gridScreen;
        gridScreen.setRowsAndCols(20,20);
        gridScreen.init(screen[0],screen[1],screen[2],screen[3]);
        glColor3fv(glGreen);
        gridScreen.draw();
        glPopMatrix();

        // Draw the screen normal
        screenCenter = (screen[0]+screen[1]+screen[2]+screen[3])/4.0;
        Vector3d screenCenterDirection = screenCenter + 20*screenNormal;
        glPushMatrix();
        glBegin(GL_LINES);
        glVertex3dv(screenCenter.data());
        glVertex3dv(screenCenterDirection.data());
        glEnd();
        glPopMatrix();
    }


    // Draw the vector from screen center to mirror center
    glPushMatrix();
    glColor3fv(glRed);
    glBegin(GL_LINES);
    glVertex3dv(screenCenter.data());
    glVertex3dv(mirrorCenter.data());
    glEnd();
    glPopMatrix();

    // Draw the vector from mirror center to X axis
    Vector3d orthoProjMirrorCenter(mirrorCenter);
    glPushMatrix();
    glColor3fv(glRed);
    glBegin(GL_LINES);
    glVertex3dv(mirrorCenter.data());
    glVertex3d(mirrorCenter.x(),mirrorCenter.y(),0);
    glEnd();
    glPopMatrix();

    // draw mirror saved center
    if ( calibrationPhase == 1)
    {
        glPushMatrix();
        glTranslatef(mirrorSavedCenter.x(),mirrorSavedCenter.y(),mirrorSavedCenter.z());
        glutSolidSphere(1,10,20);
        glPopMatrix();
    }

    mirrorYaw = mathcommon::toDegrees(acos(mirrorNormal.z()));
    mirrorPitch = mathcommon::toDegrees(acos(mirrorNormal.y()));

    screenYaw = mathcommon::toDegrees(acos(screenNormal.z()));
    screenPitch = mathcommon::toDegrees(acos(screenNormal.y()));
    glColor3fv(glWhite);

    double tolerance = 0.5; //degrees
    bool mirrorYawOK = abs(mirrorYaw - 45) <= tolerance;
    bool screenYawOK = abs(screenYaw - 90) <= tolerance;
    bool mirrorPitchOK = abs(mirrorPitch-90) <= tolerance;
    bool screenPitchOK = abs(screenPitch-90) <= tolerance;


    GLText text;
    text.init(1024,768,glWhite,GLUT_BITMAP_HELVETICA_18);
    text.enterTextInputMode();
    switch( calibrationPhase )
    {
    case 0:
        text.draw("==== INSTRUCTIONS ====");
        text.draw("Put the markers on the mirror vertices, try to keep them aligned. Press SPACEBAR to save their coordinates");
        text.draw("Press 2/8 to zoom out/in. Press '+/-' to go forward or backward");
        break;
    case 1:
    {
        text.draw("Now remove the mirror, mirror coordinates are saved to calibrationCoordinates.txt");
        text.draw("Now you must put the markers on the screen");
        text.draw("");
        text.draw("");
        text.draw("Mirror current center= " + util::stringify< Eigen::Matrix<double,1,3> >(mirrorSavedCenter.transpose() ) );
        text.draw("Distance from mirror center to screen center " + util::stringify<double>( (mirrorSavedCenter - screenCenter).norm()	 ));
        text.draw("Projection error of screen normal on mirror center  " + util::stringify<Eigen::Matrix<double,1,3> >( screenProjectionOnMirror.transpose() ) );
    }
    break;
    case 2:
        break;
    }

    text.draw("==== MIRROR INFO ====");
    mirrorPitchOK ? glColor3fv(glGreen) : glColor3fv(glRed);
    text.draw("Mirror Pitch= " + util::stringify<double>(mirrorPitch) );
    mirrorYawOK ? glColor3fv(glGreen) : glColor3fv(glRed);
    text.draw("Mirror Yaw=" + util::stringify<double>(mirrorYaw));
    glColor3fv(glWhite);
    text.draw("==== SCREEN INFO ====");
    screenPitchOK ? glColor3fv(glGreen) : glColor3fv(glRed);
    text.draw("Screen Pitch=" + util::stringify<double>(screenPitch));
    screenYawOK ? glColor3fv(glGreen) : glColor3fv(glRed);
    text.draw("Screen Yaw=" + util::stringify<double>(screenYaw));
    // Here print the relative orientation of screen and mirror
    text.draw("EQUATION OF MIRROR PLANE");
	text.draw("normal= " + util::stringify<Eigen::RowVector3d>(mirrorPlane.normal().transpose()));
	text.draw("offset= " + util::stringify<double>(mirrorPlane.offset()));
    text.draw("EQUATION OF SAVED MIRROR PLANE");
	text.draw("normal= " + util::stringify<Eigen::RowVector3d>(savedMirrorPlane.normal().transpose()));
	text.draw("offset= " + util::stringify<double>(savedMirrorPlane.offset()));
    
	text.draw("EQUATION OF SCREEN PLANE");
	text.draw("normal= " + util::stringify<Eigen::RowVector3d>(screenPlane.normal().transpose()));
	text.draw("offset= " + util::stringify<double>(screenPlane.offset()));
	double d1 = savedMirrorPlane.offset()/cos(savedMirrorYaw/180*M_PI);
	double d2 = screenPlane.offset();
	text.draw("FocalDistance f = d1/cos(mirrorYaw)+d2 = " + util::stringify<double>(d1+d2) );
	
	text.leaveTextInputMode();
}

void drawScene()
{   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  	// clear background to black
    glClearDepth(100.0);            // set depth buffer to the most distant value
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,1);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
//glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();
    glTranslatef(0,0,eyeShift);
// Qua mettere tutte le funzioni di disegno
    glPushMatrix();
    glScalef(3,3,3);
    drawReference();
    glPopMatrix();
    drawGeneratedPlanes();
//grid.draw();
    glRotated(90,1,0,0);
    glColor3fv(glGray50);
    grid.draw();
//drawInfo();
    glutSwapBuffers();
}

//Called every TIMER_MS milliseconds
void update(int value)
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();

    // Set the screen and mirror points
    screen[0] = markers.at(5).p;//Vector3d(0,0,-100);
    screen[1] = markers.at(6).p;//Vector3d(0,0,-200);
    screen[2] = markers.at(7).p;//Vector3d(0,100,-100);
    screen[3] = markers.at(8).p;//Vector3d(0,100,-200);

    mirror[0] = markers.at(9).p;//Vector3d(-50,0,-200) + Vector3d(100,0,0);
    mirror[1] = markers.at(10).p;//Vector3d(50,0,-100)+ Vector3d(100,0,0);
    mirror[2] = markers.at(11).p;//Vector3d(-50,100,-200)+ Vector3d(100,0,0);
    mirror[3] = markers.at(12).p;//Vector3d(50,100,-100)+ Vector3d(100,0,0);

    double markersOffset=3.0;
    double mirrorThickness=0.0;
    for (int i=0; i<4; i++)
    {
        screen[i].x()-=markersOffset;
        mirror[i] -= (markersOffset+mirrorThickness)*Vector3d(sqrt(2.0)/2,0,-sqrt(2.0)/2);
    }


    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
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


int main(int argc, char** argv)
{   glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("Optotrak mirror and plane calibrations");
    initRendering();
    initVariables();
    myReusableQuadric =  gluNewQuadric();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(TIMER_MS, update, 0);

    glutMainLoop();
    return 0;
}

