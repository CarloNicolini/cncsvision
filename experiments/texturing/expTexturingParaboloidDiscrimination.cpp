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
#include "TrialGenerator.h"
#include "GLText.h"
#include "Timer.h"

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
static const double focalDistance= -500.0; //Millimeters

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES  **********************/
Vector3d eyeLeft, eyeRight;
static double interoculardistance=65;
double frame=0.0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;
bool drawInfo=false;
/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
VolumetricSurfaceIntersection surface[2];
ParaboloidPointsStimulus paraboloidPoints[2];
ParaboloidPointsStimulus maskStimulus;
StimulusDrawer stimDrawerWhite[2];
StimulusDrawer stimDrawerBlack[2];
StimulusDrawer stimDrawerMask;
// Trial related things
int trialNumber=0;
bool experimentStarted=false;
ParametersLoader parameters;
TrialGenerator<double> trial;

static const int TRIAL_MODE_REFERENCE=0;
static const int TRIAL_MODE_MASK=1;
static const int TRIAL_MODE_STAIRCASE=2;
static const int TRIAL_MODE_PROBE=3;

int trialMode = TRIAL_MODE_REFERENCE;
int prevTrialMode=TRIAL_MODE_STAIRCASE;

int trialmodes[4]={TRIAL_MODE_REFERENCE,TRIAL_MODE_MASK,TRIAL_MODE_STAIRCASE,TRIAL_MODE_PROBE};
int trialIndex=0;

/*** EXPERIMENTAL VARIABLES ***/
Timer trialTimer;
double stimulusTime=1000; //presentation time in milliseconds, is set to 1000 by default, must be specified in the parameters
void updateStimulus(double refCurv, double stairCurv);
/*************************** FUNCTIONS ***********************************/
/**
 * @brief advanceTrial
 * @param responseKey
 * @param resp
 */
void advanceTrial(int responseKey,bool resp)
{
    responseFile << trialNumber << "\t" <<
                    interoculardistance << "\t" <<
                    trial.getCurrent().first.at("IODFactor") <<"\t" <<
                    trial.getCurrent().first.at("CurvatureZ") <<"\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
                    trial.getCurrent().second->getCurrentStaircase()->getAscending()<< "\t" <<
                    responseKey << "\t" <<
                    resp << "\t" <<
                    trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                    prevTrialMode << endl;

    if ( trial.isEmpty() ) // The experiment finished
    {
        cerr << "Experiment finished!" << endl;
        exit(0);
    }
    else
    {
        trial.next(resp);
    }
    interoculardistance = str2num<double>(parameters.find("IOD"))*trial.getCurrent().first.at("IODFactor");
    updateStimulus(trial.getCurrent().first.at("CurvatureZ"),trial.getCurrent().second->getCurrentStaircase()->getState());
    trialNumber++;
    /*
    if ( rand()%2 )
    {
        std::swap(trialmodes[0],trialmodes[2]);
    }
    */
    trialIndex=0;
    trialMode=trialmodes[trialIndex];
    trialTimer.start();
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    double radiusX = util::str2num<double>(parameters.find("RadiusX"));
    switch (trialMode)
    {
    case TRIAL_MODE_REFERENCE:
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glScaled(radiusX,radiusX,radiusX);
        //glRotated(90,0,1,0); //for lateral debug mode
        if ( parameters.find("UsePolkaDotsReference")=="1")
            surface[0].draw();
        //stimDrawerBlack[0].draw();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_TEXTURE_3D);
        stimDrawerWhite[0].draw();
        glPopAttrib();
        glPopMatrix();
        break;
    }
    case TRIAL_MODE_MASK:
    {
        glDisable(GL_TEXTURE_3D);
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glScaled(radiusX,radiusX,radiusX);
        //glRotated(90,0,1,0); //for lateral debug mode
        stimDrawerMask.draw();
        //stimDrawerWhite[0].draw();
        glPopMatrix();
        break;
    }
    case TRIAL_MODE_STAIRCASE:
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glScaled(radiusX,radiusX,radiusX);
        //glRotated(90,0,1,0); //for lateral debug mode
        if ( parameters.find("UsePolkaDotsStaircase")=="1")
            surface[1].draw();
        //stimDrawerBlack[1].draw();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_TEXTURE_3D);
        stimDrawerWhite[1].draw();
        glPopAttrib();
        glPopMatrix();
        break;
    }
    case TRIAL_MODE_PROBE:
    {
        glDisable(GL_TEXTURE_3D);
        glColor3fv(glWhite);
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glutSolidSphere(2,20,20);
        glPopMatrix();
        break;
    }
    }
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
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_TEXTURE_3D);
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        text.draw("TrialMode= "+ util::stringify<int>(trialMode));
        text.draw("IOD x k = " + util::stringify<double>(interoculardistance));
        text.draw("Reference curvature= "+util::stringify<double>(trial.getCurrent().first.at("CurvatureZ")));
        text.draw("Staircase curvature= "+util::stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getState()));
        text.draw("Staircase ID= " + util::stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getID() ));
        text.draw("Staircase reversals = " + util::stringify<double>(trial.getCurrent().second->getCurrentStaircase()->getReversals() ));
        text.leaveTextInputMode();
        glPopAttrib();
    }

    drawStimulus();
}

