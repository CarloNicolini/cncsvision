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
#ifdef _WIN32
#include "RoveretoMotorFunctions.h"
#endif

#include <boost/filesystem.hpp>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "GLUtils.h"
#include "Util.h"
#include "VRCamera.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "BalanceFactor.h"
#include "GLText.h"
#include "Timer.h"

#include "RoveretoMotorFunctions.h"
#include "VolumetricSurfaceIntersection.h"
#include "ParaboloidPointsStimulus.h"
#include "StimulusDrawer.h"

/********* #DEFINE DIRECTIVES **************************/
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace boost::filesystem;
using namespace boost;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
double focalDistance= -500.0; //Millimeters

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES  **********************/
Vector3d eyeLeft, eyeRight;
static double interoculardistance=65;
double frame=0.0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;
bool orthoMode=true;
bool drawInfo=false;
/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
VolumetricSurfaceIntersection surface;

// Trial related things
int trialNumber=0;
bool experimentStarted=false;
ParametersLoader parameters;
BalanceFactor<double> trial;

ParaboloidPointsStimulus paraboloidPoints;
StimulusDrawer stimDrawerBlack,stimDrawerRed;

static const int STIMULUSMODE=0;
static const int PROBEMODE=1;
int trialMode = STIMULUSMODE;

/*** EXPERIMENTAL VARIABLES ***/
string subjectName;
float CurvatureZ=1.0;

bool horizontal=false;
Timer trialTimer;
double stimulusTime=1000; //presentation time in milliseconds, is set to 1000 by default, must be specified in the parameters
void updateStimulus(double CurvatureZ);
/*************************** FUNCTIONS ***********************************/
void beepOk(int tone)
{
#ifdef WIN32
    switch(tone)
    {
    case 0:
        // Remember to put double slash \\ to specify directories!!!
        //PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\da ta\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 1:
        //PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    }
    return;
#endif
}

void advanceTrial()
{
    // Save trial
    responseFile << trialNumber << "\t" <<
                    trial.getCurrent().at("CurvatureZ") << "\t" <<
                    trial.getCurrent().at("IODFactor") << "\t" <<
                    CurvatureZ*util::str2num<double>(parameters.find("RadiusX")) << "\t"  << endl;
    if (!trial.next())
        exit(0);

    trialNumber++;
    interoculardistance = str2num<double>(parameters.find("IOD"))*trial.getCurrent().at("IODFactor");
    updateStimulus(trial.getCurrent().at("CurvatureZ"));
    // Randomize CurvatureZ again
    CurvatureZ= 0;//(double)mathcommon::unifRand(1,10)/10;    //This is needed to randomize between trials
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    double radiusX = util::str2num<double>(parameters.find("RadiusX"));
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    //if (horizontal) glRotated(90,0,0,1);
    glScaled(radiusX,radiusX,radiusX);
    //glRotated(90,0,1,0); //for lateral debug mode
    if ( parameters.find("UsePolkaDots")=="1")
        surface.draw();
    glPopMatrix();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_TEXTURE_3D);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance+0.02);
    //    if ( horizontal ) glRotated(90,0,0,1);

    glScaled(radiusX,radiusX,radiusX);
    //glRotated(90,0,1,0); //for lateral debug mode
    if (parameters.find("AddRandomNoise")=="1")
    {
        stimDrawerRed.draw();
        //stimDrawerBlack.draw();
    }
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief drawSideParabola
 */
void drawSideParabola()
{
    double xRange = util::str2num<double>(parameters.find("RadiusX"));
    double yPosition = xRange;
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_TEXTURE_3D);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINE_STRIP);
    for ( double x=-xRange; x<=xRange; x+=1.0 )
    {
        glVertex3d(x,CurvatureZ*(x*x-(xRange*xRange))+yPosition/2,focalDistance);
    }
    glEnd();
    glPopAttrib();
}

/**
 * @brief drawProbe
 */
void drawProbe()
{
    glPushMatrix();
    glLoadIdentity();
    drawSideParabola();
    glPopMatrix();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    if (!experimentStarted)
        return;

    if (drawInfo)
    {
        double radiusX = util::str2num<double>(parameters.find("RadiusX"));
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_TEXTURE_3D);
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        text.draw("CurvatureZ = " + util::stringify<double>(CurvatureZ));
        text.draw("CurvatureZ x RadiusX= " + util::stringify<double>(CurvatureZ*radiusX));
        text.draw("Factor CurvatureZ = " + util::stringify<double>(trial.getCurrent().at("CurvatureZ")));
        text.draw("IOD x k = " + util::stringify<double>(interoculardistance));
        if (parameters.find("UsePolkaDots")=="1")
            text.draw("Actual curvature controlled= "+util::stringify<double>(surface.parabolaSurface.curvature));
        text.leaveTextInputMode();
        glPopAttrib();
    }

    if ( trialMode == PROBEMODE )
        drawProbe();

    if (trialMode == STIMULUSMODE )
        drawStimulus();
}

