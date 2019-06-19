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
#include <cmath>
#include <math.h>
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
#include "RoveretoMotorFunctions.h"
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "BoundChecker.h"
#include "BoxNoiseStimulus.h"
#include "CoordinatesExtractor2.h"
#include "EulerExtractor.h"
#include "GLText.h"
#include "GLUtils.h"
#include "LatestCalibration.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "Optotrak2.h"
#include "ParametersLoader.h"
#include "StimulusDrawer.h"
#include "TrialGenerator.h"
#include "Util.h"
#include "VRCamera.h"

// NAMESPACE DIRECTIVES
using namespace std;
using namespace Eigen;
using namespace RoveretoMotorFunctions;

// VISUALIZATION VARIABLES
static const bool gameMode=true;
static const bool stereo=true;

// GEOMETRY AND EXPERIMENT SETUP
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords;
Screen screen;
Vector3d eyeLeft, eyeRight,cyclopeanEye,projPointEyeRight;
vector <Marker> markers;
static double interoculardistance=65;
double focalDistance= -480.0;
// Questo rende conto del fatto che l'occhio non sarà mai in zero quando passa ad una certa velocità
double centerTolerance = 2.5;
// Streams File
ofstream markersFile, responseFile, matrixFile,flowsFile;

// STIMULI and TRIAL variables
BalanceFactor<double> trial;
bool response=false;
double maxXOscillation=50;
int nOscillationsFixation=-1;
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
ParametersLoader parameters;
map <string, double> factors;
Affine3d objectActiveTransformation=Affine3d::Identity();
Affine3d objectPassiveTransformation=Affine3d::Identity();
static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
bool infoDrawn=true;
//static const int CALIBRATIONMODE=3;
int headCalibrationDone=0,answer=0,trialNumber=0, maxTotalTrials=0,trialMode = FIXATIONMODE;
bool allVisibleHead=false,allVisiblePatch=false;
// Checkbound related variables
bool canCalibrate=false,conditionInside=true,wasInside=true;
Timer translationTimer,totalTimer,responseTimer;
double deltaT=TIMER_MS, translationTime=0.0, speedLimit=100;
double minOscTime=0.1,maxOscTime=1.0;
#ifdef _WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}

void beepLeft()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(880,220);
    return;
}
void beepRight()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(660,220);
    return;
}
void tweeter()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(840,660);
    return;
}
void woofer()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(240,660);
    return;
}
void beepBad()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(1200,440);
}
void outsideBeep()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(660,66);
    Beep(330,33);
    return;
}
#endif

/*************************** FUNCTIONS ***********************************/
void drawCircle(double radius, double x, double y, double z)
{
    glBegin(GL_LINE_LOOP);
    double deltatheta=mathcommon::toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
}



