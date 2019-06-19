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

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <string>
#include <Eigen/Core>


#include "LatestCalibration.h"
#include "IncludeGL.h"
#include "Util.h"
#include "RoveretoMotorFunctions.h"
#include "GLUtils.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "GLText.h"
#include "Marker.h"
#include "VmxRotationMotor.h"


/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace util;
using namespace Eigen;

void idle();

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;
ObjLoader obj;
VmxRotationMotor rotor;

double focalDistance= -568.5;
double tmpAlignmentX=0.0;
double tmpAlignmentY=0.0;
/********** EYES AND MARKERS **********************/
vector <Marker> markers;
double interoculardistance=65;
/********* VISUALIZATION VARIABLES *****************/
bool gameMode=true;
bool stereo=false;
bool tetheredMode=false;
bool allVisibleHead=false;
bool allVisiblePatch=false;
static const int StillBuddha=0,RotatingBuddha=1,ExpandingBuddha=2,MovingBuddha=3;
int buddhaMode=StillBuddha;
int headCalibrationDone=0;
bool targetInside=false;
bool infoDraw=true;
Affine3d m1,m2;
double frame=0.0;

/******* FUNCTIONS ***********************************/
void drawCircle(double radius, double x, double y, double z, const GLfloat *color)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor3fv(color);
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glPopAttrib();
}

void drawFixation()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    double circleRadius = 25;	// millimeters
    double zBoundary    = 250;	// millimeters
    // Projection of view normal on the focal plane
    Vector3d directionOfSight = (headEyeCoords.getRigidStart().getFullTransformation().linear()*Vector3d(0,0,-1)).normalized();
    Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( headEyeCoords.getRightEye() , headEyeCoords.getRightEye()+directionOfSight );
    Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );
    double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);
    Vector3d opticalAxisToFocalPlaneIntersection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (headEyeCoords.getRightEye());

    switch ( headCalibrationDone )
    {
    case 0:
    {
        // STIM_FIXED stimulus at (0,0,focalDistance)
        glPushAttrib(GL_POINT_BIT);
        glColor3fv(glRed);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glVertex3d(headEyeCoords.getRightEye().x(),headEyeCoords.getRightEye().y(),focalDistance);
        glEnd();
        glPopAttrib();
        break;
    }
    case 1:
    case 2:
    {
        // STIM_FIXED stimulus + projected points
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glPointSize(5);
        glLineWidth(2);

        glBegin(GL_POINTS);
        glColor3fv(glRed);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glBlue);
        glVertex3dv(opticalAxisToFocalPlaneIntersection.data());
        glColor3fv(glWhite);
        glVertex3d(headEyeCoords.getRightEye().x(),headEyeCoords.getRightEye().y(),focalDistance);
        glEnd();

        double r2EyeRight = pow(headEyeCoords.getRightEye().x(),2)+pow(headEyeCoords.getRightEye().y(),2);
        // Draw the calibration circle
        switch (headCalibrationDone)
        {
        case 1: // just align the parallel projection
        {
            if ( abs(headEyeCoords.getRightEye().z()) < zBoundary && r2EyeRight<circleRadius*circleRadius )
            {   drawCircle(circleRadius,0,0,focalDistance,glGreen);
                targetInside=true;
            }
            else
            {   drawCircle(circleRadius,0,0,focalDistance,glRed);
                targetInside=false;
            }
            break;
        }
        case 2: // must align both the tethered point and the parallel projection and the z
        {
            if ( pow(opticalAxisToFocalPlaneIntersection.x(),2)+pow(opticalAxisToFocalPlaneIntersection.y(),2) <= circleRadius*circleRadius && abs(headEyeCoords.getRightEye().z()) < zBoundary && r2EyeRight<circleRadius*circleRadius )
            {
                drawCircle(circleRadius,0,0,focalDistance,glGreen);
                targetInside=true;
            }
            else
            {
                drawCircle(circleRadius,0,0,focalDistance,glRed);
                targetInside=false;
            }
            break;
        }
        }

        glPopAttrib();
        break;
    }
    }
    glPopAttrib();
}