/**
 * @brief updateStimulus
 * @param CurvatureZ
 */
void updateStimulus(double CurvatureZ)
{
    double radiusX = util::str2num<double>(parameters.find("RadiusX"));
    double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));

    int nRandomDots = mathcommon::getParaboloidArea(radiusX,CurvatureZ)*noiseDensity;
    paraboloidPoints.setNpoints(nRandomDots);
    paraboloidPoints.setCurvature(CurvatureZ);
    paraboloidPoints.setFluffiness(0.0);
    paraboloidPoints.compute();

    stimDrawerBlack.setSpheres(false);
    stimDrawerRed.setSpheres(false);

    stimDrawerRed.setStimulus(&paraboloidPoints);
    stimDrawerBlack.setStimulus(&paraboloidPoints);

    float randomDotsSize = util::str2num<float>(parameters.find("RandomDotsSize"));
    if (parameters.find("DotsColor")=="RedAndBlack")
    {
        stimDrawerRed.initList(&paraboloidPoints,glRed,randomDotsSize);
        paraboloidPoints.compute();
        stimDrawerBlack.initList(&paraboloidPoints,glBlack,randomDotsSize);
    }
    else
    {
        stimDrawerRed.initList(&paraboloidPoints,glWhite,randomDotsSize);
        paraboloidPoints.compute();
        stimDrawerBlack.initList(&paraboloidPoints,glWhite,randomDotsSize);
    }

    if ( parameters.find("UsePolkaDots")=="1" )
    {
        int nSpheres = util::str2num<int>(parameters.find("TotalPolkaDots"));
        int dotsRadiusMin = util::str2num<int>(parameters.find("PolkaDotsRadiusMin"));
        int dotsRadiusMax = util::str2num<int>(parameters.find("PolkaDotsRadiusMax"));
        int textureResolution = util::str2num<int>(parameters.find("TextureResolution"));
        //surface.fillVolumeWithSpheres2(nSpheres,nSpheres,nSpheres*util::str2num<double>(parameters.find("MaxCurvatureZ")),dotsRadiusMin,dotsRadiusMax);
        surface.fillVolumeWithSpheres(nSpheres,dotsRadiusMin ,dotsRadiusMax);
        surface.initializeTexture();
        surface.parabolaSurface.curvature=CurvatureZ;
        surface.setUniformColor(glWhite);
    }
}

void initVariables()
{
	focalDistance=util::str2num<double>(parameters.find("FocalDistance"));
	RoveretoMotorFunctions::moveScreenAbsolute(focalDistance,-418.5,3500);
    cam.setOrthoGraphicProjection((bool)str2num<int>(parameters.find("OrthographicMode")));
    drawInfo = (bool)util::str2num<int>(parameters.find("DrawInfo"));
    //cam.setOrthoGraphicProjection(orthoMode);
    cerr << "COMPUTED IOD=" << interoculardistance  << endl;
    trial.init(parameters);

    stimulusTime=util::str2num<double>(parameters.find("StimulusTime"));
    trialTimer.start();

    trial.next();
    interoculardistance = str2num<double>(parameters.find("IOD"))*trial.getCurrent().at("IODFactor");

    int textureResolution = util::str2num<int>(parameters.find("TextureResolution"));
    int maxTextureResolutionZ = (int) std::ceil(textureResolution*util::str2num<double>(parameters.find("MaxCurvatureZ")));
    // allocate the texture memory
    if (parameters.find("UsePolkaDots")=="1")
    {
        surface.resize(textureResolution,textureResolution, maxTextureResolutionZ);
        surface.initializeSurfaceShaders(VolumetricSurfaceIntersection::SurfaceParaboloid);
    }
    updateStimulus(trial.getCurrent().at("CurvatureZ"));
    CurvatureZ=0.0;
}

