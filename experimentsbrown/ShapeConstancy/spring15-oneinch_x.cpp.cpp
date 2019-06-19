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
double mirrorAlignment=0.0, screenAlignmentZ=0.0, screenAlignmentY=0.0;
Screen screen;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[6];
CylinderPointsStimulus cylinder[6];
//bool isStimulusDrawn=true;
bool isSquareDrawn=false;
int leftButtonDown = 0, rightButtonDown = 0;
// square measures
Timer timer;
Timer globalTimer;
bool pause = false, paused = false;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, physicalRigidBodyTip(0,0,0);
vector <Marker> markers;
static double interoculardistance=0.0;

/********* CALIBRATION VARIABLES *********/
bool headCalibration=false;
int headCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisiblePlatform=false;
bool visibleInfo=true;

/********* TRIAL VARIABLES *********/
static const int TWORODS=0;
int trialMode = TWORODS;
int trialNumber = 0;
ParametersLoader parameters;
TrialGenerator<double> trial;
int occludedFrames=0;
bool frameShown=true, isStimulusDrawn=false;;
int viewingFrames=0;
int responsekey=0;
double jitter=0.0;

ParStaircase parStairCase;
bool resp;
double xsep = 0.0;

/********** STREAMS **************/
ofstream responseFile, markersFile;

/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
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
void drawTwoRods(double deltax);
void drawLetter(int letter);
void drawCross();
void drawBlack();
void drawBreak();

/*************************** EXPERIMENT SPECS ****************************/

// experiment directory
string experiment_directory = "S:/Domini-ShapeLab/carlo/2014-2015/oneinch/";

// paramters file directory and name
string parametersFile_directory = experiment_directory + "parametersOneInch.txt";

// response file name
string responseFile_name = "spring15_oneinch_X_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tStairID\tStairState\tReversals\tAscending\tresponsekey\tresp\ttime\tstepsDone\tAbsDepth\tmirrorTheta";

/*************************** FUNCTIONS ***********************************/


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
	
	if(interoculardistance>0.0)
	{
		text.draw("####### SUBJECT #######");
		text.draw("#");
		text.draw("# Name: " +parameters.find("SubjectName"));
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
	text.draw("# Screen Alignment Y = " +stringify<double>(screenAlignmentY));
	text.draw("# Screen Alignment Z = " +stringify<double>(screenAlignmentZ));
	text.draw("# Fixation Z = " +stringify<double>(markers[19].p.x()-120.0)+ " [mm]");
	text.draw("# Fixation X = " +stringify<double>(markers[19].p.z()+363.0)+ " [mm]");

	if(interoculardistance>0.0)
	{
		glColor3fv(glWhite);
		text.draw("#");
		text.draw("# trial: " +stringify<double>(trialNumber));
		text.draw("#");
		text.draw("#######################");
		text.leaveTextInputMode();
	}

	}
}

void drawCalibration()
{
	glColor3fv(glWhite);
}

void drawTwoRods(double deltax)
{

	// Rear Left rod
	glPushMatrix();
	glTranslated(-deltax/2, 0, -12.7); // rods are half an inch to the ba
	stimDrawer[0].draw();
	glPopMatrix();

	// Rear right rod
	glPushMatrix();
	glTranslated(deltax/2, 0, -12.7);
	stimDrawer[0].draw();
	glPopMatrix();
}

void drawBlack()
{
	glColor3fv(glBlack);
	glBegin(GL_QUADS);
		glVertex3f(-30.0f, 50.0f, 0.0f);		// Top Left
		glVertex3f( 30.0f, 50.0f, 0.0f);		// Top Right
		glVertex3f( 30.0f,-50.0f, 0.0f);		// Bottom Right
		glVertex3f(-30.0f,-50.0f, 0.0f);		// Bottom Left
	glEnd();
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

	if(allVisibleHead || !headCalibration)
	{
		if(	isStimulusDrawn )
		{
			glLoadIdentity();

			// ###### draw rotated stimulus (adjusted) ######
			glTranslated(0.0,0,trial.getCurrent().first["AbsDepth"]);

			xsep = trial.getCurrent().second->getCurrentStaircase()->getState();
	
			drawTrial(xsep); // draw two rods

			glLoadIdentity();
		}

	}

}

