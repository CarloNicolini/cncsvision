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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/filesystem.hpp>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#include <windows.h>
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "GLMaterial.h"
#include "GLLight.h"
#include "GLUtils.h"
#include "GLMaterial.h"
#include "Util.h"
#include "Imageloader.h"
#include "ObjLoader.h"
#include "VRCamera.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "BalanceFactor.h"
#include "GLText.h"
#include "Timer.h"
#include "StimulusDrawer.h"
#include "EllipsoidPointsStimulus.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "LatestCalibration.h"

/********* #DEFINE DIRECTIVES **************************/
//#define TIMER_MS 15
//#define SCREEN_WIDTH  1024      // pixels
//#define SCREEN_HEIGHT 768       // pixels
//static const double SCREEN_WIDE_SIZE = 290.0;    // millimeters is the width of the visualization area, it depends on the monitor! 
// you should measure it with a ruler by changing glClearColor(0,0,0,1); to glClearColor(1,0,0,1); and measure the red area width

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace boost::filesystem;
using namespace boost;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
static const double focalDistance= -500.0; //Millimeters

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES  **********************/
Vector3d eyeLeft, eyeRight;
static double interoculardistance=65;
double frame=0.0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;
bool orthoMode=true;
/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
// Trial related things
int trialNumber=0;
ParametersLoader parameters;
TrialGenerator<double> trial;
map <string, double> factors;
pair<map <string, double>,ParStaircase*> factorStaircasePair;

EllipsoidPointsStimulus cylinderFactor,cylinderStaircase,cylinderMask;
StimulusDrawer stimDrawerFactor, stimDrawerStaircase, stimDrawerMask;

static const int textureHeight = 1024;

static const int STIMULUSMODE0=0;
static const int MASKMODE=1;
static const int STIMULUSMODE1=2;
static const int PROBEMODE=3;

int trialMode = STIMULUSMODE0;
int prevTrialMode=STIMULUSMODE1;

int trialmodes[4]={STIMULUSMODE0,MASKMODE,STIMULUSMODE1,PROBEMODE};
int trialIndex=0;

bool drawInfo=true;
bool experimentStarted=false;

double dotsSize=10;

/*** EXPERIMENTAL VARIABLES ***/
string subjectName;

float radiusX=50.0;
float height=radiusX;
float radiusZ=radiusX;
bool horizontal=false;
Timer trialTimer;
double stimulusTime=1000, maskTime=500; //presentation time in milliseconds, is set to 1000 by default, must be specified in the parameters

