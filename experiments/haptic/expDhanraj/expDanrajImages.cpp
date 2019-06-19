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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Core>

#include "IncludeGL.h"

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Marker.h"
#include "Util.h"
#include "Optotrak2.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "GLText.h"
#include "GLTexture.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"

#include "LatestCalibration.h"

using namespace std;
using Eigen::Vector3d;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

// Function prototypes
void generateStimulus();
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);

Vector2d imageShift(-8.0,-16.5);

/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;
double homeFocalDistance=-418.5;
double baseSize=0.0; // this means that the image is 10.0 centimeters at -420.0
Vector3d platformCalibration(250.0,-100.0,-500.0);

static const Vector3d rodTipHome(344.95,-213.77,-824.6); //relativo calibration 29 maggio 2013 in coordinate OpenGL

/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords, indexCoords;
Vector3d eyeLeft;
Vector3d eyeRight;
Vector3d index;

Vector3d physicalRigidBodyTip;
Vector3d platformIndex;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=3;
int fingerCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;

bool allVisibleIndex=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool infoDraw=true;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
BalanceFactor<std::string> factors;

const int BLACKSCREENMODE=0;
const int STIMULUSMODE=1;

int trialMode = BLACKSCREENMODE;

/****** TIMING *****/
Timer globalTimer;
Timer trialTimer;

unsigned int invisibleIndexFrames=0;
unsigned int trialFrame=0;
int drawingTrialFrame=0;
int totalTrialNumber=0;

/****** STIMULI VARIABLES ******/
GLTexture textureStimulus;

/**** FILES VARIABLES ***/
ofstream markersFile;
Eigen::RowVector3d junk(9999,9999,9999);
int paramIndex=0;
char *parametersFileNames[] = {"C:/cncsvisiondata/parametersFiles/Danraj/parametersExpDanrajImages_training.txt",
                               "C:/cncsvisiondata/parametersFiles/Danraj/parametersExpDanrajImages.txt"
                              };

char *texturesList[] = {
	"C:/cncsvisiondata/parametersFiles/Danraj/images/image01.bmp",
	"C:/cncsvisiondata/parametersFiles/Danraj/images/image02.bmp",
	"C:/cncsvisiondata/parametersFiles/Danraj/images/image03.bmp",
	"C:/cncsvisiondata/parametersFiles/Danraj/images/image04.bmp"
};

/********* SOUNDS ********/

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
        case 3:  // When the head calibration is done then calibrate the fingers
        {   switch ( fingerCalibrationDone )
            {
            case 0:
                text.draw("Press F to record platform markers");
                break;
            case 1:
                text.draw("Move index on platform marker to record ghost finger tip, then press F");
                break;
            case 2:
            {
                text.draw("Move index to rigidbody tip to define starting position, then press F");
            }
            break;
            }
        }
        break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {   //text.draw("Trial mode " + stringify<int>(trialMode ));
			//text.draw("Trial number" + stringify<int>(totalTrialNumber));
			text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("InvIndexFrame= " + stringify<int>(invisibleIndexFrames));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("TrialTimer= " + stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
		}
        text.leaveTextInputMode();
    }
}


/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial()
{
        if (factors.isEmpty())
        {
            beepLong();
            beepLong();
            beepLong();
			RoveretoMotorFunctions::moveScreenAbsolute(-620.0,-418.5,3500);
            exit(0);
        }
		
		// Reinsert the trial if too much occluded frames
        if ( (invisibleIndexFrames ) / (drawingTrialFrame) > util::str2num<double>(parameters.find("MaxPercentOccludedFrames"))/100.0 )
        {
            map<string,string> currentFactorsList = factors.getCurrent();
            beepBad();
            cerr << "Reinserted current trial (InvIndex,drawingTrialFrame,totTrialFrame)" << invisibleIndexFrames << " " << drawingTrialFrame << " " << trialFrame << endl;
            factors.reinsert(currentFactorsList);
         }
		
		invisibleIndexFrames = trialFrame = drawingTrialFrame = 0;
        trialMode=BLACKSCREENMODE;
		paintGL();
		Timer sleepTimer; sleepTimer.sleep(util::str2num<int>(parameters.find("ISI")));

        factors.next();
        generateStimulus();
		initProjectionScreen(util::str2num<double>(factors.getCurrent().at("Distance")));

		totalTrialNumber++;
		boost::thread trialBeepThread( beepTrial );
		trialMode=STIMULUSMODE;
}

/**
 * @brief idle
 */
void idle()
{
	double deltaT = (double)TIMER_MS;
    optotrak.updateMarkers(deltaT);
    markers = optotrak.getAllMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);

    //if ( allVisiblePatch )
    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,deltaT);
    // update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,deltaT);

    index = indexCoords.getP1().p;
	
	eyeLeft = Vector3d(0,0,0);
    eyeRight = Vector3d(0,0,0);

    if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
    {
        if ( !allVisibleIndex )
			boost::thread invisibleBeep( beepInvisible);
		
		drawingTrialFrame+=(trialMode==STIMULUSMODE);
        trialFrame++;
		
		invisibleIndexFrames+=!allVisibleIndex;

        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    parameters.find("Condition") << "\t" <<
					totalTrialNumber << "\t" <<
                    trialFrame << "\t" <<
                    globalTimer.getElapsedTimeInMilliSec() << "\t" <<
					factors.getCurrent().at("ImageName") << "\t" << 
					factors.getCurrent().at("Distance") << "\t" <<
					( allVisibleIndex ? index.transpose() : junk ) << "\t" <<
					endl;
		}
}

