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
#include <Eigen/Core>
#include "Arcball.h"
#include "ParametersLoader.h"
#include "ExtrudedTrapezePointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "LatestCalibration.h"
#include "Util.h"
#include <windows.h>

using namespace std;
using namespace Eigen;
using util::stringify;

static const bool gameMode=true;
static const bool stereo=true;
bool infoDraw=true;
Arcball arcball;
VRCamera cam;
ParametersLoader parameters;
double focalDistance=-418.5;
double eyeZ=0.0;
int width=1024, height=768;
StimulusDrawer stimDrawer;
ExtrudedTrapezePointsStimulus stim;

double stimHeight=25;
double stimWidthFar=50;
double stimWidthNear=25;
double stimCenterY=0;
double stimZNear = 20;
double stimZFar = -20;
double stimDensity = 0.05;

char *parametersFileNames[]=
{
    "C:/cncsvisiondata/parametersFiles/Chiara/ExpTrapezoid/parametersTrapezoidCalibration.txt",
};

void initVariables();
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
    case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case 'a':
    {
        stimHeight+=1;
        break;
    }
    case 'z':
    {
        stimHeight-=1;
        break;
    }
    case 's':
    {
        stimWidthFar+=1;
        break;
    }
    case 'x':
    {
        stimWidthFar-=1;
        break;
    }
    case 'd':
    {
        stimWidthNear+=1;
        break;
    }
    case 'c':
    {
        stimWidthNear-=1;
        break;
    }
    case 'f':
    {
        stimZFar+=1;
        break;
    }
    case 'v':
    {
        stimZFar-=1;
        break;
    }
    case 'g':
    {
        stimZNear+=1;
        break;
    }
    case 'b':
    {
        stimZNear-=1;
        break;
    }
    case 'h':
    {
        stimCenterY+=1;
        break;
    }
    case 'n':
    {
        stimCenterY-=1;
        break;
    }
    case '+':
    {
        stimDensity+=0.005;
        break;
    }
    case '-':
    {
        stimDensity-=0.005;
        break;
    }
    case 13:
    {
		string outputfilename = "C:/cncsvisiondata/parametersFiles/Chiara/ExpTrapezoid/parameters_test.txt";
		ofstream output;
		// Check for output file existence
		output.open(( outputfilename ).c_str());
		output <<
			"StimCenterY = " << stimCenterY << endl <<
			"StimWidthFar = " << stimWidthFar<< endl <<
			"StimWidthNear = " << stimWidthNear << endl <<
			"StimDensity = " << stimDensity<< endl <<
			"StimZNear = " << stimZNear<< endl <<
			"StimZFar = " << stimZFar<< endl <<
			"StimHeight = " << stimHeight<< endl;
        break;
    }
	case 'i':
		{
			infoDraw=!infoDraw;
			break;
		}
	default:
		return;
    }
    initVariables();
}


void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
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
    glDepthFunc(GL_LEQUAL);
    glLoadIdentity();
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0,0,focalDistance),true);
    cam.setNearFarPlanes(10.0,800.0);
    //cam.setOrthoGraphicProjection(true);
}

void initVariables()
{
    stim.setHeight(stimHeight);
    stim.setWFar(stimWidthFar);
    stim.setWNear(stimWidthNear);
    stim.setZNear(stimZNear);
    stim.setZFar(stimZFar);
    stim.setDensity(stimDensity);
    stim.compute(); // Update the stimulus points
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&stim);
    stimDrawer.initList(&stim,glRed);

	parameters.loadParameterFile(parametersFileNames[0]);
	cout << parameters.find("BaseDir") << endl;
	cout << parameters.get("IOD") << endl;
}

void drawText()
{
    GLText text(width,height,glWhite);
    text.enterTextInputMode();
    text.draw("StimHeight= " + stringify<double>(stimHeight));
	text.draw("StimZNear= " + stringify<double>(stimZNear));
	text.draw("StimZFar= " + stringify<double>(stimZFar));
	text.draw("StimWidthNear= " + stringify<double>(stimWidthNear));
	text.draw("stimWidthFar= " + stringify<double>(stimWidthFar));
	text.draw("stimCenterY= " + stringify<double>(stimCenterY));
    text.draw("PRESS ENTER TO SAVE TO trapezoid_parameters.txt");
	text.leaveTextInputMode();
}

void drawStimulus()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,stimCenterY,focalDistance);
    arcball.applyRotationMatrix();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0, 2.0);
    Vector3d *P = stim.getVertices();
    glColor3fv(glBlack);
    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[1].data());
    glVertex3dv(P[2].data());
    glVertex3dv(P[5].data());
    glVertex3dv(P[6].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[0].data());
    glVertex3dv(P[1].data());
    glVertex3dv(P[3].data());
    glVertex3dv(P[2].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[4].data());
    glVertex3dv(P[5].data());
    glVertex3dv(P[7].data());
    glVertex3dv(P[6].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[0].data());
    glVertex3dv(P[1].data());
    glVertex3dv(P[4].data());
    glVertex3dv(P[5].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[2].data());
    glVertex3dv(P[3].data());
    glVertex3dv(P[6].data());
    glVertex3dv(P[7].data());
    glEnd();
    glPopAttrib();
    stimDrawer.draw();
    glPopMatrix();
}
/*
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d(0.0,0.0,0.0));
    drawStimulus();
    drawText();
    glutSwapBuffers();
}
*/

void drawScene()
{
 if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
		cam.setEye(Vector3d(parameters.get("IOD")/2,0.0,0.0));
		if(infoDraw)
			drawText();
        drawStimulus();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(Vector3d(-parameters.get("IOD")/2,0.0,0.0));
		if(infoDraw)
			drawText();
        drawStimulus();
        glutSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		cam.setEye(Vector3d(0.0,0.0,0.0));
		if(infoDraw)
			drawText();
        drawStimulus();
        glutSwapBuffers();
    }
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
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
{
    glutInit(&argc, argv);
	if ( stereo )
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	if (gameMode==false)
    {  
		glutInitWindowSize(width, height);
        glutCreateWindow("Trapezoid Calibration");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);
	glutSetCursor(GLUT_CURSOR_NONE);
    initRendering();
    initVariables();


    glutMainLoop();
    return 0;
}
