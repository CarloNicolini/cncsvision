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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <math.h>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
//#include "Optotrak.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "ParametersLoader.h"

#include "Util.h"
#include "DaisyFilter.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 10
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
//using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor2 headEyeCoords, fingerCoords;


/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
double focalDistance= -568.5;
static const Vector3d center(0,0,focalDistance);
Vector3d platformThumb;
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, translationFactor(0,0,0);
vector <Marker> markers;
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* RIGID BODIES VARIABLES *********/
int calibrationDone=0;
bool orthographicMode=false;
bool infodrawn=true;
/** STREAMS **/
ofstream timeFile;
Timer globalTime;
double frameTime[2];
void drawCircle(double radius, double x, double y, double z)
{
glBegin(GL_LINE_LOOP);
double deltatheta=toRadians(5);
for (double i=0; i<2*M_PI; i+=deltatheta)
	glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
glEnd();
}

/*************************** FUNCTIONS ***********************************/
void cleanup()
{
// Stop the optotrak
optotrak->stopCollection();
delete optotrak;
}

void drawInfo()
{
	if ( infodrawn )
	{
	GLText text;
	if ( gameMode )
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
	else
		text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
	text.enterTextInputMode();
	string marker1=stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose());
	text.draw("Eye Right");
	text.draw( stringify< Eigen::Matrix<double,1,3> > (eyeRight.transpose())+ " [mm]" );
	text.draw("Eye Left");
	text.draw( stringify< Eigen::Matrix<double,1,3> > (eyeLeft.transpose())+ " [mm]" );
	text.draw("Marker 1 vel");
	text.draw(stringify< Eigen::Matrix<int,1,3> > (markers[1].v.transpose().cast<int>())+ " [mm/s]" );
	text.draw("Marker 2 vel");
	text.draw(stringify< Eigen::Matrix<int,1,3> > (markers[2].v.transpose().cast<int>())+ " [mm/s]" );
	text.draw("Marker 3 vel");
	text.draw(stringify< Eigen::Matrix<int,1,3> > (markers[3].v.transpose().cast<int>())+ " [mm/s]" );
	text.draw("P2 velocity");
	Vector3i P2vel = headEyeCoords.getP2().v.cast<int>();
	text.draw(stringify< Eigen::Matrix<int,1,3> > (P2vel));
	
	text.draw("Thumb velocity");
	Vector3i x = fingerCoords.getP1().v.cast<int>();
	text.draw(stringify< Eigen::Matrix<int,1,3> > (x));
	text.leaveTextInputMode();
	}
}

void drawCalibration()
{
        bool allVisibleHead=false;
        bool allVisibleFinger=false;
        GLText text;
	if ( gameMode )
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
	else
		text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
        
        switch ( calibrationDone )
	{
	case 0:
	{   
	// XXX mettere marker della piattaforma e markers della mano destra
	allVisibleFinger = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
		if ( allVisibleFinger )
		{	glClearColor(0.0,1.0,0.0,1.0); //green light
		        text.enterTextInputMode();
		        text.draw("==== OK ==== Press Spacebar to continue");
		        text.leaveTextInputMode();
		}
		else
		{
			glClearColor(1.0,0.0,0.0,1.0); //red light
			text.enterTextInputMode();
		        text.draw("Move fingers to the platform and be visible");
		        text.leaveTextInputMode();
	        }
	}
	break;
	case 1:
	{
	allVisibleHead = isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
		if ( allVisibleHead )
		{
			glClearColor(0.0,1.0,0.0,1.0); //green light
		        text.enterTextInputMode();
		        text.draw("==== OK ==== Press Spacebar to continue");
		        text.leaveTextInputMode();
		}
		else
		{
		        glClearColor(1.0,0.0,0.0,1.0); //red light
		        text.enterTextInputMode();
		        text.draw("Be visible with the head and glasses");
		        text.leaveTextInputMode();
	        }
	}
	break;
	}
}

void drawTrial()
{
	Vector3d v= fingerCoords.getP1().v;
	Vector3d f = fingerCoords.getP1().p;
	Vector3d fv= f+v;
	/*
	//glLineWidth(2);
	//glPointSize(3);
	// Questo pezzo di codice fa slittare tutto di un frame, qualche operazione costosa??? XXX
	glPushMatrix();
	glColor3fv(glRed);
	glBegin(GL_LINES);
	glVertex3d(f.x(),f.y(),f.z());
	glVertex3d(fv.x(),fv.y(),fv.z());
	glEnd();
	glBegin(GL_POINTS);
	glColor3fv(glWhite);
	glVertex3dv(fingerCoords.getP1().p.data());
	glVertex3dv( markers.at(11).p.data() );
	glVertex3dv( markers.at(12).p.data());
	glVertex3dv( markers.at(13).p.data()); 
	glEnd();
	glPopMatrix();
	*/
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

// LIGHTS
/*
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glEnable (GL_BLEND);
glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
glEnable(GL_LIGHT0);
glEnable(GL_LIGHTING);
*/
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
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

	// Draw right eye view
	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	cam.setEye(eyeLeft);
	drawTrial();

	glutSwapBuffers();
	}
	else
	{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.setEye(eyeRight);
	drawInfo();
	drawTrial();
	glutSwapBuffers();
	}
}