void drawStimulus()
{
	double depth = util::str2num<double>(factors.getCurrent().at("Distance"));
	double sizeX = abs(baseSize/420.0*depth);
	double sizeY = sizeX/1600.0*1200.0;

	glPushMatrix();
	glTranslated(imageShift.x(),imageShift.y(),0.0);
	textureStimulus.drawFrame(sizeX,sizeY,depth);
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
        if (headCalibrationDone==3 && fingerCalibrationDone==3)
			drawStimulus();
    }
    break;
    case BLACKSCREENMODE:
    {
		
    }
    break;
    }
}

/**
 * @brief generateStimulus
 * @param xAxis
 * @param zAxis
 * @param height
 */
void generateStimulus()
{
	std::string basepath("");
	std::multimap<string,string> imagesDirectory = parameters.getAllParameters();
	cout << "......" << endl;
	for (std::multimap<string,string>::iterator iter = imagesDirectory.begin();iter!=imagesDirectory.end();++iter)
	{
		if (iter->first=="ImagesDirectory")
		{
			if (iter->second=="C")
				basepath+=iter->second+":";
			else
				basepath+=iter->second;
		}
	}
	cout << "......" << endl;
	std::string imageAbsoluteName = basepath + factors.getCurrent().at("ImageName");
	char *imageFilename = (char*)imageAbsoluteName.c_str(); 
	textureStimulus.loadBMP( imageFilename );
	textureStimulus.use();
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
    RoveretoMotorFunctions::homeMirror(3500);
    RoveretoMotorFunctions::homeScreen(3500);
    RoveretoMotorFunctions::homeObject(3500);
	RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome,3500);
	
    optotrak.setTranslation(calibration);
    optotrak.init(LastAlignedFile);
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
        glutCreateWindow("Shape Constancy Experiment");
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
	    break;
	}
	case '4':
	{
		imageShift.x()+=0.5;
		break;
	}
	case '6':
	{
		imageShift.x()-=0.5;
		break;
	}
	case '2':
	{
		imageShift.y()-=0.5;
		break;
	}
	case '8':
	{
		imageShift.y()+=0.5;
		break;
	}
	case ' ':
	{
		if ( headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE )
		{
			trialMode=BLACKSCREENMODE;
			advanceTrial();
			beepOk();
			break;
		}
    }
    break;
    case 'i':
    case 'I':
	{
		infoDraw=!infoDraw;
		cout << imageShift.transpose() << endl;
        break;
	}
    case 'f':
    case 'F':
    {
        if (headCalibrationDone < 3 )
            break;
        // Here we record the finger tip physical markers
        if ( allVisiblePlatform && (fingerCalibrationDone==0) )
        {   platformIndex=markers.at(16).p;
           
            fingerCalibrationDone=1;
            beepOk();
            break;
        }
        if ( (fingerCalibrationDone==1) && allVisibleFingers )
        {   
            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {
            infoDraw=false;
            paintGL();
			trialMode=STIMULUSMODE;
            physicalRigidBodyTip = indexCoords.getP1().p;
            fingerCalibrationDone=3;
			RoveretoMotorFunctions::moveObjectAbsolute(Vector3d(350,0,-800),rodTipHome,3500);
            beepTrial();
            trialFrame=0;
            drawingTrialFrame=0;
			trialTimer.start();
            globalTimer.start();
            break;
        }
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
	cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,_focalDist));
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
}

/**
 * @brief initializeExperiment Set all the variables for the experiment, streams, parameter files and factors list
 */
void initializeExperiment()
{
    ifstream inputParameters(parametersFileNames[paramIndex]);
    parameters.loadParameterFile(inputParameters);

	baseSize = util::str2num<double>(parameters.find("ImageSizeMM"));
    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string markersFileName  = baseDir + "markersFile_" + subjectName + "_" + parameters.find("Condition")+".txt";

    // Check for output file existence
    if ( !util::fileExists((markersFileName)) )
        markersFile.open(( markersFileName ).c_str());
    cerr << "File " << markersFileName << " loaded successfully" << endl;

    markersFile << fixed << setprecision(3) <<
                "SubjectName\tCondition\tTrialNumber\tTrialFrame\tTotTime\tfImageName\tfDistance\tIndexXraw\tIndexYraw\tIndexZraw" << endl;

	// Initialize the factors list
    factors.init(parameters);
	//factors.print(cerr); // to see debug information
    factors.next();

	generateStimulus();

    // Generate the correct focal distance
    initProjectionScreen(util::str2num<double>(factors.getCurrent().at("Distance")));

}

/**
 * @brief main
 */
int main()
{
    cout << "Please select the parametersFileName" << endl;
    for (int i=0; i<2; i++)
        cout << i << ") " << parametersFileNames[i] << endl;
    cin >> paramIndex;
    cout << "Selected " << parametersFileNames[paramIndex] << endl;

    randomizeStart();
    initializeOptotrakMonitor();
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
