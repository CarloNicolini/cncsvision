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
#include "Timer.h"
#include "GLUtils.h"
#include "Util.h"
#include "GLText.h"

using namespace std;
using namespace Eigen;
int width=640, height=480;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double frame=0.0;

#define TIMER_MS 100

Vector2d p(0,0);
int sumOutside=0;
bool conditionInside=true,wasInside=true,triggerStimulus=false;
Timer translationTimer,totalTimer;
double minOscTime=0.1,maxOscTime=4.0,maxXOscillation=2.5;


int trialMode=0;

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
void idle();

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    case '4':
        if (trialMode==PROBEMODE)
        {
            trialMode=FIXATIONMODE;
        }
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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

}

void initVariables()
{
    translationTimer.start();
}

void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

void drawThings()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(5);
    // Draw the point
    glBegin(GL_POINTS);
    glVertex3d( p.x() ,p.y(), 10.0);
    glEnd();

    // Draw the borders
    glBegin(GL_POINTS);
    glVertex3d( -maxXOscillation ,0,10.0);
    glVertex3d( maxXOscillation ,0, 10.0);
    glPointSize(15);
    switch(trialMode)
    {
    case STIMULUSMODE:
        glColor3fv(glRed);
        glVertex3d( 1,-1, 10.0);
        break;
    case PROBEMODE:
        glColor3fv(glGreen);
        glVertex3d( 0 ,-1, 10.0);
        break;
    case FIXATIONMODE:
        glColor3fv(glWhite);
        glVertex3d( -1 ,-1, 10.0);
        break;
    }
    glEnd();
    glPopAttrib();

    GLText text;text.init(width,height,glWhite);
    text.enterTextInputMode();
    switch(trialMode)
    {
    case STIMULUSMODE:
        text.draw("STIMULUSMODE");
        break;
    case PROBEMODE:
        text.draw("PROBEMODE");
        break;
    case FIXATIONMODE:
        text.draw("FIXATIONMODE");
        break;
    }
    text.draw("Time = " + util::stringify<double>(translationTimer.getElapsedTimeInSec()));
    text.leaveTextInputMode();
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    drawThings();
    glutSwapBuffers();
}

void fastBeep(double x)
{
    cerr << "Too fast " << x <<endl;
}
void slowBeep(double x)
{
    cerr << "too slow " << x << endl;
}

void goodBeep()
{
    cerr << "GOOD" << endl;
}

void checkBounds(int _nOscillations, int _trialMode, double _minOscTime, double _maxOscTime, double _maxOscillation, void (*_goodBeepTrigger)(), void (*_fastBeepTrigger)(double t),void (*_slowBeepTrigger)(double t) )
{
    conditionInside = abs(p.x()) < _maxOscillation;
    bool isEnteringArea = !( !conditionInside || (wasInside) );
    bool isLeavingArea =  !( conditionInside || (!wasInside) );

    if ( trialMode==PROBEMODE )
        return;
    static int sumExitFromRight=0, sumExitFromLeft=0;
    bool canIncreaseSum = abs(sumExitFromLeft-sumExitFromRight)<2;
    double elapsedTime = translationTimer.getElapsedTimeInSec();
    bool tooSlow = elapsedTime > maxOscTime;
    bool tooFast = elapsedTime < minOscTime;
    bool correctRythm = !tooSlow && !tooFast;

    if ( isLeavingArea )
    {
        static int correctDirection = 1;
        // First check that the rythm was correct
        if ( tooSlow)
        {
            _slowBeepTrigger(elapsedTime);
            sumExitFromRight=sumExitFromLeft=0;
        }
        if ( tooFast )
        {
            _fastBeepTrigger(elapsedTime);
            sumExitFromRight=sumExitFromLeft=0;
        }
        if ( correctRythm)
        {
            //_goodBeepTrigger();
            if ( p.x()*correctDirection < 0 )
            {
                cerr << "reset!" << endl;
                sumExitFromLeft=sumExitFromRight=0;
            }
            else
            {
                if ( p.x() > 0 && (correctDirection==1) )
                {
                    cerr << "<--" << endl;
                    correctDirection=-1;
                    sumExitFromRight++;
                }
                if ( p.x() < 0 && (correctDirection==-1) )
                {
                    cerr << "-->" << endl;
                    correctDirection=1;
                    sumExitFromLeft++;
                }
            }
        }
        else
        {
            sumExitFromRight=sumExitFromLeft=0;
        }
        if ( trialMode == STIMULUSMODE )
            trialMode=PROBEMODE;
        else
            trialMode=FIXATIONMODE;
    }

    if (  !(canIncreaseSum) )
    {
        sumExitFromRight=sumExitFromLeft=0;
    }

    if ( (p.x()>0) && isEnteringArea && sumExitFromRight>=(_nOscillations) &&  canIncreaseSum && trialMode == FIXATIONMODE )
    {
        trialMode=STIMULUSMODE;
        sumExitFromRight=sumExitFromLeft=0;
    }

    if ( (isLeavingArea || isEnteringArea) )
    {
        // Inizia a contare il movimento
        translationTimer.start();
    }
    wasInside = conditionInside;
}

void update(int value) {

    idle();
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
    int nOscillations=2;
    checkBounds(nOscillations,trialMode,minOscTime,maxOscTime,maxXOscillation, goodBeep ,fastBeep,slowBeep);
}

void mouseFunc(int state, int button, int _x , int _y)
{
    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    p.x()=10*(((double)_x/(double)width)-0.5);
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("Checkbounds demo");
    initRendering();
    initVariables();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(TIMER_MS, update, 0);
    //glutIdleFunc(idle);
    //glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}

