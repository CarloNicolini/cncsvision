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
#include <string>

#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "StimulusDrawer.h"
#include "CylinderPointsStimulus.h"
#include "GLText.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "Util.h"

using namespace std;
using namespace Eigen;

using util::stringify;
using util::str2num;

Arcball arcball;
#define SCREEN_WIDTH 1536
#define SCREEN_HEIGHT 1152
int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 300, Z=-200;

double frame=0.0;

StimulusDrawer draw_ernst_banks[3];
CylinderPointsStimulus ernst_banks[3];

double stimulus_height = 80.0, stim_fluffiness = 133.0;
int pos=0;
// coordinates of the points
double *g_pointsX;
double *g_pointsY;
double *g_pointsZ;
double *c_pointsX;
double *c_pointsY;
double *c_pointsZ;
int numPoints=0;
double sizePoints = 0.0;
double *sizePointsC;

void buildErnstBanksDots(double height, double fluffiness);
void buildErnstBanksSpheres(double height, double fluffiness, double distance);
void buildComparison(double height, double fluffiness, double distance);
void drawInfo();

//----------------- FUNCTIONS

void drawInfo()
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	GLText text;

	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);

//	text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);

	text.enterTextInputMode();

	glColor3fv(glWhite);
	text.draw("# stim height: " + stringify<double>(stimulus_height));

	text.leaveTextInputMode();
}

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
	case 'q': //Escape key
        	exit(0);
        break;
	case '2':
	{
		Z--;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
	break;
	case '8':
	{
		Z++;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
	break;
	case '6':
	{
		stimulus_height += .5;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
	break;
	case '4':
	{
		stimulus_height -= .5;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
	break;
	case '3':
	{
		stim_fluffiness += .3;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
	break;
	case '1':
	{
		stim_fluffiness -= .3;
		buildComparison(stimulus_height, stim_fluffiness, Z);
	}
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
    glClearColor(0.0,0.0,0.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//    glTranslatef(0,0,eyeDistance);

}

void buildErnstBanksDots(double height, double fluffiness)
{
	//###### build ernst & banks stimulus ######
	// build the top and bottom sectors
	for (int i=0; i<2; i++)
	{
		ernst_banks[i].setNpoints(100);
		ernst_banks[i].setRadiusAndHeight(150, 0, (150-height)/2); // x, z, y (in mm)
		ernst_banks[i].setFluffiness(fluffiness);
		ernst_banks[i].compute();
		draw_ernst_banks[i].setStimulus(&ernst_banks[i]);
		draw_ernst_banks[i].setSpheres(true);
		draw_ernst_banks[i].initList(&ernst_banks[i], glRed);
	}

	// build the central sector (relief)
	ernst_banks[2].setNpoints(100);
	ernst_banks[2].setRadiusAndHeight(150, 0, height); // x, z, y (in mm)
	ernst_banks[2].compute();
	draw_ernst_banks[2].setStimulus(&ernst_banks[2]);
	draw_ernst_banks[2].setSpheres(true);
	draw_ernst_banks[2].initList(&ernst_banks[2], glGreen);
}

void buildErnstBanksSpheres(double height, double fluffiness, double distance)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / abs(distance))*180/M_PI;
	// density is 9 dots per degree
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle);
	// the size of the spheres is also dependent on distance
	sizePoints = tan(8.0/60.0*M_PI/180.0)*abs(distance);
	
	g_pointsX = new double[numPoints];
	g_pointsY = new double[numPoints];
	g_pointsZ = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		g_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		g_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		g_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/fluffiness));
		if(g_pointsY[i] < height/2 && g_pointsY[i] > -height/2)
			g_pointsZ[i] = g_pointsZ[i] + 30.0;
	}
}

void buildComparison(double height, double fluffiness, double distance)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / 2.0 / abs(distance))*180/M_PI*2.0;
	// density is 9 dots per degree^2
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle * stimulus_visual_angle);
	
	c_pointsX = new double[numPoints];
	c_pointsY = new double[numPoints];
	c_pointsZ = new double[numPoints];
	sizePointsC = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		c_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(30.0*fluffiness/100.0))) - (30.0*fluffiness/100.0)/2.0;
		if(c_pointsY[i] < height/2.0 && c_pointsY[i] > -height/2.0)
			c_pointsZ[i] = c_pointsZ[i] + 30.0;
	}
	// the size of the spheres is also dependent on distance
	for(int i = 0; i < numPoints; ++i ) 
	{
		sizePointsC[i] = 8.0/60.0*M_PI/180.0 * abs(distance+c_pointsZ[i]) / 2.0;
	}

}

void drawErnstBanksDots(double height)
{
	glLoadIdentity();
	glTranslated(0.0,0,Z);
	glRotatef(M_PI/2, 0, 1, 0);
	arcball.applyRotationMatrix();

	// print top sector
	glPushMatrix();
	glTranslated(0, (150+height)/4.0, 0);
	draw_ernst_banks[0].draw();
	glPopMatrix();

	// print bottom sector
	glPushMatrix();
	glTranslated(0, -(150+height)/4.0, 0);
	draw_ernst_banks[1].draw();
	glPopMatrix();

	// print relief
	glPushMatrix();
	glTranslated(0, 0, 30.0);
	draw_ernst_banks[2].draw();
	glPopMatrix();
}

void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double sizeSpheres, double distance)
{
	glLoadIdentity();
	glTranslated(0.0,0,Z);
	arcball.applyRotationMatrix();

	glColor3fv(glRed);

	for (int i = 0; i < numSpheres; ++i )
	{ 
		glPushMatrix();
		glTranslated(px[i], py[i], pz[i]);
		glutSolidSphere(1.5*sizeSpheres,10,10);
		glPopMatrix();
	}
}

void drawComparison(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres, double distance)
{
	glLoadIdentity();
	glTranslated(0.0,0,Z);
	arcball.applyRotationMatrix();

	glColor3fv(glRed);

	for (int i = 0; i < numSpheres; ++i )
	{ 
		glPushMatrix();
		glTranslated(px[i], py[i], pz[i]);
		glutSolidSphere(1.5*sizeSpheres[i],10,10);
		glPopMatrix();
	}
}

void initVariables()
{
	buildErnstBanksSpheres(stimulus_height, stim_fluffiness, Z);
}

void handleResize(int w, int h) {

    arcball.setWidthHeight(w, h);

    glViewport(0,0,width, height);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 300.0);
}

void drawGLScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawInfo();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();

    drawComparison(c_pointsX, c_pointsY, c_pointsZ, numPoints, sizePointsC, Z);

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

//-------------------------- END OF FUNCTIONS

int main(int argc, char** argv)
{
	mathcommon::randomizeStart();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutCreateWindow("ErnstBanks2002 Stimulus");
	initRendering();
	initVariables();
	buildComparison(stimulus_height, stim_fluffiness, Z);
	cerr << sizePoints << endl;
	buildErnstBanksDots(55.0, 30.0);
	cerr << ernst_banks[1].getHeight() << endl;
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseDrag);
	glutTimerFunc(15, update, 0);

	glutMainLoop();
	return 0;
}

