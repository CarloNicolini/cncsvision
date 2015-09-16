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

#include <cstdlib>
#include <iostream>
#include <vector>
#include "IncludeGL.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "Util.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;

// Alignment between optotrak z axis and screen z axis
double temporaryAlignmentX =  0.0;
double temporaryAlignmentY =  0.0;

double focalDistance=-418.5;
static const Vector3d center(0,0,focalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, translationFactor;
vector <Marker> markers(20);
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisiblePatch=false;
bool visibleInfo=true;
/********* TRIAL VARIABLES *********/
static const int CIRCLEMODE=0;
static const int CUBEMODE=1;
int trialMode = CIRCLEMODE;
double dT=0.0;
Timer frameTime;
/***** SOUND THINGS *****/
void beepOk()
{
#ifdef WIN32
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
#endif
    return;
}
void drawCircle(double radius, double x, double y, double z)
{
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
}

/*************************** FUNCTIONS ***********************************/

void drawInfo()
{
	if ( visibleInfo )
	{
	GLText text;	
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
	text.enterTextInputMode();
	text.draw("Press SPACEBAR to calibrate the eyes");
	text.draw("Press '+' to change mode CUBE <--> CIRCLE");
	text.draw("Circles from r=20 to r=70 mm");
	text.draw("Press 2,8,4,6 to move OpenGL origin w.r.t Optotrak");
	text.draw("TRANSLATION CALIBRATION "+ stringify< Eigen::Matrix<double,1,3> > (calibration.transpose())+ " [mm]" );
	text.draw("HeadCalibration= " + stringify<int>(headCalibrationDone) );
	if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
	glColor3fv(glWhite);

	if ( isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(5)+stringify< Eigen::Matrix<double,1,3> > (markers[5].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(6)+stringify< Eigen::Matrix<double,1,3> > (markers[6].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(7)+stringify< Eigen::Matrix<double,1,3> > (markers[7].p.transpose())+ " [mm]" );
	glColor3fv(glWhite);
	Vector3d cyclopeanEye = 0.5*(eyeLeft+eyeRight);
	text.draw("EyeRight= "+stringify< Eigen::Matrix<double,1,3> > (eyeRight.transpose())+ " [mm]" );
	text.draw("EyeLeft= "+stringify< Eigen::Matrix<double,1,3> > (eyeLeft.transpose())+ " [mm]" );
	text.draw("Cyclopean= "+stringify< Eigen::Matrix<double,1,3> > (cyclopeanEye.transpose())+ " [mm]" );
	text.draw("Current alignment(X,Y)= " +stringify<double>(temporaryAlignmentX)+","+stringify<double>(temporaryAlignmentY));
	text.draw("IOD = " +stringify<double>((eyeLeft-eyeRight).norm()));
	text.draw("Plane Normal from markers 5,6,7");
	Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( markers[5].p, markers[6].p,markers[7].p);
	Vector3d normal = focalPlane.normal().normalized();
	text.draw("NORMAL="+ stringify< Eigen::Matrix<float,1,3> > (normal.transpose().cast<float>())+ " [mm]" );
	if ( isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("G1="+stringify<double>(toDegrees(atan2(normal.x(),normal.y())))+"[deg]");
	text.draw("G2="+stringify<double>(toDegrees(atan2(normal.y(),normal.z())))+"[deg]");
	text.draw("G3="+stringify<double>(toDegrees(atan2(normal.z(),normal.x())))+"[deg]");
	text.draw("Put the marker 15 in the center and check its position");
	text.draw("Marker 15 (" + stringify<double>(markers[15].p.x()) + ", " + stringify<double>(markers[15].p.y()) + ", " + stringify<double>(markers[15].p.z()));	 
	text.draw("FrameRate= " + stringify<double>(dT) );
	text.leaveTextInputMode();
	}
}

void drawCalibration()
{
	glColor3fv(glWhite);
	drawCircle(2,0,0,focalDistance);	//center
	drawCircle(2,50,0,focalDistance);
	drawCircle(2,-50,0,focalDistance);
	drawCircle(2,0,50,focalDistance);
	drawCircle(2,0,-50,focalDistance);
	for (double r=20; r<=70; r+=10)
		drawCircle(r,0,0,focalDistance);
	drawCircle(0.2,0,0,focalDistance);

	glColor3fv(glRed);
	// draw a circle 5 cm radius
	drawCircle(50,0,0,focalDistance);
	glPointSize(3);
	glBegin(GL_POINTS);
	glVertex3dv(markers[15].p.data());
	glVertex3dv(markers[11].p.data());
	glVertex3dv(markers[12].p.data());
	glVertex3dv(markers[13].p.data());
	glVertex3dv(markers[7].p.data());
	glVertex3dv(markers[8].p.data());
	glVertex3dv(markers[9].p.data());
	glEnd();
	glColor3fv(glWhite);

}	

void drawCubes()
{
	glColor3fv(glRed);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,focalDistance);
	glutWireCube(50);
	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	glTranslated(markers[15].p.x(),markers[15].p.y(),markers[15].p.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
}

void drawTrial()
{

switch (trialMode)
{
case CIRCLEMODE:
	drawCalibration();
break;
case CUBEMODE:
	drawCubes();
break;
}

}

void drawGLScene()
{
    if (stereo)
    {   glDrawBuffer(GL_BACK);

        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawTrial();
		drawInfo();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawTrial();
		drawInfo();

        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawTrial();
		drawInfo();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
	case 'i':
		visibleInfo=!visibleInfo;
		break;
	case '+':
	{
		trialMode++;
		trialMode=trialMode%2;
	}
	break;
	case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisiblePatch )
        {
            headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=1;
			beepOk();
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=2;
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=3;
        }
    }
    break;
      case '2':
      {  
		  temporaryAlignmentY-=0.25;
		  screen.translate(0,-0.25,0);
         cam.updateMonitorPoint(screen);
	  }
      break;
      case '8':
      {
		  temporaryAlignmentY+=0.25;
		  screen.translate(0,0.25,0);
         cam.updateMonitorPoint(screen);
      }
      break;
      case '4':
      {  
		  temporaryAlignmentX-=0.25;
		screen.translate(-0.25,0,0);
         cam.updateMonitorPoint(screen);
      }
      break;
      case '6':
      {  
		  temporaryAlignmentX+=0.25;
		screen.translate(0.25,0,0);
         cam.updateMonitorPoint(screen);
      }
      break;
    case 'p':
	{
		screen.print(cout);
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

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(temporaryAlignmentX,temporaryAlignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void idle();

void update(int value)
{
frameTime.start();
	idle();
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
	dT = frameTime.getElapsedTimeInMilliSec();
}

void idle()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    // Coordinates picker
	allVisiblePatch = isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p);
    headEyeCoords.update(markers[1],markers[2],markers[3]);
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();
}

void initOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {
        exit(0);
    }
	for ( int i=0; i<10; i++)
	{
		optotrak.updateMarkers();
		markers = optotrak.getAllMarkers();
	}
}

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glLineWidth(1.5);
}

int main(int argc, char*argv[])
{
	cerr << "Insert interocular distance (default 65)" << endl;
    interoculardistance=65;
    //cin >> interoculardistance ;

// Initializes the optotrak and starts the collection of points in background
	initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("FULL CALIBRATOR MOTORS");
//glutFullScreen();
    }
    else
	{
		glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    //glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
