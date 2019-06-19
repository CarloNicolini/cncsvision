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
#include "ParametersLoader.h"
#include "IOCommon.h"
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

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 6 // 85 hz
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(179,182,-73);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  49.25;
double alignmentY =  32.75;
double focalDistance= -270.0, homeFocalDistance=-270.0;
static const Vector3d center(0,0,focalDistance);
Screen screen;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor headEyeCoords, thumbCoords,indexCoords;
/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer stimDrawer[3];
CylinderPointsStimulus cylinder[3];
bool isStimulusDrawn=true;
bool isSquareDrawn=true;
int leftButtonDown = 0, rightButtonDown = 0;
// square measures
double edge = 0.0, dedge = 0, jitterX = 0.0;
Timer timer;
/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, index, thumb;
vector <Marker> markers;
static double interoculardistance=66;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisiblePatch=false;
bool visibleInfo=false;

/********* TRIAL VARIABLES *********/
static const int CALIBRATIONMODE=0;
static const int STIMULUSMODE=1;
static const int SQUAREMODE=2;
int trialMode = SQUAREMODE;
int trialNumber = 0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map<string,double> factors;

/********** STREAMS **************/
ofstream responseFile, markersFile;


/********** FUNCTION PROTOTYPES *****/
void beepOk(int tone);
void drawCircle(double radius, double x, double y, double z);
void cleanup();
void drawInfo();
void drawCalibration();
void drawSquare();
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


