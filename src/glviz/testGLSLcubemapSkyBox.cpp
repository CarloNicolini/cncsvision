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
#include "GLMaterial.h"
#include "GLLight.h"
#include "GLUtils.h"
#include "GLMaterial.h"
#include "Util.h"
#include "Imageloader.h"
#include "GLSL.h"
#include "Arcball.h"
#include "ObjLoader2.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;
static const double BOX_SIZE=2.0;

using namespace std;
int width=1280, height=1024;
double eyeZ=-5;
double eyeDistance=-20;
double FOV = 45;
double frame=0.0;

std::string imageNameFile;
Arcball arcball;
ObjLoader2 obj;

glsl::glShaderManager *SM;
glsl::glShader *shader;
GLuint cubeTexture;
GLfloat lightPos[3];

//Makes the image into a texture, and returns the id of the texture
void loadTexture(const char *filename, GLenum tex )
{
    Image *image = loadBMP(filename);
    glTexImage2D(tex,
                 0,
                 GL_RGB,                     image->width, image->height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    gluBuild2DMipmaps(tex, GL_RGB,  image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Set up texture maps
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGetError();
    delete image;
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {
    case 'q': //Escape key
        delete shader;
        exit(0);
    case 'a':
    {
        eyeZ+=0.1;
        break;
    }
    case 'z':
    {
        eyeZ-=0.1;
        break;
    }
        break;
    }
}

void initRendering()
{
    
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);
    glGetError();
    // Hyperbolic texturing correction
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
/*
    GLfloat diffuseColor[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat ambientLight[] = {1.0,1.0,1.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

    GLfloat specularcolor[] = {1.0,1.0,1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularcolor);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
*/
    // CubeMap rendering things
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

    loadTexture("../data/textures/cubemapping/cubemap_clouds/xneg.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("../data/textures/cubemapping/cubemap_clouds/xpos.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("../data/textures/cubemapping/cubemap_clouds/ypos.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
    loadTexture("../data/textures/cubemapping/cubemap_clouds/yneg.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT);
    loadTexture("../data/textures/cubemapping/cubemap_clouds/zneg.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
    loadTexture("../data/textures/cubemapping/cubemap_clouds/zpos.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);

    /*
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_positive_x.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_negative_x.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_positive_y.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT); //così è giusto!
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_negative_y.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT);
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_positive_z.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
    loadTexture("data/textures/cubemapping/cubemap_autumn/autumn_negative_z.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
*/
    /*
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_positive_x.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_negative_x.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_positive_y.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT); //così è giusto!
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_negative_y.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_positive_z.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
    loadTexture("data/textures/cubemapping/cubemap_brightday/brightday_negative_z.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
*/
    /*
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_positive_x.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_negative_x.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_positive_y.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT); //così è giusto!
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_negative_y.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_positive_z.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
    loadTexture("data/textures/cubemapping/cubemap_noise/noise_negative_z.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
*/
    /*
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_posx.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_negx.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_posy.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT); //così è giusto!
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_negy.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_posz.bmp",GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
    loadTexture("data/textures/cubemapping/cubemap_circles/circles_negz.bmp",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
*/

    glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f );
    glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION , 0.0f );
    glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION , 0.0002 );


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);


}

void handleResize(int w, int h)
{   arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

void drawThings()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeZ);
    arcball.applyRotationMatrix();

    shader->begin();
    shader->setUniform1f("time",frame);
    obj.draw(GL_FILL);
    //glutSolidTorus(0.5,1.0,50,50);
    //glutSolidTeapot(1);
    //drawCylinder(1,0,0,-0.5,0,0,0.5,64,0);
    shader->end();
    glPopMatrix();
    getGLerrors();

    // Draw the light
    glPushMatrix();
    arcball.applyRotationMatrix();
    glTranslated(lightPos[0],lightPos[1],lightPos[2]);
    glutWireSphere(1,10,10);
    glPopMatrix();
}

void drawScene()
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    frame+=0.01;
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();

    drawThings();
    glutSwapBuffers();
}
bool rightPressed=false;

void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    if ( button == GLUT_RIGHT_BUTTON )
        rightPressed=true;
    else
        rightPressed=false;

    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}
//Called every 15 milliseconds
void update(int value)
{
    lightPos[0] = 0.0f;
    lightPos[1] = 1000.0f;
    lightPos[2] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("GLSL cubemapskybox");
    initRendering();

    // Load (and compile, link) from file
    std::string shadervert("/home/carlo/workspace/cncsvisioncmake/data/glslshaders/VertexNoise.vert");
    std::string shaderfrag("/home/carlo/workspace/cncsvisioncmake/data/glslshaders/VertexNoise.frag");
    
    SM = new glsl::glShaderManager();
    shader = SM->loadfromFile((char*)shadervert.c_str(), (char*)shaderfrag.c_str());
    delete SM;
    obj.load("/home/carlo/workspace/cncsvisioncmake/data/objmodels/happyBuddha.obj");
    //obj.load("/home/carlo/Desktop/obj/head.OBJ","/home/carlo/Desktop/obj/");
    obj.normalizeToUnitBoundingBox();
    obj.initializeBuffers();

    
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}