/*************************** FUNCTIONS ***********************************/
/*
void advanceTrial(bool answer)
{
    trial.next(answer);

    if ( trial.isEmpty() )  // The experiment finished
    {
        cerr << "Experiment finished!" << endl;
        exit(0);
    }


    factorStaircasePair = trial.getCurrent();
    factors = factorStaircasePair.first;
    
    double scaleZ = factorStaircasePair.second->getCurrentStaircase()->getState();
    
    int ncirclesFactorStim =util::str2num<double> (parameters.find("DensityPointOnSurfaceFactor"))*mathcommon::ellipseCircumferenceBetter(radiusX,factors["ScaleZ"]*radiusX)*height;
    int ncirclesStaircaseStim =             util::str2num<double> (parameters.find("DensityPointOnSurfaceStaircase"))*mathcommon::ellipseCircumferenceBetter(radiusX,scaleZ*radiusX)*height;

    int ncirclesMask = util::str2num<double> (parameters.find("DensityPointOnSurfaceMask"))*radiusX*height;

    // Cylinder staircase
    cylinderStaircase.setNpoints(ncirclesStaircaseStim);
    cylinderStaircase.setAxesAndHeight(radiusX,radiusX*scaleZ,height);
    cylinderStaircase.compute();

    // Cylinder mask
    cylinderMask.setNpoints(ncirclesMask);
    cylinderMask.setAxesAndHeight(radiusX,0,height);
    cylinderMask.compute();

    // Cylinder factor
    cylinderFactor.setNpoints(ncirclesFactorStim);
    cylinderFactor.setAxesAndHeight(radiusX,radiusX*factors["ScaleZ"],height);
    cylinderFactor.compute();

    // Set the stimulus drawers
    // Staircase
    stimDrawerStaircase.initList(&cylinderStaircase,glWhite,dotsSize);
    // Mask
    stimDrawerMask.initList(&cylinderMask,glWhite,dotsSize);
    // Factor
    stimDrawerFactor.initList(&cylinderFactor,glWhite,dotsSize);

    responseFile << trialNumber << "\t" <<
                    factors["ScaleZ"] <<"\t" <<
                    factorStaircasePair.second->getCurrentStaircase()->getID()  << "\t" <<
                    factorStaircasePair.second->getCurrentStaircase()->getState() << "\t" <<
                    factorStaircasePair.second->getCurrentStaircase()->getInversions() << "\t" <<
                    factorStaircasePair.second->getCurrentStaircase()->getAscending()
                 << endl;
    trialNumber++;
    radiusZ = radiusX*factorStaircasePair.second->getCurrentStaircase()->getState();
}
*/
void drawStimulus()
{
    if (trialMode==STIMULUSMODE0)
    {
        glPushMatrix();
        if (horizontal)
            glRotated(90,0,0,1);
        //glRotated(90,1,0,0);
        stimDrawerFactor.draw();
        //drawCylinderElliptic( radiusX,height,radiusX*factors["ScaleZ"],1E-1);
        glPopMatrix();
    }
    if (trialMode == MASKMODE )
    {
        glPushMatrix();
        if (horizontal)
            glRotated(90,0,0,1);
        //glRotated(90,1,0,0);
        stimDrawerMask.draw();
        //drawCylinderElliptic( radiusX,height,0,2.5*1E-1);
        glPopMatrix();
    }
    if (trialMode==STIMULUSMODE1)
    {
        glPushMatrix();
        double staircaseScale = factorStaircasePair.second->getCurrentStaircase()->getState();
        if (horizontal)
            glRotated(90,0,0,1);
        //glRotated(90,1,0,0);
        stimDrawerStaircase.draw();
        //drawCylinderElliptic( radiusX,height,radiusX*staircaseScale,2.5*1E-1);
        glPopMatrix();
    }
}

void drawProbe()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(1,20,20);
    glPopMatrix();
}

void drawTrial()
{
    if ( drawInfo )
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            text.draw(iter->first + " = " + util::stringify<double>(iter->second) );
        }
        string stairstate =
                "ID= "  + util::stringify<int>(factorStaircasePair.second->getCurrentStaircase()->getID())
                + " State= " +util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getState())
                + " Ascending= " + util::stringify<int>(factorStaircasePair.second->getCurrentStaircase()->getAscending())
                + " Inversions= " + util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getInversions())
                + " Steps= " +(util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getStepsDone())) ;
        text.draw(stairstate);
        text.draw(util::stringify<int>(trialTimer.getElapsedTimeInMilliSec() ));
        if (trialMode==STIMULUSMODE0)
            text.draw("Presenting REFERENCE (factor)");
        if (trialMode==STIMULUSMODE1)
            text.draw("Presenting TEST (staircase)");
        if (trialMode==MASKMODE)
            text.draw("Presenting MASK");
        if (trialMode==PROBEMODE)
        {
            text.draw("Presenting PROBE");
            if (prevTrialMode==STIMULUSMODE0)
                text.draw("Previous mode= REFERENCE (factor)");
            if (prevTrialMode==STIMULUSMODE1)
                text.draw("Previous mode= TEST (staircase)");
            if (prevTrialMode==MASKMODE)
                text.draw("Previous mode= MASK");
            if (prevTrialMode==PROBEMODE)
                text.draw("Previous mode= PROBEMODE");
        }
        text.leaveTextInputMode();
    }

    if (!experimentStarted)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    if (trialMode!=PROBEMODE)
        drawStimulus();
    else
        drawProbe();
    glPopAttrib();
    // Restore base state
    getGLerrors();
}

