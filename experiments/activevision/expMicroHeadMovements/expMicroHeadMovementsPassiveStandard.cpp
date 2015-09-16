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

#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#endif

#include <boost/thread/mutex.hpp>

#include "Util.h"
#include "Mathcommon.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "Screen.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headCoordsFromFile,headCoordsRealtime;
BoxNoiseStimulus stimulus;
StimulusDrawer stimDrawer;
// Trial related things
ParametersLoader parameters;
std::map<string,double> factors;
/********** EYES AND MARKERS **********************/
vector <Marker> markersFromFile;
vector <Marker> markersRealTime;
bool readyToStart=false;
bool infoDraw=true;

int headCalibrationFromFileDone=0;
int headCalibrationRealTimeDone=0;
Vector3d eyeRightCalibration(0.0,0.0,0.0);
static const int PRECALIBRATION_MODE=-1;
static const int BLACK_MODE=0;
static const int PROBE_MODE=1;
static const int STIMULUS_MODE=2;
static const int RESPONSE_MODE=3;
int trialMode=PRECALIBRATION_MODE;
int trialNumber=0;


bool allVisiblePatch=false;
bool allVisibleHead=false;
// Set the passive mode, 0 is the standard passive, 
// 1 is the fixed eye
static const int PASSIVE_MODE_FILE_COORDS=0;
static const int PASSIVE_MODE_REALTIME_COORDS=1;
int passiveMode=0;


/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;
//bool useCircularOcclusor=false;
double focalDistance=-418.5;
double interocularDistance=65;
/****** OUTPUT FILES AND STREAMS *****/
ofstream responseFile;
ifstream markersFile;

/****** READLINE FUNCTION *******/
bool readline(ifstream &is, int &_trialNumber, int &_headCalibration, int &_trialMode, std::vector< Marker > &markers, std::map<string,double> &factors )
{  
   std::string linestring;
   if (!getline(is,linestring))
	   return false; // file is finished
   std::vector<std::string> tokens;
   tokenizeString(linestring,tokens," \t");
   _trialNumber = str2num<int>(tokens[0]);
   _headCalibration = str2num<int>(tokens[1]);
   _trialMode = str2num<int>(tokens[2]);
   
   for (int i=0; i<3;i++)
   {
	markers[1].p[i] = str2num<double>(tokens[9+i]);
	markers[2].p[i] = str2num<double>(tokens[12+i]);
	markers[3].p[i] = str2num<double>(tokens[15+i]);
	// Don't need to cast 17 and 18 when head is calibrated
	if (_headCalibration > 2 )
		continue;
	markers[17].p[i] = str2num<double>(tokens[3+i]);
	markers[18].p[i] = str2num<double>(tokens[6+i]);
   }

   factors["ZWidth"] = str2num<double>(tokens[18]);
   factors["Slant"] = str2num<double>(tokens[19]);
   factors["Tilt"] = str2num<double>(tokens[20]);
   factors["StimulusAnchored"] = str2num<double>(tokens[21]);
   return true;
}


/********* TIMING AND STIMULUS VARIABLES ***********/
double counter=0.0;
double frame=0.0;
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{
#ifdef _WIN32
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
#endif
    return;
}

void beepBad()
{
#ifdef _WIN32
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(220,660);
#endif
    return;
}

void drawCircle(double radius, double x, double y, double z, const GLfloat *color)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor3fv(color);
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glPopAttrib();
}

/**
 * @brief drawInfo
 */
void drawInfo()
{
	if (infoDraw==false)
		return;
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
	text.enterTextInputMode();
	text.draw("TrialMode=" + stringify<int>(trialMode));
	text.draw("ReadyToStart="+stringify<int>(readyToStart) );
	text.draw("FROM FILE:::");
	text.draw("HeadCalibration="+stringify<int>(headCalibrationFromFileDone));
	text.draw("RightEye="+stringify< Eigen::Matrix<int,1,3> >(headCoordsFromFile.getRightEye().transpose().cast<int>()) );
	text.draw("REALTIME:::");
	text.draw("HeadCalibration="+stringify<int>(headCalibrationRealTimeDone));
	text.draw("RightEye="+stringify< Eigen::Matrix<int,1,3> >(headCoordsRealtime.getRightEye().transpose().cast<int>()) );
	
	if ( allVisibleHead && allVisiblePatch )
	{
		text.draw("===== HEAD CALIBRATION POSSIBLE ==== PRESS SPACEBAR");
	}
	else
	{
		text.draw("=== HEAD CALIBRATION NOT POSSIBLE ====");
	}
	text.leaveTextInputMode();
}

/**
 * @brief initializeOptotrak
 */
void initializeOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {
        exit(0);
    }

    optotrak.updateMarkers();
    markersRealTime = optotrak.getAllMarkers();
}

/**
 * @brief idle
 */
void idle()
{
    // Update markers
    optotrak.updateMarkers();
	markersRealTime = optotrak.getAllMarkers();
    allVisiblePatch = isVisible(markersRealTime.at(1).p) && isVisible(markersRealTime.at(2).p) && isVisible(markersRealTime.at(3).p); 
	allVisibleHead =  isVisible(markersRealTime.at(17).p) && isVisible(markersRealTime.at(18).p) && allVisiblePatch;
	headCoordsRealtime.update(markersRealTime.at(1).p,markersRealTime.at(2).p,markersRealTime.at(3).p);
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
	char* argv[1];  //dummy argument list for glutinit()
	char dummyString[8];
	argv[0] = dummyString;  //hook up the pointer
	int argc = 1;
	glutInit(&argc, argv);

    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STEREO | GLUT_STENCIL );
    else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL );
    if (gameMode)
    {
        glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
    }
	else
	{
		glutInitWindowSize( SCREEN_WIDTH,SCREEN_HEIGHT);
        glutCreateWindow("Experiment Active Rigidity");
	}
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GLUT_STENCIL);
	
	glEnable(GL_STENCIL_TEST);
	// Increase quality of points
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
	glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief updateStimulus
 */
void updateStimulus()
{
    double stimSize = parameters.get("StimulusSize");
	double stimPeriod = parameters.get("StimulusPeriod");
	double zWidth = factors.at("ZWidth");
	int nStimulusPoints = (int)(stimSize*stimSize*parameters.get("StimulusDensity"));
	// Generate the plane stimulus
    if (parameters.find("StimulusType")=="Plane")
    {
        stimulus.setDimensions(stimSize,stimSize,0.001);
        stimulus.setNpoints(nStimulusPoints);
        stimulus.setFluffiness(0.001);
        stimulus.compute();
    }
    else if (parameters.find("StimulusType")=="Sinusoid")
    {
        stimulus.setDimensions(stimSize,stimSize,0.001);
        stimulus.setNpoints(nStimulusPoints);
        stimulus.setFluffiness(0.001);
        for (PointsRandIterator iter = stimulus.pointsRand.begin(); iter!=stimulus.pointsRand.end();++iter)
        {
            (*iter)->x = mathcommon::unifRand(-stimSize/2,stimSize/2);
            (*iter)->y = mathcommon::unifRand(-stimSize/2,stimSize/2);
            (*iter)->z = 0.5*zWidth*sin(((*iter)->x)/stimPeriod*M_PI);
        }
    }
    else
    {
        throw std::runtime_error("Wrong argument! Must be Sinusoid or Plane");
    }

    // Generate the front stimulus
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&stimulus);
    stimDrawer.initList(&stimulus,glRed,parameters.get("PointsSize"));
}

/**
* @brief readPassiveCalibrations
**/
void readPassiveCalibrations(ifstream &markersFile)
{
	int oldHeadCalibration=0;
	while ( headCalibrationFromFileDone < 3 )
	{
		oldHeadCalibration=headCalibrationFromFileDone;
		readline(markersFile,trialNumber,headCalibrationFromFileDone,trialMode,markersFromFile,factors);
		// First calibration
		if ( headCalibrationFromFileDone == 1 && oldHeadCalibration==0 )
		{
			headCoordsFromFile.init(markersFromFile.at(17).p,markersFromFile.at(18).p,markersFromFile.at(1).p,markersFromFile.at(2).p,markersFromFile.at(3).p,interocularDistance);
		}
		// Second calibration
		if ( headCalibrationFromFileDone == 2 && oldHeadCalibration==1 )
		{
			headCoordsFromFile.init(headCoordsFromFile.getLeftEye(),headCoordsFromFile.getRightEye(),markersFromFile.at(1).p,markersFromFile.at(2).p,markersFromFile.at(3).p,interocularDistance);
		}
		// Last calibration
		if ( headCalibrationFromFileDone == 3 && oldHeadCalibration==2 )
		{
			eyeRightCalibration = headCoordsFromFile.getRightEye();
			headCoordsFromFile.init(headCoordsFromFile.getLeftEye(),headCoordsFromFile.getRightEye(),markersFromFile.at(1).p,markersFromFile.at(2).p,markersFromFile.at(3).p,interocularDistance);
		}
	}
}

