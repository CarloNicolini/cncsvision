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


/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/filesystem.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Timer.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "EulerExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BoundChecker.h"
#include "ParametersLoader.h"
#include "Util.h"
#include "FramerateDisplayer.h"
#include "MatrixStream.h"
#include "LatestCalibrationTrieste.h"


/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
ifstream inputStream;

// Alignment between optotrak z axis and screen z axis
double focalDistance= baseFocalDistance-200.0;
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND FIXATION **********************/
Vector3d eyeLeft, eyeRight, translationFactor(0,0,0),cyclopeanEye,projPointEyeRight,projPointEyeLeft,projPointCyclopeanEye, eyeCalibration;

static double interoculardistance=65;
///// AGGIUNTE
static double x_dim[2];
int num_dots= 100;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;
int headCalibrationDone=0;

/*** Streams File ***/
ofstream responseFile;
ofstream flowsFileMoving,flowsFileStill;
ifstream inputParameters;

/** Euler angles **/
EulerExtractor eulerAngles;
/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane[2];
StimulusDrawer stimDrawer[2];

bool allVisibleHead=false;
bool passiveMode=true;
bool orthographicMode=false;
bool stimOutside=false;

// Trial related things
int trialNumber=0;
ParametersLoader parameters;
bool answer=false;
map <string, double> factors;

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
static const int CALIBRATIONMODE=3;
int trialMode = FIXATIONMODE;
int linenumber=1;
int latestStimulusLineNumber=0;
int stimulusRepetitionsForThisTrial=0;

double stimulusTime=0;
Timer frameTimer;
Timer responseTimer, stimulusDuration;
// Object passive matrix
Affine3d objectPassiveTransformation[2] ;
Affine3d objectActiveTransformation[2] ;
bool whichPlaneDrawUp=false;
bool overallTilt=false;
double alpha=0,alphaMultiplier=0,instantPlaneSlant=0;
Vector3d flowsAnglesAlphaMoving,flowsAnglesBetaMoving,flowsAnglesAlphaStill,flowsAnglesBetaStill;
bool currentStaircaseIsAscending=false;

/** Passive reading things **/
Matrix<double,3,5> pointMatrix;
int headCalibration;
bool isReading=true;
int stimulusFrames=0;
bool start=true;


/***** SOUND THINGS *****/
#ifdef __linux__
#include "beep.h"
#endif
void beepOk()
{
    Beep(440,440);
    return;
}

/*************************** FUNCTIONS ***********************************/
void initProjectionScreen(double _focalDist, const Affine3d &_transformation);


void rectangle(double width, double height, double distance)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);

    /*glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    // glPolygonMode(GL_FRONT, GL_FILL);
    glColor4f (0.0, 1.0, 0.0, 0.5);
    glBegin(GL_QUADS);
    glVertex3f (width/2, height/2, distance);
    glVertex3f (width/2, -height/2, distance);
    glVertex3f (-width/2, -height/2, distance);
    glVertex3f (-width/2, height/2, distance);
    glEnd();
    glPopAttrib();
    glFlush ();

    /*glPushAttrib(GL_COLOR_BUFFER_BIT);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f (0.0, 1.0, 0.0,0.5);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
       glVertex3f (width/2, height/2, distance);
       glVertex3f (width/2, -height/2, distance);
       glVertex3f (-width/2, -height/2, distance);
       glVertex3f (-width/2, height/2, distance);
    glEnd();
    glPopAttrib();
    glFlush ();*/

    /*glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_BLEND);*/

}


void drawRedDotsPlane()
{
    // Draw the first plane
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();

    glMultMatrixd(objectActiveTransformation[0].data());
    // Tilt 0 backprojection
    glScaled(1/sin(toRadians( -90-(factors.at("DeltaSlant")+factors.at("StillPlaneSlant")))),1,1);	//backprojection phase
    stimDrawer[0].draw();
    glPopMatrix();	// POP MATRIX

    // Draw the second plane
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();

    glMultMatrixd(objectActiveTransformation[1].data());
    glScaled(1/sin(toRadians( -90-factors.at("StillPlaneSlant"))),1,1);
    stimDrawer[1].draw();
    glPopMatrix();	// POP MATRIX
}