void initVariables()
{
    drawInfo = (bool)(util::str2num<int>(parameters.find("DrawInfo")));
    cam.setOrthoGraphicProjection((bool)str2num<int>(parameters.find("OrthographicMode")));
    interoculardistance = str2num<double>(parameters.find("IOD"));
    trial.init(parameters);

    dotsSize = util::str2num<double>(parameters.find("DotsSize"));
    height = util::str2num<double>(parameters.find("Height"));

    factorStaircasePair = trial.getCurrent();
    factors = factorStaircasePair.first;
    double scaleZ = factorStaircasePair.second->getCurrentStaircase()->getState();
    radiusX =  str2num<double>(parameters.find("RadiusX"));
    radiusZ = radiusX*factorStaircasePair.second->getCurrentStaircase()->getState();

    trialTimer.start();
    stimulusTime = util::str2num<double>(parameters.find("StimulusTime"));
    maskTime = util::str2num<double>(parameters.find("MaskTime"));

    int ncirclesFactorStim =util::str2num<double> (parameters.find("DensityPointOnSurfaceFactor"))*mathcommon::ellipseCircumferenceBetter(radiusX,factors["ScaleZ"]*radiusX)*height;
    int ncirclesStaircaseStim =             util::str2num<double> (parameters.find("DensityPointOnSurfaceStaircase"))*mathcommon::ellipseCircumferenceBetter(radiusX,scaleZ*radiusX)*height;

    int ncirclesMask = util::str2num<double> (parameters.find("DensityPointOnSurfaceMask"))*radiusX*height;

    // Cylinder staircase
    cylinderStaircase.setNpoints(ncirclesStaircaseStim);
    cylinderStaircase.setAperture(0,M_PI);
    cylinderStaircase.setFluffiness(0.001);
    cylinderStaircase.setAxesAndHeight(radiusX,radiusX*scaleZ,height);
    cylinderStaircase.compute();

    // Cylinder mask
    cylinderMask.setNpoints(ncirclesMask);
    cylinderMask.setAperture(0,M_PI);
    cylinderMask.setFluffiness(0.001);
    cylinderMask.setAxesAndHeight(radiusX,0,height);
    cylinderMask.compute();

    // Cylinder factor
    cylinderFactor.setNpoints(ncirclesFactorStim);
    cylinderFactor.setAperture(0,M_PI);
    cylinderFactor.setFluffiness(0.001);
    cylinderFactor.setAxesAndHeight(radiusX,radiusX*factors["ScaleZ"],height);
    cylinderFactor.compute();

    // Set the stimulus drawers
    // Staircase
    stimDrawerStaircase.initList(&cylinderStaircase,glWhite,dotsSize);
    // Mask
    stimDrawerMask.initList(&cylinderMask,glWhite,dotsSize);
    // Factor
    stimDrawerFactor.initList(&cylinderFactor,glWhite,dotsSize);
    
}

