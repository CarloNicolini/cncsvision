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
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
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
double mirrorAlignment=0.0;
Screen screen;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[2];
CylinderPointsStimulus cylinder[2];
Timer timer;
Timer globalTimer;
double indexObjectDistance, thumbObjectDistance, offsetZ;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb, platformIndex(0,0,0), platformThumb(0,0,0), noindex(-999,-999,-999), nothumb(-999,-999,-999);
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
bool tooSlow=false;

/********* TRIAL VARIABLES *********/
static const int THREERODS=0;
int trialMode = THREERODS;
int trialNumber = 0;
int totTrials = 0;

ParametersLoader parameters, parametersOpenLoop, parametersClosedLoop;

int actualOffset[6] = {33,70,30,20,10,0};
int cycle = 0;
BalanceFactor<double> trial[sizeof(actualOffset)/sizeof(int)];

int fingersOccluded=0;
int frameN=0;
bool handAtStart = true, endOfTrial = false;
bool indexDisappeared = false, thumbDisappeared = false, fingersDisappeared = false;
bool isStimulusDrawn = false;

/********** STREAMS **************/
ofstream responseFile, trialFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void drawCircle(double radius, double x, double y, double z);
void cleanup();
void drawInfo();
void drawCalibration();
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
void drawCylinder30();
void drawCylinder50();
void drawFingers(double offsetZ);
void drawBlack();
void drawBlackSquare();
void repeatTrial();
void drawNoFingers();

/***** SOUNDS *****/
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

/*************************** EXPERIMENT SPECS ****************************/

// paramters file directory and name
string parametersFile_directory = "C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersClaireHandOcclusionTrialFall14.txt";

// experiment directory
string experiment_directory = "C:/Users/visionlab/Copy/Shape Lab Shared/Claire/Fall14/";

// response file name
string responseFile_name = "claireHandOcclusion_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttotTrials\ttrialN\tRelDepthObj\tAbsDepth\toffset\ttrialDuration\tblock";

// trial file headers
string trialFile_headers = "subjName\ttrialN\ttime\tframeN\tindexXraw\tindexYraw\tindexZraw\tthumbXraw\tthumbYraw\tthumbZraw\teyeRXraw\teyeRYraw\teyeRZraw\teyeLXraw\teyeLYraw\teyeLZraw\tfingersOccluded\tindexDisappeared\tthumbDisappeared\toffset\tglobalTime\tRelDepthObj\tAbsDepth";

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
				{
					if ( allVisibleHead )
						text.draw("==== Head Calibration OK 2 ==== Press Spacebar one more time ");
					else
						text.draw("Be visible with the head and glasses");
				}
			case 2:
				{
					if ( allVisiblePatch )
						text.draw("Move the head in the center and then press Enter to start");
					else
						text.draw("Be visible with the patch");
				}
				break;
			}

	if(interoculardistance>0.0)
	{
		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " + parameters.find("SubjectName"));
	} else {
		text.draw(" ");
		text.draw(" ");
		text.draw(" ");
	}
	text.draw("# IOD: " +stringify<double>(interoculardistance));
	if ( abs(mirrorAlignment - 45.0) < 0.2 )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("# Mirror Alignment = " +stringify<double>(mirrorAlignment));

	if(interoculardistance>0.0)
	{
		glColor3fv(glWhite);
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
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
		text.draw("AbsDepth " + stringify<double>(trial[cycle].getCurrent()["AbsDepth"]));
		text.draw("Offset " + stringify<int>(actualOffset[cycle]) + 
			" | Cycle " + stringify<int>(cycle) +
			" " + stringify<int>(sizeof(actualOffset)/sizeof(int)));
		text.leaveTextInputMode();
	}
	}
}

void drawCalibration()
{
	glColor3fv(glWhite);
	drawCircle(50,0,0,focalDistance);
	drawCircle(0.2,0,0,focalDistance);
}

