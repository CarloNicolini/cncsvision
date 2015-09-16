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
#include "EllipsoidPointsStimulus.h"
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
#define SCREEN_WIDTH  1280 //1024      // pixels
#define SCREEN_HEIGHT 1024//768       // pixels
static const double SCREEN_WIDE_SIZE = 360; //306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
static const Vector3d centercal(42.0,-320.0, -188.0);
//Vector3d centercal(0.0,-300.0,-178.0);
Screen screen;
double mirrorAlignment=0.0;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;

/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[6];
CylinderPointsStimulus cylinder[6];
EllipsoidPointsStimulus ellipsoid;
//bool isStimulusDrawn=true;
bool isSquareDrawn=false; //evan-comment
int leftButtonDown = 0, rightButtonDown = 0;
// square measures
double edge = 0.0, dedge = 0, xedge = 0.0, zedge = 0.0, jitter = 0.0, theta=90, phi=M_PI*3/4, dz = 0.0, dx = 0.0, r = 0.0, fdx = 1.0, axz = 1.0;
Timer timer;
Timer globalTimer;
double tFHP=0.0;
bool pause = false, paused = false, isStimulusDrawn=false;
GLfloat myColor;
bool pulsingColors = true;
int stimPosn;
int LEFT = 1;
int RIGHT = 2;
float tFrac;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb, platformFingers(0,0,0), platformIndex(0,0,0), platformThumb(0,0,0), singleMarker;
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
static const int ROD=3;//evces 031414
int trialMode = ROD; 
int trialNumber = 0;
double frameN=0;
double TGA_frame=0;

double left_height = 5;
double right_height = 5;
double x_dist;
double y_dist;
double z_dist;

double stim_translation_z = -400.0;

ParametersLoader parameters;
BalanceFactor<double> trial;
map <std::string, double> factors;
static const Vector3d objLocation;

int responsekey=0;
int st=100, sl=100;

//int MISMATCH=2, MATCH=1;
//int REDGREEN = 0;
//int greenStim;

int attempt = 1;
double num_lost_frames = 0;

int fingersOccluded=0;
bool fingersShown=false;
bool frameShown=true;

bool training=true;

bool handAtStart = true;
bool started=false;
bool reachedObject = false;
bool within_time_limit;
bool stim_vanish_when_reached = false;

int stimCond;
bool debug_no_motion;
//int hapticLarger;
int BG_WHITE;

//int randCond;
//int randIncrement;
int rand_match;

GLUquadric* qobj;

clock_t t;

/********** STREAMS **************/
ofstream responseFile, trialFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void drawCircle(double radius, double x, double y, double z);
void cleanup();
void drawInfo();
void drawCalibration();
void drawStimulus();
void drawTrial();
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
void mouseFunc(int button, int state, int x, int y);
void drawTwoRods(double height, int sl, int st);
//void drawOneSolidRod(double deltaz, int sl, int st);
void drawNoFingers();
void drawFingers();
void gluCylinder(GLUquadric* gluNewQuadric(),  double  base,  double  top,  double  height,  int  slices,  int  stacks);
GLfloat myColorGen(int pulseColor, clock_t time, int modInterval);

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
			text.init(1280,1024,glWhite,GLUT_BITMAP_HELVETICA_12);
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
	text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );
	
	glColor3fv(glWhite);
	text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );

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

	text.draw("Alignment(X,Y)= " +stringify<double>(alignmentX)+","+stringify<double>(alignmentY));
	if ( abs(mirrorAlignment - 45.0) < 0.2 )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Mirror Alignment = " +stringify<double>(mirrorAlignment));
	text.draw("Index= " +stringify< Eigen::Matrix<double,1,3> >(index.transpose()));
	text.draw("Thumb= " +stringify< Eigen::Matrix<double,1,3> >(thumb.transpose()));
	text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
	text.draw("Trial= " + stringify<int>(trialNumber));
	text.draw("Frame= " + stringify<int>(frameN));
	text.draw("theta= " + stringify<int>(theta));
	text.leaveTextInputMode();
	}
}

