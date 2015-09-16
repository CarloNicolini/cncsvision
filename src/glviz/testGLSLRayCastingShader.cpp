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
#include <map>
#include <Eigen/Core>

#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "GLSL.h"
#include "Arcball.h"
#include "GLText.h"
#include "Circle3D.h"
#include "VolumetricMeshIntersection.h"

#include "VRCamera.h"
#include "Screen.h"

using namespace std;
using namespace Eigen;
int width=800, height=800;
double eyeDistance=-5;
double FOV = 45;
double frame=0.0;
std::string imageNameFile;
Arcball arcball;
GLuint texName;

int textureSizeX=512;
int textureSizeY=512;
int textureSizeZ=512;

GLuint volume3DTextureID;
glsl::glShader *shader;

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
Eigen::Vector3d offset(0.0,0.0,0.0);
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case '+':
    {
        eyeDistance+=0.1;
        break;
    }
    case '-':
    {
        eyeDistance-=0.1;
        break;
    }
    }
}

void initRendering()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

    int nspheres=10;
    int minRadius=50;
    int maxRadius=120;
    glewInit();
    getGLerrors();
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.01, 1000.0);
}

void initialize3DVolumetric()
{
    glGenTextures(1, &(volume3DTextureID));

    glBindTexture(GL_TEXTURE_3D, volume3DTextureID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    // Important to enable bilinear filtering and smoothing
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    std::vector<GLubyte> texture3DVolume;
    texture3DVolume.resize(textureSizeX*textureSizeY*textureSizeZ);
    memset(&(texture3DVolume.at(0)),0,sizeof(GLubyte)*textureSizeX*textureSizeY*textureSizeZ);
    int r = 32;

    std::map<int,Circle3D<int> > spheres;
    for (int i=0; i<20;i++)
    {
        spheres[i]=Circle3D<int>( rand()%(textureSizeX-r)+r,rand()%(textureSizeX-r)+r,rand()%(textureSizeX-r)+r,r);
        //spheres[i]=Circle3D<int>( textureSizeX/2,textureSizeY/2,textureSizeZ/2,r);
        int radius= spheres[i].radius;
        int centerx = spheres[i].centerx;
        int centery = spheres[i].centery;
        int centerz = spheres[i].centerz;

        int cxmin=centerx-radius;
        int cxmax=centerx+radius;
        int cymin=centery-radius;
        int cymax=centery+radius;
        int czmin = centerz-radius;
        int czmax = centerz+radius;

        int radius2 = SQR(radius);
        for (int z=czmin; z<czmax;++z)
        {
            int z2 = SQR((z-centerz));
            int tYtXx  = textureSizeY* textureSizeX* z;
            for ( int y=cymin; y<cymax;++y)
            {
                int z2y2=z2+SQR(y-centery);
                int tXY = textureSizeX* y;
                for ( int x= cxmin; x<cxmax; ++x)
                {
                    if ( z2y2+SQR(x-centerx) < radius2)
                        texture3DVolume[tYtXx+tXY + x] = 255;
                }
            }
        }
    }
    /*
    for (int z=textureSizeZ/2-r; z<textureSizeZ/2+r;++z)
    {
        int tYtXx  = textureSizeY* textureSizeX* z;
        for ( int y=textureSizeY/2-r; y<textureSizeY/2+r;++y)
        {
            int tXY = textureSizeX* y;
            for ( int x= textureSizeX/2-r; x<textureSizeX/2+r; ++x)
            {
                texture3DVolume[tYtXx+tXY + x] = 255;
            }
        }
    }
*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, textureSizeX, textureSizeY, textureSizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,(GLvoid *) &(texture3DVolume.at(0)));

    glsl::glShaderManager SM;
    shader = SM.loadfromFile("/home/carlo/workspace/cncsvisioncmake/data/glslshaders/TestRayCast.vert","/home/carlo/workspace/cncsvisioncmake/data/glslshaders/TestRayCast.frag");
}

/**
 * @brief drawVolume
 */
void drawVolume()
{
    static double time=0.0;
    time+=1.0/60.0;
    glEnable(GL_TEXTURE_3D);
    shader->begin();
    shader->setUniform1f("time",time);
    glutSolidCube(1.0);
    shader->end();
    glDisable(GL_TEXTURE_3D);
}

/**
 * @brief drawThings
 */
void drawThings()
{
    GLText text(width,height,glWhite);
    text.init(width,height,glWhite);
    text.enterTextInputMode();

    text.leaveTextInputMode();

    drawVolume();

    getGLerrors();
}

/**
 * @brief drawScene
 */
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    frame+=0.01;
    glTranslatef(0.0f, 0.0f, eyeDistance);
    arcball.applyRotationMatrix();
    drawThings();
    glutSwapBuffers();
}


void mouseFunc(int state, int button, int _x , int _y)
{  if ( button == GLUT_LEFT_BUTTON )
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


//Called every 15 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

int main(int argc, char* argv[])
{
    if (argc<1)
    {
        cerr << "Usage: ./testGLSL2 shadername texture.bmp" << endl;
        cerr << "will load shadername.vert and shadername.frag" << endl;
        exit(0);
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE );
    glutInitWindowSize(width, height);

    glutCreateWindow("test GLSL Volumetric raycasting");
    initRendering();
    initialize3DVolumetric();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

