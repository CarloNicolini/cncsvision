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

#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

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

#include "MatrixStream.h"

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 15
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
ifstream inputStream;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
// Alignment between optotrak z axis and screen z axis
static const double alignmentX =  -2.5 ;
static const double alignmentY =  20.0;
static const double focalDistance= -568.5;
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND FIXATION **********************/
Vector3d eyeLeft, eyeRight, translationFactor(0,0,0),cyclopeanEye,projPointEyeRight,projPointEyeLeft,projPointCyclopeanEye, eyeCalibration;

static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;
int headCalibrationDone=0;

/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/** Euler angles **/
EulerExtractor eulerAngles;
/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
double instantPlaneSlant=45;
double previousOmega=0;

double probeAngle=0;
double probeStartingAngle=0;
bool allVisibleHead=false;
bool passiveMode=true;
bool orthographicMode=false;
bool zOnFocalPlane=false;
bool stimOutside=false;
// Trial related things
int trialNumber=0;
ParametersLoader parameters;

map <string, double> factors;

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
static const int CALIBRATIONMODE=3;
int trialMode = FIXATIONMODE;

// Object passive matrix
Affine3d objectPassiveTransformation = Affine3d::Identity();
Affine3d objectActiveTransformation = Affine3d::Identity();

/** Passive reading things **/
Matrix<double,3,5> pointMatrix;
int headCalibration;
bool isReading=true;
int stimulusFrames=0;

/***** SOUND THINGS *****/
#ifdef _WIN32
boost::mutex beepMutex;
void beepOk()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}

void tweeter()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(840,440);
    return;
}

void woofer()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(240,440);
    return;
}
#endif

/*************************** FUNCTIONS ***********************************/
void initProjectionScreen(double _focalDist, const Affine3d &_transformation);
void cleanup()
{   // Close all the file streams
    responseFile.close();
}

/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void drawCube()
{
    glColor3fv(glGreen);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(projPointEyeRight.x(), projPointEyeRight.y(), projPointEyeRight.z());
    glutWireCube(20);
    glPopMatrix();

    glColor3fv(glYellow);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutWireCube(20);
    glPopMatrix();

    glColor3fv(glBlue);
    Affine3d passive = (headEyeCoords.getRigidStart().getFullTransformation())*Translation3d(Vector3d(0,0,focalDistance)-eyeCalibration);
    Vector3d cubePassive = passive*Vector3d(0,0,0);

    glPushMatrix();
    glLoadIdentity();
    glTranslated(cubePassive.x(),cubePassive.y(), cubePassive.z());
    glutWireCube(20);
    glPopMatrix();

}

void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    
    objectActiveTransformation.translation() = projPointEyeRight ;
    if ((int)factors["Translation"]==-1 || (int)factors["Translation"]==-2 )
		objectActiveTransformation.linear().setIdentity();
	else
	objectActiveTransformation.linear()	= (AngleAxis<double>(eulerAngles.getYaw(), Vector3d::UnitY())*AngleAxis<double>(eulerAngles.getPitch(), Vector3d::UnitX())).toRotationMatrix();
        
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();
    glMultMatrixd(objectActiveTransformation.data());
    Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight-eyeCalibration);
    double argslant = acos( cos(toRadians(factors["Slant"]))*(focalDistance-posAlongLineOfSight.z() )/((focalDistance )));
	instantPlaneSlant = toDegrees(argslant);

    switch ( (int) factors["Tilt"] )
    {
    case 0:
        glRotated( -instantPlaneSlant ,0,1,0);
        objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitY() );
        glScaled(1/sin(toRadians( -90-factors["Slant"])),1,1);	//backprojection phase
        break;
    case 90:
        glRotated( -instantPlaneSlant ,1,0,0);
        objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitX() );
        glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
        break;
    case 180:
        glRotated( -instantPlaneSlant ,0,1,0);
        objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitY() );
        glScaled(1/sin(toRadians( -90-factors["Slant"] )),1,1); //backprojection phase
        break;
    case 270:
        glRotated( -instantPlaneSlant ,1,0,0);
        objectActiveTransformation*=AngleAxisd( toRadians(-instantPlaneSlant), Vector3d::UnitX() );
        glScaled(1,1/sin(toRadians( -90-factors["Slant"] )),1); //backprojection phase
        break;
    }
    stimDrawer.draw();
    glPopMatrix();	// POP MATRIX
}

