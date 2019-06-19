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
// Monitor tilt in mezzo
// Monitor zoom -23

#include <iostream>
#include <stdlib.h>
#include <stdio.h>


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#include <windows.h>
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "GLUtils.h"
#include "GLText.h"
#include "Util.h"
#include "Imageloader.h"
#include "GLSL.h"
#include "ObjLoader.h"

#include "Screen.h"
#include "Optotrak2.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "Marker.h"
#include "LatestCalibration.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 10
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
#define SCREEN_WIDE_SIZE 310.0    // millimeters
static const double focalDistance=-418.5;

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace Eigen;

using mathcommon::isVisible;
using util::stringify;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords, thumbCoords,indexCoords;
int headCalibrationDone=0,fingerCalibrationDone=0;
bool allVisibleHead,allVisibleFingers,allVisibleThumb,allVisibleIndex,allVisiblePlatform,infoDraw=true;

static const double focalPlane= -418.5;
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight;
vector <Marker> markers;
double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/** GRASPING TASK RELATED VARIABLES **/
RigidBody rigidStart,rigidCurrent;
// Object to display
ObjLoader model;
// Textures and shaders
glsl::glShaderManager SM;
glsl::glShader *shader;
bool attached=true;
bool showWire=true;

double frame=0.0;

/************ CUBEMAPPING VARIABLES ************/
GLuint cubeMapTexturesId[6];
GLuint cubeTexture;


/******* FUNCTIONS ***********************************/
void loadTexture(const char *filename, GLenum target )
{
    Image *image = loadBMP(filename);

	glTexImage2D(target,
                     0,
                     GL_RGB,                     image->width, image->height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     image->pixels);

        gluBuild2DMipmaps(target, GL_RGB,  image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Set up texture maps
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGetError();
    delete image;
}

void drawInfo()
{   if ( infoDraw )
    {   glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        GLText text;
        if ( gameMode )
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
        else
            text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
        text.enterTextInputMode();

        switch ( headCalibrationDone )
        {
        case 0:
        {   if ( allVisibleHead )
                text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
            else
                text.draw("Be visible with the head and glasses");
        }
        break;
        case 1:
        case 2:
        {   if ( allVisibleHead )
                text.draw("Cyclopean(x,y,z)= " + stringify<int>((eyeRight.x()+eyeLeft.x())/2)+", "+ stringify<int>((eyeRight.y()+eyeLeft.y())/2)+", "+ stringify<int>((eyeRight.z()+eyeLeft.z())/2)+", " );
            else
                text.draw("Be visible with the patch");
        }
        break;
        case 3:  // When the head calibration is done then calibrate the fingers
        {   switch ( fingerCalibrationDone )
            {
            case 0:
                text.draw("Press F to record platform markers");
                break;
            case 1:
                text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
                break;
            case 2:
            {
                text.draw("Move index and thumb to rigidbody tip to define starting position, then press F");
            }
            break;
            }
        }
        break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {
        }
        text.leaveTextInputMode();
    }
}
void drawSomething()
{
	Matrix4f M = headEyeCoords.getRigidStart().getFullTransformation().matrix().cast<float>();
	Vector3d eyeDir = headEyeCoords.getRigidStart().getFullTransformation()*Vector3d(0.0f,0.0f,-1.0f);
	Vector3f eyeDirf = eyeDir.cast<float>();

    // Draw the object
    glPushMatrix();
    
	if (attached)
	{
		glMultMatrixd( rigidStart.getFullTransformation().data());
	}
	else
	{
		glLoadIdentity();
		glTranslated(0,0,focalDistance);
	}

	Vector3d eyeDirection = headEyeCoords.getRigidStart().getFullTransformation().linear()*Vector3d(0,0,-1);
	shader->begin();
	shader->setUniform3f("eyeDirection",eyeDirection.x(),eyeDirection.y(),eyeDirection.z());
	if (!attached)
		shader->setUniform1f("Time",frame/100.0);

	glScaled(2.0,1.0,1.0);
	glutSolidSphere(30,50,50);
	shader->end();
    glPopMatrix();

	if ( !showWire )
		return;
	glPushMatrix();
	if (attached)
	{
		glMultMatrixd( rigidStart.getFullTransformation().data());
	}
	else
	{
		glLoadIdentity();
		glTranslated(0,0,focalDistance);
	}

	glScaled(2.1,1.1,1.1);
	glutWireSphere(30,20,20);

    glPopMatrix();
}

void cleanup()
{
    delete shader;
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);

	/*
	GLfloat lightPos[] = {0.0f, 5.0f, 0.0f};
    GLfloat diffuseColor[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat ambientLight[] = {1.0,1.0,1.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    GLfloat specularcolor[] = {1.0,1.0,1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularcolor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	*/
	// CubeMap rendering things
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

	loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_positive_x.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_negative_x.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_positive_y.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
    loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_negative_y.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT);
    loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_positive_z.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
    loadTexture("../../data/textures/cubemapping/cubemap_brightday/brightday_negative_z.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    if (stereo)
    {
        glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeRight);
        drawInfo();
        drawSomething();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeLeft);
        drawInfo();
        drawSomething();
        glutSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawInfo();
        drawSomething();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {   cleanup();
        exit(0);
    }
    break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=1;
            Beep(440,1000);
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1 )
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=2;
            Beep(440,1000);
            break;
        }
        if ( headCalibrationDone==2 )
        {   headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 )
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            Beep(220,1000);
            Vector3d centroid = ( markers[11].p+markers[12].p+markers[13].p )/3;
            rigidStart.setRigidBody(markers[11].p-centroid,markers[12].p-centroid,markers[13].p-centroid);
            infoDraw=false;
            headCalibrationDone=3;
        }
    }
    break;
	case '1':
	{
		attached=!attached;
	}
	break;
	case 'w':
	{
		showWire=!showWire;
	}
	break;
	}
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

