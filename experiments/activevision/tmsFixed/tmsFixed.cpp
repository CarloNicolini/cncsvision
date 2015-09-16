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

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <MMSystem.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif


/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"

#include "GLUtils.h"
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"


/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 290.0;    // millimeters is the width of the visualization area, it depends on the monitor! 
// you should measure it with a ruler by changing glClearColor(0,0,0,1); to glClearColor(1,0,0,1); and measure the red area width

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace boost::filesystem;
using namespace boost;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
//CoordinatesExtractor headEyeCoords;
ObjLoader model;
/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  0;//-2.5 ;
static const double alignmentY =  0;//20.0;
static const double focalDistance= -480.0;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight;
vector <Vector3d> markers(18);
static double interoculardistance=65;
double eyeZ=57;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* Timing variables  ************************/
/*** Streams File ***/
ofstream responseFile;

ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
// responses
double probeAngle=0;

// Trial related things
int trialNumber=0;
bool experimentStarted=false;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;

/***** SOUND THINGS *****/
void beepOk()
{
    //Beep(440,440);
    PlaySound((LPCSTR) "D:\\workspace\\cncsvision\\data\\beep\\beep-2.wav", NULL, SND_FILENAME | SND_ASYNC);
	return;
}

void beepBad()
{
    //Beep(440,440);
    PlaySound((LPCSTR) "D:\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
	return;
}

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
int trialMode = FIXATIONMODE;

/********** CONTROLS AND DEVICES **********/
int mouseButton=-1;
Timer globalTimer;
/** EXPERIMENTAL VARIABLES **/
string subjectName;

/*************************** FUNCTIONS ***********************************/
void drawCircle(double centerx, double centery, double radius)
{
	glColor3fv(glWhite);
	glBegin(GL_LINE_STRIP);
	for (double i=0; i<2*M_PI; i+=(2*M_PI/360.0) )
	{
		glVertex3d(radius*cos(i)+centerx, radius*sin(i)+centery,focalDistance);
	}
	glEnd();
	glColor3fv(glRed);
}

void cleanup()
{   // Close all the file streams
    responseFile.close();
}

/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{   trialMode++;
    trialMode=trialMode%3;
    globalTimer.start();
    //boost::thread beepThread(beepOk);
    //beepThread.detach();
}

void keyPressed()
{
    double responseTime=globalTimer.getElapsedTimeInMilliSec();

    bool contraction = ( factors["Tilt"]==90 ) || (factors["Tilt"]==0);
    if ( trial.getRemainingTrials()==0 )
    {
        responseFile << setw(6) << left <<  trialNumber << " " << factors["Slant"] << " " << probeAngle << " " << responseTime << " " << endl;
        cleanup();
        exit(0);
    }
    else
    {
        responseFile << setw(6) << left <<  trialNumber << " " << factors["Slant"] << " " << probeAngle << " " << responseTime << " " << endl;
        factors = trial.getNext();
        trialNumber++;
    }
    advanceTrial();
    if ( parameters.find("DrawOccluder") == "1" )
    {
        int punti = atoi(parameters.find("NPoints").c_str());
        redDotsPlane.setNpoints(punti);
        redDotsPlane.setDimensions(100,100,0.1);

    }
    else
    {
        redDotsPlane.setNpoints(500);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane.setDimensions(150,150,0.1); 
    }
    //redDotsPlane.setSlantTilt(factors["Slant"], (int) factors["Tilt"] );
	stimDrawer.setSpheres(true);
    stimDrawer.initList(&redDotsPlane);
}


void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
	glPointSize(2);
    glColor3fv(glRed);
    glPushMatrix();
    // Reset every previous transformation, put the stimulus in (0,0,0) with no rotation ( fronto-parallel )
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glRotated( factors["Slant"] , 0,1,0 );
	glScaled(1/sin(toRadians( -90-factors["Slant"])),1,1);
    stimDrawer.draw();
    glPopMatrix();

}


void drawProbe()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

	if ( experimentStarted )
	{
	glColor3fv(glRed);
    glPushMatrix();
	glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(1,10,10);
    glPopMatrix();
	}
	/* This circle should appear 10 cm radius if calibration is correct
	glPushMatrix();
	glLoadIdentity();
	drawCircle(0,0,100);
	glPopMatrix();
	*/
}

void drawFixation()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    /** DRAW THE FIXATION POINT **/
	if ( experimentStarted )
	{
    glColor3fv(glRed);
    glPushMatrix();
	glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(1,10,10);
    glPopMatrix();
	}
}