void drawProbe()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
	glColor3fv(glRed);
    glPointSize(5);
    glBegin(GL_POINTS); // the last position of projection point
    glVertex3dv(projPointEyeRight.data());
    glEnd();
    glColor3fv(glRed);
    glPointSize(1);
}

void drawFixation()
{
    glColor3fv(glRed);
    switch ( headCalibrationDone  )
    {
    case 1:
        // Fixed stimulus
        glDisable(GL_BLEND);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glEnd();
        glPointSize(1);
        break;
    case 2:
        // Fixed stimulus + projected points
        glDisable(GL_BLEND);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glRed);
        glVertex3dv(projPointEyeRight.data());
        glEnd();
        glPointSize(1);
        break;
	case 3:
        glColor3fv(glRed);
        glDisable(GL_BLEND);
        glPointSize(5);
        glBegin(GL_POINTS);
		glVertex3dv(projPointEyeRight.data());
		glEnd();
		glPointSize(1);
    }
}

void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();
        //drawCube();
    }
    break;
    case PROBEMODE :
    {   
		drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
        //drawCube();
    }
    break;
    default:
    {
        drawFixation();
    }
    }
}

bool readline(ifstream &is, int &_trialNumber, int &_headCalibration, int &_trialMode, Matrix<double,3,5> &_pointMatrix )
{
    if ( is.eof() )
        return true;

    is >> _trialNumber >> _headCalibration >> _trialMode ;
    MatrixStream< Matrix<double,3,5> > pointStream; 
    _pointMatrix = pointStream.next(is);

    is >> factors["Tilt"] >> factors["Slant"] >> factors["Translation"] >> factors["Onset"] >> factors["TranslationConstant"] ;
	// Ignore everything else til next line
    //is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return false;
}

void keyPressed()
{
    if ( trialMode == PROBEMODE )
    {
        if ( trialNumber==0)
        {   responseFile << setw(6) << left <<
                         "TrialNumber" << "Tilt Slant Translation Onset TranslationConstant EyeCal.x EyeCal.y EyeCal.z StimFrames ProbeAngle" << endl;
        }
        responseFile << setw(6) << left <<
                     trialNumber << " " <<
                     factors["Tilt"] << " "  <<
                     factors["Slant"] << " " <<
                     factors["Translation"] << " " <<
                     factors["Onset"] << " " <<
                     factors["TranslationConstant"] << " " <<
                     eyeCalibration.transpose() << " " <<
					 stimulusFrames << " " <<
                     probeAngle << endl;

        double possibleAngles[]= {0,90,180,270};
        probeStartingAngle=possibleAngles[rand()%4];
        redDotsPlane.compute();
        
		stimDrawer.initList(&redDotsPlane);

        isReading=true;
        // Winds down the file until probemode isn't finished
        while ( trialMode == PROBEMODE  || trialMode == CALIBRATIONMODE )
        {   if ( readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix ) )
            {
                cleanup();
                exit(0);
            }
        }
    beepOk();
	}
}

void initStreams()
{
    // Load the parameters file
    inputParameters.open("parametersActiveZFixed.txt");
    if ( !inputParameters.good() )
    {
        cerr << "File doesn't exist" << endl;
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !exists(baseDir) )
    	create_directory(baseDir);

    // Subject name
    string subjectName = parameters.find("SubjectName");
	cerr << baseDir << " " << subjectName << endl;
    string responseFileName = baseDir + "responseFilePassive_" + subjectName +".txt";
    string markersFileName = baseDir + "markersFile_" + subjectName + "_filtered.txt";
    // Open the full input stream
    inputStream.open(markersFileName.c_str());
    
	if ( inputStream.good() )
    {
        cerr << "Markers file " << markersFileName << " ok" << endl;
    }
    else
        cerr << "Can't open " << markersFileName << endl;

    // Open the response file in write mode 
	if ( !fileExists((responseFileName)) )
		responseFile.open((responseFileName).c_str());
	
}