/*
void recordHeadEyeRelativePositions()
{  bool allVisibleHead=false;
bool allVisibleFinger=false;
Vector3d centroid;
for (int i=0; i<10; i++)
{  optotrak.updateMarkers();
markers=optotrak.getAllMarkers();
allVisibleHead= isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
centroid = ( markers[11].p+markers[12].p+markers[13].p )/3;
allVisibleFinger= isVisible(markers[11].p) && isVisible(markers[12].p) && isVisible(markers[13].p) ;

cerr << "Move the head and the finger such that all markers are visible: Press Enter to continue " << i << endl;
cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
if ( allVisibleHead && allVisibleFinger )

{  headEyeCoords.init(markers[17].p,markers[18].p,
markers[1].p,markers[2].p,markers[3].p,interoculardistance);
rigidStart.setRigidBody(markers[11].p-centroid,markers[12].p-centroid,markers[13].p-centroid);

break;
}
}
}
*/

void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();

    rigidCurrent.setRigidBody(markers[11].p, markers[12].p,markers[13].p);
    rigidStart.computeTransformation(rigidCurrent);
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisibleHead = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisibleHead && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,1E-2);
    // update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,1E-2);
    // update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,1E-2);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;
	
	frame+=1.0;
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void initOptotrak()
{   optotrak.setTranslation(calibration);
    if ( optotrak.init("../../src/optotrak/cameraFiles/Aligned20110823") != 0)
    {   cleanup();
        exit(0);
    }

    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}


void loadGLSLshaders()
{
    shader = SM.loadfromFile((char*)"../../data/glslshaders/CubeMapOptotrakRotation.vert", (char*)"../../data/glslshaders/CubeMapOptotrakRotation.frag");
}

int main(int argc, char*argv[])
{
    initOptotrak();
    randomizeStart();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP KATJA GRASPING");
        glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    loadGLSLshaders();
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);
    glutMainLoop();

    cleanup();

    return 0;
}
