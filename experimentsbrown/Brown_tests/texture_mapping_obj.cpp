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
#include <math.h>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

#include "GLLight.h"
#include "Arcball.h"
#include "StimulusDrawer.h"
#include "CylinderPointsStimulus.h"
#include "BrownMotorFunctions.h"
#include "Optotrak2.h"
#include "Marker.h"

//#include "objl.h"
//objloaderass obj;

#include "glm.h"

#ifndef SIMULATION
    #include <direct.h> // mkdir
#endif

using namespace std;
using namespace Eigen;
using namespace BrownMotorFunctions;

Arcball arcball;
int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;

GLUquadricObj* myReusableQuadric;
double frame=0.0;

char*      model_file = NULL;   /* name of the obect file */
GLuint     model_list = 0;    /* display list for object */
GLMmodel*  model;     /* glm model data structure */
GLfloat    scale;     /* original scale factor */
GLfloat    smoothing_angle = 90.0;  /* smoothing angle */
GLboolean  facet_normal = GL_FALSE; /* draw with facet normal? */
GLuint     material_mode = 0;   /* 0=none, 1=color, 2=material */

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

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    }
}

GLuint load_obj(char* mdl_name, GLMmodel* mdl)
{
    /* read in the model */
    mdl = glmReadOBJ(mdl_name);
    //scale = glmUnitize(model);
    glmVertexNormals(mdl, smoothing_angle, GL_TRUE);

    if (mdl->nummaterials > 0)
      material_mode = 2;

/*
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat shininess = 65.0;
    */
    GLuint mode = 0;
/*
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    if (model_list)
  glDeleteLists(model_list, 1);

    /* generate a list */
    if (facet_normal)
  mode = GLM_FLAT;
    else
  mode = GLM_SMOOTH;
    if (material_mode == 1) {
  mode |= GLM_COLOR;
    } else if (material_mode == 2) {
  mode |= GLM_MATERIAL | GLM_TEXTURE;
    }
    //mdl_list = glmList(mdl, mode);

    return glmList(mdl, mode);
}

void draw_obj (GLuint obj)
{
	glCallList(obj);
}

void draw_parabolic_cylinder()
{
  glScaled(1.0,1.0,1.0);
  draw_obj(model_list);
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);
 /*
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
 */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

}

void initVariables()
{
	#ifdef SIMULATION
		chdir("../experimentsbrown/Brown_tests/objmodels");
	#else
		chdir("C:/workspace/cncsvisioncmake/experimentsbrown/Brown_tests/objmodels/");
	#endif

    model_list = load_obj("parabolic_cylinder_50_grid.obj", model);
    //model_list = obj.load("shell.obj");
}

void handleResize(int w, int h) {

    arcball.setWidthHeight(w, h);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
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

    glScaled(.1,.1,.1);
    draw_parabolic_cylinder();
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

    glutCreateWindow("Parabolic cylinder texture");
    initRendering();
    
    initVariables();

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