void drawTwoRods(int condition, int sl, int st) {
	if (pulsingColors) {
		clock_t t = clock();
		t = (t%2000)+1;
		tFrac = abs( (t/2000.0) - 0.5 );
	} else
		tFrac = 0.5;

	switch (condition) {
		case 0: // Bh-Bv Sh-Sv
		case 3: // Sh-Bv Bh-Sv
			left_height = 52;
			right_height = 48;
			break;
		case 1: //Bh-Sv Sh-Bv
		case 2: //Sh-Sv Bh-Bv
			left_height = 48;
			right_height = 52;
			break;
	}

	// Left Rod
	glPushMatrix();
	glLoadIdentity();
	glTranslated(-30.0, 0.0, stim_translation_z);
	glRotated(theta, 1,0,0);
	if (stimPosn==LEFT) {
		glColor3f(0.5+tFrac, 0.0, 0.0);
		//cout << "L: Left rod" << endl;
	} else {
		glColor3f(0.5, 0.0, 0.0);
		//cout << "L: Right rod" << endl;
	}
	gluCylinder(qobj, 4.0, 4.0, left_height, sl, st);
	glPopMatrix();

	// Right rod
	glPushMatrix();
	glLoadIdentity();
	glTranslated(70.0, 0.0, stim_translation_z);
	glRotated(theta, 1,0,0);
	if (stimPosn==RIGHT) {
		glColor3f(0.5+tFrac, 0.0, 0.0);
		//cout << "R: Right rod" << endl;
	} else {
		glColor3f(0.5, 0.0, 0.0);
		//cout << "R: Left rod" << endl;
	}
	gluCylinder(qobj, 4.0, 4.0, right_height, sl, st);
	glPopMatrix();

}

void drawNoFingers()
{
		glLoadIdentity();
		glTranslated(0.0,0,-600);
		
		edge = 100;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,0.0);
		glVertex3d(edge,-edge,0.0);
		glVertex3d(-edge,-edge,0.0);
		glVertex3d(-edge,edge,0.0);
		glEnd();
}

void drawStimulus()
{
	if (fingerCalibrationDone==3)
	{
		glLoadIdentity();

		if(isStimulusDrawn)
		{
			if( !allVisibleFingers)
				drawNoFingers();

			if(fingersShown)
				drawFingers();
			if (stim_vanish_when_reached) {
				if( !reachedObject ) {
					drawTrial();
				}
			}else{
				if( timer.getElapsedTimeInMilliSec() <= trial.getCurrent()["RodVisibleTime"] ) {
					drawTrial();
				}
			}
		}
		
		if(handAtStart && started) // && (timer.getElapsedTimeInMilliSec() > 2000))
			advanceTrial();
	}
}

void drawFingers()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslated(index.x(),index.y(),index.z());
	glutSolidSphere(1.5,10,10);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslated(thumb.x(),thumb.y(),thumb.z());
	glutSolidSphere(1.5,10,10);
	glPopMatrix();
}

void drawTrial()
{
	drawTwoRods(stimCond, sl, st);
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
        glClearColor(0.0+BG_WHITE,0.0+BG_WHITE,0.0+BG_WHITE,1.0);
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
	started = false;
	handAtStart=true;
	reachedObject = false;
	fingersOccluded = 0;
	num_lost_frames = 0;
	isStimulusDrawn=false;
	stimPosn = trial.getCurrent()["StimPosn"];
	cout << "initTrial: " + trialNumber << endl;
	cout << "initTrial: " + stimPosn << endl;

	// roll on
	drawGLScene();
	
	// move the screen
	initProjectionScreen(trial.getCurrent()["AbsDepth"]); //evces 031414

	if (!debug_no_motion) {
	if(trialNumber==0){
			Vector3d pos1(0,-35,trial.getCurrent()["AbsDepth"]-25);
			moveObjectAbsolute(pos1, centercal, 7500);
	}
	}

	string trialFileName = "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/spatial2/" + parameters.find("SubjectName") + "/" + parameters.find("SubjectName") + "trial_" + stringify<double>(trialNumber) + "_" + stringify<double>(attempt) + ".txt";
	trialFile.open(trialFileName.c_str());
	trialFile << fixed << "subjName\ttrialN\ttime\tframeN\tindexXYZraw\tthumbXYZraw\tfingersOccluded\tattempt\treachedObject\tnum_lost_frames" << endl;

	isStimulusDrawn=true;
	beepOk(0);
	timer.start();
}