/***** SOUND THINGS *****/
void beepOk(int tone)
{
	switch(tone)
	{
	case 0:
    // Remember to put double slash \\ to specify directories!!!
    PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
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
    g
    optotrak->stopCollection();
    delete optotrak;
}

void drawInfo()
{
	if ( visibleInfo )
	{
	GLText text;	
	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
	text.enterTextInputMode();
	text.draw("HeadCalibration= " + stringify<int>(headCalibrationDone) );
	if ( isVisible(markers[1].p) && isVisible(markers[2].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(1)+stringify< Eigen::Matrix<double,1,3> > (markers[1].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(2)+stringify< Eigen::Matrix<double,1,3> > (markers[2].p.transpose())+ " [mm]" );
	
	glColor3fv(glWhite);
	text.draw("Marker "+ stringify<int>(3)+stringify< Eigen::Matrix<double,1,3> > (markers[3].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(4)+stringify< Eigen::Matrix<double,1,3> > (markers[4].p.transpose())+ " [mm]" );
	
	if ( isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p) )
		glColor3fv(glGreen);
	else
		glColor3fv(glRed);
	text.draw("Marker "+ stringify<int>(5)+stringify< Eigen::Matrix<double,1,3> > (markers[5].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(6)+stringify< Eigen::Matrix<double,1,3> > (markers[6].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(7)+stringify< Eigen::Matrix<double,1,3> > (markers[7].p.transpose())+ " [mm]" );
	text.draw("Marker "+ stringify<int>(9)+stringify< Eigen::Matrix<double,1,3> > (markers[9].p.transpose())+ " [mm]" );
	
	glColor3fv(glWhite);

	text.draw("EyeRight= "+stringify< Eigen::Matrix<double,1,3> > (eyeRight.transpose())+ " [mm]" );
	text.draw("EyeLeft= "+stringify< Eigen::Matrix<double,1,3> > (eyeLeft.transpose())+ " [mm]" );
	text.draw("Alignment(X,Y)= " +stringify<double>(alignmentX)+","+stringify<double>(alignmentY));
	text.draw("IOD = " +stringify<double>((eyeLeft-eyeRight).norm()));
	text.leaveTextInputMode();
	}
}

void drawCalibration()
{
	glColor3fv(glWhite);
	drawCircle(50,0,0,focalDistance);
	drawCircle(0.2,0,0,focalDistance);
}

void drawSquare()
{
	if (isSquareDrawn)
	{
		//double edge = factors["SquareSize"]*0.5;
		edge = str2num<double>(parameters.find("SquareSize")) + dedge + jitterX;
		glColor3fv(glRed);
		glBegin(GL_LINE_LOOP);
		glVertex3d(edge,edge,focalDistance);
		glVertex3d(edge,-edge,focalDistance);
		glVertex3d(-edge,-edge,focalDistance);
		glVertex3d(-edge,edge,focalDistance);
		glEnd();
	}
}

void drawStimulus()
{
	if ( isStimulusDrawn )
	{
	// Central rod on focal plane
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,factors["AbsDepth"]+factors["RelDepth"]/2);
	stimDrawer[0].draw();
	glPopMatrix();

	// Left rod shifted 2 cm behind the focal plane
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,factors["AbsDepth"]-factors["RelDepth"]/2);
	stimDrawer[1].draw();
	glPopMatrix();

	// Right rod shifted 2 cm behind the focal plane
	glPushMatrix();
	glLoadIdentity();
	glTranslated(50,0,factors["StimDepth"]-factors["StimRelativeDepth"]);
	stimDrawer[2].draw();
	glPopMatrix();
	}
}

void drawFingers()
{
	// Draw the thumb
	glPushMatrix();
	glLoadIdentity();
	glTranslated(thumb.x(),thumb.y(),thumb.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
	// Draw the index
	glPushMatrix();
	glLoadIdentity();
	glTranslated(index.x(),index.y(),index.z());
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
}

void drawTrial()
{

switch (trialMode)
{
case CALIBRATIONMODE:
	drawCalibration();
break;
case STIMULUSMODE:
	drawStimulus();
break;
case SQUAREMODE:
	drawSquare();
break;
}

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
        drawTrial();
		drawInfo();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
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

void initTrial()
{
	factors = trial.getNext();
	isSquareDrawn=false;
	drawGLScene();
	initProjectionScreen(factors["AbsDepth"]);
	beepOk(0);
	dedge = 0;
	jitterX = unifRand(str2num<double>(parameters.find("SideSquareLowBound"))/2,
		str2num<double>(parameters.find("SideSquareUpBound"))/2);
	timer.start();
	isSquareDrawn=true;
}
void advanceTrial()
{
	beepOk(0);
	double timeElapsed = timer.getElapsedTimeInMilliSec();
	responseFile << trialNumber << " " <<
					edge << " " << 
					jitterX + factors["SquareSize"] << " " <<
					factors["AbsDepth"] << " " <<
					factors["RelDepth"]  << " " <<
					timeElapsed
					<< endl;
	trialNumber++;
	if(trial.getRemainingTrials() != 0)
		initTrial();
	else
	{
		cleanup(); // shut down Optotrak
		exit(0);
	}
}

void mouseMotionFunc(int x, int y)
{
	if(leftButtonDown == 1)
		dedge -= 0.25;
	if(rightButtonDown == 1)
		dedge += 0.25;
}

// Funzione di callback per gestire azioni del mouse quando viene attivato da un click
void mouseFunc(int button, int state, int x, int y)
{
	// Implementala come vuoi
	cerr << button << " " << state << endl;
	if (button==GLUT_LEFT_BUTTON)
	{
		 if(state==0)
			leftButtonDown = 1;
		else
			leftButtonDown = 0;
	}
	
	if (button==GLUT_RIGHT_BUTTON)
	{
		 if(state==0)
			rightButtonDown = 1;
		else
			rightButtonDown = 0;
	}
	
	if (button==GLUT_MIDDLE_BUTTON)
		 if(state==0) 
		 {
			 advanceTrial();
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
		factors = trial.getNext();
		isStimulusDrawn=false;
		drawGLScene();
		initProjectionScreen(factors["AbsDepth"]);
		isStimulusDrawn=true;
		break;
	case 'i':
		visibleInfo=!visibleInfo;
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
		// trialMode++;
		// trialMode=trialMode%3;
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
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=3;
			visibleInfo=false;
        }
    }
    break;
      case '2':
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
	  // In genere a me piace attaccare al tasto p una funzione che stampi 
	  // le tue variabili di interesse su standard output ad esempio la lista dei fattori attuali con nome
    case 'p':
	{
		for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter )
		{
			cout << iter->first << " " << iter->second << endl;
		}
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
		moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
	else
		moveScreenAbsoluteAsynchronous(_focalDist,homeFocalDistance,3500);
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
    markers = optotrak->getAllMarkers();
    // Coordinates picker
	allVisiblePatch = isVisible(markers[5].p) && isVisible(markers[6].p) && isVisible(markers[7].p);
    headEyeCoords.update(markers[5],markers[6],markers[7]);
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();
	
	eyeRight = Vector3d(interoculardistance/2,0,0);
	eyeLeft = -eyeRight;

	thumb = thumbCoords.getP1();
	index = indexCoords.getP1();
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
    if ( optotrak->init("Aligned20111014",NULL,NULL) != 0)
    {   cleanup();
		cerr << "Something during Optotrak initialization failed, press ENTER to continue. A error log has been generated, look \"opto.err\" in this folder" << endl;
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
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open("../parametersFiles/parameters.txt");
	parameters.loadParameterFile(parametersFile);
	// Riempie la lista dei fattori a partire da quanto descritto nei fattori dell'oggetto parameters
	// Per dubbi prova i vari esempi di test del modulo experiments
	trial.init(parameters);
	initTrial();
}

void initGLVariables()
{
	// Ora inizializza i 3 rods (stimoli)
	for (int i=0; i<3; i++)
	{
		cylinder[i].setNpoints(500);
		cylinder[i].setRadiusAndHeight(2.5,60); // raggio 25 mm altezza 60 mm
		// Dispone i punti random sulla superficie cilindrica 
		cylinder[i].compute();
		stimDrawer[i].setStimulus(&cylinder[i]);
		// seguire questo ordine altrimenti setspheres non ha effetto se chiamata dopo StimulusDrawer::initList
		stimDrawer[i].setSpheres(false);
		stimDrawer[i].initList(&cylinder[i]);
	}
}

// Inizializza gli stream, apre il file per poi scriverci
void initStreams()
{
	responseFile.open("responseFile.txt");
	markersFile.open("markersFile.txt");
}
// Porta tutti i motori nella posizione di home e azzera i contatori degli steps
void initMotors()
{
	homeEverything(5000,3500);
}

int main(int argc, char*argv[])
{

// Initializes the optotrak and starts the collection of points in background
    initMotors();
	
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
	
	// Meccanismo di callback di glut - Mantenere questa porzione di codice intatta
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotionFunc);
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