/**
 * @brief updateStimulus
 * @param curvatureReference
 * @param curvatureStaircase
 */
void updateStimulus(double curvatureReference, double curvatureStaircase)
{
    // Update the volumetric object surface stimulus
    double c[2];
    c[0]=curvatureReference;
    c[1]=curvatureStaircase;

    int nSpheres = util::str2num<int>(parameters.find("TotalPolkaDots"));
    int dotsRadiusMin = util::str2num<int>(parameters.find("PolkaDotsRadiusMin"));
    int dotsRadiusMax = util::str2num<int>(parameters.find("PolkaDotsRadiusMax"));
    for (int i=0; i<2; i++)
    {
        surface[i].fillVolumeWithSpheres(nSpheres,dotsRadiusMin ,dotsRadiusMax);
        surface[i].initializeTexture();
        surface[i].parabolaSurface.curvature=c[i];
    }
    surface[0].setUniformColor(glWhite);
    surface[1].setUniformColor(glWhite);

    // Then update the random noise on it
    double radiusX = util::str2num<double>(parameters.find("RadiusX"));
    double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));

    float randomDotsSize = util::str2num<float>(parameters.find("RandomNoiseDotsSize"));
    for (int i=0; i<2;i++)
    {
        int nRandomDots = mathcommon::getParaboloidArea(radiusX,c[i])*noiseDensity;
        paraboloidPoints[i].setNpoints(nRandomDots);
        paraboloidPoints[i].setCurvature(c[i]);
        paraboloidPoints[i].setFluffiness(0.0);
        paraboloidPoints[i].compute();

        stimDrawerBlack[i].setSpheres(false);
        stimDrawerWhite[i].setSpheres(false);

        stimDrawerWhite[i].setStimulus(&paraboloidPoints[i]);
        stimDrawerBlack[i].setStimulus(&paraboloidPoints[i]);

        if ( parameters.find("RandomNoiseColor")=="RedAndBlack" )
        {
            stimDrawerWhite[i].initList(&paraboloidPoints[i],glWhite,randomDotsSize);
            paraboloidPoints[i].compute();
            stimDrawerBlack[i].initList(&paraboloidPoints[i],glBlack,randomDotsSize);
        }
        else
        {
            stimDrawerWhite[i].initList(&paraboloidPoints[i],glWhite,randomDotsSize);
            paraboloidPoints[i].compute();
            stimDrawerBlack[i].initList(&paraboloidPoints[i],glBlack,randomDotsSize);
        }
    }
    maskStimulus.setNpoints(util::str2num<int>(parameters.find("NumMaskCircles")));
    maskStimulus.setCurvature(0.0);
    maskStimulus.compute();
    stimDrawerMask.setStimulus(&maskStimulus);
    stimDrawerMask.setSpheres(false);
    stimDrawerMask.initList(&maskStimulus,glWhite,randomDotsSize);
}

/**
 * @brief initVariables
 */