void drawCylinder30()
{

	glPushMatrix();
	stimDrawer[0].draw();
	glPopMatrix();

}

void drawBlack()
{

	glPushMatrix();
	glPopMatrix();

}

void drawCylinder50()
{
	glPushMatrix();
	stimDrawer[1].draw();
	glPopMatrix();
}

void drawStimulus()
{
	if (fingerCalibrationDone==3 && isStimulusDrawn)
	{
		if( !allVisibleFingers)
				drawNoFingers();

		if(!endOfTrial)
		{
			if(timer.getElapsedTimeInMilliSec() < str2num<int>(parameters.find("trialDuration")))
			{
				glLoadIdentity();

				drawFingers(actualOffset[cycle]);

				// ###### draw rotated stimulus (adjusted) ######
				glTranslated(0.0,0,trial[cycle].getCurrent()["AbsDepth"]);
			
				drawBlackSquare();

				if(trial[cycle].getCurrent()["RelDepthObj"]==30)
				{
					drawCylinder30();
				} else {
					drawCylinder50();
				}

				glLoadIdentity();
			}
		} else 
			advanceTrial();
	}
}

void drawFingers(double offsetZ)
{
	indexObjectDistance = sqrt( pow(index.x(), 2) + pow( index.z() - trial[cycle].getCurrent()["AbsDepth"] , 2) );
	thumbObjectDistance = sqrt( pow(thumb.x(), 2) + pow( thumb.z() - trial[cycle].getCurrent()["AbsDepth"] , 2) );
	
	if(!thumbDisappeared)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated(index.x(),index.y(),index.z());
		if(actualOffset[cycle]==33)
			glColor3fv(glBlack);
		else
			glColor3fv(glRed);
		glutSolidSphere(1,10,10);
		glPopMatrix();

		indexDisappeared = indexDisappeared;

	} else
		indexDisappeared = true;
	
	if(thumbObjectDistance >= (trial[cycle].getCurrent()["RelDepthObj"]/2) + offsetZ && !thumbDisappeared)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated(thumb.x(),thumb.y(),thumb.z());
		if(actualOffset[cycle]==33)
			glColor3fv(glBlack);
		else
			glColor3fv(glRed);
		glutSolidSphere(1,10,10);
		glPopMatrix();

		thumbDisappeared = thumbDisappeared;
	} else
		thumbDisappeared = true;
}

// X CARLO: In questa funzione mettere solamente chiamate a funzioni e non inserire codice libero!!!
// Preferibilmente deve restare cosi e devi solo lavorare sulla drawTrial altrimenti il codice diventa un casino
// Se mai volessi simulare che il centro di proiezione si muove rispetto alla vera coordinate dell'occhio 
// devi modificare cam.setEye(...)
void drawGLScene()
{
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

void drawNoFingers()
{
		glLoadIdentity();
		glTranslated(0.0,0,-600);
		
		int edge = 100;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,0.0);
		glVertex3d(edge,-edge,0.0);
		glVertex3d(-edge,-edge,0.0);
		glVertex3d(-edge,edge,0.0);
		glEnd();
}

void initTrial()
{
	Sleep(500);
	// initializing all variables
	isStimulusDrawn = false;
	tooSlow=false;
	endOfTrial = false;
	indexDisappeared = false;
	thumbDisappeared = false;
	fingersDisappeared = false;
	frameN=0;
	fingersOccluded = 0;

	// roll on
	drawGLScene();
	initProjectionScreen(trial[cycle].getCurrent()["AbsDepth"]);

	// trial file
	string trialFileName = experiment_directory + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "trial_" + stringify<int>(totTrials) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << trialFile_headers << endl;

	isStimulusDrawn = true;
	timer.start();
	globalTimer.start();
}

