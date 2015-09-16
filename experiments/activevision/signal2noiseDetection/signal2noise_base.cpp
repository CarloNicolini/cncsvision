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
#include <stdexcept>
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
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/*** INCLUDE BOOST HEADERS ***/
#include <boost/filesystem.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "GLText.h"
#include "LatestCalibration.h"

/*** NAMESPACE DIRECTIVES ***/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using util::str2num;

/**** CAMERA, EYES EXTRACTOR, OBJECTS ***/
VRCamera cam;
CoordinatesExtractor headEyeCoords;

/*** EYES, SCREEN AND MARKERS ***/
static const double focalDistance= -418.5;
Screen screen;
vector <Vector3d> markers(18);
Vector3d eyeLeft, eyeRight, eyeCalibration;
static double interoculardistance=65;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/*** OUTPUT STREAMS ***/
ofstream timingFile("time.dat");
ofstream responseFile;

/*** TIMING VARIABLES ***/
double timeFrame=0;
Timer responseTimer;
Timer stimulusTimer;
Timer totalTimer;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane, movingDotsPlane;
StimulusDrawer stimDrawer,stimDrawer2;
Eigen::Affine3d stimTransformation;
bool allVisibleHead=false;
bool drawInfo=false;
bool experimentStarted=false;
double fixationDurationInSeconds=1;
double stimulusDurationInMilliSeconds=1000;
double initialAdaptationTimeInSeconds=10.0;
double dotsSize=2;

/*** POINTS STRIP VARIABLES ***/
double periodicValue=0.0;
double oscillationAmplitude=1.0;

bool useCircularMask=false;
int circularMaskRadius=100;
/*** USER INTERACTION  VARIABLES **/
double probeAngle=0;
double probeStartingAngle=0;

/*** TRIAL VARIABLES ***/
int trialNumber=0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
enum TrialMode
{
    INITIALADAPTATION,
    FIXATIONMODE,
    STIMULUSMODE,
    PROBEMODE
} trialMode;

enum StimulusMotion
{
    SAWTOOTH_MOTION,
    TRIANGLE_MOTION,
    SINUSOIDAL_MOTION
} stimMotion;

/*** SOUND THINGS ***/
//boost::mutex beepMutex;
void beepOk()
{
    //Beep(440,440);
    return;
}

void clearAndWait(double milliSeconds)
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearStencil(0);
    glClearDepth(1.0);
    glutSwapBuffers();
    Timer waitTimer;
    waitTimer.sleep(milliSeconds);
}


/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{
    if ( trial.isEmpty() )
    {
#ifdef WIN32
        for (int i=0; i<3; i++)
        {
            Beep(220,440);
            totalTimer.sleep(20);
        }
#endif
        exit(0);
    }

    switch (trialMode)
    {
    case FIXATIONMODE:
    {
#ifdef WIN32
        Beep(440,440);
#endif
        totalTimer.start();
        stimulusTimer.start();
        //clearAndWait(2);
        trialMode=STIMULUSMODE;
    }
    break;
    case STIMULUSMODE:
    {
#ifdef WIN32
        Beep(440,440);
#endif
        trialMode=PROBEMODE;
        totalTimer.start();
        stimulusTimer.start();
    }
    break;
    case PROBEMODE:
    {
#ifdef WIN32
        Beep(880,440);
#endif

        responseTimer.start();
        totalTimer.start();
        trialMode=FIXATIONMODE;
    }
    }

    factors = trial.getNext();
    trialNumber++;
    responseFile << setw(6) << left <<
                 trialNumber << "\t" <<
                 factors["Tilt"] << "\t"  <<
                 factors["Slant"] << "\t" <<
                 factors["Def"] << "\t" <<
                 factors["FlowDirection"] << "\t" << endl;

    double responseTime=responseTimer.getElapsedTimeInMilliSec();
    timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in

    movingDotsPlane.compute();

}

void movePointOnStimSurface(BoxNoiseStimulus *stim)
{
    for (PointsRand::iterator iter = stim->pointsRand.begin(); iter!=stim->pointsRand.end(); ++iter)
    {
        double theta = mathcommon::unifRand( 0.0, 2*M_PI);
        (*iter)->x += factors["StepLenght"]*cos(theta);
        (*iter)->y += factors["StepLenght"]*sin(theta);
    }
}