void initCalibration()
{
// Now wind the input points file while the head calibration isn't done
    while ( true )
    {
        //cerr << trialNumber << endl;
        // Here we load the variables needed to keep track of the experiment status
        readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix );
        //First phase of calibration (equivalent when spacebar is pressed first time )
        if ( (headCalibration== 1) && (headCalibrationDone==0 ))
        {   headEyeCoords.init(pointMatrix.col(3),pointMatrix.col(4), pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2),interoculardistance );
            headCalibrationDone=headCalibration;
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
{
    interoculardistance = str2num<double>(parameters.find("IOD"));

    redDotsPlane.setNpoints(75);  //XXX controllare densita di distribuzione dei punti
    redDotsPlane.setDimensions(50,50,0.1);
	redDotsPlane.compute();
    stimDrawer.setSpheres(true);
	stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane);
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
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void drawInfo()
{
	if ( headCalibrationDone!=3 )
	{
    GLText text;
    text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite, GLUT_BITMAP_HELVETICA_18);
    text.enterTextInputMode();
    if ( passiveMode )
    text.draw( "Passive"  );
    else
    text.draw(" Active " );
    if ( orthographicMode )
    text.draw(" Ortho " );
    else
    text.draw( "Perspective " );
    
    string row1 = stringify< Eigen::Matrix<double,1,4> >( objectPassiveTransformation.matrix().row(0) );
    string row2 = stringify< Eigen::Matrix<double,1,4> >( objectPassiveTransformation.matrix().row(1) );
    string row3 = stringify< Eigen::Matrix<double,1,4> >( objectPassiveTransformation.matrix().row(2) );
    
    text.draw( row1 );
    text.draw( row2 );
    text.draw( row3 );
    text.draw( stringify<int> (factors["Translation"]) );
	text.draw( stringify< Eigen::Vector3d >(projPointEyeRight) );
    text.draw( stringify<int>(instantPlaneSlant));
	text.leaveTextInputMode();
	}
}