void advanceTrial()
{
	if(!tooSlow)
		beepOk(0);
	else
		beepOk(1);
	tooSlow=false;
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3);
	responseFile << 
					parameters.find("SubjectName") << "\t" <<
					interoculardistance << "\t" <<
					totTrials << "\t" <<
					trialNumber << "\t" <<
                    trial[cycle].getCurrent()["RelDepthObj"] <<"\t" <<
					trial[cycle].getCurrent()["AbsDepth"] <<"\t" <<
					actualOffset[cycle] <<"\t" <<
                    timer.getElapsedTimeInMilliSec() << "\t" <<
					cycle << "\t" <<
					endl;
	
	trialFile.close();

	if(abs(mirrorAlignment - 45.0) < 0.25)
	{
		trialNumber++;
		totTrials++;
		
		if(trial[cycle].getRemainingTrials() != 0)
		{
			trial[cycle].next();
			initTrial();
		} else 
		{
			cycle++;
			trialNumber = 0;
			if(cycle < sizeof(actualOffset)/sizeof(int))
			{
				trial[cycle].next();
				initTrial();
			} else
			{	
				responseFile.close();
				cleanup(); // shut down Optotrak
				exit(0);
			}
		}
	}else
	{
		visibleInfo = !visibleInfo;
	}
}

