///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <noise/noise.h>
#include "noiseutils.h"

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
#include <MMSystem.h>
#endif

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "Util.h"

/********* NAMESPACE DIRECTIVES ************************/

using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace noise;

/////
#define checkImageWidth 512
#define checkImageHeight 512
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLuint texName;
///

double stimulus_height = 150.0;
double theta = 0.0, textdepth = 0.0, motion = 0.0, motion2 = stimulus_height;
double direction_motion = 1.0;
double motion_theta = 0.0;

#ifndef SIMULATION
	#include <direct.h>
#endif

#include "Optotrak2.h"
#include "Marker.h"
#include "BrownMotorFunctions.h"
using namespace BrownMotorFunctions;

/***** CALIBRATION THINGS *****/
#include "LatestCalibration.h"

static const Vector3d center(0,0,focalDistance);
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0;
Screen screen;
static const Vector3d centercal(29.75,-133.94,-296.16); //updated 9/25/14
/********* VISUALIZATION VARIABLES *****************/
#ifndef SIMULATION
	static const bool gameMode=true;
	static const bool stereo=true;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer;
BoxNoiseStimulus cylinder;
Timer timer;
Timer globalTimer;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, ind, thu, platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999);
vector <Marker> markers;
static double interoculardistance=0.0;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int fingerCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool expFinished=false;
bool expStarted=false;

/********* TRIAL VARIABLES *********/
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
double objwidth = 0.0, adjDz = 0.0;
bool isStimulusDrawn = false;
Vector3d object_reset_position(0,0,0), object_position(0,0,0);
double objdistance = 0.0, objlength = 0.0;
/********** STREAMS **************/
ofstream responseFile, markersFile;


/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void cleanup();
void drawInfo();
void drawStimulus();
void drawGLScene();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist, const Affine3d &_transformation=Affine3d::Identity(),bool synchronous=true);
void update(int value);
void idle();
void initMotors();
void initGLVariables();
void initVariables();
void initStreams();
void initOptotrak();
void initRendering();
void advanceTrial();
void initTrial();
void calibration_fingers(int phase);
void calibration_head(int phase);
void drawSquare();
void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres);
void buildComparison(double height, double fluffiness, double distance);
void buildStandard(double fluffiness, double distance, double delta);
void drawStandardComparison();
void prepareSecondHeight();
void moveAway();
void mouseFunc(int button, int state, int x, int y);
void makeCheckImage();
void drawTexture1();
void drawTexture2();
void drawBlackSquare();
void drawDots();
void drawContactPoints();

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
#ifndef SIMULATION
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/armlength/";
#else
string experiment_directory = "/media/shapelab/Domini-ShapeLab/carlo/2014-2015/texture/";
#endif

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parameters_texture.txt";

// response file name
string responseFile_name = "spring15-armlength-adj_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tobjwidth\tadjDepth\ttime";

/*************************** FUNCTIONS ***********************************/

/***** SOUNDS *****/
void beepOk(int tone)
{
#ifndef SIMULATION
	switch(tone)
	{
	case 0:
    // Remember to put double slash \\ to specify directories!!!
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	case 1:
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
	break;
	}
	return;
#endif
}

void updateTheMarkers()
{
	optotrak.updateMarkers();
	markers = optotrak.getAllMarkers();
}

void cleanup()
{
	// Stop the optotrak
	optotrak.stopCollection();
}

void calibration_fingers(int phase)
{
	switch (phase)
	{
	case 1:
		{
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
		}
		break;
	case 2:
		{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
		}
		break;
	}
}

void calibration_head(int phase)
{
	switch (phase)
	{
	case 1:
		{
			headEyeCoords.init(markers[3].p-Vector3d(70,0,0),markers[3].p, markers[10].p,markers[11].p,markers[12].p,interoculardistance );

		}
		break;
	case 2:
		{
			headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
		}
		break;
	}
}

void drawInfo()
{
	if ( visibleInfo )
	{
		GLText text;	
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text.enterTextInputMode();
		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
		text.draw("# IOD: " +stringify<double>(interoculardistance));
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
		text.draw("#######################");
		text.draw(" ");
		if ( isVisible(markers[3].p) )
			glColor3fv(glGreen);
		else
			glColor3fv(glRed);
		text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );

		text.leaveTextInputMode();
	}

	if ( expFinished )
	{
		GLText text2;	
		text2.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
		text2.enterTextInputMode();
		text2.draw("The experiment is finished.");
		text2.leaveTextInputMode();
	}

}

void drawStimulus()
{
	if ( isStimulusDrawn )
	{
		if(trial.getCurrent()["Texture"])
		{
			drawTexture2();
		} else {
			drawDots();
		}
		//drawContactPoints();

	}
}

