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

#include <cstdlib>;
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <direct.h>

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
//#include "Optotrak.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "EllipsoidPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParStaircase.h"
#include "Staircase.h"
#include "ParametersLoader.h"
#include "TrialGenerator.h"
#include "Util.h"
#define BROWN 
#ifdef BROWN
#include "BrownMotorFunctions.h"
#else
#include "RoveretoMotorFunctions.h"
#endif
/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace BrownMotorFunctions;

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 11 // 85 hz
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
Screen screen;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[6];
CylinderPointsStimulus cylinder[6];
EllipsoidPointsStimulus ellipsoid;
//bool isStimulusDrawn=true;
bool isSquareDrawn=false;
int leftButtonDown = 0, rightButtonDown = 0;
// square measures
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=0, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;
Timer timer;
Timer globalTimer;
double tFHP=0.0;
bool moveToPartII = false, moveToPartIII = false, moveToPartIV = false, moveToPartV = false, moveToPartVI = false, moveToPartVII = false, moveToPartVIII = false;
bool pause = false, paused = false;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb, physicalRigidBodyTip(0,0,0), platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
vector <Marker> markers;
static double interoculardistance=57.5;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int platformCalibrationDone=0;
int fingerCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleIndex=false;
bool allVisibleThumb=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;
bool visibleFingers=false;

/********* TRIAL VARIABLES *********/
static const int THREERODS=0;
static const int ELLIPSOID=1;
static const int SURFACES=2;
int trialMode = THREERODS;
int trialNumber = 0;
ParametersLoader parameters;
TrialGenerator<double> trial;
static const Vector3d objLocation;
int fingersOccluded=0;
bool frameShown=true;
int frameN=0;
int responsekey=0;

ParStaircase parStairCase;
bool resp;
double recFHP = 0.0;
double stepVisual = 1000.0, timeToFHP = 0.0;
bool pointing = false, handRecorded = false;
double step = 0.0, firstD = 0.0, secondD = 0.0, stepT = 0.0;
int part = 1, condition = 0, attempt = 1;
bool handAtStart = true;
double jitterObj = 0.0;
bool checkHandAtStart = false;

/********** STREAMS **************/
ofstream responseFile, trialFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void drawCircle(double radius, double x, double y, double z);
void cleanup();
void drawInfo();
void drawCalibration();
void drawStimulus();
void drawTrial(double delz);
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
void advanceTrial(bool response);
void initTrial();
void mouseFunc(int button, int state, int x, int y);
void drawThreeRods(double deltaz);
void drawLetter(int letter);
void drawCross();
void drawBlack();
void drawBreak();
void drawEllipsoid(double axz);
void drawSurfaces(double sepz);