void checkBounds(int _nOscillations, double _signal, int &_trialMode, int _headCalibrationMode, double _minOscTime, double _maxOscTime, double _maxOscillation, Timer &_translationTimer, void (*_goodBeepTrigger)(), void (*_fastBeepTrigger)(),void (*_slowBeepTrigger)(),void (*_badBeep)() )
{
    if (_trialMode == PROBEMODE || _headCalibrationMode < 2)
        return;
    conditionInside = abs(_signal) < _maxOscillation;
    bool isEnteringArea = !( !conditionInside || (wasInside) );
    bool isLeavingArea =  !( conditionInside || (!wasInside) );

    static int sumExitFromRight=0, sumExitFromLeft=0;
    bool canIncreaseSum = abs(sumExitFromLeft-sumExitFromRight)<2;
    double elapsedTime = _translationTimer.getElapsedTimeInSec();
    bool tooSlow = elapsedTime > maxOscTime;
    bool tooFast = elapsedTime < minOscTime;
    bool correctRythm = !tooSlow && !tooFast;

    if ( isLeavingArea )
    {
        static int directionToGo=1;
        // First check that the rythm was correct
        if ( tooSlow )
        {
            boost::thread beepSlowThread(_slowBeepTrigger);
            sumExitFromRight=sumExitFromLeft=0;
        }
        if ( tooFast )
        {
            boost::thread beepFastThread(_fastBeepTrigger);
            sumExitFromRight=sumExitFromLeft=0;
        }
        if ( correctRythm )
        {

            if ( _signal*directionToGo < 0 )
            {
                boost::thread beepSlowThread(_badBeep);
                sumExitFromRight=sumExitFromLeft=0;
                sumExitFromLeft=sumExitFromRight=0;
            }
            else
            {
                if ( _signal >0 && directionToGo==1)
                {
                    boost::thread okBeepThread(_goodBeepTrigger);
                    directionToGo=-1;
                    sumExitFromRight++;
                }

                if ( _signal<0 && directionToGo==-1 )
                {
                    boost::thread okBeepThread(_goodBeepTrigger);
                    directionToGo=1;
                    sumExitFromLeft++;
                }
            }
        }
        else
        {
            sumExitFromRight=sumExitFromLeft=0;
        }
        if (_trialMode == STIMULUSMODE )
        {
            responseTimer.start();
            _trialMode = PROBEMODE;
        }
        else
            _trialMode = FIXATIONMODE;
    }

    if (!(canIncreaseSum) )
    {
        boost::thread beepSlowThread(_badBeep);
        sumExitFromRight=sumExitFromLeft=0;
    }

    if ( (_signal> 0) && isEnteringArea && sumExitFromLeft>=_nOscillations && (sumExitFromRight>=_nOscillations) && canIncreaseSum && trialMode == FIXATIONMODE )
    {
        _trialMode=STIMULUSMODE;
        sumExitFromRight=sumExitFromLeft=0;
    }

    if ( (isLeavingArea || isEnteringArea) )
    {
        // Inizia a contare il movimento
        translationTimer.start();
    }

    wasInside = conditionInside;
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void drawInfo()
{
    if ( !infoDrawn )
        return;
    GLText text;
    text.init(SCREEN_WIDTH, SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();
    string strfactors;
    for ( map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
    {
        text.draw( (iter->first) + "= " + util::stringify<int>( (int )(iter->second)) );
    }
    switch ( headCalibrationDone )
    {
    case 0:
    {
        if ( allVisibleHead )
            text.draw("OK! Press SPACEBAR");
        else
            text.draw("---");
    }
    break;
    case 1:
    case 2:
    {
        text.draw( "EL " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()) );
        text.draw( "ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()) );
        text.draw("EC " + stringify<int>(cyclopeanEye.x())+" " + stringify<int>(cyclopeanEye.y())+" " + stringify<int>(cyclopeanEye.z()));
        text.draw("Dist " + stringify<int>(cyclopeanEye.z()-focalDistance+60.0));
        text.draw(" ");
        text.draw( "PITCH " + stringify<int>(toDegrees(headEyeCoords.getPitch())));
        text.draw( "YAW " + stringify<int>(toDegrees(headEyeCoords.getYaw())));
        text.draw( "ROLL " + stringify<int>(toDegrees(headEyeCoords.getRoll())));
    }
    break;
    }
    text.leaveTextInputMode();
}


void drawRedDotsPlane()
{	
// questo serve per disegnare lo stimolo solo se l'occhio ha passato da destra verso sinistra, facendo così appare nel centro
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(1);
    // Draw the stimulus ( red-dots plane )
    double angle = factors.at("Slant") + deltaT/1000.0*factors.at("OmegaY");
    //angle = mathcommon::toDegrees(headEyeCoords.getYaw());
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    switch ( (int)factors.at("Tilt") )
    {
    case 0:
    {
        glRotated( angle ,0,1,0);
    }
    break;
    case 90:
    {
        glRotated( angle,1,0,0);
    }
    break;
    case 180:
    {
        glRotated( angle,0,-1,0);
    }
    break;
    case 270:
    {
        glRotated( angle,-1,0,0);
    }
    break;
    }
    glGetDoublev(GL_MODELVIEW_MATRIX,objectActiveTransformation.data());
    if ( (eyeRight.x()) < centerTolerance )
	stimDrawer.draw();
    glPopMatrix();
    glPopAttrib();
}

void drawProbe()
{
	double probeRadius = 5;
    canCalibrate = abs(eyeRight.x()) < probeRadius;
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPointSize(5);
    if ( canCalibrate )
        glColor3fv(glGreen);
    else
        glColor3fv(glRed);
    glBegin(GL_POINTS);
    glVertex3d(eyeRight.x(),0,focalDistance);
    glVertex3d(0,0,focalDistance);
    glEnd();
    drawCircle(probeRadius,0,0,focalDistance);
    glPopAttrib();
}

void drawCalibration()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    double circleRadius=5.0;
    glColor3fv(glRed);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glVertex3dv(projPointEyeRight.data());
    glEnd();
    // Draw the calibration circle
    if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius )
        glColor3fv(glGreen50);
    else
        glColor3fv(glRed);
    drawCircle(circleRadius,0,0,focalDistance);
    glPopAttrib();
}

void drawFixation()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    switch ( headCalibrationDone )
    {
    case 1:
        // Fixed stimulus
        glColor3fv(glWhite);
        glDisable(GL_BLEND);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glEnd();
        glPointSize(1);
        break;
    case 2:
        glColor3fv(glWhite);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glRed);
        glVertex3dv(projPointEyeRight.data());
        glColor3fv(glBlue);
        glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
        glEnd();

        // Draw the calibration circle
        if ( (sqrt(pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2)) < 3) && (sqrt( pow(eyeRight.x(),2) +pow(eyeRight.y(),2) ) < 3) )
        {
            canCalibrate=true;
            glColor3fv(glGreen);
        }
        else
        {
            canCalibrate=false;
            glColor3fv(glRed);
        }

        drawCircle(3,0,0,focalDistance);
        break;
    case 3:
    {
        /*
            glPointSize(3);
            glColor3fv(glRed);
            glBegin(GL_POINTS);
            glVertex3d(eyeRight.x(),0,focalDistance);
            glVertex3d(-maxXOscillation,0,focalDistance);
            glVertex3d(maxXOscillation,0,focalDistance);
            glEnd();
        	*/
        /*
        glColor3fv(glWhite);
        glPointSize(3);
        glBegin(GL_POINTS);
        glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
        glEnd();
        if ( conditionInside )
            glColor3fv(glGreen);
        else
            glColor3fv(glRed);
        glBegin(GL_LINE_LOOP);
        glVertex3d( -maxXOscillation,-maxXOscillation,focalDistance);
        glVertex3d( maxXOscillation, -maxXOscillation, focalDistance);
        glVertex3d( maxXOscillation, maxXOscillation, focalDistance);
        glVertex3d( -maxXOscillation, maxXOscillation, focalDistance);
        */
        glEnd();
    }
    break;
    }
    glPopAttrib();
}