GLfloat const glGray[3] = {.5,.5,.5};

void drawDots()
{
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);
	glRotatef(motion, 1, 0, 0);
	glRotatef(objwidth + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);

//	glTranslated(0, motion, 0);
	glPushMatrix();
	stimDrawer.draw();
	glPopMatrix();

/*	glLoadIdentity();
	glTranslated(0, motion2, trial.getCurrent()["AbsDepth"]);
	glRotatef(objwidth + theta, 0, 1, 0);
	glRotatef(motion, 0, 0, 1);
	glPushMatrix();
	stimDrawer.draw();
	glPopMatrix();
*/
	glLoadIdentity();

	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+50);

	glColor3fv(glBlack);
	glBegin(GL_QUADS);

	glVertex3f(-100.0f, -200.0f, 0.5f);		// Bottom Left
	glVertex3f( 100.0f, -200.0f, 0.5f);		// Bottom Right
	glVertex3f( 100.0f, -20.0f, 0.5f);		// Top Right
	glVertex3f(-100.0f, -20.0f, 0.5f);		// Top Left

	glVertex3f(-100.0f, 20.0f, 0.5f);		// Top Left
	glVertex3f( 100.0f, 20.0f, 0.5f);		// Top Right
	glVertex3f( 100.0f, 200.0f, 0.5f);		// Bottom Right
	glVertex3f(-100.0f, 200.0f, 0.5f);		// Bottom Left

	glEnd();
}

void drawContactPoints()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+0.5);
	glRotatef(objwidth + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);
//	glTranslated(objlength/2*cos((objwidth+theta)*M_PI/180.0), 0, trial.getCurrent()["AbsDepth"]);
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();


	glPushMatrix();
	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+0.5);
	glRotatef(objwidth + theta, 0, 1, 0);
	glTranslated(-objlength/2, 0, 0);
//	glTranslated(-objlength/2*cos((objwidth+theta)*M_PI/180.0), 0, trial.getCurrent()["AbsDepth"]);
	glColor3fv(glRed);
	glutSolidSphere(1,10,10);
	glPopMatrix();
}

void drawGLScene()
{
    if (stereo)
    {   
	// back on both windows (left and right)
	glDrawBuffer(GL_BACK);

	// now start switching between presentations:
	// Draw left eye view first
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawTexture2();
	drawInfo();

        // Draw right eye view afterwards
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawTexture2();
	drawInfo();

	// swap between buffers
        glutSwapBuffers();
    }
    else
    {   
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_MULTISAMPLE);
        glLoadIdentity();
        cam.setEye(eyeRight);
	drawStimulus();
	drawInfo();
        glutSwapBuffers();
    }
}

void drawBlackSquare()
{
	
}

void makeCheckImage()
{
	module::Voronoi myModule;
	myModule.SetSeed(unifRand(25.0, 75.0));
	myModule.SetDisplacement(1);
	myModule.EnableDistance(false);
	myModule.SetFrequency(1.0);

//	module::Checkerboard myModule;
//	module::Turbulence turbo;
//	turbo.SetSourceModule(0,myModule);
//	turbo.SetFrequency(.5);
// 	turbo.SetRoughness(1);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (checkImageWidth+2, checkImageHeight+2);
	heightMapBuilder.SetBounds (1.0, 10.0, 1.0, 10.0);
	heightMapBuilder.Build ();

	const float* pSource3 = heightMap.GetConstSlabPtr(1,1);
	cerr << *(pSource3+1) << endl;

	int i, j;
	float c;

	for (i = 1; i < checkImageHeight; i++) 
	{
		const float* pSource3 = heightMap.GetConstSlabPtr(i);
	
		for (j = 1; j < checkImageWidth; j++) 
		{
			if(*(pSource3+1) != *pSource3 || *(pSource3-1) != *pSource3)
				c = 255.0;
			else
				c = 0.0;
//			 c = (*pSource3 - 2.0);
//			 c = c/4.0 * 255.0;

			 checkImage[j][i][0] = (GLubyte) c;
			 checkImage[j][i][1] = (GLubyte) 0;
			 checkImage[j][i][2] = (GLubyte) 0;
			 checkImage[j][i][3] = (GLubyte) 255;

  			 pSource3++;
		}
	}
}

void drawTexture1()
{    
   glShadeModel(GL_SMOOTH);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                checkImage);
}

void drawTexture2()
{
	glLoadIdentity();
//	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);
//	glRotatef(objwidth + theta, 0, 1, 0);

	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]);
	glRotatef(motion, 1, 0, 0);
	glRotatef(objwidth + theta, 0, 1, 0);
	glTranslated(objlength/2, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);

	glBegin(GL_QUADS);