/***** SOUND THINGS *****/
void beepOk(int tone)
{
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
void cleanup()
{
// Stop the optotrak
    optotrak->stopCollection();
    delete optotrak;
}

void drawInfo()
{
	if ( visibleInfo )
	{
		glDisable(GL_COLOR_MATERIAL);
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
			{
			if ( allVisibleHead )
				text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
			else
				text.draw("Be visible with the head and glasses");
			}
			break;
		case 1:
		case 2:
			{
			if ( allVisiblePatch )
				text.draw("Move the head in the center");
			else
				text.draw("Be visible with the patch");
			}
			break;
		case 3:	// When the head calibration is done then calibrate the fingers
			{
			switch ( fingerCalibrationDone )
				{
			case 0:
				text.draw("Press F to record platform markers");
				break;
			case 1:
				text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
				break;
			case 2:
				text.draw("Move index and thumb to rigidbody tip to define starting position, then press F");
				break;
			case 3:
				text.draw("Finger calibration successfull!");
				break;
				}
			}
			break;
			}

	text.draw("####### SUBJECT #######");
	text.draw("#");
	text.draw("# Name: " +parameters.find("SubjectName"));
	text.draw("# IOD: " +stringify<double>(interoculardistance));
	text.draw("#");
	text.draw("# trial: " +stringify<double>(trialNumber));
	text.draw("#");
//	text.draw("# obj depth: " +stringify<double>(trial.getCurrent()["RelDepthObj"] + dz));
//	text.draw("#");
	/*
	for ( std::map<string,double>::iterator iter = trial.getCurrent().begin(); iter!=trial.getCurrent().end();++iter)
	{
		text.draw(iter->first + ": " + stringify<double>(iter->second));
	}
	*/
	text.draw("#######################");
	text.draw("HeadCalibration= " + stringify<int>(headCalibrationDone) );
	text.draw("FingerCalibration= " + stringify<int>(fingerCalibrationDone) );

	glColor3fv(glWhite);
	text.draw("Calibration Platform" );

	if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
	
	glColor3fv(glWhite);
	text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );

	glColor3fv(glWhite);
	text.draw(" " );
	text.draw("Mirror" );

	if ( isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(5)+stringify< Eigen::Matrix<double,1,3> > (markers[5].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(6)+stringify< Eigen::Matrix<double,1,3> > (markers[6].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(7)+stringify< Eigen::Matrix<double,1,3> > (markers[7].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(8)+stringify< Eigen::Matrix<double,1,3> > (markers[8].p.transpose())+ " [mm]" );

	glColor3fv(glWhite);
	text.draw(" " );
	text.draw("Index" );

	if ( isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(13)+stringify< Eigen::Matrix<double,1,3> > (markers[13].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(14)+stringify< Eigen::Matrix<double,1,3> > (markers[14].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(16)+stringify< Eigen::Matrix<double,1,3> > (markers[16].p.transpose())+ " [mm]" );

	glColor3fv(glWhite);
	text.draw(" " );
	text.draw("Thumb" );

	if ( isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(15)+stringify< Eigen::Matrix<double,1,3> > (markers[15].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(17)+stringify< Eigen::Matrix<double,1,3> > (markers[17].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(18)+stringify< Eigen::Matrix<double,1,3> > (markers[18].p.transpose())+ " [mm]" );

	glColor3fv(glWhite);

	text.draw("EyeRight= "+stringify< Eigen::Matrix<double,1,3> > (eyeRight.transpose())+ " [mm]" );
	text.draw("EyeLeft= "+stringify< Eigen::Matrix<double,1,3> > (eyeLeft.transpose())+ " [mm]" );
	text.draw("Alignment(X,Y)= " +stringify<double>(alignmentX)+","+stringify<double>(alignmentY));
	text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
	text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
	text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
	text.draw("Trial= " + stringify<int>(trialNumber));
	text.draw("Index Speed= " + stringify<double>(markers[14].v.norm()));
	text.draw("Condition= " + stringify<int>(condition));
	text.draw("StairState= " + stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getState()));
	text.draw("StairID= " + stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getID()));
	text.leaveTextInputMode();
	}
}

void drawCalibration()
{
	glColor3fv(glWhite);
	drawCircle(50,0,0,focalDistance);
	drawCircle(0.2,0,0,focalDistance);
}

void drawThreeRods(double deltaz)
{
	// Front rod
	glPushMatrix();
	glTranslated(0, 0, deltaz/2);
	stimDrawer[0].draw();
	glPopMatrix();
	
	// Rear Left rod
	glPushMatrix();
	glTranslated(-(trial.getCurrent().first["RelDepthObj"]/2), 0, -deltaz/2);
	stimDrawer[1].draw();
	glPopMatrix();

	// Rear right rod
	glPushMatrix();
	glTranslated((trial.getCurrent().first["RelDepthObj"]/2), 0, -deltaz/2);
	stimDrawer[2].draw();
	glPopMatrix();
}

void drawEllipsoid(double axz)
{
	glPushMatrix();
	glScaled(1,1,axz);
	stimDrawer[3].draw();
	glPopMatrix();
}

void drawSurfaces(double sepz)
{
	glPushMatrix();
	glTranslated(0, 0, -sepz/2);
	stimDrawer[4].draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, sepz/2);
	stimDrawer[5].draw();
	glPopMatrix();
}

void drawBlack()
{

	glPushMatrix();
	glPopMatrix();

}

void drawLetter(int letter)
{
	if (letter <= 0) // vision (L)
	{
		GLText text;	
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed,GLUT_BITMAP_TIMES_ROMAN_24);
		text.enterTextInputMode();
		for(int i=0;i<13;i++)
			text.draw("");    
		text.draw("                                                             L");
		text.leaveTextInputMode();
	} else // pointing
	{
		GLText text;	
		text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed,GLUT_BITMAP_TIMES_ROMAN_24);
		text.enterTextInputMode();
		for(int i=0;i<13;i++)
			text.draw("");
		text.draw("                                                             P");
		text.leaveTextInputMode();
	}
}

void drawCross()
{
	GLText text;	
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed,GLUT_BITMAP_TIMES_ROMAN_24);
	text.enterTextInputMode();
	for(int i=0;i<13;i++)
		text.draw("");
	text.draw("                                                             +");
	text.leaveTextInputMode();
}