void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {
        drawFixation();
    }
    break;
    case PROBEMODE :
    {
        drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawFixation();
        drawRedDotsPlane();
    }
    break;
    /*
    case CALIBRATIONMODE:
    {
        drawCalibration();
    }
    break;
    */
    default:
    {
        drawFixation();
    }
    }
}

void initVariables()
{
    interoculardistance = str2num<double>(parameters.find("IOD"));
    // Create the factors-staircase object TrialGenerator
    trial.init(parameters);
    maxTotalTrials = trial.getRemainingTrials();
    cerr << "There are " << maxTotalTrials << " total trials to do..." << endl;
    factors = trial.getNext();

    redDotsPlane.setNpoints(300);
    redDotsPlane.setDimensions(50,50,0.1);

    redDotsPlane.compute();
    //stimDrawer.drawSpecialPoints();
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.setSpheres(false);
    stimDrawer.initList(&redDotsPlane,glRed,3);


    // Set the maximum x displacement of head
    maxXOscillation = util::str2num<double>(parameters.find("MaxXOscillation"));
    nOscillationsFixation = util::str2num<int>(parameters.find("NOscillationsFixation"));
    minOscTime = util::str2num<double>(parameters.find("MinOscillationTime"));
    maxOscTime = util::str2num<double>(parameters.find("MaxOscillationTime"));

	centerTolerance = util::str2num<double>(parameters.find("CenterTolerance"));

    totalTimer.start();
}