void initStreams()
{
#ifdef _WIN32
    string parametersFileName("C:/cncsvisiondata/parametersFiles/Fulvio/expTexturing/parametersExpTexturingAdjustmentParaboloid.txt");
#endif
#ifdef __linux__
    string parametersFileName("/home/carlo/Desktop/params/parametersExpTexturingAdjustment.txt");
#endif
    inputParameters.open(parametersFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersFileName << " doesn't exist, enter a valid path, press Enter to exit" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    string subjectName = parameters.find("SubjectName");
    string outputFilename = parameters.find("BaseDir")+string("responseFileAdjustment_") + subjectName + string(".txt");
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
        responseFile << "TrialNumber\tCurrCurvature\tIODFactor\tAdjustedCurvatureZ" << endl;
    }
}

void initRendering()
{   glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_MULTISAMPLE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    //glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void drawGLScene()
{
    //cam.setOrthoGraphicProjection(true);
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
    //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
        break;
    case 'i':
        drawInfo=!drawInfo;
        break;
    case '+':
    {
        surface.parabolaSurface.curvature+=0.01;
        break;
    }
    case '-':
    {
        surface.parabolaSurface.curvature-=0.01;
        break;
    }
    case ' ':
    {
        if  ( trialMode==STIMULUSMODE)
            trialMode=PROBEMODE;
        else
            trialMode=STIMULUSMODE;
        break;
    }
    case 13:
    {
        if (!experimentStarted)
        {
            experimentStarted=true;
            beepOk(0);
            trialMode=STIMULUSMODE;
            trialTimer.start();
        }
        if (trialMode==PROBEMODE)
        {
            beepOk(0);
            trialMode=STIMULUSMODE;
            advanceTrial();
            trialTimer.start();
        }
    }
        break;
    }
}

void mouseFunc(int button, int state, int _x , int _y)
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
    eyeLeft = Vector3d(-interoculardistance/2,0,0);
    eyeRight = -eyeLeft;

    if (!experimentStarted)
        return;

    if ( trialMode == STIMULUSMODE && trialTimer.getElapsedTimeInMilliSec() > stimulusTime )
    {
        //trialMode= PROBEMODE;
        trialTimer.start();
    }
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
    cam.setNearFarPlanes(0.2,1000);
}

/**
 * @brief arrowFunc
 * @param key
 * @param x
 * @param y
 */
void arrowFunc(int key, int x, int y)
{
    switch(key)
    {
    /*
    case GLUT_KEY_LEFT:
    {
        if (horizontal)
        {
            if (CurvatureZ>0.2)
                CurvatureZ+=util::str2num<double>(parameters.find("ParabolaStepSize"));
            else
                CurvatureZ+=util::str2num<double>(parameters.find("ParabolaStepSize"))/10.0;
        }
    }
        break;
    case GLUT_KEY_RIGHT:
    {
        if (horizontal)
        {
            if (CurvatureZ>0.2)
                CurvatureZ-=util::str2num<double>(parameters.find("ParabolaStepSize"));
            else //finer resolution
                CurvatureZ-=util::str2num<double>(parameters.find("ParabolaStepSize"))/10.0;
        }
    }
        break;
        */
    case GLUT_KEY_UP:
    {
        if (!horizontal)
        {
            if (CurvatureZ>0.2)
                CurvatureZ-=util::str2num<double>(parameters.find("ParabolaStepSize"));
            else //finer resolution
                CurvatureZ-=util::str2num<double>(parameters.find("ParabolaStepSize"))/10.0;
            if (CurvatureZ<0)
                CurvatureZ=0;
        }
    }
        break;
    case GLUT_KEY_DOWN:
    {
        if (!horizontal)
        {
            if (CurvatureZ>0.2)
                CurvatureZ+=util::str2num<double>(parameters.find("ParabolaStepSize"));
            else //finer resolution
                CurvatureZ+=util::str2num<double>(parameters.find("ParabolaStepSize"))/10.0;
            if (CurvatureZ<0)
                CurvatureZ=0;
        }
    }
        break;
    }
}

int main(int argc, char*argv[])
{
	#ifdef _WIN32
	RoveretoMotorFunctions::homeScreen(3500);
    //RoveretoMotorFunctions::moveScreenAbsolute(focalDistance,-418.5,3500);
	#endif

    /*
    cout << "Select Horizontal 'o' or Vertical 'v' session" << endl;
    char val='h';
    //cin >> val;
    //horizontal = val=='h';

    if ( val!='h' && val !='v' )
    {
        cerr << "You must specify 'h' or 'v' and then press enter to select horizontal or vertical" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
*/
    mathcommon::randomizeStart();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO | GLUT_MULTISAMPLE );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    if ( gameMode==false )
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP TEXTURING ADJUSTMENT");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
    initStreams();
    initVariables();
	initScreen();
    
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSpecialFunc(arrowFunc);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