void drawBreak()
{
	GLText text;	
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed,GLUT_BITMAP_TIMES_ROMAN_24);
	text.enterTextInputMode();
	for(int i=0;i<13;i++)
		text.draw("");
	text.draw("                                                           BREAK");
	text.leaveTextInputMode();

}

void drawStimulus()
{
	if (fingerCalibrationDone==3 && checkHandAtStart)
	{
	
	glLoadIdentity();

	// ###### draw rotated stimulus (adjusted) ######
	glTranslated(0.0,0,trial.getCurrent().first["AbsDepth"] + jitter);

	firstD = trial.getCurrent().second->getCurrentStaircase()->getState();
	
	if(!paused)
	{
	if(part == 1)
		{
			drawLetter(condition); // draw L or P
			if(moveToPartII)
				part += 1;
		} else if (part == 2)
		{
			drawCross();
			if(moveToPartIII)
				part += 1;
		} else if (part == 3)
		{
			drawTrial(firstD); // draw first triple of rods
			if(moveToPartIV)
				part += 1;
		} else if (part == 4)
		{
			drawBlack();
		}
	} else
		drawBreak();

	glLoadIdentity();

	}
}

void drawFingers()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(index.x(),index.y(),index.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
}

void drawTrial(double delz)
{

switch (trialMode)
{
case THREERODS:
	drawThreeRods(delz);
break;
case ELLIPSOID:
	drawEllipsoid(delz/40);
break;
case SURFACES:
	drawSurfaces(delz);
break;

}

}
// X CARLO: In questa funzione mettere solamente chiamate a funzioni e non inserire codice libero!!!
// Preferibilmente deve restare cosi e devi solo lavorare sulla drawTrial altrimenti il codice diventa un casino
// Se mai volessi simulare che il centro di proiezione si muove rispetto alla vera coordinate dell'occhio 
// devi modificare cam.setEye(...)
void drawGLScene()
{
    if ( trial.isEmpty() )
		exit(0);
	if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawStimulus();
		drawInfo();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawStimulus();
		drawInfo();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawStimulus();
		drawInfo();
        glutSwapBuffers();
    }
}

void initTrial()
{
	// initializing all variables
	frameN=0;
	responsekey=0;
	recFHP = 0.0;
	tFHP = 0.0;
	timeToFHP = 0.0;
	step = 0.0;
	firstD = 0.0;
	secondD = 0.0;
	moveToPartII = false;
	moveToPartIII = false;
	moveToPartIV = false;
	moveToPartV = false;
	moveToPartVI = false;
	moveToPartVII = false;
	moveToPartVIII = false;
	handRecorded = false;
	pointing = false;
	checkHandAtStart = false;
	part = 1;
	jitter = unifRand(str2num<double>(parameters.find("JitterMonLowBound")),
	str2num<double>(parameters.find("JitterMonUpBound")));
	condition = trial.getCurrent().first["Condition"];
	jitterObj = unifRand(str2num<double>(parameters.find("RelDepthLowBound")),
	str2num<double>(parameters.find("RelDepthUpBound")));
	fingersOccluded = 0;
	trialMode=0;

	// roll on
	drawBlack();
	initProjectionScreen(trial.getCurrent().first["AbsDepth"] + jitter);
	drawGLScene();

	string trialFileName = "C:/Users/visionlab/Dropbox/Shape Lab Shared/Carlo/spring13-pointingEquilateralTriangle/Staircase/twoBlocks2/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "trial_" + stringify<double>(trialNumber) + "_" + stringify<double>(attempt) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tindexVelraw\trecFHP\ttFHP\tpart\tstep\tmoveToPartII\tmoveToPartIII\tmoveToPartIV\tfirstD\tisHandRecorded\tpointing\tstepVisual\tattempt" << endl;
	
	timer.start();
}