void initStreams()
{
    string parametersFileName = "../../data/parametersFiles/parametersExpDPrimeTMSoverMT.txt";
    ifstream inputParameters;
    inputParameters.open(parametersFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersFileName  << "doesn't exist" << endl;
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

    // Principal streams file
    string markersFileName =	"markersFile_"	+ parameters.find("SubjectName") + ".txt";
    string responseFileName =	"responseFile_" + parameters.find("SubjectName") + ".txt";
    string matrixFileName = "matrixFile_" + parameters.find("SubjectName") + ".txt";
    string flowsFileName = "flowsFile_" + parameters.find("SubjectName") + ".txt";

    // Check for output file existence
    if (!util::fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());

    if (!util::fileExists( (baseDir+responseFileName) ) )
        responseFile.open( (baseDir+responseFileName).c_str() );

    if (!util::fileExists((baseDir+matrixFileName)) )
        matrixFile.open((baseDir+matrixFileName).c_str());

    if (!util::fileExists((baseDir+flowsFileName)) )
        flowsFile.open((baseDir+flowsFileName).c_str());

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
    // Active immediate mode drawing to enable simple points with no shading
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
}

void drawGLScene()
{
    if (stereo)
    {
        // Draw the right eye view on the left buffer
        glDrawBuffer(GL_BACK);
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeRight);
        drawInfo();
        drawTrial();
        // Draw the left eye view on the right buffer
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        //cam.setEye(eyeLeft); così è senza disparità
		cam.setEye(eyeRight);
        if ( headCalibrationDone <3)
        {
            drawInfo();
            drawTrial();
        }
        drawInfo();
        // Doesn't draw the right eye view if is not binocular
        if ( (int)(factors.at("Binocular"))==1 && (headCalibrationDone==3) )
        {
            drawTrial();
        }
        glutSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
		optotrak.stopCollection();
		Sleep(1000);
        exit(0);
    }
    break;
    case 'i':
        infoDrawn=!infoDrawn;
        break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisiblePatch )
        {
            headEyeCoords.init(markers[17].p,markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1  && allVisiblePatch)
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=2;
            beepOk();
            break;
        }
        if ( headCalibrationDone==2  && allVisiblePatch )
        {   headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            beepOk();
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration
    case 13:
    {
        if ( canCalibrate && headCalibrationDone == 2 && allVisiblePatch )
        {
            headEyeCoords.init( headEyeCoords.getP1().p,headEyeCoords.getP2().p, markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=3;
            infoDrawn=false;
            for (int i=0; i<3; i++)
                beepOk();
        }
    }
    break;
    }
}

void mouseFunc(int button, int state, int _x , int _y)
{
    if ( trialMode!=PROBEMODE || !canCalibrate )
        return;
    if ( button== GLUT_LEFT_BUTTON && canCalibrate )
    {
        response=true;
    }

    if (button == GLUT_RIGHT_BUTTON && canCalibrate )
    {
        response=false;
    }

    responseFile <<
                 trialNumber << "\t" <<
                 factors.at("Tilt") << "\t" <<
                 factors.at("Slant") << "\t" <<
                 factors.at("Binocular") << "\t" <<
                 factors.at("OmegaY") << "\t" <<
                 responseTimer.getElapsedTimeInMilliSec() << "\t" <<
                 response << endl;
    beepOk();
    factors = trial.getNext();

    trialMode=FIXATIONMODE;
    translationTimer.start();
    responseTimer.start();
    trialNumber++;
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
}

void writeContinuosDataFile()
{
    ofstream outputfile;
    outputfile.open("data.dat");

    outputfile << "Yaw: " << toDegrees(headEyeCoords.getYaw()) << endl <<"Pitch: " << toDegrees(headEyeCoords.getPitch()) << endl;
    outputfile << "EyeLeft: " <<  eyeLeft.transpose() << endl;
    outputfile << "EyeRight: " << eyeRight.transpose() << endl << endl;
    outputfile << "Factors:" << endl;
    for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
    {
        outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
    }
    outputfile << "Trials done: " << trialNumber << endl;
	if (trialNumber!=0)
		outputfile << "Last response (right=0, left=1): " << response << endl;
    outputfile.clear();
    outputfile.seekp(0,ios::beg);
}

Vector3d getEyeProjectionPoint()
{
    Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through   ( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

    // Projection of view normal on the focal plane
    Vector3d directionOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation()*Vector3d(0,0,-1)).normalized();
    Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( eyeRight , eyeRight+directionOfSight );

    double dist = lineOfSightRight.intersection(focalPlane);

    return (dist*(directionOfSight)+ (eyeRight));
}

Affine3d getPassiveMatrix()
{
    Screen screenPassive;
    screenPassive.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screenPassive.setOffset(alignmentX,alignmentY);
    screenPassive.setFocalDistance(0);
    screenPassive.transform(headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance)));
    VRCamera camPassive;
    camPassive.init(screenPassive);
    camPassive.setDrySimulation(true);
    camPassive.setEye(eyeRight);
    return ( camPassive.getModelViewMatrix()*objectActiveTransformation );
}

