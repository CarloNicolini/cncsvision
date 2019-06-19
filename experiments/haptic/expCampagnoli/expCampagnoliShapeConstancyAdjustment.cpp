// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2013 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include "IncludeGL.h"

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Marker.h"
#include "Util.h"
//#include "Optotrak2.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "EllipsoidPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"

#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;
using namespace RoveretoMotorFunctions;

// Function prototypes
void generateStimulus(double XRadius,double zAxis, double height);
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;
double homeFocalDistance=-418.5;

/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords;
Vector3d eyeLeft;
Vector3d eyeRight;

Vector3d physicalRigidBodyTip;
Vector3d visualStimCenter;
static double interoculardistance=65;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=3;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool infoDraw=false;
bool isSaving=true;

int fingerInside=1;
int fingerWasInside=1;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
//Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
BalanceFactor<double> factors;
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;

const int ONLY_THUMB=0;
const int ONLY_INDEX=1;
const int BOTH_INDEX_AND_THUMB=2;

int trialMode = STIMULUSMODE;

/********* STIMULI *******/
EllipsoidPointsStimulus cylinder;
StimulusDrawer stimDrawer;
double scaleAdjustmentZ=1.0;
double adjustmentZ=0.0;
bool debugRotate=false;
/****** TIMING *****/
Timer globalTimer;
Timer trialTimer;

unsigned int invisibleThumbFrames=0;
unsigned int invisibleIndexFrames=0;
unsigned int trialFrame=0;
int drawingTrialFrame=0;

int totalTrialNumber=0;
bool isMovingRod=false;

/**** FILES VARIABLES ***/
ofstream outputFile;
int paramIndex=0;
char *parametersFileNames[] = {"C:/cncsvisiondata/parametersFiles/Campagnoli/parametersExpCampagnoliShapeConstancyAdjustment.txt",
                               "C:/cncsvisiondata/parametersFiles/Campagnoli/parametersExpCampagnoliShapeConstancyAdjustment_training.txt"
                              };
/********* SOUNDS ********/
#ifdef _WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepInvisible()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(1000,5);
    return;
}
void beepLong()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(220,880);
    return;
}
void beepOk()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}
void beepBad()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(200,200);
}

void beepTrial()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(660,200);
    return;
}
#endif

/**
 * @brief drawInfo
 */
void drawInfo()
{   if ( infoDraw )
    {   glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        GLText text;
        if ( gameMode )
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
        else
            text.init(640,480,glWhite,GLUT_BITMAP_HELVETICA_12);
        text.enterTextInputMode();
        switch ( headCalibrationDone )
        {
        case 0:
        {   if ( allVisibleHead )
                text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
            else
                text.draw("Be visible with the head and glasses");
        }
        break;
        case 1:
        case 2:
        {   if ( allVisiblePatch )
                text.draw("Cyclopean(x,y,z)= " + stringify<int>((eyeRight.x()+eyeLeft.x())/2)+", "+ stringify<int>((eyeRight.y()+eyeLeft.y())/2)+", "+ stringify<int>((eyeRight.z()+eyeLeft.z())/2)+", " );
            else
                text.draw("Be visible with the patch");
        }
        break;
        }
        if ( headCalibrationDone==3)
        {   
			double cylHeight = cylinder.getHeight();
		    double cylDiameterX = 2*cylinder.getXAxis();
			double cylDiameterZ = 2*cylinder.getZAxis();

			text.draw("CylDiamX= " + stringify<double>(cylDiameterX) );
			text.draw("CylDiamZ= " + stringify<double>(cylDiameterZ) );
			text.draw("CylHeight= " + stringify<double>(cylHeight) );
			text.draw("ScaleAdjustment*Z= " + stringify<double>(1+adjustmentZ/cylDiameterZ) );
			text.draw("AdjustedZ= " + stringify<double>( (1+adjustmentZ/cylDiameterZ)*cylDiameterZ ) );
		}
        text.leaveTextInputMode();
    }
}