void drawTrial(double delz)
{

switch (trialMode)
{
case TWORODS:
	drawTwoRods(delz);
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
	responsekey=0;
	xsep = 0.0;
	trialMode=0;

	// roll on
	isStimulusDrawn=false;
	drawGLScene();
	initProjectionScreen(trial.getCurrent().first["AbsDepth"]);
	Sleep(500);
	isStimulusDrawn=true;
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
                    trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getAscending()<< "\t" <<
                    responsekey << "\t" <<
                    response << "\t" <<
                    timer.getElapsedTimeInMilliSec() << "\t" <<
					trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
					trial.getCurrent().first["AbsDepth"] << "\t" <<
					mirrorAlignment << "\t" <<
					endl;
	resp=response;

	if(abs(mirrorAlignment - 45.0) < 0.25)
	{
		trial.next(response);
		trialNumber++;

		if( !trial.isEmpty() )
			initTrial();
		else
		{	
			cleanup();
			exit(0);
		}
	} else
		visibleInfo = !visibleInfo;
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
        if ( headCalibrationDone == 2 && allVisiblePatch || !headCalibration )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[10].p, markers[11].p,markers[12].p,interoculardistance );
            headCalibrationDone=3;
			//headCalibration=true;
			visibleInfo=false;
			initTrial();
        }
    }
    break;
	case '4':
		{
			if(headCalibrationDone==3 && timer.getElapsedTimeInMilliSec()>500)
			{
				responsekey=4;
				if ( trial.isEmpty() )
					exit(0);
				advanceTrial(false);
			}
		}
	break;
	case '6':
		{
			if(headCalibrationDone==3 && timer.getElapsedTimeInMilliSec()>500)
			{
				responsekey=6;
				if ( trial.isEmpty() )
					exit(0);
				advanceTrial(true);
			}
		}
	break;
	case 'c':
		headCalibration = !headCalibration;
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
		moveScreenAbsoluteAsynchronous(_focalDist,homeFocalDistance,5000);
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
	allVisiblePlatform = true;
	
	allVisiblePatch = isVisible(markers[10].p) && isVisible(markers[11].p) && isVisible(markers[12].p);
	allVisibleHead = allVisiblePatch && isVisible(markers[9].p);

	mirrorAlignment = asin(
			abs((markers[6].p.z()-markers[7].p.z()))/
			sqrt(
			pow(markers[6].p.x()-markers[7].p.x(), 2) +
			pow(markers[6].p.z()-markers[7].p.z(), 2)
			)
			)*180/M_PI;

	screenAlignmentY = asin(
			abs((markers[19].p.y()-markers[21].p.y()))/
			sqrt(
			pow(markers[19].p.x()-markers[21].p.x(), 2) +
			pow(markers[19].p.y()-markers[21].p.y(), 2)
			)
			)*180/M_PI;

	screenAlignmentZ = asin(
			abs(markers[19].p.z()-markers[20].p.z())/
			sqrt(
			pow(markers[19].p.x()-markers[20].p.x(), 2) +
			pow(markers[19].p.z()-markers[20].p.z(), 2)
			)
			)*180/M_PI*
			abs(markers[19].p.x()-markers[20].p.x())/
			(markers[19].p.x()-markers[20].p.x());

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
    int numMarkers=22;
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
	parametersFile.open("C:/workspace/cncsvisioncmake/experimentsbrown/parameters/parametersAmandaOneInch-x.txt");
	parameters.loadParameterFile(parametersFile);

	// Subject name
    string subjectName = parameters.find("SubjectName");
	// Block
	string block = parameters.find("Block");

		// Principal streams file
    string responseFileName =  "C:/Users/visionlab/Dropbox/Shape Lab Shared/Amanda/OneInch/AmandaOneInch-xResp_"   + subjectName + block + ".txt";

	// Check for output file existence
	/// Response file
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());

	responseFile << fixed << "subjName\tIOD\ttrialN\tStairID\tStairState\tReversals\tAscending\tresponsekey\tresp\ttime\tstepsDone\tAbsDepth\tmirrorTheta" << endl;

	globalTimer.start();
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeScreen(3500);
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