void drawProbe()
{
    glPushAttrib(GL_POINT_BIT);
    glPointSize(5);

    double circleRadius=2.5;
    glColor3fv(glRed);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    glPopAttrib();
}

void drawFixation()
{   switch ( headCalibrationDone )
    {
    case 1:
        // Fixed stimulus at (0,0,focalDistance)
        glPushAttrib(GL_POINT_BIT);
        glColor3fv(glRed);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glEnd();
        glPopAttrib();
        break;
    case 2:
        // Fixed stimulus + projected points
        glPushAttrib(GL_POINT_BIT | GL_COLOR_BUFFER_BIT );
        glColor3fv(glWhite);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glRed);
        glVertex3dv(projPointEyeRight.data());
        glColor3fv(glBlue);
        glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
        glEnd();
        glPopAttrib();
        break;

    case 3:
        // Fixation point must be anchored to the orthogonal projection of eye right x coordinate
        glPushAttrib(GL_COLOR_BUFFER_BIT);	//Begin glPushAttrib colors
        glColor3fv(glRed);
        double circleRadius=str2num<double>(parameters.find("MaxCircleRadius"));	// millimeters
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,focalDistance);
        glutSolidSphere(1,10,10);
        glPopMatrix();
        glPopAttrib();	// end glPopAttrib colors
        break;
    }
}


void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();

    }
    break;
    case PROBEMODE :
    {
        // this is to avoid to flickering of probe...
        initProjectionScreen(focalDistance,Affine3d::Identity());
        // updates the model view and projection matrix
        cam.setEye(eyeRight);
        drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        responseTimer.start();
        /*
        // x_dim= str2num<double>(parameters.find("planeXdimension")) + mathcommon::unifRand(0,100);
        num_dots= int(floor((x_dim[0]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
        redDotsPlane[0].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane[0].setDimensions(x_dim[0],str2num<double>(parameters.find("planeYdimension")),0.01);
        //x_dim= str2num<double>(parameters.find("planeXdimension")) + mathcommon::unifRand(0,100);
        num_dots= int(floor((x_dim[1]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
        redDotsPlane[1].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane[1].setDimensions(x_dim[1],str2num<double>(parameters.find("planeYdimension")),0.01);

        //	redDotsPlane[0].setDimensions(str2num<double>(parameters.find("planeXdimension"))+mathcommon::unifRand(0,100),str2num<double>(parameters.find("planeYdimension")),0.01);
        //   redDotsPlane[1].setDimensions(str2num<double>(parameters.find("planeXdimension"))+mathcommon::unifRand(0,100),str2num<double>(parameters.find("planeYdimension")),0.01);
        */
        drawRedDotsPlane();
        if ( str2num<int>(parameters.find("Occluder")))
            //if ((int) factors.at("Occluder")) /// if factor scommenta
            rectangle(str2num<double>(parameters.find("OccluderWidth")),str2num<double>(parameters.find("OccluderHeight")),focalDistance + str2num<double>(parameters.find("OccluderDistance"))); /// Added: Draw an occluding rectangle infront of the twsited structure

    }
    break;
    default:
    {   drawFixation();
    }
    }
}

bool readline(ifstream &is, int &_trialNumber, int &_headCalibration, int &_trialMode, Matrix<double,3,5> &_pointMatrix )
{   if ( is.eof() )
        return true;

    is >> _trialNumber >> _headCalibration >> _trialMode ;
    MatrixStream< Matrix<double,3,5> > pointStream;
    _pointMatrix = pointStream.next(is);

    //is >> factors["Rotation"] >> factors["FollowingSpeed"] >> factors["Onset"] ;
    is >> factors["StillPlaneSlant"] >>  factors["DeltaSlant"] >>  factors["PlanesCentersYDistance"] >>  alphaMultiplier >>
       factors["Onset"] >>  whichPlaneDrawUp >> currentStaircaseIsAscending >> overallTilt >> x_dim[0] >> x_dim[1];
    // Ignore everything else til next line
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    linenumber++;

    return false;
}