/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial()
{
		double adjustedZ = (1.0+adjustmentZ/(cylinder.getZAxis()*2))*cylinder.getZAxis()*2;
		outputFile << fixed << setprecision(3) << totalTrialNumber << " " << factors.getCurrent().at("CylinderHeight") << " " << factors.getCurrent().at("CylinderOrientation") << " " << factors.getCurrent().at("Distances") << " " << adjustedZ << " " << cylinder.getZAxis()*2 << " " <<  trialTimer.getElapsedTimeInMilliSec() << endl;
		if (factors.isEmpty())
		{
			Beep(1000,1000);
			exit(0);
		}
        trialMode = HANDONSTARTMODE;
        paintGL();

		factors.next();
		double cylHeight = factors.getCurrent().at("CylinderHeight");
		double cylXRadius = cylHeight * util::str2num<double>(parameters.find("CylinderXAxisProportionToHeight"));
		double rangeBoundScaleZ = util::str2num<double>(parameters.find("RangeBoundaryScaleZ"));
		double cylZAxis = (double) mathcommon::unifRand( cylHeight*rangeBoundScaleZ, cylHeight*(2-rangeBoundScaleZ) );
        generateStimulus(cylXRadius,cylZAxis,cylHeight);
        initProjectionScreen(factors.getCurrent().at("Distances"));
		// This generates 38 possible different initial adjustments in a range 0.0-2.0
		scaleAdjustmentZ = (double) mathcommon::unifRand(2,40)/20.0;
		totalTrialNumber++;
		beepTrial();
		trialMode=STIMULUSMODE;
		trialTimer.start();
}

/**
 * @brief idle
 */
void idle()
{
    eyeLeft = Vector3d(-interoculardistance/2,0,0);//headEyeCoords.getLeftEye().p;
    eyeRight = Vector3d(interoculardistance/2,0,0);//headEyeCoords.getRightEye().p;

    if ( headCalibrationDone==3 )
    {
		trialFrame++;
        if (!isSaving)
            return;
    }
}
double frame=0.0;
void drawStimulus()
{
	frame+=1;
    double theta = factors.getCurrent().at("CylinderOrientation") + 90;
	double cylDiameterZ = 2*cylinder.getZAxis();
	// Draw inner black cylinder, very complicate transformation!
    double cylHeight = cylinder.getHeight();
    double XRadius = cylinder.getXAxis();

    glPushMatrix();
    glPushMatrix();
    glLoadIdentity();
    glTranslated(visualStimCenter.x(),visualStimCenter.y(),visualStimCenter.z());
    glRotated(theta,0,1,0);
    if (debugRotate)
		glRotated(90,1,0,0);
	glScaled(1,1, 1+adjustmentZ/(cylDiameterZ));
	//glRotated(frame,0,1,0);
	stimDrawer.draw();
    glPopMatrix();
	
    glPushMatrix();
    glLoadIdentity();
    glTranslated(visualStimCenter.x(), visualStimCenter.y(), visualStimCenter.z());
    glColor3fv(glBlack);
    GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad,GLU_FILL);
	glRotated(90,1,0,0);
	glTranslated(0,0,-cylHeight/2);
	glRotated(-theta,0,0,1);
	if (debugRotate)
		glRotated(-90,1,0,0);

	glScaled(1.0,(1.0+adjustmentZ/cylDiameterZ)*cylinder.getZAxis()/cylinder.getXAxis(),1.0);
	glScaled(0.99,0.99,0.99);
	gluCylinder(quad,XRadius,XRadius,cylHeight,40,2);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch ( trialMode )
    {
    case STIMULUSMODE:
    {
        drawStimulus();
    }
    break;
    }
}

/**
 * @brief generateStimulus
 * @param XRadius
 * @param zAxis
 * @param height
 */
void generateStimulus(double XRadius,double zAxis, double height)
{
	double stimulusDensity = util::str2num<double>(parameters.find("StimulusDensity"));
    int nStimulusPoints=(int)(stimulusDensity*height*mathcommon::ellipseCircumferenceBetter(XRadius,zAxis));

    cylinder.setNpoints(nStimulusPoints);
    cylinder.setAperture(0,2*M_PI);
    cylinder.setAxesAndHeight(XRadius,zAxis,height);
    cylinder.setFluffiness(0.001);
    cylinder.compute();

    // Update the stimulus drawer
    stimDrawer.setSpheres(false);

    stimDrawer.setStimulus(&cylinder);
    stimDrawer.initList(&cylinder,glRed,3);

    visualStimCenter << 0,0,factors.getCurrent().at("Distances");
}

/**
 * @brief paintGL
 */
void paintGL()
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
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

/**
 * @brief updateGL
 */
void updateGL(int val)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, updateGL, 0);
}

