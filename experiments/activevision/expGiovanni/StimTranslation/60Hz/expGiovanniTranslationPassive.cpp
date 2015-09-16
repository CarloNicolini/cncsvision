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

void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	Matrix3d objrotation ;
	Vector3d stimulusCenter;
    
	// IMPORTANT Reset the previous status of transformation
	objectActiveTransformation.setIdentity();
	double xStim = factors["FollowingSpeed"]*(projPointEyeRight.x() + str2num<double>(parameters.find("MaxOscillation")) );
	objectActiveTransformation.translation() = Vector3d(xStim,0,focalDistance);
	
	//cerr << instantPlaneSlant << endl;

	glPushMatrix();     // PUSH MATRIX
	glLoadIdentity();
	glMultMatrixd(objectActiveTransformation.data());
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
		// Fixed stimulus + projected points
		glColor3fv(glWhite);
		glDisable(GL_BLEND);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3d(0,0,focalDistance);
		glColor3fv(glRed);
		glVertex3dv(projPointEyeRight.data());
		glColor3fv(glBlue);
		glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
		glEnd();
		glPointSize(1);
	
		break;
	
	case 3:
		// DRAW THE FIXATION POINT 
		double eyeToCenterAngleX= toDegrees(atan(eyeRight.x()/(-focalDistance-eyeRight.z()) ));
		double eyeToCenterAngleY= toDegrees(atan(eyeRight.y()/(-focalDistance-eyeRight.z()) ));
		double projPointAngleX = toDegrees( atan( (projPointEyeRight.x()-eyeRight.x())/abs(projPointEyeRight.z())));
		double maxAllowedTranslationYaw = str2num<double>(parameters.find("MaxAllowedTranslationYaw"));

		if ( ((int) factors["Rotation"]) == 0 )
		{	
			if ( abs(projPointAngleX)< maxAllowedTranslationYaw )	
				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}
		if ( ((int) factors["Rotation"]) == 1 )
		{
			if ( abs(eyeToCenterAngleY) < maxAllowedTranslationYaw )

				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}
		if ( ((int) factors["Rotation"]) == 2 )
		{
			if ( abs(eyeToCenterAngleX) < maxAllowedTranslationYaw )
				glColor3fv(glRed);
			else
				glColor3fv(glBlue);
		}

	glPushMatrix();
	glTranslated(projPointEyeRight.x(),projPointEyeRight.y(),projPointEyeRight.z());
	glutSolidSphere(1,10,10);
	glPopMatrix();
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
		drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
        
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

    is >> factors["Rotation"] >> factors["FollowingSpeed"] >> factors["Onset"] ;
	// Ignore everything else til next line
    //is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    //    cerr << _pointMatrix << endl;
    return false;
}

void keyPressed()
{
    if ( trialMode == PROBEMODE )
    {
        if ( trialNumber==0)
        {   responseFile << setw(6) << left <<
                         "TrialNumber" << "Tilt Slant Rotation RotationSpeed FollowingSpeed Onset EyeCal.x EyeCal.y EyeCal.z StimFrames ProbeAngle" << endl;
        }
        responseFile << setw(6) << left <<
                     trialNumber << " " <<
                     factors["Tilt"] << " "  <<
                     factors["Slant"] << " " <<
                     factors["Rotation"] << " " <<
                     factors["RotationSpeed"] << " " <<
                     factors["FollowingSpeed"] << " " <<
                     factors["Onset"] << " " <<
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
        
    #ifdef _WIN32
    beepOk();
    #endif
	}
}

void initStreams()
{
    // Load the parameters file
    inputParameters.open("parametersActiveTranslation.txt");
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

    string responseFileName = baseDir + "responseFilePassive_" + subjectName +".txt";
    string markersFileName = baseDir + "markersFile_" + subjectName + ".txt";
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

    redDotsPlane.setNpoints(1);  //XXX controllare densita di		distribuzione dei punti
	redDotsPlane.setDimensions(0.1,0.1,0.1);

	redDotsPlane.compute();
	//stimDrawer.drawSpecialPoints();
	stimDrawer.setStimulus(&redDotsPlane);
	stimDrawer.setSpheres(true);
	stimDrawer.setRadius(5);
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
    text.draw( stringify<int> (factors["Rotation"]) );
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

	cam.setEye(eyeRight);
	drawInfo();
	drawTrial();

	// Draw right eye view
	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);

	cam.setEye(eyeLeft);
	drawInfo();
	drawTrial();

	glutSwapBuffers();
	}
	else
	{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cam.setEye(eyeRight);
	if ( (int) factors["Rotation"] == 0 )
		cam.setEye(Vector3d(eyeRight.x()*factors["FollowingSpeed"],eyeRight.y(),eyeRight.z()));
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
	
	//double lenghtOnZ = (active*(center-eyeCalibration )+eyeRight).z();
	projPointEyeRight = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
	projPointEyeLeft= lineOfSightLeftDistanceToFocalPlane * (directionOfSight) + (eyeLeft);
	// second projection the fixation point computed with z non constant but perfectly parallel to projPointEyeRight
	lineOfSightRightDistanceToFocalPlane= (( active.rotation()*(center)) - eyeRight).norm();
	Vector3d secondProjection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
	
	if ( !zOnFocalPlane )
	projPointEyeRight=secondProjection ;

	// Compute the translation to move the eye in order to avoid share components
	Vector3d posAlongLineOfSight = (headEyeCoords.getRigidStart().getFullTransformation().rotation())*(eyeRight -eyeCalibration);
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