void repeatTrial()
{
	//beepOk(1);
	//initTrial();
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
		//factors = trial[cycle].getNext();
	//	trial[cycle].next();
		drawGLScene();
//		initProjectionScreen(trial[cycle].getCurrent()["AbsDepth"]);
		break;
	case 'i':
		visibleInfo=!visibleInfo;
		break;
	case 'm':
	{
		interoculardistance += 0.5;
		headEyeCoords.setInterOcularDistance(interoculardistance);
	}
	break;
	case 'n':
	{
		interoculardistance -= 0.5;
		headEyeCoords.setInterOcularDistance(interoculardistance);
	}
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
            headEyeCoords.init(markers[3].p-Vector3d(70,0,0),markers[3].p, markers[10].p,markers[11].p,markers[12].p,interoculardistance );
            headCalibrationDone=1;
			beepOk(0);
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
            headCalibrationDone=2;
			beepOk(0);
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( (headCalibrationDone == 2 && allVisiblePatch && allVisibleFingers && fingerCalibrationDone==3 ) || (fingerCalibrationDone==3 && !headCalibration) )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
            headCalibrationDone=3;
			headCalibration=true;
        }
    }
    break;
	case 'f':
	case 'F':
		{
		// Here we record the finger tip physical markers
		if ( allVisiblePlatform && fingerCalibrationDone==0 )
			{
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
			fingerCalibrationDone=3;
			beepOk(0);
			visibleInfo=false;
			trial[cycle].next();
			initTrial();
			break;
			}
		}
		break;
      case 's':
      {  
		  advanceTrial();
	  }
      break;
      case 'c':
      {  
		  headCalibration = !headCalibration;
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
	//cerr << trial[cycle].getCurrent()["AbsDepth"] << endl;
	markers = optotrak->getAllMarkers();
	// Coordinates picker
	allVisiblePlatform = isVisible(markers[1].p) && isVisible(markers[2].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisiblePatch = isVisible(markers[10].p) && isVisible(markers[11].p) && isVisible(markers[12].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[9].p);
	
	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	if ( allVisiblePatch )
		headEyeCoords.update(markers[10].p,markers[11].p,markers[12].p);

	if(headCalibration)
	{
	eyeLeft = headEyeCoords.getLeftEye();
	eyeRight = headEyeCoords.getRightEye();
	} else	{
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = -eyeRight;
	}

	if ( allVisibleFingers )
	{
		indexCoords.update(markers[13].p, markers[14].p, markers[16].p );
		thumbCoords.update(markers[15].p, markers[17].p, markers[18].p );
		fingersOccluded = 0;
	}else
	{
		fingersOccluded=1;
	}

	if (fingerCalibrationDone==3 )
	{

		frameN++;
		//if(allVisibleIndex)
			index = indexCoords.getP1();
		//else
		//	index = noindex;

		//if(allVisibleThumb)
			thumb = thumbCoords.getP1();
		//else
		//	thumb = nothumb;

		if(thumbDisappeared)
			fingersDisappeared = true;
		else
			fingersDisappeared = fingersDisappeared;

		if(index.z() > (-150))
			handAtStart = true;
		else
			handAtStart = false;
		
		if(handAtStart && timer.getElapsedTimeInMilliSec() < str2num<int>(parameters.find("trialDuration")))
		{
			timer.start();
			endOfTrial = endOfTrial;
		} else if(handAtStart && timer.getElapsedTimeInMilliSec() > str2num<int>(parameters.find("trialDuration")))
			endOfTrial = true;

		// Write to trialFile

		trialFile << fixed <<
			parameters.find("SubjectName") << "\t" <<		//subjName
			totTrials << "\t" <<							//trialN
			timer.getElapsedTimeInMilliSec() << "\t" <<		//time
			frameN << "\t" <<								//frameN
			index.transpose() << "\t" <<					//indexXraw, indexYraw, indexZraw
			thumb.transpose() << "\t" <<					//thumbXraw, thumbYraw, thumbZraw
			eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
			eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
			fingersOccluded	<< "\t" <<						//fingersOccluded
			indexDisappeared << "\t" <<	
			thumbDisappeared << "\t" <<
			actualOffset[cycle] << "\t" <<
			globalTimer.getElapsedTimeInMilliSec() << "\t" <<
			trial[cycle].getCurrent()["RelDepthObj"] << "\t" <<
			trial[cycle].getCurrent()["AbsDepth"]
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
	for(int i = 0; i < sizeof(actualOffset)/sizeof(int); i++)
	{
		trial[i].init(parameters);
		trial[i].print();
	}
}

void drawBlackSquare()
{
	glColor3fv(glBlack);
	glBegin(GL_QUADS);
		glVertex3f(-30.0f, 20.0f, 0.0f);		// Top Left
		glVertex3f( 30.0f, 20.0f, 0.0f);		// Top Right
		glVertex3f( 30.0f,-20.0f, 0.0f);		// Bottom Right
		glVertex3f(-30.0f,-20.0f, 0.0f);		// Bottom Left
	glEnd();
}

void initGLVariables()
{
	// ########## Ora inizializza i 4 rods
		
	// cilindro rosso 3 cm di diametro
		cylinder[0].setNpoints(300);
		cylinder[0].setRadiusAndHeight(15,80); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[0].compute();
		stimDrawer[0].setStimulus(&cylinder[0]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[0].setSpheres(true);
		stimDrawer[0].initList(&cylinder[0], glRed);
	
	// cilindro rosso 5 cm di diametro
		cylinder[1].setNpoints(450);
		cylinder[1].setRadiusAndHeight(25,80); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[1].compute();
		stimDrawer[1].setStimulus(&cylinder[1]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[1].setSpheres(true);
		stimDrawer[1].initList(&cylinder[1], glRed);

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

	// Subject folder
	string dirName  = experiment_directory + subjectName;
	mkdir(dirName.c_str());

		// Principal streams file
    string responseFileName =  experiment_directory + parameters.find("SubjectName") + "/" + responseFile_name + subjectName + ".txt";

	// Check for output file existence
	/// Response file
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());

	responseFile << fixed << responseFile_headers << endl;

	globalTimer.start();
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(3500,3500);
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

	// Move object far away from observer
	Vector3d pos1(0,-328,-1000);//-500);
	Vector3d centercal(-21.8,-327.2,-330.0);
	moveObjectAbsolute(pos1, centercal, 6500);
/*
	// Inizializza gli stream di output o input. Consiglio di usare questo ordine perche' in initVariables
	// normalmente vorrai parsare un file di testo contenente i parametri
	initStreams();
	initVariables();
	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
	// Alla fine e dopo aver disegnato tutto nero inizializza in background
	boost::thread initVariablesThread(&initVariables);
    // Application main loop
    glutMainLoop();
*/
    cleanup();

    return 0;
}