void initStreams()
{
    string parametersPassiveFileName("C:/cncsvisiondata/parametersFiles/Fulvio/expTexturing/parametersExpTexturingDiscriminationRandomDots.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path, press Enter to exit" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);
    string subjectName = parameters.find("SubjectName");
	string outputFilename = parameters.find("BaseDir")+string("responseFileDiscriminationRandomDots_") + subjectName + string(".txt");
	if ( util::fileExists(outputFilename) )
	{
		int i=0;
		cerr << "file " << outputFilename << "already exists, press Enter to quit" << endl;
		std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
	}
	else
	{
		responseFile.open( outputFilename.c_str() );
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
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(interoculardistance/2,0,0));
        glTranslated(0,0,focalDistance);
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(-interoculardistance/2,0,0));
        glTranslated(0,0,focalDistance);
        drawTrial();
        glutSwapBuffers();
    }
    else	// MONOCULAR
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
        glTranslated(0,0,focalDistance);
        drawTrial();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
    case 27:
    {
        exit(0);
    }
        break;
    case '4':
    {
        int responsekey=4;
        if (trialMode==PROBEMODE)
        {
            bool resp=false;
            if (prevTrialMode==STIMULUSMODE0)   // ha appena presentato lo stimolo del factor
            {
                resp=true;
            }
            if (prevTrialMode==STIMULUSMODE1)   // ha appena presentato lo stimolo della staircase
            {
                resp=false;
            }

            responseFile << trialNumber << "\t" <<
                            factors["ScaleZ"] <<"\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getID()  << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getState() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getInversions() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getAscending()<< "\t" <<
                            responsekey << "\t" <<
                            resp << "\t" <<
                            trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                            prevTrialMode << endl;

            trial.next(resp);

            if ( trial.isEmpty() )  // The experiment finished
            {
                cerr << "Experiment finished!" << endl;
                exit(0);
            }

            factorStaircasePair = trial.getCurrent();
            factors = factorStaircasePair.first;
            trialNumber++;

            // ************************//
            double scaleZ=factorStaircasePair.second->getCurrentStaircase()->getState();
            int ncirclesFactorStim =util::str2num<double> (parameters.find("DensityPointOnSurfaceFactor"))*mathcommon::ellipseCircumferenceBetter(radiusX,factors["ScaleZ"]*radiusX)*height;
            int ncirclesStaircaseStim =             util::str2num<double> (parameters.find("DensityPointOnSurfaceStaircase"))*mathcommon::ellipseCircumferenceBetter(radiusX,scaleZ*radiusX)*height;
            int ncirclesMask = util::str2num<double> (parameters.find("DensityPointOnSurfaceMask"))*radiusX*height;

            // Cylinder staircase
            cylinderStaircase.setNpoints(ncirclesStaircaseStim);
            cylinderStaircase.setAperture(0,M_PI);
            cylinderStaircase.setAxesAndHeight(radiusX,radiusX*scaleZ,height);
            cylinderStaircase.compute();

            // Cylinder mask
            cylinderMask.setNpoints(ncirclesMask);
            cylinderMask.setAperture(0,M_PI);
            cylinderMask.setAxesAndHeight(radiusX,0,height);
            cylinderMask.compute();

            // Cylinder factor
            cylinderFactor.setNpoints(ncirclesFactorStim);
            cylinderFactor.setAperture(0,M_PI);
            cylinderFactor.setAxesAndHeight(radiusX,radiusX*factors["ScaleZ"],height);
            cylinderFactor.compute();

            // Set the stimulus drawers
            // Staircase
            stimDrawerStaircase.initList(&cylinderStaircase,glWhite,dotsSize);
            // Mask
            stimDrawerMask.initList(&cylinderMask,glWhite,dotsSize);
            // Factor
            stimDrawerFactor.initList(&cylinderFactor,glWhite,dotsSize);
            // ************************//

            if (rand()%2)
                std::swap(trialmodes[0],trialmodes[2]);

            trialIndex=0;
            trialMode=trialmodes[trialIndex];
            trialTimer.start();
        }
    }
        break;
    case '6':
    {
        if (trialMode==PROBEMODE)
        {
            int responsekey=6;
            bool resp=false;
            if (prevTrialMode==STIMULUSMODE0)   // ha appena presentato lo stimolo del factor
            {
                resp=false;
            }
            if (prevTrialMode==STIMULUSMODE1)   // ha appena presentato lo stimolo della staircase
            {
                resp=true;
            }
            responseFile << trialNumber << "\t" <<
                            factors["ScaleZ"] <<"\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getID()  << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getState() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getInversions() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getAscending() << "\t" <<
                            responsekey << "\t" <<
                            resp << "\t" <<
							trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                            prevTrialMode << endl;
            trial.next(resp);

            if ( trial.isEmpty() )  // The experiment finished
            {
                cerr << "Experiment finished!" << endl;
                exit(0);
            }

            factorStaircasePair = trial.getCurrent();
            factors = factorStaircasePair.first;
            trialNumber++;

            // ************************//
            double scaleZ=factorStaircasePair.second->getCurrentStaircase()->getState();
            int ncirclesFactorStim =util::str2num<double> (parameters.find("DensityPointOnSurfaceFactor"))*mathcommon::ellipseCircumferenceBetter(radiusX,factors["ScaleZ"]*radiusX)*height;
            int ncirclesStaircaseStim =             util::str2num<double> (parameters.find("DensityPointOnSurfaceStaircase"))*mathcommon::ellipseCircumferenceBetter(radiusX,scaleZ*radiusX)*height;
            int ncirclesMask = util::str2num<double> (parameters.find("DensityPointOnSurfaceMask"))*radiusX*height;

            // Cylinder staircase
            cylinderStaircase.setNpoints(ncirclesStaircaseStim);
            cylinderStaircase.setAxesAndHeight(radiusX,radiusX*scaleZ,height);
            cylinderStaircase.compute();

            // Cylinder mask
            cylinderMask.setNpoints(ncirclesMask);
            cylinderMask.setAxesAndHeight(radiusX,0,height);
            cylinderMask.compute();

            // Cylinder factor
            cylinderFactor.setNpoints(ncirclesFactorStim);
            cylinderFactor.setAxesAndHeight(radiusX,radiusX*factors["ScaleZ"],height);
            cylinderFactor.compute();

            // Set the stimulus drawers
            // Staircase
            stimDrawerStaircase.initList(&cylinderStaircase,glWhite,dotsSize);
            // Mask
            stimDrawerMask.initList(&cylinderMask,glWhite,dotsSize);
            // Factor
            stimDrawerFactor.initList(&cylinderFactor,glWhite,dotsSize);
            // ************************//

            if (rand()%2)
                std::swap(trialmodes[0],trialmodes[2]);

            trialIndex=0;
            trialMode=trialmodes[trialIndex];
            trialTimer.start();

        }
    }
        break;
    case 13:
    {
        if (experimentStarted)
            break;
        experimentStarted=true;
        trialTimer.start();
    }
        break;
    }
}