void drawOcclusion()
{
    double holeRadius=55; // 35
    double tanMaxSlant=tan(M_PI/4);
    glColor3fv(glBlack);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeZ+focalDistance+holeRadius/tanMaxSlant);
    glScalef(holeRadius,holeRadius,2000); //diameter of inner hole is 5 cm @ focalDistance
    glRotated(90,1,0,0);
    model.draw();
    glPopMatrix();

}


void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();
    }
    break;
    case PROBEMODE :
    {   drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
        if ( atoi(parameters.find("DrawOccluder").c_str())==1 )
            drawOcclusion();
    }
    break;
    default:
    {
        drawFixation();
    }
    }
}

void initVariables()
{
    interoculardistance = str2num<double>(parameters.find("IOD"));
    trial.init(parameters);
    //trial.print();
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1

    if ( parameters.find("DrawOccluder") == "1" )
    {
        int punti = atoi(parameters.find("NPoints").c_str());
        redDotsPlane.setNpoints(punti);
        redDotsPlane.setDimensions(100,100,0.1);

    }
    else
    {
        redDotsPlane.setNpoints(500);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane.setDimensions(150,150,0.1);
    }
    //redDotsPlane.setSlantTilt(factors["Slant"], (int) factors["Tilt"] );
    redDotsPlane.compute();
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.setSpheres(false);
    stimDrawer.initList(&redDotsPlane);

    //headEyeCoords.init(Vector3d(-interoculardistance/2,0,0),Vector3d(interoculardistance/2,0,0), Vector3d(0,0,0),Vector3d(0,10,0),Vector3d(0,0,10),interoculardistance );
    model.load("../data/objmodels/occluder.obj");
}

void initStreams()
{
    string parametersPassiveFileName("parametersFixed.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path, press Enter to exit" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);
#ifdef _WIN32
    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !exists(baseDir) )
        create_directory(baseDir);

    // Subject name
    string subjectName = parameters.find("SubjectName");

    // Principal streams file
    string responseFileName("responseFile_");

    // Add the subject name to file extension
    responseFileName		+=string(subjectName)+".txt";

    // Check for output file existence
    /** Response file **/
    if ( !util::fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());
#endif

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

    /** END LIGHTS **/
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
    else	// MONOCULAR
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
        drawTrial();
        glutSwapBuffers();
    }
}



void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {   cleanup();
        exit(0);
    }
    break;
	case ' ':
		{
		if ( !experimentStarted )
		{
			experimentStarted=true;
			trialMode=FIXATIONMODE;
			globalTimer.start();
		}
		}
		break;
    case 'p':
    {
		
        // Print the current trial
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            cerr << iter->first << " " << iter->second << " ";
        }
        cerr << endl;
    }
    break;
    case '4':
    {
        if ( trialMode == PROBEMODE )
        {
			beepOk();
            probeAngle=0;
            keyPressed();
        }
    }
    break;
    case '6':
    {
        if ( trialMode == PROBEMODE )
        {
			beepOk();
            probeAngle=180;
            keyPressed();
        }
    }
    break;
    case '+':
        eyeZ+=1;
        cout << eyeZ << endl;
        break;
    case '-':
        eyeZ-=1;
        cout << eyeZ << endl;
        break;
    }
}

void mouseFunc(int button, int state, int x, int y)
{
    if ( trialMode == PROBEMODE )
    {
        switch ( button )
        {
        case  GLUT_LEFT_BUTTON:
            probeAngle=0;
            keyPressed();
            break;
        case GLUT_RIGHT_BUTTON:
            probeAngle=180;
            keyPressed();
            break;
        }
    }
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
	if ( experimentStarted )
	{
	if ( trialMode == STIMULUSMODE )
	{
    if ( globalTimer.getElapsedTimeInMilliSec() > str2num<double>(parameters.find("StimulusDuration")) && trialMode != PROBEMODE )
        advanceTrial();
	}
	if ( trialMode == FIXATIONMODE )
	{
    if ( globalTimer.getElapsedTimeInMilliSec() > str2num<double>(parameters.find("FixationDuration")) && trialMode != PROBEMODE )
        advanceTrial();
	}
	}
    //headEyeCoords.update(markers[1],markers[2],markers[3]);
    //eyeLeft = headEyeCoords.getLeftEye();
    //eyeRight = headEyeCoords.getRightEye();
	eyeLeft = Vector3d(-interoculardistance/2,0,0);
	eyeRight = -eyeLeft;
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);

    cam.init(screen);

}
int main(int argc, char*argv[])
{
    mathcommon::randomizeStart();
    initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP WEXLER");
//glutFullScreen();
    }
    else
	{   glutGameModeString("1024x768:32@120");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
	glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    globalTimer.start();
    glutMainLoop();

    cleanup();

    return 0;
}