/*	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, -31.0+motion, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, -31.0+motion, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, 31.0+motion, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, 31.0+motion, 0.0);

	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, -31.0+motion2, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, -31.0+motion2, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, 31.0+motion2, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, 31.0+motion2, 0.0);
*/
	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, stimulus_height/2, 0.0);

	glTexCoord2f(1.0, 0.0); glVertex3f(-objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(objlength/2, -stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(objlength/2, stimulus_height/2, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-objlength/2, stimulus_height/2, 0.0);

	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();
	glTranslated(0, 0, trial.getCurrent()["AbsDepth"]+50);

	glColor3fv(glBlack);
	glBegin(GL_QUADS);

	glVertex3f(-100.0f, -100.0f, 0.1f);		// Top Left
	glVertex3f( 100.0f, -100.0f, 0.1f);		// Top Right
	glVertex3f( 100.0f, -30.0f, 0.1f);		// Bottom Right
	glVertex3f(-100.0f, -30.0f, 0.1f);		// Bottom Left

	glVertex3f(-100.0f, 30.0f, 0.1f);		// Top Left
	glVertex3f( 100.0f, 30.0f, 0.1f);		// Top Right
	glVertex3f( 100.0f, 100.0f, 0.1f);		// Bottom Right
	glVertex3f(-100.0f, 100.0f, 0.1f);		// Bottom Left

	glEnd();
}

void initTrial()
{
	drawTexture1();	
	motion = 0.0; 
	motion2 = stimulus_height; 

	// draw an empty screen
	isStimulusDrawn=false;

	// set the width of the object to be randomly between 30 and 70 mm
	objwidth = trial.getCurrent()["Theta"];

	objlength = trial.getCurrent()["RelDepthObj"];
	initGLVariables();

	// set the depth of the object to be randomly between 30 and 70 mm
	adjDz = unifRand(25.0, 75.0);

	// refresh the scene
	drawGLScene();

	objdistance = trial.getCurrent()["AbsDepth"];

	// set the monitor at the right ditance
	initProjectionScreen(objdistance);

	timer.start();

	// draw the stimulus		
	isStimulusDrawn=true;
}

void advanceTrial()
{
	beepOk(0);
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3); // max three decimal positions
	responseFile << fixed << 
			parameters.find("SubjectName") << "\t" <<
			interoculardistance << "\t" <<
			trialNumber << "\t" <<
			trial.getCurrent()["AbsDepth"] << "\t" << 
			objwidth << "\t" << 
			adjDz << "\t" << 
			timeElapsed
			<< endl;

	trialNumber++;
	if(trial.hasNext())
	{
		trial.next();
		initTrial();
	} else
	{	
		isStimulusDrawn=false;
		drawGLScene();
		responseFile.close();
		expFinished = true;
	}

}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   
	switch (key)
	{   
		case 'x': // test for final screen
			expFinished=true;
			break;

		case 'i': // show info
			visibleInfo=!visibleInfo;
			break;

		case 'm': // increase IOD
			interoculardistance += 0.5;
			break;

		case 'n': // decrease IOD
			interoculardistance -= 0.5;
			break;

		case '8': // increase object's depth
			textdepth += .25;
			break;

		case '5': // decrease object's depth
			textdepth -= .25;
			break;

		case '6': // increase object's depth
			theta -= 1.5;
			break;

		case '4': // decrease object's depth
			theta += 1.5;
			break;

		case '0': // go to the next trial
		{
			if(expStarted)
				advanceTrial();
		} break;

		case 27: // press escape to quit
		{   
			cleanup(); // clean the optotrak buffer
			exit(0);
		} break;

		case 13: // press enter to start
		{   
			// turn off info
			visibleInfo = false;

			// check where the object is
			object_reset_position = markers[3].p.transpose();
			// calculate where the object has to go
			Vector3d object_position(0.0,object_reset_position.y(),-330.0);
			// move the object to position from where it is
			moveObjectAbsolute(object_position, object_reset_position, 5000);

			// start the experiment
			expStarted = true;
			trial.next();
			initTrial();
		} break;
	}
}

// Resize window
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glEnable(GL_MULTISAMPLE);
    glLoadIdentity();
}

// move monitor to specific distance
void initProjectionScreen(double _focalDist, const Affine3d &_transformation, bool synchronous)
{
	focalDistance = _focalDist;	
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
	screen.setOffset(alignmentX,alignmentY);
	screen.setFocalDistance(_focalDist);
	screen.transform(_transformation);
	cam.init(screen);
	if ( synchronous )
		moveScreenAbsolute(_focalDist,homeFocalDistance,4500);
	else
		moveScreenAbsoluteAsynchronous(_focalDist,homeFocalDistance,4500);
}