void keyPressed()
{   if ( trialMode == PROBEMODE )
    {   if ( trialNumber==0)
        {   responseFile << setw(6) << left <<
                         "TrialNumber DeltaSlant StillPlaneSlant PlanesCentersYDistance Onset WhichPlaneUp StairState StairAscending EyeCalx EyeCaly EyeCalz StimFrames StimDuration ResponseTime Answer OverallTilt" << endl;
        }
        responseFile << fixed <<
                     trialNumber << " " <<
                     factors.at("DeltaSlant") << " " <<
                     factors.at("StillPlaneSlant") << " " <<
                     factors.at("PlanesCentersYDistance") << " " <<
                     factors.at("Onset") << " " <<
                     whichPlaneDrawUp << " " <<
                     alphaMultiplier << " " <<
                     currentStaircaseIsAscending << " " <<
                     eyeCalibration.transpose() << " " <<
                     stimulusFrames << " " <<
                     stimulusTime << " " <<
                     responseTimer.getElapsedTimeInMilliSec() << " " <<
                     answer << " " <<
                     overallTilt << " " << endl;

        responseTimer.start();
        stimulusDuration.start();

        for (int i=0; i<2; i++)
        {   num_dots= int(floor((x_dim[i]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
            redDotsPlane[i].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
            redDotsPlane[i].setDimensions(x_dim[i],str2num<double>(parameters.find("planeYdimension")),0.01);
            redDotsPlane[i].compute();
            stimDrawer[i].initList(&redDotsPlane[i]);
        }
        isReading=true;
        // Winds down the file until probemode isn't finished
        while ( trialMode == PROBEMODE  || trialMode == CALIBRATIONMODE )
        {   if ( readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix ) )
            {   exit(0);
            }
        }
        beepOk();
    }
}

void initStreams()
{   // Load the parameters file
    string parametersFileName = "C:/cncsvisiondata/parametersFiles/parametersActiveTwoPlanesOccRand.txt";
    inputParameters.open(parametersFileName.c_str());
    if ( !inputParameters.good() )
        cerr << "File " << parametersFileName << " doesn't exist, CTRL+C to exit" << endl;

    parameters.loadParameterFile(inputParameters);

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);

    // Subject name
    string subjectName = parameters.find("SubjectName");

    string responseFileName = baseDir + "responseFilePassive_" + subjectName +".txt";
    //string markersFileName = baseDir + "markersFile_" + subjectName + "_filtered.txt";
    string markersFileName = baseDir + "markersFile_" + subjectName + ".txt";
    string flowsFileMovingName = baseDir + "flowsFileMoving_"+subjectName +".txt";
    string flowsFileStillName = baseDir + "flowsFileStill_"+subjectName +".txt";
    // Open the full input stream
#ifdef _WIN32
    inputStream.open(markersFileName.c_str());
    if ( inputStream.good() )
    {   cerr << "Markers file " << markersFileName << " ok" << endl;
    }
    else
        cerr << "Can't open " << markersFileName << endl;

    // Open the response file in write mode
    if ( !fileExists((responseFileName)) )
        responseFile.open((responseFileName).c_str());
    if ( !fileExists((flowsFileMovingName)) )
        flowsFileMoving.open((flowsFileMovingName).c_str());
    if ( !fileExists((flowsFileStillName)) )
        flowsFileStill.open((flowsFileStillName).c_str());
#endif
}

void initCalibration()
{
// Now wind the input points file while the head calibration isn't done
    while ( true )
    {   // Here we load the variables needed to keep track of the experiment status
        readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix );
        //First phase of calibration (equivalent when spacebar is pressed first time )
        if ( (headCalibration== 1) && (headCalibrationDone==0 ))
        {   headEyeCoords.init(pointMatrix.col(3),pointMatrix.col(4), pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2),interoculardistance );
            headCalibrationDone=headCalibration;
            //cerr << headCalibrationDone << endl;
        }
        // Second phase of calibration (equivalent when space bar is pressed second time )
        if ( (headCalibration== 2) && (headCalibrationDone==1 ))
        {   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2),interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=headCalibration;
        }
        // Third and final phase of calibration ( equivalent when spacebar is pressed third time )
        if ((headCalibration==3))
        {   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2),interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=3;
            break; // exit the while cycle
        }
        // simulates the update of head and eyes positions
        if ( headCalibration==headCalibrationDone)
            headEyeCoords.update(pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2));
    }
}


