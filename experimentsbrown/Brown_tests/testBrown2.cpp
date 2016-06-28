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

#include "BrownMotorFunctions.h"
using namespace BrownMotorFunctions;

using namespace std;
using namespace Eigen;

Arcball arcball;
#define SCREEN_WIDTH 1536
#define SCREEN_HEIGHT 1152
int width=1536, height=1152;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 300, Z=-200;

double frame=0.0;

StimulusDrawer draw_ernst_banks[3];
CylinderPointsStimulus ernst_banks[3];

double stimulus_height = 80.0;

void buildErnstBanks(double height);
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
	text.draw("# stim height: ");// +stringify<double>(stimulus_height));

	text.leaveTextInputMode();
}

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
	case 'q': //Escape key
        	exit(0);
        break;
	case '2':
		Z--;
	break;
	case '8':
		Z++;
	break;
	case '6':
	{
		stimulus_height++;
		buildErnstBanks(stimulus_height);
	}
	break;
	case '4':
	{
		stimulus_height--;
		buildErnstBanks(stimulus_height);
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

void buildErnstBanks(double height)
{
	//###### build ernst & banks stimulus ######
	// build the top and bottom sectors
	for (int i=0; i<2; i++)
	{
		ernst_banks[i].setNpoints(100);
		ernst_banks[i].setRadiusAndHeight(150, 0, (150-height)/2); // x, z, y (in mm)
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

void drawErnstBanks(double height)
{
	glLoadIdentity();
	glTranslated(0.0,0,Z);
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
	glTranslated(0, 0, 0);
	draw_ernst_banks[2].draw();
	glPopMatrix();
}

void initVariables()
{
	buildErnstBanks(stimulus_height);
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
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();

    drawErnstBanks(stimulus_height);

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

void initMotors()
{
	homeEverything(4051,3500);
}

//-------------------------- END OF FUNCTIONS

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("Fanculo");
    initRendering();
    initVariables();
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