void drawGLScene()
{
	if (stereo)
	{   glDrawBuffer(GL_BACK);

	// Draw left eye view
	glDrawBuffer(GL_BACK_LEFT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	cam.setEye(eyeRight+translationFactor);
	drawInfo();
	drawTrial();

	// Draw right eye view
	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	cam.setEye(eyeLeft+translationFactor);
	drawInfo();
	drawTrial();
	glutSwapBuffers();
	}
	else
	{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if ( trialMode == STIMULUSMODE )
		cam.setEye(eyeRight+translationFactor);
	else
		cam.setEye(eyeRight);
	if ( (int) factors["Translation"] == -2 )
		cam.setEye(Vector3d(0,0,eyeRight.z()));
	drawInfo();
	drawTrial();
	glutSwapBuffers();
	}
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'o':
    orthographicMode=!orthographicMode;
    cam.setOrthoGraphicProjection(orthographicMode);
    break;
    case 'z':
    zOnFocalPlane=!zOnFocalPlane;
    break;
    case 'p':
        passiveMode=!passiveMode;
        break;
    case 'q':
    case 27:
    {   cleanup();
        exit(0);
    }
    break;
    case '2':
    {
        probeAngle=270;
        keyPressed();
    }
    break;
    case '8':
    {
        probeAngle=90;
        keyPressed();
    }
    break;
    case '4':
    {
        probeAngle=180;
        keyPressed();
    }
    break;
    case '6':
    {
        probeAngle=0;
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

Vector2d getPlaneDimensions()
	{
	Point3D *pUpCenter = redDotsPlane.specialPointsRand[5];
	Point3D *pLowCenter = redDotsPlane.specialPointsRand[6];

	Point3D *pRightCenter = redDotsPlane.specialPointsRand[7];
	Point3D *pLeftCenter = redDotsPlane.specialPointsRand[8];

	Vector3d vUpCenter = objectActiveTransformation*Vector3d( pUpCenter->x, pUpCenter->y, pUpCenter->z);
	Vector3d vLowCenter = objectActiveTransformation*Vector3d( pLowCenter->x, pLowCenter->y, pLowCenter->z);
	Vector3d vRightCenter = objectActiveTransformation*Vector3d( pRightCenter->x, pRightCenter->y, pRightCenter->z);
	Vector3d vLeftCenter = objectActiveTransformation*Vector3d( pLeftCenter->x, pLeftCenter->y, pLeftCenter->z);
	
	double junkz=0;
	double vUpCenterWinx,vUpCenterWiny, vLowCenterWinx,vLowCenterWiny, vRightCenterWinx,vRightCenterWiny, vLeftCenterWinx,vLeftCenterWiny;
	
	gluProject(vUpCenter.x(),vUpCenter.y(),vUpCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vUpCenterWinx,&vUpCenterWiny,&junkz);
	gluProject(vLowCenter.x(),vLowCenter.y(),vLowCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLowCenterWinx,&vLowCenterWiny,&junkz);

	gluProject(vRightCenter.x(),vRightCenter.y(),vRightCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vRightCenterWinx,&vRightCenterWiny,&junkz);
	gluProject(vLeftCenter.x(),vLeftCenter.y(),vLeftCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLeftCenterWinx,&vLeftCenterWiny,&junkz);

	double planeHeight = abs( vUpCenterWiny - vLowCenterWiny);
	double planeWidth = abs( vLeftCenterWinx - vRightCenterWinx );

	return Vector2d(planeHeight,planeWidth);
	}

void update(int value)
{   // Read the experiment from file, if the file is finished exit suddenly
    if ( inputStream.eof() )
    {   cleanup();
        exit(0);
    }

    if ( isReading )
    {   // This reads a line (frame) in inputStream
        readline(inputStream, trialNumber,  headCalibration,  trialMode, pointMatrix );

        headEyeCoords.update(pointMatrix.col(0),pointMatrix.col(1),pointMatrix.col(2));
        Affine3d active = headEyeCoords.getRigidStart().getFullTransformation();
        eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );

        eyeLeft = headEyeCoords.getLeftEye();
        eyeRight= headEyeCoords.getRightEye();

        cyclopeanEye = (eyeLeft+eyeRight)/2.0;

		if ( trialMode == STIMULUSMODE )
			stimulusFrames++;
		if ( trialMode == FIXATIONMODE )
			stimulusFrames=0;

        // Projection of view normal on the focal plane
        Vector3d directionOfSight = (active.rotation()*Vector3d(0,0,-1)).normalized();
        Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( eyeRight , eyeRight+directionOfSight );
        Eigen::ParametrizedLine<double,3> lineOfSightLeft  = Eigen::ParametrizedLine<double,3>::Through( eyeLeft, eyeLeft+directionOfSight );

        double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);
        double lineOfSightLeftDistanceToFocalPlane = lineOfSightLeft.intersection(focalPlane);

        projPointEyeRight = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
        projPointEyeLeft  = lineOfSightLeftDistanceToFocalPlane * (directionOfSight) + (eyeLeft);

        // second projection the fixation point computed with z non constant but perfectly parallel to projPointEyeRight
        lineOfSightRightDistanceToFocalPlane= (( active.rotation()*(center)-eyeCalibration) - eyeRight).norm();
        Vector3d secondProjection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);

        if ( !zOnFocalPlane )
        {
            projPointEyeRight=secondProjection ;
        }
        // Compute the translation to move the eye in order to avoid share components
        Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight -eyeCalibration);
        switch ( (int)factors["Translation"] )
        {
		case -2:
        case -1:
             translationFactor.setZero();
             if ( trialMode == STIMULUSMODE || trialMode == FIXATIONMODE )
				 projPointEyeRight=center;
             break;
        case 0:
            translationFactor.setZero();
            break;
        case 1:
            translationFactor = factors["TranslationConstant"]*Vector3d(posAlongLineOfSight.z(),0,0);
            break;
        case 2:
            translationFactor = factors["TranslationConstant"]*Vector3d(0,posAlongLineOfSight.z(),0);
            break;
        }
		projPointEyeRight+=translationFactor;
		projPointEyeLeft+=translationFactor;
        
		// GENERATION OF PASSIVE MODE.
        // HERE WE MOVE THE SCREEN TO FACE THE OBSERVER's EYE
        if ( passiveMode )
        {
            initProjectionScreen(0, headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(center));
        }
        else
            initProjectionScreen(focalDistance, Affine3d::Identity());
        
		objectPassiveTransformation = ( cam.getModelViewMatrix()*objectActiveTransformation );
    
	ofstream outputfile;
	outputfile.open("data.dat");
	outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
	outputfile << "Passive matrix:" << endl << objectPassiveTransformation.matrix() << endl;
	outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl;
	outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().transpose() << endl;
	outputfile << "EyeRight: " << headEyeCoords.getRightEye().transpose() << endl << endl;
	outputfile << "Slant: " << instantPlaneSlant << endl;
	outputfile << "(Width,Height)= " << getPlaneDimensions().transpose() << endl;
	outputfile << "Factors:" << endl;
	for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
	{
		outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
	}
	
	}

    if ( trialMode == PROBEMODE )
        isReading=false;

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


int main(int argc, char*argv[])
{
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
    {   glutGameModeString("1024x768:32@60");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(focalDistance,Affine3d::Identity());
    initStreams();
    initVariables();
    initCalibration();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