void idle()
{
    if (trialNumber >= maxTotalTrials )
        exit(0);
	double elapsedFrameTime = totalTimer.getTimeIntervalInMilliSec();
    optotrak.updateMarkers(elapsedFrameTime);
    markers = optotrak.getAllMarkers();
    headEyeCoords.update(markers[1],markers[2],markers[3],TIMER_MS);

    allVisiblePatch =  markers[1].isVisible() && markers[2].isVisible()
                       && markers[3].isVisible();
    allVisibleHead = markers[17].isVisible() && markers[18].isVisible() && allVisibleHead;

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;
    cyclopeanEye = (eyeLeft+eyeRight)/2.0;

    projPointEyeRight = getEyeProjectionPoint();

    checkBounds(nOscillationsFixation,
                eyeRight.x(),
                trialMode,
                headCalibrationDone,
                minOscTime,
                maxOscTime,
                maxXOscillation,
                translationTimer,
                beepOk,
                tweeter,
                woofer,tweeter);

    if ( trialMode == STIMULUSMODE )
        deltaT+=TIMER_MS;
    else
        deltaT=0;

    if (headCalibrationDone == 3 && trialMode != PROBEMODE )
    {
        // Questo rende conto del fatto che lo stimolo appare solo quando l'occhio è quasi in centro
		int actualTrialMode = trialMode;
		if ( trialMode == STIMULUSMODE && ( eyeRight.x()) > centerTolerance )
			actualTrialMode=FIXATIONMODE;

	markersFile << fixed <<   trialNumber << " " << actualTrialMode << " " ;
        markersFile << fixed << setprecision(3) << eyeRight.transpose() << " " << eyeLeft.transpose() << " " << toDegrees(headEyeCoords.getPitch()) << " " << toDegrees(headEyeCoords.getYaw()) << " " << toDegrees(headEyeCoords.getRoll()) << " " ;
        markersFile <<	fixed << setprecision(0)<<
                    factors["OmegaY"] << " " <<
                    factors["Binocular"] << " " <<
                    factors["Tilt"] << " " <<
                    factors["Slant"] << " " <<
                    totalTimer.getElapsedTimeInMilliSec() << endl;

        //objectPassiveTransformation.setIdentity();
		if ( actualTrialMode == STIMULUSMODE )
		{
        objectPassiveTransformation = getPassiveMatrix();
        matrixFile << setw(6) << left <<
                   trialNumber << " "  ;
        for ( int i=0; i<3; i++)
            matrixFile << objectPassiveTransformation.matrix().row(i) << " " ;
        matrixFile << endl;
		}

		if ( actualTrialMode == STIMULUSMODE )
		{
				vector< Vector3d> projPoints = stimDrawer.projectStimulusPoints(objectActiveTransformation,headEyeCoords.getRigidStart().getFullTransformation(),cam,focalDistance,screen,Vector3d(0,0,0),false,false);

				MatrixXd a1toa6 = stimDrawer.computeOpticFlow(projPoints, focalDistance, elapsedFrameTime/1000);
				flowsFile << trialNumber << " " << a1toa6.transpose() << endl;
		}
		}

    writeContinuosDataFile();

}


void initOptotrak()
{
    optotrak.setTranslation(calibration);
    optotrak.init(LastAlignedFile);

    for (int i=0; i<10; i++)
    {
        optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}

int main(int argc, char*argv[])
{
	bool motorMovement=false;
	cerr << "Muovi i motori? Premi '1' per yes '0' per no e poi invio" << endl;
	cin >> motorMovement ;
	if ( motorMovement )
	{
		cerr << "Hai spostato la mentoniera? Premi invio quando ok" << endl;
		cin.ignore(1E6,'\n');
		double deltaZChinRest=100;
		cerr << "Moving screen to home..." ;
		RoveretoMotorFunctions::homeScreen(3500);
		cerr << "done!" << endl;
		cerr << "Moving screen to focal distance "<< focalDistance << " [mm]..." ;
		RoveretoMotorFunctions::moveScreenAbsolute(focalDistance,-418.5,3500);
		VmxLinearMotor linearMotor,linearMotor2;
		linearMotor.move3d(VmxLinearMotor::MONITORCOMPORT,Vector3d(0,0,-deltaZChinRest),3500);
		RoveretoMotorFunctions::homeMirror(3500);
		cerr << " done!" << endl;
		cerr << "Homing mirror..." ;
		linearMotor2.home3d(VmxLinearMotor::MIRRORCOMPORT,3500);
		cerr << "Shifting mirror -100 mm from home" << endl;
		linearMotor2.move3d(VmxLinearMotor::MIRRORCOMPORT,Vector3d(0,0,-deltaZChinRest),3500);
	}

    randomizeStart();

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP WEXLER");
        glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }

    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMouseFunc(mouseFunc);
    /* Application main loop */
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