// Handle refresh of the screen
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{

	updateTheMarkers();
	
	if(motion > motion_theta || motion < 0)
		direction_motion = direction_motion * -1;

	motion += trial.getCurrent()["Motion"]*direction_motion;

/*	if(motion2 > -stimulus_height)
		motion2 -= trial.getCurrent()["Motion"];
	else
		motion2 = stimulus_height;
*/
	// mirror alignment check
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	// screen Y alignment check
	screenAlignmentY = asin(
			abs((markers[19].p.y()-markers[21].p.y()))/
			sqrt(
			pow(markers[19].p.x()-markers[21].p.x(), 2) +
			pow(markers[19].p.y()-markers[21].p.y(), 2)
			)
			)*180/M_PI;

	// screen Z alignment check
	screenAlignmentZ = asin(
			abs(markers[19].p.z()-markers[20].p.z())/
			sqrt(
			pow(markers[19].p.x()-markers[20].p.x(), 2) +
			pow(markers[19].p.z()-markers[20].p.z(), 2)
			)
			)*180/M_PI*
			abs(markers[19].p.x()-markers[20].p.x())/
			(markers[19].p.x()-markers[20].p.x());

	// eye coordinates
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = Vector3d(-interoculardistance/2,0,0);

}

void initOptotrak()
{
    optotrak.setTranslation(calibration);

    if ( optotrak.init(LastAlignedFile, OPTO_NUM_MARKERS, OPTO_FRAMERATE, OPTO_MARKER_FREQ, OPTO_DUTY_CYCLE,OPTO_VOLTAGE) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }

    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        updateTheMarkers();
    }
}

void initRendering()
{   
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Set depth buffer clear value */
	glClearDepth(1.0);
	/* Enable depth test */
	glEnable(GL_DEPTH_TEST);
	/* Set depth function */
	glDepthFunc(GL_LEQUAL);
	// scommenta solo se vuoi attivare lo shading degli stimoli
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
	// Tieni questa riga per evitare che con l'antialiasing attivo le linee siano piu' sottili di un pixel e quindi
	// ballerine (le vedi vibrare)
	glLineWidth(1.5);
}

void initVariables()
{
	trial.init(parameters);
	interoculardistance = str2num<int>(parameters.find("IOD"));
	motion_theta = str2num<int>(parameters.find("motion_angle"));
}

void initGLVariables()
{
	// fixed density of 10 dots per cm^2
	int numDots = floor(10.0 * (objlength/10.0*cos((objwidth+theta)*M_PI/180.0)*stimulus_height/10.0) );
	cerr << floor(10.0 * (objlength/10.0*stimulus_height/10.0)*cos((objwidth+theta)*M_PI/180.0) ) << endl;
	cylinder.setNpoints(numDots);
	cylinder.setDimensions(objlength, stimulus_height, 1.0);
//	cylinder.setRadiusAndHeight(objlength, 0.0, stimulus_height); // raggio (mm) altezza (mm)
	// Dispone i punti random sulla superficie cilindrica 
	cylinder.compute();
	stimDrawer.setStimulus(&cylinder);
	cylinder.setFluffiness(1E-6);
	// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
	stimDrawer.setSpheres(true);
	stimDrawer.initList(&cylinder, glRed);
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	// Subject name
	string subjectName = parameters.find("SubjectName");

	// Principal streams files

	// response file (if haptic)
	string responseFileName = experiment_directory + responseFile_name + subjectName + ".txt";

	// Check for output file existence
	if (util::fileExists(responseFileName))
	{
		string error_on_file_io = responseFileName + string(" already exists");
		cerr << error_on_file_io << endl;
#ifndef SIMULATION
		MessageBox(NULL, (LPCSTR)"FILE ALREADY EXISTS\n Please check the parameters file.",NULL, NULL);
#endif
		exit(0);
	}
	else
	{
		responseFile.open(responseFileName.c_str());
		cerr << "File " << responseFileName << " loaded successfully" << endl;
	}

	responseFile << fixed << responseFile_headers << endl;

}

// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(4051,3500);
}

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();

	// initializes optotrak and velmex motors
	initOptotrak();

	initMotors();

	// initializing glut
	glutInit(&argc, argv);
	#ifndef SIMULATION
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
		glutGameModeString("1024x768:32@85");
		glutEnterGameMode();
		glutFullScreen();
	#else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutCreateWindow("Simulation test");
	#endif

	// initializing experiment's parameters
	initRendering();
	initGLVariables();
	initStreams();
	initVariables();
	drawTexture1();
	// glut callback
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutIdleFunc(idle);
	glutTimerFunc(TIMER_MS, update, 0);
	glutSetCursor(GLUT_CURSOR_NONE);

	boost::thread initVariablesThread(&initVariables);

	// Application main loop
	glutMainLoop();

	cleanup();

	return 0;
}