void mouseFunc(int state, int button, int _x , int _y)
{
    glutPostRedisplay();
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
    frame+=(double)1.0/(double)TIMER_MS;
}

void idle()
{
    if (!experimentStarted)
        return;

    eyeLeft = Vector3d(-interoculardistance/2,0,0);
    eyeRight = -eyeLeft;
    prevTrialMode=trialmodes[2];
    if (trialTimer.getElapsedTimeInMilliSec() > stimulusTime  && (trialMode==STIMULUSMODE0 || trialMode==STIMULUSMODE1) )
    {
        trialIndex++;
        trialIndex %= 4;
        // cerr << "Stimulus " << trialMode <<" time= " << trialTimer.getElapsedTimeInMilliSec() << endl;
        trialMode=trialmodes[trialIndex];
        trialTimer.start();
        return;
    }

    if (trialTimer.getElapsedTimeInMilliSec() > maskTime  && (trialMode==MASKMODE) )
    {
        trialIndex++;
        trialIndex %= 4;
        trialMode=trialmodes[trialIndex];
        //cerr << "Mask time= " << trialTimer.getElapsedTimeInMilliSec() << endl;
        trialTimer.start();
        return;
    }
}


void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
}


int main(int argc, char*argv[])
{
    cout << "Select Horizontal 'h' or Vertical 'v' session" << endl;
    char val='h';
    cin >> val;
    horizontal = val=='h';

    if ( val!='h' && val !='v' )
    {
        cerr << "You must specify 'h' or 'v' and then press enter to select horizontal or vertical" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }

    mathcommon::randomizeStart();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO | GLUT_MULTISAMPLE);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP TEXTURING RANDOM DOTS");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initScreen();
    
    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