void advanceTrial(bool response)
{
	beepOk(0);
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3);
    responseFile << 
					parameters.find("SubjectName") << "\t" <<
					interoculardistance << "\t" <<
					trialNumber << "\t" <<
                    trial.getCurrent().first["RelDepthObj"] <<"\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getAscending()<< "\t" <<
                    responsekey << "\t" <<
                    response << "\t" <<
					jitter << "\t"<<
                    timer.getElapsedTimeInMilliSec() << "\t" <<
					trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
					trial.getCurrent().second->getCurrentStaircase()->getState()+trial.getCurrent().first["RelDepthObj"] << "\t" <<
					condition << "\t" <<
					trial.getCurrent().first["sequence"] << "\t" <<
					timeToFHP << "\t" <<
					jitterObj << "\t" <<
					trialMode << "\t" <<
					endl;
	
	trialFile.close();

	resp=response;
	
	if(condition > 0)
		stepVisual = (stepVisual + timeToFHP) / 2;
	else
		stepVisual = 1500;
	
	if( !trial.isEmpty() )
		initTrial();
	else
	{
		exit(0);
	}

	trial.next(response);
	trialNumber++;
}

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
	case 'x':
		// Facendo cosi si cancella lo stimolo durante il movimento (SINCRONO) del monitor.
		// Si imposta il isStimulusDrawn a FALSE e si riaggiorna la schermata con una drawGLScene()
		// infine si muove il monitor, la chiamata blocca il programma per x secondi, si 
		// simula lo spostamento dello schermo di proiezione ed infine si reimposta isStimulusDrawn a TRUE
		// cosi' la prossima chiamata di drawStimulus() lo disegna correttamente a schermo. Provare per credere...
		//factors = trial.getNext();
	//	trial.next();
		drawGLScene();
//		initProjectionScreen(trial.getCurrent()["AbsDepth"]);
		break;
	case 'i':
		visibleInfo=!visibleInfo;
		break;
	case 'm':
		interoculardistance += 0.5;
	break;
	case 'n':
		interoculardistance -= 0.5;
	break;
	case '+':
	{
		// Il trucco per avanzare alla modalita' trial successiva: incrementi di uno e poi tieni il resto della 
		// divisione per due, in questo caso ad esempio sara' sempre:
		// 0,1,0,1,0,1
		// se metti il resto della divisione per 3 invece la variabile trialMode sar'
		// 0,1,2,0,1,2
		// ogni ciclo della variabile trialMode normalmente e' un trial (1)
		// puoi anche definire una funzione void advanceTrial() che si occupa di andare al trial successivo,
		// deve contenere una chiamata alla BalanceFactor::getNext() cosi' passi alla nuova lista di fattori
		// Ad esempio
		 trialMode++;
		 trialMode=trialMode%3;
	}
	break;
	case 't':
	{
		break;
	}
	case 'Q':
	case 'q':
    case 27:	//corrisponde al tasto ESC
    {   
		// Ricorda quando chiami una funzione exit() di chiamare prima cleanup cosi
		// spegni l'Optotrak ed i markers (altrimenti loro restano accesi e li rovini) 
		cleanup();
        exit(0);
    }
    break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisiblePatch )
        {
            headEyeCoords.init(markers[1].p-Vector3d(230,0,0),markers[1].p, markers[5].p,markers[6].p,markers[7].p,interoculardistance );
            headCalibrationDone=1;
			beepOk(0);
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[5].p, markers[6].p,markers[7].p,interoculardistance );
            headCalibrationDone=2;
            break;
        }
    }
    break;
	case 'f':
	case 'F':
		{
		// Here we record the finger tip physical markers
		if ( allVisiblePlatform && (fingerCalibrationDone==0) )
			{
			//platformFingers=markers[1].p;
			platformIndex=markers[1].p;
			platformThumb=markers[2].p;
			fingerCalibrationDone=1;
			beepOk(0);
			break;
			}
		if ( (fingerCalibrationDone==1) && allVisibleFingers )
			{
			indexCoords.init(platformIndex, markers.at(13).p, markers.at(14).p, markers.at(16).p );
			thumbCoords.init(platformThumb, markers.at(15).p, markers.at(17).p, markers.at(18).p );
			fingerCalibrationDone=2;
			beepOk(0);
			break;
			}
		if ( fingerCalibrationDone==2  && allVisibleFingers )
			{
			physicalRigidBodyTip = index;
			fingerCalibrationDone=3;
			visibleInfo=!visibleInfo;
			initTrial();
			break;
			}
		}
		break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[5].p, markers[6].p,markers[7].p,interoculardistance );
            headCalibrationDone=3;
			visibleInfo=false;
        }
    }
    break;
      case '5':
      {  
		  
	  }
      break;
      case '8':
      {  
		
      }
      break;
	  case '4':
      {  
		
	  }
      break;
      case '6':
      {  
		
      }
      break;
      case 'a':
      {  
		  
      }
      break;
      case 's':
      {  
		  advanceTrial(true);
	  }
      break;
	  case '0':
	  {
		 
	  }
	  break;
	}
}