void initVariables()
{   interoculardistance = str2num<double>(parameters.find("IOD"));
//x_dim[0]=10.0;
//x_dim[1]=10.0;
    readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix );
// int num_dots= 100;
    for (int i=0; i<2; i++)
    {
        // x_dim= str2num<double>(parameters.find("planeXdimension")) + mathcommon::unifRand(0,100);
        num_dots= int(floor((x_dim[i]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
        // redDotsPlane[i].setNpoints(str2num<int>(parameters.find("numberdots")));  //XXX controllare densita di distribuzione dei punti
        redDotsPlane[i].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
        // redDotsPlane[i].setDimensions(str2num<double>(parameters.find("planeXdimension"))+mathcommon::unifRand(0,100),str2num<double>(parameters.find("planeYdimension")),0.01);
        redDotsPlane[i].setDimensions(x_dim[i],str2num<double>(parameters.find("planeYdimension")),0.01);
        redDotsPlane[i].compute();
        stimDrawer[i].setStimulus(&redDotsPlane[i]);
        stimDrawer[i].setSpheres(true);
        stimDrawer[i].setRadius(0.5);
        stimDrawer[i].initList(&redDotsPlane[i]);
    }
    responseTimer.start();
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


void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void drawGLScene()
{
    answer=false;
    keyPressed();
    if (stereo)
    {   //glDrawBuffer(GL_BACK);

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
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawTrial();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{

    answer=false;
    keyPressed();

    switch (key)
    {   //Quit program

        answer=false;
        keyPressed();

    case ' ':
    {   start=true;
        beepOk();
    }
    break;
    case 'P':
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {   cout << iter->first << " " << iter->second << endl;
        }
        break;
    case 'p':
        passiveMode=!passiveMode;
        break;
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    case '4':
    {
        if (whichPlaneDrawUp==0)
            answer=true;
        else
            answer=false;
        /*if ( !overallTilt )
        	answer = false;
        else
        	answer=true;*/
        keyPressed();
    }
    break;
    case '6':
    {
        if (whichPlaneDrawUp==0)
            answer=false;
        else
            answer=true;

        /*if ( !overallTilt )
        	answer = true;
        else
        	answer=false;*/
        keyPressed();
    }
    break;
    }
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{
    frameTimer.start();
// Read the experiment from file, if the file is finished exit suddenly
    if ( inputStream.eof() )
    {   exit(0);
    }

    if ( isReading )
    {   // This reads a line (frame) in inputStream
        readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix );
        headEyeCoords.update(pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2));
        Affine3d active = headEyeCoords.getRigidStart().getFullTransformation();
        eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );

        eyeLeft = headEyeCoords.getLeftEye();
        eyeRight= headEyeCoords.getRightEye();
        //cerr << eyeRight.transpose() << endl;
        cyclopeanEye = (eyeLeft+eyeRight)/2.0;

        if ( trialMode == STIMULUSMODE )
            stimulusFrames++;
        if ( trialMode == FIXATIONMODE )
            stimulusFrames=0;

        // Projection of view normal on the focal plane
        Vector3d directionOfSight = (active.rotation()*Vector3d(0,0,-1)).normalized();
        Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( eyeRight , eyeRight+directionOfSight );

        double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);

        //double lenghtOnZ = (active*(center-eyeCalibration )+eyeRight).z();
        projPointEyeRight = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
        // second projection the fixation point computed with z non constant but perfectly parallel to projPointEyeRight
        lineOfSightRightDistanceToFocalPlane= (( active.rotation()*(center)) - eyeRight).norm();
        Vector3d secondProjection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
        projPointEyeRight=secondProjection ;

        // Compute the translation to move the eye in order to avoid share components
        Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight -eyeCalibration);
        // GENERATION OF PASSIVE MODE.
        // HERE WE MOVE THE SCREEN TO FACE THE OBSERVER's EYE
        if ( passiveMode )
        {   initProjectionScreen(0, headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(center));
        }
        else
            initProjectionScreen(focalDistance, Affine3d::Identity());

        if ( trialMode == STIMULUSMODE )
        {
            // IMPORTANT Reset the previous status of transformations
            objectActiveTransformation[0].setIdentity();
            objectActiveTransformation[1].setIdentity();
            // PLANE 0 Transformation QUELLO CHE STA SOTTO
            alpha = atan( eyeRight.x()/abs(projPointEyeRight.z()) );
            if ( overallTilt )
            {
                instantPlaneSlant = alphaMultiplier*alpha+toRadians(-factors.at("DeltaSlant")-factors.at("StillPlaneSlant"));
                AngleAxis<double> aa0( instantPlaneSlant,Vector3d::UnitY());
                objectActiveTransformation[0]*=aa0;
                double planesYOffset = factors.at("PlanesCentersYDistance")*(whichPlaneDrawUp ? 1 : -1);
                objectActiveTransformation[0].translation() = Vector3d(0,planesYOffset,focalDistance);

                // PLANE 1 Transformation QUELLO CHE STA SOPRA
                AngleAxis<double> aa1(-toRadians(factors.at("StillPlaneSlant")),Vector3d::UnitY());
                objectActiveTransformation[1]*=aa1;
                objectActiveTransformation[1].translation() = Vector3d(0,-planesYOffset,focalDistance);
            }
            else
            {
                instantPlaneSlant = alphaMultiplier*alpha+toRadians(factors.at("DeltaSlant")+factors.at("StillPlaneSlant"));
                AngleAxis<double> aa0( instantPlaneSlant,Vector3d::UnitY());
                objectActiveTransformation[0]*=aa0;
                double planesYOffset = factors.at("PlanesCentersYDistance")*(whichPlaneDrawUp ? 1 : -1);
                objectActiveTransformation[0].translation() = Vector3d(0,planesYOffset,focalDistance);

                // PLANE 1 Transformation QUELLO CHE STA SOPRA
                AngleAxis<double> aa1(toRadians(factors.at("StillPlaneSlant")),Vector3d::UnitY());
                objectActiveTransformation[1]*=aa1;
                objectActiveTransformation[1].translation() = Vector3d(0,-planesYOffset,focalDistance);
            }

            objectPassiveTransformation[0] = ( cam.getModelViewMatrix()*objectActiveTransformation[0] );
            objectPassiveTransformation[1] = ( cam.getModelViewMatrix()*objectActiveTransformation[1] );

            //cout << toDegrees(instantPlaneSlant) << endl;

            // **************** COMPUTE THE OPTIC FLOWS **************************
            // 1) Project the points to screen by computing their coordinates on focalPlane in passive (quite complicate, see the specific method)
            // *********** FOR THE MOVING PLANE *************
            vector<Vector3d> projPointsMovingPlane = stimDrawer[0].projectStimulusPoints(objectActiveTransformation[0],headEyeCoords.getRigidStart().getFullTransformation(),cam,focalDistance, screen, eyeCalibration,passiveMode,false);

            // 2) Get the angles formed by stimulus and observer
            // updating with the latest values
            Vector3d oldAlphaMoving = flowsAnglesAlphaMoving,oldBetaMoving=flowsAnglesBetaMoving;
            // alpha is the "pitch" angle, beta is the "yaw" angle
            // Here me must use the points 4,5,8 of the stimulus
            flowsAnglesAlphaMoving(0)  =  ( atan2(projPointsMovingPlane[4].x(), abs(focalDistance) ) );
            flowsAnglesAlphaMoving(1)  =  ( atan2(projPointsMovingPlane[5].x(), abs(focalDistance) ) );
            flowsAnglesAlphaMoving(2)  =  ( atan2(projPointsMovingPlane[8].x(), abs(focalDistance) ) );

            flowsAnglesBetaMoving(0)      =  ( atan2(projPointsMovingPlane[4].y(), abs(focalDistance) ) );
            flowsAnglesBetaMoving(1)      =  ( atan2(projPointsMovingPlane[5].y(), abs(focalDistance) ) );
            flowsAnglesBetaMoving(2)      =  ( atan2(projPointsMovingPlane[8].y(), abs(focalDistance) ) );

            // 3) Fill the matrix of derivatives
            MatrixXd angVelocitiesMoving(6,1);
            angVelocitiesMoving(0) = flowsAnglesAlphaMoving(0)-oldAlphaMoving(0);
            angVelocitiesMoving(1) = flowsAnglesBetaMoving(0)-oldBetaMoving(0);
            angVelocitiesMoving(2) = flowsAnglesAlphaMoving(1)-oldAlphaMoving(1);
            angVelocitiesMoving(3) = flowsAnglesBetaMoving(1)-oldBetaMoving(1);
            angVelocitiesMoving(4) = flowsAnglesAlphaMoving(2)-oldAlphaMoving(2);
            angVelocitiesMoving(5) = flowsAnglesBetaMoving(2)-oldBetaMoving(2);
            angVelocitiesMoving /= ((double)TIMER_MS/(double)1000);

            // 4) Fill the coefficient matrix, to solve the linear system
            MatrixXd coeffMatrixMoving(6,6);
            coeffMatrixMoving <<
                              1, flowsAnglesAlphaMoving(0),   flowsAnglesBetaMoving(0), 0, 0, 0,
                                 0, 0,    0,    1,flowsAnglesAlphaMoving(0),flowsAnglesBetaMoving(0),
                                 1, flowsAnglesAlphaMoving(1),   flowsAnglesBetaMoving(1), 0, 0, 0,
                                 0, 0,    0,    1,flowsAnglesAlphaMoving(1),flowsAnglesBetaMoving(1),
                                 1, flowsAnglesAlphaMoving(2),   flowsAnglesBetaMoving(2), 0, 0, 0,
                                 0, 0,    0,    1,flowsAnglesAlphaMoving(2),flowsAnglesBetaMoving(2)
                                 ;
            // 5) Solve the linear system by robust fullPivHouseholderQR decomposition (see Eigen for details http://eigen.tuxfamily.org/dox/TutorialLinearAlgebra.html )
            MatrixXd velocitiesMoving = coeffMatrixMoving.colPivHouseholderQr().solve(angVelocitiesMoving);
            // 6) Write the output to file flowsFileMoving
            flowsFileMoving << fixed << trialNumber << "\t" <<  //1
                            stimulusFrames << " " <<
                            factors.at("DeltaSlant")<< " " <<
                            factors.at("StillPlaneSlant") << " " <<
                            overallTilt << " " <<
                            projPointsMovingPlane[4].transpose() << " " <<
                            projPointsMovingPlane[5].transpose() << " " <<
                            projPointsMovingPlane[8].transpose() << " " <<
                            angVelocitiesMoving.transpose() << " " <<
                            velocitiesMoving.transpose() << endl;

            // ********************* FLOWS FOR THE STILL PLANE **************
            // Here we must repeat the same things for the still plane
            vector<Vector3d> projPointsStillPlane = stimDrawer[1].projectStimulusPoints(objectActiveTransformation[1],headEyeCoords.getRigidStart().getFullTransformation(),cam,focalDistance, screen, eyeCalibration,passiveMode,false);

            // 2) Get the angles formed by stimulus and observer
            // updating with the latest values
            Vector3d oldAlphaStill = flowsAnglesAlphaStill,oldBetaStill=flowsAnglesBetaStill;
            // alpha is the "pitch" angle, beta is the "yaw" angle
            // Here me must use the points 4,5,8 of the stimulus
            flowsAnglesAlphaStill(0)  =  ( atan2(projPointsStillPlane[4].x(), abs(focalDistance) ) );
            flowsAnglesAlphaStill(1)  =  ( atan2(projPointsStillPlane[5].x(), abs(focalDistance) ) );
            flowsAnglesAlphaStill(2)  =  ( atan2(projPointsStillPlane[8].x(), abs(focalDistance) ) );

            flowsAnglesBetaStill(0)      =  ( atan2(projPointsStillPlane[4].y(), abs(focalDistance) ) );
            flowsAnglesBetaStill(1)      =  ( atan2(projPointsStillPlane[5].y(), abs(focalDistance) ) );
            flowsAnglesBetaStill(2)      =  ( atan2(projPointsStillPlane[8].y(), abs(focalDistance) ) );

            // 3) Fill the matrix of derivatives
            MatrixXd angVelocitiesStill(6,1);
            angVelocitiesStill(0) = flowsAnglesAlphaStill(0)-oldAlphaStill(0);
            angVelocitiesStill(1) = flowsAnglesBetaStill(0)-oldBetaStill(0);
            angVelocitiesStill(2) = flowsAnglesAlphaStill(1)-oldAlphaStill(1);
            angVelocitiesStill(3) = flowsAnglesBetaStill(1)-oldBetaStill(1);
            angVelocitiesStill(4) = flowsAnglesAlphaStill(2)-oldAlphaStill(2);
            angVelocitiesStill(5) = flowsAnglesBetaStill(2)-oldBetaStill(2);
            angVelocitiesStill /= ((double)TIMER_MS/(double)1000);

            // 4) Fill the coefficient matrix, to solve the linear system
            MatrixXd coeffMatrixStill(6,6);
            coeffMatrixStill <<
                             1, flowsAnglesAlphaStill(0),   flowsAnglesBetaStill(0), 0, 0, 0,
                                0, 0,    0,    1,flowsAnglesAlphaStill(0),flowsAnglesBetaStill(0),
                                1, flowsAnglesAlphaStill(1),   flowsAnglesBetaStill(1), 0, 0, 0,
                                0, 0,    0,    1,flowsAnglesAlphaStill(1),flowsAnglesBetaStill(1),
                                1, flowsAnglesAlphaStill(2),   flowsAnglesBetaStill(2), 0, 0, 0,
                                0, 0,    0,    1,flowsAnglesAlphaStill(2),flowsAnglesBetaStill(2)
                                ;
            // 5) Solve the linear system by robust fullPivHouseholderQR decomposition (see Eigen for details http://eigen.tuxfamily.org/dox/TutorialLinearAlgebra.html )
            MatrixXd velocitiesStill = coeffMatrixStill.colPivHouseholderQr().solve(angVelocitiesStill);
            // 6) Write the output to file flowsFileStill
            flowsFileStill << fixed << trialNumber << "\t" <<  // 1
                           stimulusFrames << " " <<	// 2
                           factors.at("DeltaSlant")<< " " << // 3
                           factors.at("StillPlaneSlant") << " " << // 4
                           overallTilt << " " <<
                           projPointsStillPlane[4].transpose() << " " << // 5,6,7
                           projPointsStillPlane[5].transpose() << " " << // 8,9,10
                           projPointsStillPlane[8].transpose() << " " << // 11,12,13
                           angVelocitiesStill.transpose() << " " << // 14, 15, 16, 17, 18, 19
                           velocitiesStill.transpose() << endl;	// 20, 21, 22, 23, 24, 25
            // **************** END OF OPTIC FLOWS COMPUTATION
        }
        /*
        ofstream outputfile;
        outputfile.open("data.dat");
        outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
        outputfile << "Passive matrix:" << endl << objectPassiveTransformation.matrix() << endl;
        outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl;
        outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().transpose() << endl;
        outputfile << "EyeRight: " << headEyeCoords.getRightEye().transpose() << endl << endl;
        outputfile << "Factors:" << endl;
        for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
        {   outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
        }
        */

    }

    if ( trialMode == PROBEMODE )
        isReading=false;

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

int main(int argc, char*argv[])
{
    randomizeStart();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO | GLUT_STENCIL );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP WEXLER");
//glutFullScreen();
    }
    else
    {   glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    initStreams();
    initVariables();
    initCalibration();

    Timer sleeper;
    sleeper.start();
    sleeper.sleep(1000);

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
