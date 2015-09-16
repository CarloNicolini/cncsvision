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

#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"

#include "RadialPointStimulus.h"
#include "StimulusDrawer.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
ObjLoader model;
int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;

GLUquadricObj* myReusableQuadric;
double frame=0.0;
RadialPointStimulus radial;
StimulusDrawer stimDrawer;

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
        frame+=0.1;
        
        
        GLLight light2;
        light2.setPosition(1,1,0);
        light2.setDiffuse(0.6,0.5,0.5,1.0);

        light2.setSpecular(0.3,0,0.5,1);
        light2.setAmbient(0.6,0.5,0.1,0.1);
        light2.on();
        light2.apply();
        
        glPushMatrix();
        model.draw();
        glPopMatrix();
        
        
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

}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 'q': //Escape key
		exit(0);
	        break;
	        case '+':
                eyeDistance+=0.001;
                radial.setFluffiness(abs(eyeDistance/20));
                radial.compute();
                stimDrawer.initList(&radial);
            break;
            case '-':
            eyeDistance-=0.1;
            radial.setFluffiness(abs(eyeDistance/20));
            radial.compute();
            stimDrawer.initList(&radial);
            break;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.0,0.0,0.0,1.0);
	/*
	GLfloat ambientLight[] = {0.0,0.0,0.0, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	GLfloat lightColor[] = {0.3f, 0.3f, 0.0f, 0.3f};
	
	GLfloat lightPos[] = {200,100, -100.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, standardSpecular);
    glEnable(GL_LIGHT0);
	*/
	glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0,0,eyeDistance);
	
}

void initVariables()
{
    model.load("../data/objmodels/coffeemug.obj");
    
    radial.setNpoints(50);  //XXX controllare densita di distribuzione dei punti
	   radial.compute();
	   stimDrawer.setStimulus(&radial);
       stimDrawer.initList(&radial);
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
        glPushMatrix();
        
        glLoadIdentity();
        // This is a light that is attached to the teapot
        GLLight l1;
        l1.setDiffuse(0.1,0.1,0.1,1);
        l1.setAmbient(0.2,0.3,0.2,1);
        l1.setSpecular(1,1,0,1);
        l1.setDiffuse(1,0,1,1);
        l1.setPosition(0,-10,0);
        l1.setDirection(0,1,0);

        /*
        GLLight l2;
        l2.setAmbient(0.5,0,0.5,1);
        l2.setCutoff(15);
        l2.setSpecular(1,1,1,1);
        l2.setDiffuse(1,0,1,1);
        l2.setDirection(0,1,-1);
        l2.on();
        */
        l1.on();
        glTranslated(0,0,-5);
        arcball.applyRotationMatrix();
        //glutSolidTeapot(1);
        glScalef(0.5,0.5,0.5);
        stimDrawer.draw();
        l1.on();
        glPopMatrix();
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

    //drawSphereAt(0,0,0,2.0,20,1);
   /*
    GLLight light2;
    light2.setDiffuse(1,sin(frame),0.4,1);
    light2.apply();
    glPushMatrix();
    glTranslated(2,0,0);
    model.draw();
    glPopMatrix();
*/
    drawThings();
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
    initVariables();
    myReusableQuadric =  gluNewQuadric();	
    glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
	glutTimerFunc(15, update, 0);
	
	glutMainLoop();
	return 0;
}