// Funzione che gestisce il ridimensionamento della finestra
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

// Questa funzione definisce la distanza e l'orientazione dello schermo di proiezione virtuale, e' importante che 
// la lasci cosi almeno che tu non voglia:
// 1) simulare uno schermo di proiezione che si muove o comunque con un orientamento diverso da quello standard cioe' il
// piano a z=focalDistance
// 2) spostare il piano di proiezione cambiando alignmentX ed alignmentY ma per quello ti consiglio di contattarmi o
// comunque utilizzare il file headCalibration.cpp che ha legato ai tasti 2,4,6,8 il movimento dello schermo per 
// allineare mondo virtuale e mondo optotrak
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

// Questa funzione si occupa di fare il refresh della schermata ed e' chiamata ogni TIMER_MS millisecond, tienila cosi'
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

// Questa funzione e' quella che in background fa tutti i conti matematici, quindi qui devi inserire 
// 1) Scrittura su file continua delle coordinate che vuoi salvare
// 2) Estrazione delle coordinate a partire dai corpi rigidi precedentemente definiti vedi ad esempio
// come e' fatto per eyeLeft e eyeRight oppure per thumb ed index
void idle()
{
	optotrak->updateMarkers();
	//cerr << deltaT << endl;
	markers = optotrak->getAllMarkers();
	// Coordinates picker
	allVisiblePlatform = isVisible(markers[1].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[1].p);

	if ( allVisiblePatch )
		headEyeCoords.update(markers[5].p,markers[6].p,markers[7].p);

	if ( allVisibleFingers )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}

	if (fingerCalibrationDone==3 )
	{
		if ( !allVisibleFingers )
		{
			fingersOccluded=1;
		}
		
		frameN++;

		if(condition == 0)
			condition = unifRand(-1, 1);

		if(index.z() < (-180) && index.y() > -150 && markers[14].v.norm() < 90)
			pointing=true;
		else
			pointing=pointing;
		
		if(index.z() > (-80))
			handAtStart = true;
		else
			handAtStart = false;
		
		if(handAtStart)
			checkHandAtStart = true;
		else
			checkHandAtStart = checkHandAtStart;
		
		if(!checkHandAtStart)
			timer.start();

		if(pointing && !handRecorded)
		{
			recFHP = index.z();
			tFHP = timer.getElapsedTimeInMilliSec();
			handRecorded = true;
		} else
		{
			recFHP = recFHP;
			tFHP = tFHP;
			handRecorded = handRecorded;
		}

		if(condition < 0) // vision then pointing
		{
			if(timer.getElapsedTimeInMilliSec() > stepVisual) // move to drawing the cross
			{
				moveToPartII = true;
				step = stepVisual;
			}
			if(moveToPartII && timer.getElapsedTimeInMilliSec() > step + 500) // move to drawing the first stimulus
			{
				moveToPartIII = true;
				step = step + 500;
			}
			if(moveToPartIII && timer.getElapsedTimeInMilliSec() > step + 800) // move to drawing black
			{
				moveToPartIV = true;
				step = step + 800;
			}
		} else // pointing then vision
		{
			if(tFHP > 0) // move to drawing the cross
			{
				moveToPartII = true;
				timeToFHP = tFHP;
				step = tFHP;
			}
			if(moveToPartII && timer.getElapsedTimeInMilliSec() > step + 500) // move to drawing the first stimulus
			{
				moveToPartIII = true;
				step = step + 500;
			}
			if(moveToPartIII && timer.getElapsedTimeInMilliSec() > step + 800) // move to drawing black
			{
				moveToPartIV = true;
				step = step + 800;
			}
		}
			
	}

	if(headCalibration)
	{
	eyeLeft = headEyeCoords.getLeftEye();
	eyeRight = headEyeCoords.getRightEye();
	} else	{
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = -eyeRight;
	}

	index = indexCoords.getP1();
	thumb = thumbCoords.getP1();

	singleMarker = markers.at(4).p.transpose();

	if(pause && !paused)
	{
		timer.stop();
		paused = true;
	}
	else if(paused && !pause)
	{
		initTrial();
		paused = false;
	}

	// Write to trialFile
	if (fingerCalibrationDone==3 )
		{

		trialFile << fixed <<
			parameters.find("SubjectName") << "\t" <<		//subjName
			trialNumber << "\t" <<							//trialN
			timer.getElapsedTimeInMilliSec() << "\t" <<		//time
			frameN << "\t" <<								//frameN
			index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
			thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
			eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
			eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
			fingersOccluded << "\t" <<						//fingersOccluded
			markers[14].v.norm() << "\t" <<					//indexVelraw
			recFHP << "\t" <<								
			tFHP << "\t" << 
			part << "\t" << 
			step << "\t" << 
			moveToPartII << "\t" << 
			moveToPartIII << "\t" << 
			moveToPartIV << "\t" << 
			firstD << "\t" << 
			handRecorded << "\t" << 
			pointing << "\t" << 
			stepVisual << "\t" << 
			attempt
			;

		trialFile << endl;
		
		}

}