void drawRedDotsPlane()
{

    stimDrawer2.initList(&movingDotsPlane);
    //movePointOnStimSurface(&movingDotsPlane);
    int planeTilt = (int)factors.at("Tilt");
    int planeSlant = (int)factors.at("Slant");
    double planeDef = factors.at("Def");
    double planeOnset=1;
    bool backprojectionActive=true;
    double theta=0.0;
    Eigen::Affine3d modelTransformation;

    glPushMatrix();
    glMultMatrixd(stimTransformation.data());
    switch ( planeTilt )
    {
    case 0:
    {
        theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
        glRotated(toDegrees(theta),0,1,0);
        if (backprojectionActive)
            glScaled(1/sin(toRadians( -90-planeSlant)),1,1);	//backprojection phase
    }
    break;
    case 90:
    {
        theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
        glRotated( toDegrees(theta),1,0,0);
        if (backprojectionActive)
            glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
    }
    break;
    case 180:
    {
        theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
        glRotated(toDegrees(theta),0,1,0);
        if (backprojectionActive)
            glScaled(1/sin(toRadians( -90-planeSlant )),1,1); //backprojection phase
    }
    break;
    case 270:
    {
        theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
        glRotated( toDegrees(theta),1,0,0);
        if (backprojectionActive)
            glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
    }
    break;
    default:
        cerr << "Error, select tilt to be {0,90,180,270} only" << endl;
    }
    /*
        static int frame=0;
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glRotated(90,1,0,0);
        */
    stimDrawer.draw();
    stimDrawer2.draw();
    glGetDoublev(GL_MODELVIEW_MATRIX,modelTransformation.matrix().data());
    modelTransformation.matrix() = modelTransformation.matrix().eval();
    glPopMatrix();
}

void drawProbe()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3fv(glRed);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    glPopAttrib();
}

void drawFixation()
{
    // This draws the central dot to stare
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(5);
    glColor3fv(glRed);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    glPopMatrix();
    glPopAttrib();
}

void drawTrial()
{
    if (!experimentStarted)
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed);
        text.enterTextInputMode();
        text.draw("Press ENTER key to start the experiment");
        text.leaveTextInputMode();
        return;
    }

    if (drawInfo)
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glRed);
        text.enterTextInputMode();
        text.draw(util::stringify<int>(totalTimer.getElapsedTimeInMilliSec()));
        text.leaveTextInputMode();
    }
    switch ( trialMode )
    {
    case INITIALADAPTATION:
    {
        drawFixation();
    }
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
    }
    break;
    }

    if ( useCircularMask )
        circleMask(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,circularMaskRadius);

    glGetError();
}


void initVariables()
{
    totalTimer.start();
    interoculardistance = str2num<double>(parameters.find("IOD"));
    trial.init(parameters);
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1

    useCircularMask = util::str2num<int>( parameters.find("CircularMask")) == 1 ;
    circularMaskRadius = util::str2num<int>(parameters.find("CircularMaskRadius"));
    if (useCircularMask)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);

    fixationDurationInSeconds = util::str2num<double>(parameters.find("AdaptationDurationInSeconds"));
    // Imposta stimolo e drawer
    redDotsPlane.setNpoints(util::str2num<int>(parameters.find("NumStimulusPoints")));
    redDotsPlane.setDimensions(
        util::str2num<int>(parameters.find("StimulusEdgeLength")),
        util::str2num<int>(parameters.find("StimulusEdgeLength")),0.1);
    redDotsPlane.compute();

    movingDotsPlane.setNpoints(200);
    movingDotsPlane.setDimensions(
        util::str2num<int>(parameters.find("StimulusEdgeLength")),
        util::str2num<int>(parameters.find("StimulusEdgeLength")),0.1);

    stimDrawer.initList(&redDotsPlane,glRed);

    stimulusDurationInMilliSeconds = util::str2num<double>(parameters.find("StimulusDuration"));
    initialAdaptationTimeInSeconds = util::str2num<double>(parameters.find("InitialAdaptationTime"));
    trialMode = INITIALADAPTATION;

    headEyeCoords.init(Vector3d(interoculardistance/2,0,0),Vector3d(interoculardistance/2,0,0), Vector3d(0,0,0),Vector3d(0,10,0),Vector3d(0,0,10),interoculardistance );
    eyeCalibration=headEyeCoords.getRightEye();
}