void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
{   //Quit program
case 'i':
	infodrawn=!infodrawn;
	break;
case 'o':
	orthographicMode=!orthographicMode;
	cam.setOrthoGraphicProjection(orthographicMode);
	break;
case 'q':
case 27:
	{   cleanup();
	exit(0);
	}
	break;
case ' ':
	{
		// Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
		headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
		//headEyeCoords.setFilterVelocity(true,0.8); se si filtra e per un frame i markers non son visibili allora tutti i frame dopo diventano invisibili perchè non riesce più a ritornare ai valori base
		calibrationDone=2;
		platformThumb=markers.at(15).p;
	}
	break;
	// Enter key: press to make the final calibration
case 13:
	{
		if ( calibrationDone == 2)
		{
			headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
			fingerCoords.init(platformThumb,markers.at(11).p, markers.at(12).p, markers.at(13).p);
			calibrationDone=3;
			infodrawn=false;
			Beep(440,880);
		}
	}
	break;
}
}

void handleResize(int w, int h)
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
if ( gameMode )
	glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
else
	glViewport(0,0,640, 480);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screen.setOffset(alignmentX,alignmentY);
	screen.setFocalDistance(_focalDist);
	screen.transform(_transformation);
	cam.init(screen);
}

/*
void update(int value)
{
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}
*/
void idle()
{
	Timer localTimer;
	localTimer.start();
    frameTime[1] = frameTime[0];
	frameTime[0] = globalTime.getElapsedTimeInMilliSec();
	double deltaT = abs( frameTime[0]-frameTime[1] );
    optotrak->updateMarkers();
	// Coordinates picker
	markers = optotrak->getAllMarkers();
	headEyeCoords.update(markers[1],markers[2],markers[3],deltaT);
	fingerCoords.update(markers.at(11).p, markers.at(12).p, markers.at(13).p,deltaT);
	eyeLeft = headEyeCoords.getLeftEye().p;
	eyeRight = headEyeCoords.getRightEye().p;
	if ( calibrationDone== 3 )
	{
	// colonne [1,9]
	for (int i=1; i<=3; i++) // i markers vanno da 1 a 3 compresi
		for (int j=0; j<3; j++)
			timeFile << fixed << markers[i].p[j]  << "\t" ;
	// colonne [10,12]
	for (int j=0; j<3; j++)
			timeFile << fixed << headEyeCoords.getLeftEye().p[j] << "\t" ;
	// colonne [13,15]
	for (int j=0; j<3; j++)
			timeFile << fixed << headEyeCoords.getRightEye().p[j] << "\t" ;
	// colonne [16,24]
	for (int i=1; i<=3; i++) // i markers vanno da 1 a 3 compresi
		for (int j=0; j<3; j++)
			timeFile << fixed << markers[i].v[j] << "\t" ;
	// colonne [25,27]
	for (int j=0; j<3; j++)
			timeFile << fixed << headEyeCoords.getLeftEye().v[j] << "\t" ;
	// colonne [28,30]
	for (int j=0; j<3; j++)
			timeFile << fixed << headEyeCoords.getRightEye().v[j] << "\t" ;
	// colonne [31,32]
	timeFile << deltaT << "\t" << globalTime.getElapsedTimeInMilliSec() << endl;
	}
	while ( localTimer.getElapsedTimeInMilliSec() < (double)TIMER_MS );
	glutPostRedisplay();
}

void initOptotrak()
{   
optotrak=new Optotrak2();
optotrak->setTranslation(calibration);
if ( optotrak->init("cameraFiles/Aligned20110823") != 0)
{   cleanup();
exit(0);
}
optotrak->updateMarkers();
markers = optotrak->getAllMarkers();
}

int main(int argc, char*argv[])
{

frameTime[0]=frameTime[1]=0.0;
timeFile.open("times.dat");

// Initializes the optotrak and starts the collection of points in background
initOptotrak();
glutInit(&argc, argv);
if (stereo)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
else
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

if (gameMode==false)
{   glutInitWindowSize( 640,480 );
glutCreateWindow("EXP WEXLER");
//glutFullScreen();
}
else
{   glutGameModeString("1024x768:32@100");
glutEnterGameMode();
glutFullScreen();
}
initRendering();
initProjectionScreen(focalDistance,Affine3d::Identity());

glutDisplayFunc(drawGLScene);
glutKeyboardFunc(handleKeypress);
glutReshapeFunc(handleResize);
glutIdleFunc(idle);
//glutTimerFunc(TIMER_MS, update, 0);
glutSetCursor(GLUT_CURSOR_NONE);
/* Application main loop */
globalTime.start();
glutMainLoop();

cleanup();

return 0;
}