void advanceTrial()
{
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile.precision(3);
    responseFile << 
					parameters.find("SubjectName") << "\t" <<
					//interoculardistance << "\t" <<
					trialNumber << "\t" <<
					stimPosn << "\t" <<
					trial.getCurrent()["RodVisibleTime"] << "\t" <<
					trial.getCurrent()["AbsDepth"] <<"\t" <<
                    //"50" << "\t" <<//trial.getCurrent()["ObjHeight"] <<"\t" <<
                    timer.getElapsedTimeInMilliSec() << "\t" <<
					//trial.getCurrent()["HapticFB"] << "\t" <<
					//randCond << "\t" <<
					//randIncrement << "\t" <<
					stimCond << "\t" <<
					BG_WHITE << "\t" <<
					endl;
	
	trialFile.close();

	double percent_occluded_frames = num_lost_frames/TGA_frame;
	bool not_enough_frames = percent_occluded_frames > 0.20;
	cout << num_lost_frames << "_" << frameN << "_" << percent_occluded_frames << endl;
	int overwrite = 0;

	if (training || !reachedObject || not_enough_frames ) {
		cout << training << "_" << !reachedObject << "_" << not_enough_frames << endl;
		map<std::string,double> currentFactorsList = trial.getCurrent();
		trial.reinsert(currentFactorsList);
		beepOk(1);
		overwrite = 1;
	}

	if( !trial.isEmpty() )
	{
		if (!overwrite)
			trialNumber++;
		cout << trialNumber << endl;
		factors = trial.getNext();
		initTrial();
	}
	else
	{
		exit(0);
	}

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
		 trialMode=trialMode%4;
	}
	break;
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
	case 'd':
		{
		fingersShown = !fingersShown;
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
			//centercal = markers[4].p;
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
			beepOk(0);
			fingerCalibrationDone=3;
			visibleInfo=!visibleInfo;
			factors = trial.getNext();
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
		  alignmentY -= .5;
	  }
      break;
      case '8':
      {  
		alignmentY += .5;
      }
      break;
	  case '4':
      {  
		  alignmentX -= .5;
	  }
      break;
      case '6':
      {  
		alignmentX += .5;
      }
	  break;
	  case '1':
      {  
		  stim_translation_z -= 1;
	  }
      break;
      case '3':
      {  
		stim_translation_z += 1;
      }
	  break;
      case 's':
      {  
		  advanceTrial();
	  }
      break;
	  case '0':
	  {
		 training=!training;
	  }
	  break;
	  case 'c':
	  {
		 pulsingColors=!pulsingColors;
	  }
	  break;
	  case 'l':
	  {
		 cout << left_height << "\t" << right_height << endl;
	  }
	  break;
	  case 'v':
	  {
		  stim_vanish_when_reached=!stim_vanish_when_reached;
	  }
	  break;
	  case 'w':
	  {
		 BG_WHITE=(BG_WHITE+1)%2;
	  }
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
	
	/*cerr << trial.getCurrent()["AbsDepth"] << "\t" << 
		trial.getCurrent()["ObjHeight"] << "\t" <<
		trial.getCurrent()["HapticFB"] << endl;
	*/
	//cerr << parameters.find("fObjHeight",1) << endl;

	markers = optotrak->getAllMarkers();
	// Coordinates picker
	allVisiblePlatform = isVisible(markers[1].p);
	allVisibleIndex = isVisible(markers[13].p) && isVisible(markers[14].p) && isVisible(markers[16].p);
	allVisibleThumb = isVisible(markers[15].p) && isVisible(markers[17].p) && isVisible(markers[18].p);
	allVisibleFingers = allVisibleIndex && allVisibleThumb;

	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[1].p);

	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

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
		// check for finger occlusion
		if ( !allVisibleFingers )
		{
			fingersOccluded=1;
			num_lost_frames += 1;
		}
		
		// advance frame number
		frameN++;

		// check that we're at the start
		// if so, keep resetting timer
		if(((-500) < index.y()) && (index.y() < (-315)) && ((-500) < thumb.y()) && (thumb.y() < (-315)))
		{
			handAtStart = true;
			timer.start();
		} else // we've moved from the start, begin counting
		{
			handAtStart = false;
		}
		
		// if we're in the reaching area
		if(!handAtStart && ((-315) <= index.y()) && (index.y() < 200) && ((-315) <= thumb.y()) && (thumb.y() < 100))
		{
			started=true; // we've started
		}else{
			started=started;
		}

		// find y_dist and z_dist
		y_dist = (abs(index.y() - (-25)) + abs(thumb.y() - (-25)));
		z_dist = (abs(index.z() - (trial.getCurrent()["AbsDepth"])) + abs(thumb.z() - (trial.getCurrent()["AbsDepth"])));
		within_time_limit = (timer.getElapsedTimeInMilliSec() <= str2num<double>(parameters.find("TimeLimit")));

		if(!reachedObject && (stimPosn==LEFT) && started) {
			x_dist = (abs(index.x() - (-26)) + abs(thumb.x() - (-26)));
			if ( (x_dist <= 25) && (y_dist <= 70) && (z_dist <= 25) && within_time_limit) {
				reachedObject = true;
				TGA_frame = frameN;
			}
		}else if(!reachedObject && (stimPosn==RIGHT) && started) {
			x_dist = (abs(index.x() - (74)) + abs(thumb.x() - (74)));
			if ( (x_dist <= 25) && (y_dist <= 70) && (z_dist <= 25) && within_time_limit) {
				reachedObject = true;
				TGA_frame = frameN;
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
			//eyeRight.transpose() << "\t" <<					//eyeRXraw, eyeRYraw, eyeRZraw
			//eyeLeft.transpose() << "\t" <<					//eyeLXraw, eyeLYraw, eyeLZraw
			fingersOccluded << "\t" <<						//fingersOccluded
			attempt << "\t" <<	
			reachedObject << "\t" <<	
			num_lost_frames
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
{   glClearColor(1.0,1.0,1.0,1.0);
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
	trial.print();
	//initTrial();
}

void initGLVariables()
{
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluQuadricDrawStyle(qobj, GLU_FILL );

	// ########## Ora inizializza gli 4 rods
		cylinder[0].setNpoints(100);
		cylinder[0].setRadiusAndHeight(1,60); // raggio (mm) altezza (mm)
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[0].compute();
		stimDrawer[0].setStimulus(&cylinder[0]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[0].setSpheres(true);
		stimDrawer[0].initList(&cylinder[0], glRed);
		
		// build ellipsoid
		ellipsoid.setNpoints(100);
		ellipsoid.setAxesAndHeight(40,40,60);
		ellipsoid.compute();
		stimDrawer[1].setStimulus(&ellipsoid);
		stimDrawer[1].setSpheres(true);
		stimDrawer[1].initList(&ellipsoid, glRed);

		// build back surface
		cylinder[2].setNpoints(140);
		cylinder[2].setRadiusAndHeight(45,0,60);
		cylinder[2].compute();
		stimDrawer[2].setStimulus(&cylinder[2]);
		stimDrawer[2].setSpheres(true);
		stimDrawer[2].initList(&cylinder[2], glRed);

		// build front surface
		cylinder[3].setNpoints(100);
		cylinder[3].setRadiusAndHeight(25,0,25);
		cylinder[3].compute();
		stimDrawer[3].setStimulus(&cylinder[3]);
		stimDrawer[3].setSpheres(true);
		stimDrawer[3].initList(&cylinder[3], glRed);
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open("C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersEvanHaptic2.txt");
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	
	// Markersfile directory
	string dirName  = "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/spatial2/" + subjectName;
	mkdir(dirName.c_str());

	// Principal streams file
    string responseFileName =  "C:/Users/visionlab/Google Drive/DATA/Evan-haptic/spatial2/" + parameters.find("SubjectName") + "/evan-haptic_" + subjectName + ".txt";

	// Check for output file existence 
	/// Response file
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());

	responseFile << fixed << "subjName\ttrialN\tstimPosn\tRodVisibleTime\tAbsDepth\ttrialDuration\tcondition\tbg_white" << endl; //\tHapticFB\trandCond\trandIncrement" << endl;

	globalTimer.start();
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(7500,4200);
}

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	cout << "Select a condition (0-3): "; //
	cin >> stimCond;
	if (stimCond>3) { // randomize
		rand_match = rand()%2;
		if (stimCond==4){ // Conditions 0 (match) and 2 (mismatch)
			if (rand_match)
				stimCond = 0;
			else
				stimCond = 2;
		}else if (stimCond==5){
			if (rand_match)
				stimCond = 1;
			else
				stimCond = 3;
		}
	}
		
	cout << "Background white? ";
	cin >> BG_WHITE;
	cout << "Debug no arm? (0/1): "; //
	cin >> debug_no_motion;

	// Initializes the optotrak and starts the collection of points in background
    initMotors();
	initOptotrak();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 1280, 1024);//640,480 );
        glutCreateWindow("EXP WEXLER");
		glutFullScreen();
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
	initVariables(); // variable "trial" is built
	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
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
