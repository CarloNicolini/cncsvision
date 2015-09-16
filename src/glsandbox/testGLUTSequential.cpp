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
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Screen.h"
#include "VRCamera.h"
#include "GLUtils.h"
#include "Timer.h"
#include "GLText.h"

#include "Util.h"

#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters
static const double focalDistance=-500;

void initGLUTContext(int argc, char *argv[],bool gameMode)
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
    glutCreateWindow("GLUT test");
    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
    }
    else
    {
        glutGameModeString("1680x1050:32@60");
        glutEnterGameMode();
        glutFullScreen();
    }
    //glutSetCursor(GLUT_CURSOR_NONE);
}

void setupRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw2DInfo()
{
    GLText text(1024,768,glRed);
    text.enterTextInputMode();
    text.draw(util::stringify<double>((double)rand()/RAND_MAX));
    text.leaveTextInputMode();
}

void draw3DScene(VRCamera &cam, const  Vector3d &eye, double z)
{
    cam.setEye(eye);
    glPushMatrix();
    glLoadIdentity();
    //glTranslated(sphereCenter.x(),sphereCenter.y(),sphereCenter.z());
    glTranslated(0,0,focalDistance+z);
    glutSolidSphere(40,40,40);
    glPopMatrix();
}

int main(int argc, char*argv[])
{
    bool gamemode=true;
    initGLUTContext(argc,argv,gamemode);
    setupRendering();

    Timer timer;
    timer.start();

    Screen screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0,0,focalDistance);
    VRCamera cam(screen);

    double oldTime=timer.getElapsedTimeInMilliSec();
    ofstream timeFile("times.txt");
    int frame=0;
    while (timer.getElapsedTimeInMilliSec()<4000)
    {
        Timer frameTimer; frameTimer.start();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        draw2DInfo();
        draw3DScene(cam,Vector3d::Zero(),60*sin(timer.getElapsedTimeInSec()));
        glutSwapBuffers();
        oldTime = timer.getElapsedTimeInMilliSec();
        timeFile << frame++ << " " << oldTime << endl;

        double timeToLoopFrame = frameTimer.getElapsedTimeInMicroSec();
        frameTimer.sleepMicro(16666.666666667-timeToLoopFrame);
    }

    return 0;
}