void drawSomething()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POINT_BIT);
    glPointSize(8);
	glColor3fv(glWhite);
    glBegin(GL_POINTS);
    glVertex3dv(markers.at(24).p.data());
    glEnd();
    glPointSize(3);
	glColor3fv(glBlack);
    glBegin(GL_POINTS);
    glVertex3dv(markers.at(24).p.data());
    glEnd();
	glPopAttrib();
    
    switch (buddhaMode)
    {
    case StillBuddha:
    {
        BLUEREDLIGHTS
    	if (!tetheredMode)
    	{
    		glPushMatrix();
    		glMatrixMode(GL_MODELVIEW);
    		glLoadIdentity();
    		glTranslatef(0,0,focalDistance);
    		glRotated(-90,0,0,1);
    		glRotated(15,1,0,0);
    		glScalef(60,60,60);
    		obj.draw();
    		glPopMatrix();

    		glPushMatrix();
    		glMatrixMode(GL_MODELVIEW);
    		glLoadIdentity();
    		glTranslatef(50,50,focalDistance+150);
    		glRotated(-90,0,0,1);
    		glRotated(15,1,0,0);
    		glScalef(60,60,60);
    		obj.draw();
    		glPopMatrix();

    		glPushMatrix();
    		glMatrixMode(GL_MODELVIEW);
    		glLoadIdentity();
    		glTranslatef(-60,-20,focalDistance-150);
    		glRotated(-90,0,0,1);
    		glRotated(15,1,0,0);
    		glScalef(60,60,60);
    		obj.draw();
    		glPopMatrix();

    	}
    	else
    	{
    		glPushMatrix();
    		glMatrixMode(GL_MODELVIEW);
    		glLoadMatrixd(headEyeCoords.getRigidStart().getFullTransformation().data());
    		glTranslatef(0,0,focalDistance);
    		glRotated(-90,0,0,1);
    		glRotated(15,1,0,0);
    		glScalef(160,160,160);
    		obj.draw();
    		glPopMatrix();
    	}
    }
    break;
    case RotatingBuddha:
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance);
        glRotated(-90,0,0,1);
        glRotated(15,1,0,0);
        glScalef(80,80,80);
        obj.draw();
        glPopMatrix();
    }
    break;
    case ExpandingBuddha:
    {
        double scaleFactor=1+0.2*(sin(frame*10));
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance);
        glRotated(-90,0,0,1);
        glRotated(15,1,0,0);
        glScalef(80,80,80);
        glScalef(scaleFactor,scaleFactor,scaleFactor);
        obj.draw();
        glPopMatrix();
    }
    break;
    case MovingBuddha:
    {
        double movefactor=sin(10*frame);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,focalDistance+30*movefactor);
        glRotated(-90,0,0,1);
        glRotated(15,1,0,0);
        glScalef(80,80,80);
        obj.draw();
        glPopMatrix();
    }
    }
    
}


void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //BLUEREDLIGHTS
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawInfo()
{
    if (infoDraw==false)
        return;
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();
    text.draw("Marker 24="+util::stringify< Eigen::Matrix<double,1,3> >(markers.at(24).p.transpose()) );
	text.draw("(Ax,Ay)=" + util::stringify<double>(tmpAlignmentX)+","+util::stringify<double>(tmpAlignmentY) );
    switch ( headCalibrationDone )
    {
    case 0:
    {
        if ( allVisibleHead )
            text.draw("PRESS SPACEBAR TO CALIBRATE");
        else
            text.draw("BE VISIBLE...");
        break;
    }
    case 1:
    {
        break;
    }
    }
    text.draw("HeadCalibration="+util::stringify<int>(headCalibrationDone));
    text.draw("RightEye="+util::stringify< Eigen::Matrix<int,1,3> >(headEyeCoords.getRightEye().transpose().cast<int>()) );
    text.leaveTextInputMode();
}

void drawGLScene()
{
    if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(headEyeCoords.getRightEye());
        drawInfo();
        drawFixation();
        if ( headCalibrationDone==3 )
            drawSomething();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(headEyeCoords.getLeftEye());
        drawInfo();
        drawFixation();
        if (headCalibrationDone==3)
            drawSomething();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(headEyeCoords.getRightEye());
        drawInfo();
        drawFixation();
        if ( headCalibrationDone==3 )
            drawSomething();
        glutSwapBuffers();
    }
}

void moveRod(const Vector3d& v, int speed)
{
    // must be called 10 times!!!
    for ( int i=0; i<10; i++)
        idle();

    if ( !isVisible(v) )
    {   cerr << "CAN'T MOVE MOTOR TO INFINITY!!!" << endl;
        return ;
    }
    // a precise feedback based misuration - 2 corrections
    for ( int i=0; i<2; i++)
    {
        for ( int j=0; j<10; j++) // must be called 10 times!!!
            idle();
        RoveretoMotorFunctions::moveObject(v-markers.at(24).p,speed);
    }
}