// Questa funzione inizializza l'optotrak passandogli una traslazione di default "calibration" che 
// in questo caso rappresenta la coordinata dell'occhio ciclopico (in realta' andrebbe bene qualsiasi valore ma questo 
// allinea meglio coordinate optotrak e coordinate opengl cosicche abbiano lo zero molto vicino. IMPORTANTE: vedi di
// tenere "calibration" lo stesso in tutti gli esperimenti perche' altrimenti devi modificare anche 
// alignmentX e alignmentY
// Se qualcosa nell'inizializzazione dell'optotrak non va hai due possibilita'
// 1) Leggerti il log che sta qui sotto
// 2) Leggerti il log generato nel file opto.err che normalmente dovrebbe essere nella cartella dove lanci l'eseguibile // altrimenti fatti una ricerca file.
// La funzione initOptotrak deve stare all'inizio di tutto.
void initOptotrak()
{
    optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    int numMarkers=20;
    float frameRate=85.0f;
    float markerFreq=4600.0f;
    float dutyCycle=0.4f;
    float voltage = 7.0f;
	if ( optotrak->init("C:/cncsvisiondata/camerafiles/Aligned20111014",numMarkers, frameRate, markerFreq, dutyCycle,voltage) != 0)
    {   cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    // Read 10 frames of coordinates and fill the markers vector
    for (int i=0; i<10; i++)
    {
        optotrak->updateMarkers();
        markers = optotrak->getAllMarkers();
    }
}

// Questa funzione inizializza l'ambiente di rendering OpenGL. Tienila cosi perche' cosi' va bene nel 99% dei casi
void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
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
	// Riempie la lista dei fattori a partire da quanto descritto nei fattori dell'oggetto parameters
	// Per dubbi prova i vari esempi di test del modulo experiments
	trial.init(parameters);
	//trial.print();
	//initTrial();
}