/**
 * @brief initializeOptotrakMonitor
 */
void initializeOptotrakMonitor()
{
    // Move the monitor in the positions
    //RoveretoMotorFunctions::homeMirror(3500);
    //RoveretoMotorFunctions::homeScreen(3500);
    //RoveretoMotorFunctions::homeObjectAsynchronous(3500);

    //optotrak.setTranslation(calibration);
    //if ( optotrak.init(LastAlignedFile) != 0)
    //    exit(0);
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
    char *argv[] = { "", NULL }; //must be NULL terminated otherwise glut crashes
    int argc = 1;
    glutInit( &argc, argv );
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("Shape Constancy Experiment - Adjustment");
    }
    else
    {   glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }

    glClearColor(0.0,0.0,0.0,1.0);
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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 'Q':
    case 27:
    {   exit(0);
    }
    break;
	case 'h':
	case 'H':
	{
			debugRotate=!debugRotate;
	}
	break;
	case 'i':
    case 'I':
	{
        infoDraw=!infoDraw;
	}
	break;
    case '4':
    {
		double delta = util::str2num<double>(parameters.find("CylinderZAxisAdjustmentStepMM"));
		adjustmentZ -= delta;
		if ( scaleAdjustmentZ<delta )
		{	scaleAdjustmentZ=delta;
			Beep(600,500);
		}
		break;
    }
    case '6':
    {
		double delta = util::str2num<double>(parameters.find("CylinderZAxisAdjustmentStepMM"));
		adjustmentZ += delta;
		if ( scaleAdjustmentZ<delta )
		{	scaleAdjustmentZ=delta;
			Beep(600,500);
		}
		break;
    }
	case '0':
	{
		advanceTrial();
		break;
	}
    }
}

/**
 * @brief handleResize
 * @param w
 * @param h
 */
void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

/**
* @brief initProjectionScreen
* @param _focalDist The focal distance to use
*/
void initProjectionScreen(double _focalDist)
{
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0,0,_focalDist));
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
}

/**
 * @brief initializeExperiment Set all the variables for the experiment, streams, parameter files and factors list
 */
void initializeExperiment()
{
    ifstream inputParameters(parametersFileNames[paramIndex]);
    parameters.loadParameterFile(inputParameters);

    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string outputFileName  = baseDir + "adjustmentFile_" + subjectName + ".txt";

    // Check for output file existence
    if ( !util::fileExists((outputFileName)) )
        outputFile.open(( outputFileName ).c_str());
    cerr << "File " << outputFileName << " loaded successfully" << endl;
	
	outputFile << "# totalTrialNumber CylinderHeight CylinderOrientation Distances adjustedZ CylinderZDiameter TrialTime[ms] " << endl;

	interoculardistance = util::str2num<double>(parameters.find("IOD"));
	// Initialize the factors list
    factors.init(parameters);
	//factors.print(cerr); // to see debug information
    factors.next();

    // Generate and initialize the stimulus with correct dimensions
    double cylHeight = factors.getCurrent().at("CylinderHeight");
	double cylXRadius = cylHeight * util::str2num<double>(parameters.find("CylinderXAxisProportionToHeight"));
	double rangeBoundScaleZ = util::str2num<double>(parameters.find("RangeBoundaryScaleZ"));
	double cylZAxis = (double) mathcommon::unifRand( cylHeight*rangeBoundScaleZ, cylHeight*(2-rangeBoundScaleZ) );;
	generateStimulus(cylXRadius,cylZAxis,cylHeight);
    // Generate the correct focal distance
    initProjectionScreen(factors.getCurrent().at("Distances"));

	trialTimer.getElapsedTimeInMilliSec();
}

/**
 * @brief main
 */
int main()
{
    cout << "Please select the parametersFileName" << endl;
    for (int i=0; i<2; i++)
        cout << i << ") " << parametersFileNames[i] << endl;
    //cin >> paramIndex;
    cout << "Selected " << parametersFileNames[paramIndex] << endl;

    randomizeStart();
	//RoveretoMotorFunctions::homeMirror(3500);
    RoveretoMotorFunctions::homeScreen(3500);
    
    //initializeOptotrakMonitor();
    initializeGL();
    initializeExperiment();
    // Define the callbacks functions
    glutIdleFunc(idle);
    glutDisplayFunc(paintGL);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, updateGL, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    // Application main loop
    glutMainLoop();
    return 0;
}