Screen getProjectionScreen(double width, double height,double alignmentX, double alignmentY, double focalDistance, const Eigen::Affine3d &HeadTransformation)
{
	Screen screen;
	screen.init(width,height, alignmentX, alignmentY, focalDistance );
	screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
	screen.transform(HeadTransformation);
	return screen;
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
	cam.init(getProjectionScreen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,focalDistance,Eigen::Affine3d::Identity() ));

    // Initialize all the streams for file output eccetera
	parameters.loadParameterFile("C:/cncsvisiondata/parametersFiles/Fulvio/expMicroHeadMovements/parametersExpMicroHeadMovementsActive.txt");
	// Initialize focal distance and interocular distance from parameter file
    focalDistance = parameters.get("FocalDistance");
    interocularDistance = parameters.get("IOD");
    infoDraw = (int)parameters.get("DrawInfo");
	if  (parameters.find("PassiveMode")=="standard")
		passiveMode = PASSIVE_MODE_FILE_COORDS;
	else if (parameters.find("PassiveMode")=="realtime")
		passiveMode = PASSIVE_MODE_REALTIME_COORDS;
	else
		throw std::runtime_error("Error in setting PassiveMode parameter in file");
	//useCircularOcclusor = (int)parameters.get("UseCircularOcclusor");
    
	// Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
	// Response file output
    string responseFileName  = baseDir + "responseFile_MicroHeadMovementsPassiveStandard_" + subjectName +".txt";
	// Input markers file
	string markersFileName = baseDir + "markersFile_MicroHeadMovementsActive_" + subjectName + ".txt";
    
	// Check for output file existence
    if ( !fileExists((responseFileName)) )
        responseFile.open(( responseFileName ).c_str());
	cerr << "File " << responseFileName << " loaded successfully" << endl;

	// Check for output file existence
    if ( !fileExists((responseFileName)) )
        markersFile.open(( markersFileName ).c_str());
	cerr << "File " << markersFileName << " loaded successfully" << endl;

    responseFile << "# SubjectName\tTrialNumber\tFocalDistance\tKeyPressed\tResponse" << endl;

	// Winds down the markers file till it finds the appropriate head calibration
	readPassiveCalibrations(markersFile);

	cerr << "EyeCoords: " << headCoordsFromFile.getLeftEye().transpose() << "\n" << headCoordsFromFile.getRightEye().transpose() << endl;
    // Update the stimulus
    updateStimulus();
}

/**
 * @brief drawProbe
 */
void drawProbe()
{
    glColor3fv(glRed);
	glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(2.5,10,10);
    glPopMatrix();
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    //////////////////////////////
    // Update stimuli
    double oscillationAmplitude=50.0;
    double oscillationPeriod=100.0;
    static double timeFrame=0.0;
    timeFrame+=1.0;
	
    if ( !factors.at("StimulusAnchored") )
    {
        // FISSO
        glPushMatrix();
        glLoadIdentity();
        Vector3d translationFactor(0,0,focalDistance);
        double instantPlaneSlant = factors.at("Slant");

        switch ( (int) factors.at("Tilt") )
        {
        case 0:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,0,1,0);
            glScaled(1/sin(toRadians( -90-factors.at("Slant"))),1,1);	//backprojection phase
            break;
        }
        case 90:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,1,0,0);
            glScaled(1,1/sin(toRadians( -90-factors.at("Slant") )),1); //backprojection phase
            break;
        }
        case 180:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,0,1,0);
            glScaled(1/sin(toRadians( -90-factors.at("Slant") )),1,1); //backprojection phase
            break;
        }
        case 270:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,1,0,0);
            glScaled(1,1/sin(toRadians( -90-factors.at("Slant") )),1); //backprojection phase
            break;
        }
        }
        stimDrawer.draw();
        glPopMatrix();
    }
    else
    {
        // ANCORATO
        glPushMatrix();
        glLoadMatrixd( (headCoordsFromFile.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance))).data() );
        stimDrawer.draw();
        glPopMatrix();
    }

	// Draw the occlusor
    if ( ((int)parameters.get("UseCircularOcclusor")==1) )
    {
		double maskRadius=parameters.get("CircularOcclusorRadius");
        glPushMatrix();
        glLoadIdentity();
		Vector3d projCoords;
		if ( factors.at("StimulusAnchored") )
			projCoords << project( (headCoordsFromFile.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance))).translation() );
		else
			projCoords << project( Vector3d(0,0,focalDistance) );
		circleMask( projCoords.x(), projCoords.y(), maskRadius );
        glPopMatrix();
       }

}

/**
 * @brief drawFixation
 */