void initVariables()
{
    cam.setOrthoGraphicProjection((bool)str2num<int>(parameters.find("OrthographicMode")));
    drawInfo = (bool)util::str2num<int>(parameters.find("DrawInfo"));
    trial.init(parameters);

    stimulusTime=util::str2num<double>(parameters.find("StimulusTime"));
    trialTimer.start();

    interoculardistance = str2num<double>(parameters.find("IOD"))*trial.getCurrent().first.at("IODFactor");

    int textureResolution = util::str2num<int>(parameters.find("TextureResolution"));
    int maxTextureResolutionZ = (int) std::ceil(textureResolution*util::str2num<double>(parameters.find("MaxCurvatureZ")));

    for ( int i=0; i<2; i++ )
    {
        surface[i].resize(textureResolution,textureResolution, maxTextureResolutionZ);
        surface[i].initializeSurfaceShaders(VolumetricSurfaceIntersection::SurfaceParaboloid);
    }
    updateStimulus(trial.getCurrent().first.at("CurvatureZ"),trial.getCurrent().second->getCurrentStaircase()->getState());
}

/**
 * @brief initStreams
 */
void initStreams()
{
#ifdef _WIN32
    string parametersFileName("C:/cncsvisiondata/parametersFiles/Fulvio/expTexturing/parametersExpTexturingAdjustmentParaboloid.txt");
#endif
#ifdef __linux__
    string parametersFileName("/home/carlo/Desktop/params/parametersExpTexturingParaboloidDiscrimination.txt");
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
    string outputFilename = parameters.find("BaseDir")+string("responseFileDiscriminationParaboloid_") + subjectName + string(".txt");
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
        responseFile << "TrialNumber\tIOD\tIODFactor\tRefCurvature\tStairID\tStepsDone\tCurvature\tTotReversals\tIsAscending\tRespKey\tResp\tRespTimeMS\tPrevStim" << endl;
    }
}

/**
 * @brief initRendering
 */
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
}

/**
 * @brief drawGLScene
 */
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

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
    case 'i':
    {
        drawInfo=!drawInfo;
        break;
    }
    case 13:
    {
        if (experimentStarted)
            return;
        experimentStarted=true;
        trialIndex=0;
        trialTimer.start();
        break;
    }
    case '4':
    {
        if (trialMode==TRIAL_MODE_PROBE)
        {
            bool resp=false;
            if (prevTrialMode==TRIAL_MODE_REFERENCE)   // ha appena presentato lo stimolo del factor
            {
                resp=true;
            }
            if (prevTrialMode==TRIAL_MODE_STAIRCASE)   // ha appena presentato lo stimolo della staircase
            {
                resp=false;
            }
            advanceTrial(4,resp);
        }
        break;
    }
    case '6':
    {
        if (trialMode==TRIAL_MODE_PROBE)
        {
            bool resp=false;
            if (prevTrialMode==TRIAL_MODE_REFERENCE)   // ha appena presentato lo stimolo del factor
            {
                resp=false;
            }
            if (prevTrialMode==TRIAL_MODE_STAIRCASE)   // ha appena presentato lo stimolo della staircase
            {
                resp=true;
            }
            advanceTrial(6,resp);
        }
        break;
    }
    }
}

/**
 * @brief mouseFunc
 * @param button
 * @param state
 * @param _x
 * @param _y
 */
void mouseFunc(int button, int state, int _x , int _y)
{
    glutPostRedisplay();
}

/**
 * @brief handleResize
 * @param w
 * @param h
 */
void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

/**
 * @brief update
 * @param value
 */
void update(int value)
{

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
    frame+=(double)1.0/(double)TIMER_MS;
}

/**
 * @brief idle
 */
void idle()
{
    if (!experimentStarted)
        return;

    eyeLeft = Vector3d(-interoculardistance/2,0,0);
    eyeRight = -eyeLeft;

    prevTrialMode=trialmodes[2];
    if (trialTimer.getElapsedTimeInMilliSec() > stimulusTime  && (trialMode==TRIAL_MODE_REFERENCE || trialMode==TRIAL_MODE_STAIRCASE) )
    {
        trialIndex++;
        trialIndex %= 4;
        // cerr << "Stimulus " << trialMode <<" time= " << trialTimer.getElapsedTimeInMilliSec() << endl;
        trialMode=trialmodes[trialIndex];
        trialTimer.start();
        return;
    }
    double maskTime=util::str2num<double>(parameters.find("MaskTime"));
    if (trialTimer.getElapsedTimeInMilliSec() > maskTime  && (trialMode==TRIAL_MODE_MASK) )
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
    cam.setNearFarPlanes(0.2,1000);
}

int main(int argc, char*argv[])
{
#ifdef _WIN32
    //RoveretoMotorFunctions::homeScreen(3500);
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
        glutCreateWindow("EXP TEXTURING DISCRIMINATION");
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
    //    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