void mouseFunc(int button, int state, int _x , int _y)
{
    glutPostRedisplay();
}

void initStreams()
{
    ifstream inputParameters;
    string parametersPassiveFileName("parametersSignal2NoiseDetection_base.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path" << endl;
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);

    //string responseFileName = baseDir + "responseFileStripAdapt_" + parameters.find("SubjectName") +".txt";
    //responseFile.open(responseFileName.c_str());

    //responseFile << setw(6) << left <<
    //            "Trial\tTilt\tSlant\tDef\tFlowDir" << endl;
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d(interoculardistance/2,0,0));
    drawTrial();
    glutSwapBuffers();
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case '2':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '8':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '4':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case '6':
    {
        if (trialMode == PROBEMODE)
            advanceTrial();
    }
    break;
    case 13:
    {
        if (!experimentStarted )
        {
#ifdef WIN32
            Beep(880,880);
#endif
            trialMode=INITIALADAPTATION;
            totalTimer.start();
            experimentStarted=true;
        }
    }
    break;
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
    movePointOnStimSurface(&movingDotsPlane);
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
    if (totalTimer.getElapsedTimeInSec() > initialAdaptationTimeInSeconds
            && trialMode == INITIALADAPTATION && experimentStarted )
    {
        trialMode = STIMULUSMODE;
        totalTimer.start();
    }

    if (totalTimer.getElapsedTimeInSec() > fixationDurationInSeconds && trialMode == FIXATIONMODE && experimentStarted )
    {
        timeFrame=0.0;
        trialMode=STIMULUSMODE;
        totalTimer.start();
    }

    if (totalTimer.getElapsedTimeInMilliSec() > stimulusDurationInMilliSeconds && trialMode == STIMULUSMODE && experimentStarted )
    {
        timeFrame=0.0;
        trialMode=PROBEMODE;
        totalTimer.start();
    }

    if (!experimentStarted)
        return;
    Timer frameTimer;
    frameTimer.start();
    // Timing things

    double oscillationPeriod = stimulusDurationInMilliSeconds;
    switch (stimMotion)
    {
    case SAWTOOTH_MOTION:
        periodicValue = oscillationAmplitude*mathcommon::sawtooth(timeFrame,oscillationPeriod);
        break;
    case TRIANGLE_MOTION:
        periodicValue = oscillationAmplitude*mathcommon::trianglewave(timeFrame,oscillationPeriod);
        break;
    case SINUSOIDAL_MOTION:
        periodicValue = oscillationAmplitude*sin(3.14*timeFrame/(oscillationPeriod));
        break;
    default:
        SAWTOOTH_MOTION;
    }
    timingFile << totalTimer.getElapsedTimeInMilliSec() << " " << periodicValue << endl;

    // Simulate head translation
    // Coordinates picker
    markers[1] = Vector3d(0,0,0);
    markers[2] = Vector3d(0,10,0);
    markers[3] = Vector3d(0,0,10);

    headEyeCoords.update(markers[1],markers[2],markers[3]);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    stimTransformation.matrix().setIdentity();
    stimTransformation.translation() <<0,0,focalDistance;

    //movePointOnStimSurface(&movingDotsPlane);
    Timer sleepTimer;
    sleepTimer.sleep(abs(TIMER_MS - frameTimer.getElapsedTimeInMilliSec()));
    timeFrame+=1;
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setOffset(0,0);
    screen.setFocalDistance(focalDistance);

    cam.init(screen);
}

int main(int argc, char*argv[])
{
    mathcommon::randomizeStart();
    initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("PointsStrip flow");
        //glutFullScreen();
    }
    else
    {
        //glutGameModeString("1024x768:32@100");
        glutGameModeString("1680x1050:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();
    return 0;
}