void drawFixation()
{
	/*
    double circleRadius = parameters.get("MaxCircleRadius");	// millimeters
    double zBoundary    = parameters.get("MaxZOscillation"); // millimeters
    // Projection of view normal on the focal plane
    Vector3d directionOfSight = (headCoordsFromFile.getRigidStart().getFullTransformation().linear()*Vector3d(0,0,-1)).normalized();
    Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( headCoordsFromFile.getRightEye() , headCoordsFromFile.getRightEye()+directionOfSight );
    Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );
    double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);
    Vector3d opticalAxisToFocalPlaneIntersection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (headCoordsFromFile.getRightEye());
	*/
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch (trialMode)
    {
    case PRECALIBRATION_MODE:
    {
        drawFixation();
        break;
    }
    case BLACK_MODE:
    {
        break;
    }
    case PROBE_MODE:
    {
        drawProbe();
        break;
    }
    case STIMULUS_MODE:
    {
        drawStimulus();
        break;
    }
	case RESPONSE_MODE:
	{
		GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
		text.enterTextInputMode();
		text.setXY(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
		text.draw("PRESS 4 OR 6");
		text.leaveTextInputMode();
		break;
	}
    }
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

        cam.setEye(headCoordsFromFile.getRightEye());
        drawInfo();
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(headCoordsFromFile.getLeftEye());
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		if (passiveMode==PASSIVE_MODE_FILE_COORDS)
	        cam.setEye(headCoordsFromFile.getRightEye());
		if (passiveMode==PASSIVE_MODE_REALTIME_COORDS)
			cam.setEye(headCoordsRealtime.getRightEye());
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

/**
 * @brief advanceTrial
 * @param response
 */
void advanceTrial(int response)
{
    responseFile << parameters.find("SubjectName") << "\t" << response << endl;
    updateStimulus();
    beepOk();
	frame=0.0;
	while ( trialMode==RESPONSE_MODE )
	{
		if ( !readline(markersFile,trialNumber,headCalibrationFromFileDone,trialMode,markersFromFile,factors) )
			exit(0);
	}
}

/**
 * @brief keyPressEvent
 * @param key
 * @param x
 * @param y
 */
void keyPressEvent(unsigned char key, int x, int y)
{
    switch (key)
    {       //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
	case 'i':
	{
		infoDraw=!infoDraw;
		break;
	}
	case ' ':
	{
		switch ( headCalibrationRealTimeDone)
		{
		case 0:
		{
			if (allVisibleHead && allVisiblePatch )
			{
				headCoordsRealtime.init( markersRealTime.at(17).p,markersRealTime.at(18).p,	markersRealTime.at(1).p,markersRealTime.at(2).p,markersRealTime.at(3).p,interocularDistance);
				headCalibrationRealTimeDone=1;
				beepOk();
			}
			else
			{
				beepBad();
			}
			break;
		}
		case 1:
		{
			if (allVisiblePatch )
			{
				headCoordsRealtime.init( headCoordsRealtime.getLeftEye(),headCoordsRealtime.getRightEye(),markersRealTime.at(1).p,markersRealTime.at(2).p,markersRealTime.at(3).p,interocularDistance);
				beepOk();
			}
			else
			{
				beepBad();
			}
			break;
		}
		}
		break;
	}
    case 13:
    {
		if ( !readyToStart )
        {
			readyToStart=true;
            infoDraw=false;
			break;
        }
    }
    case '4':
    {
        if (trialMode==RESPONSE_MODE)
        {
            advanceTrial(4);
        }
        break;
    }
    case '6':
    {
        if (trialMode==RESPONSE_MODE)
        {
            advanceTrial(6);
        }
        break;
    }
    }
}

/**
 * @brief update
 * @param value
 */
void update(int value)
{
	if ( headCalibrationFromFileDone==3 && trialMode != RESPONSE_MODE && readyToStart )
	{
		bool isPassive=true;
		if ( isPassive )
		{
			cam.init( getProjectionScreen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,focalDistance,headCoordsFromFile.getRigidStart().getFullTransformation() ));
		}
		headCoordsFromFile.update(markersFromFile.at(1).p,markersFromFile.at(2).p,markersFromFile.at(3).p);
		if ( !readline(markersFile,trialNumber,headCalibrationFromFileDone,trialMode,markersFromFile,factors) )
			exit(0);
	}
	glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char*argv[])
{
    // Initialize the random seed
    randomizeStart();
    // Initializes the optotrak and starts the collection of points in background
    markersFromFile.resize(20);
	markersRealTime.resize(20);
    // Initialize the OpenGL context
    initializeGL();
    initializeExperiment();
	initializeOptotrak();
    // Setup the callback functions
    glutDisplayFunc(paintGL);
    glutKeyboardFunc(keyPressEvent);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();

}