void rotationfunction()
{
	static int a=0;
    rotor.rotate((a*2-1)*180.0,6000); //corrisponde a T=6s
	a++;
	a=a%2;
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case 'm':
    {
        moveRod(Vector3d(0,0,focalDistance),4000);
        break;
    }
    case 'r':
    {
        boost::thread rotationfunctionthread( rotationfunction );
        break;
    }
    case 'i':
    case 'I':
    {
        infoDraw=!infoDraw;
        break;
    }
    case ' ':
    {
        if ( allVisibleHead && allVisiblePatch && headCalibrationDone==0)
        {
            headEyeCoords.init(markers.at(19).p,markers.at(20).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance);
            headCalibrationDone=1;
            //Beep(440,440);
            break;
        }
        if ( allVisiblePatch && headCalibrationDone==1 && targetInside )
        {
            m1 = headEyeCoords.getRigidStart().getFullTransformation();
            headEyeCoords.init(m1*markers.at(19).p,m1*markers.at(20).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance);
            headCalibrationDone=2;
            //Beep(440,440);
            break;
        }
        if ( allVisiblePatch && headCalibrationDone==2 && targetInside )
        {
            m2 = m1*headEyeCoords.getRigidStart().getFullTransformation();
            headEyeCoords.init(m2*markers.at(19).p,m2*markers.at(20).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance);
            headCalibrationDone=3;
            //Beep(440,440);
            break;
        }
        break;
    }
    case 'b':
    {
        buddhaMode++;
        buddhaMode%=4;
    }
    break;
    case 'p':
    {
        tetheredMode=!tetheredMode;
        break;
    }
    case '4':
    {
		tmpAlignmentX+=0.25;
        break;
    }
    case '6':
    {
        tmpAlignmentX-=0.25;
		break;
    }
    case '2':
    {
		tmpAlignmentY+=0.25;
        break;
    }
    case '8':
    {
		tmpAlignmentY-=0.25;
        break;
    }

    }
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

Screen getProjectionScreen(double width, double height,double tmpAlignmentX, double tmpAlignmentY, double focalDistance, const Eigen::Affine3d &HeadTransformation)
{
    Screen screen;
    screen.init(width,height, 0,0, focalDistance );
    screen.setOffset(tmpAlignmentX,tmpAlignmentY);
    screen.setFocalDistance(focalDistance);
    screen.transform(HeadTransformation);
    return screen;
}

void update(int value)
{
    markers =optotrak.getAllMarkers();
    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p);

    allVisiblePatch = isVisible(markers.at(1).p) &&
                      isVisible(markers.at(2).p) &&
                      isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(19).p) && isVisible(markers.at(20).p);

    //if ( tetheredMode )
    //    cam.init( getProjectionScreen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,tmpAlignmentX,tmpAlignmentY,focalDistance,headEyeCoords.getRigidStart().getFullTransformation() ));
    //else
    cam.init(getProjectionScreen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,tmpAlignmentX,tmpAlignmentY,focalDistance,Eigen::Affine3d::Identity() ));
    frame+=1.0/(10*TIMER_MS);

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

/*
void recordHeadEyeRelativePositions()
{   bool allVisible=false;
    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers=optotrak.getAllMarkers();
        allVisible= isVisible(markers.at(23).p) && isVisible(markers.at(24).p) && isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p) ;
        cerr << "Move the head such that all markers are visible, press 'Enter' to confirm: Trial num " << i << endl;
        cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        if ( allVisible )
        {   headEyeCoords.init(markers.at(23).p,markers.at(24).p,
                               markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance);
            break;
        }
    }
}
*/
void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    markers.at(23).p.y() += 45.0;
    markers.at(24).p.y() += 45.0;
}


void initOptotrak()
{   optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile,24) != 0)
    {   exit(0);
    }

    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}


int main(int argc, char*argv[])
{
    //RoveretoMotorFunctions::homeScreen(3500);
    //RoveretoMotorFunctions::moveScreenAbsolute(focalDistance,-418.5,3500);
    cerr << "Insert interocular distance and then press enter" << endl;
    cin >> interoculardistance;
    cerr << "IOD set to " << interoculardistance << endl;

    initOptotrak();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode )
    {   glutGameModeString(ROVERETO_GAME_MODE_STRING);
        if ( glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
            glutEnterGameMode();
        else
        {   cerr << "This resolution is not available\nPress ENTER to continue" << flush;
            cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
            return -1;
        }
    }
    else
    {
        glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        glutCreateWindow("CNCSVISION Example 2 HappyBuddha");
        glutFullScreen();
    }
    initRendering();

    // Load (and compile, link) from file
    obj.load("C:/workspace/cncsvisioncmake/data/objmodels/iit3Dscaled.obj");
    //obj.getInfo();
    //obj.normalizeToUnitBoundingBox();
    //glewInit();
    //obj.initializeBuffers();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