void initGLVariables()
{
	// ########## Ora inizializza gli 4 rods
	for (int i=0; i<3; i++)
	{
		cylinder[i].setNpoints(100);
		cylinder[i].setRadiusAndHeight(1,60); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[i].compute();
		stimDrawer[i].setStimulus(&cylinder[i]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[i].setSpheres(true);
		stimDrawer[i].initList(&cylinder[i], glRed);
	}
		
		// build ellipsoid
		ellipsoid.setNpoints(100);
		ellipsoid.setAxesAndHeight(40,40,60);
		ellipsoid.compute();
		stimDrawer[3].setStimulus(&ellipsoid);
		stimDrawer[3].setSpheres(true);
		stimDrawer[3].initList(&ellipsoid, glRed);

		// build back surface
		cylinder[4].setNpoints(140);
		cylinder[4].setRadiusAndHeight(45,0,60);
		cylinder[4].compute();
		stimDrawer[4].setStimulus(&cylinder[4]);
		stimDrawer[4].setSpheres(true);
		stimDrawer[4].initList(&cylinder[4], glRed);

		// build front surface
		cylinder[5].setNpoints(100);
		cylinder[5].setRadiusAndHeight(25,0,25);
		cylinder[5].compute();
		stimDrawer[5].setStimulus(&cylinder[5]);
		stimDrawer[5].setSpheres(true);
		stimDrawer[5].initList(&cylinder[5], glRed);
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open("C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersstaircasePointing2.txt");
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	
	string dirName  = "C:/Users/visionlab/Dropbox/Shape Lab Shared/Carlo/spring13-pointingEquilateralTriangle/Staircase/twoBlocks2/" + subjectName;
	mkdir(dirName.c_str());

		// Principal streams file
    string responseFileName =  "C:/Users/visionlab/Dropbox/Shape Lab Shared/Carlo/spring13-pointingEquilateralTriangle/Staircase/twoBlocks2/" + parameters.find("SubjectName") + "/staircaseGrasping2_"   + subjectName + ".txt";

	// Check for output file existence
	/// Response file
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());

	responseFile << fixed << "subjName\tIOD\ttrialN\tRelDepthObj\tStairID\tStairState\tReversals\tAscending\tresponsekey\tresp\tjitter\ttime\tstepsDone\tobjdepth\tCondition\tsequence\ttimeToFHP\tjitterObj\tstimulus" << endl;

	globalTimer.start();
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(5000,3500);
}

void mouseFunc(int button, int state, int x, int y)
{
	// Implementala come vuoi
	//cerr << button << " " << state << y << endl;
	if (button==GLUT_LEFT_BUTTON)
	{
		 if(state==GLUT_DOWN && pointing && moveToPartIV)
		 {
			if(tFHP <=1800)
			{
				responsekey=4;
				if ( trial.isEmpty() )
					exit(0);
				advanceTrial(false);
				attempt = 1;
			} else {
				beepOk(1);
				trialFile.close();
				initTrial();
				attempt=attempt+1;
			}
		 }
		else
		{
			responsekey=responsekey;
		}

	}
	
	if (button==GLUT_RIGHT_BUTTON && pointing && moveToPartIV)
	{
		 if(state==GLUT_DOWN)
		 {
			if(tFHP <=1800)
			{
				responsekey=6;
				if ( trial.isEmpty() )
					exit(0);
				advanceTrial(true);
				attempt = 1;
			} else {
				beepOk(1);
				trialFile.close();
				initTrial();
				attempt=attempt+1;
			}
		 }
		else
		{
			responsekey=responsekey;
		}
	}

	if(button==GLUT_MIDDLE_BUTTON)
	{
		if(state==GLUT_DOWN)
			pause = !pause;
		else
			pause = pause;
	}
	
}


int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	//positionObj();
	//boost::this_thread::sleep(boost::posix_time::seconds(10));
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
	{   glutGameModeString("1024x768:32@85");
        glutEnterGameMode();
        glutFullScreen();
    }
	// Prima inizializza l'ambiente di rendering dopodiche' puoi inizializzare le variabili (questo 
	// ragionamento vale se una delle variabili che inizializzi e' del modulo GLViz cioe' richiede OpenGL attivo)
    initRendering();
	initGLVariables();
	// Inizializza gli stream di output o input. Consiglio di usare questo ordine perche' in initVariables
	// normalmente vorrai parsare un file di testo contenente i parametri
	initStreams();
	initVariables();
	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
	glutMouseFunc(mouseFunc);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
	// Alla fine e dopo aver disegnato tutto nero inizializza in background
	boost::thread initVariablesThread(&initVariables);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
