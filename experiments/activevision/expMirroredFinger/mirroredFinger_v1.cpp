// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2012 Carlo Nicolini <carlo.nicolini@iit.it>
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
#include <sstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <boost/circular_buffer.hpp>

#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "Util.h"
#include "TrialGenerator.h"
#include "RoveretoMotorFunctions.h"

#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"


/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 10
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 310.0;    // millimeters

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords,thumbCoords, indexCoords;

/******** STIMULI **********/
CylinderPointsStimulus cylinder;
StimulusDrawer stimDrawer;
bool whichColor=false;

/********* CALIBRATION 28/Febbraio/2011   **********/
static const Vector3d calibration(-419.5, 500.0, 440.0);
static const Vector3d rodTipHome(-73.6,327.82,-386.2);
//static const double alignmentX =  -2.5 ;	 // Alignment between optotrak X axis and screen X axis
//static const double alignmentY =  20.0; // Alignment between optotrak Y axis and screen Y axis
double alignmentX =  4.25;
double alignmentY =  24.0;
static const double focalDistance= -600.0;
static double interoculardistance=65;
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),center );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight,realThumb,reflectedThumb, realIndex,reflectedIndex, physicalRigidBodyTip, platformThumb, platformIndex;
vector <Marker> markers;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/*** Streams File ***/
//ofstream markersFile;
//ofstream responseFile;
#define SQRT2HALF 0.707106781
const int THUMB_BUFFER_CAPACITY=10;
/*** STIMULI and TRIAL variables ***/
int headCalibrationDone=0, fingerCalibrationDone=0, platformCalibrationDone=0;
bool allVisibleHead=false, allVisiblePatch=false, allVisibleThumb=false, allVisibleIndex=false, allVisibleFingers=false, allVisiblePlatform=false, infoDraw=true;
bool passiveMode=false;

double mirrorAngle=0;
Eigen::Hyperplane<double,3> mirrorPlane = Eigen::Hyperplane<double,3>(Vector3d(0,0,-1),focalDistance);

// Trial related things
ParametersLoader parameters;

void drawFixation();
void idle();

// To display funny points strips!
boost::circular_buffer<Vector3d> thumbBuffer(THUMB_BUFFER_CAPACITY);
boost::circular_buffer<Vector3d> indexBuffer(THUMB_BUFFER_CAPACITY);

Vector3d getReflected(const Vector3d &p, const Hyperplane<double,3> &plane)
{   Vector3d r = p- 2*plane.signedDistance(p)*plane.normal();
	return r;
}


void drawCircle(double radius, double x, double y, double z, const GLfloat *color)
{   glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor3fv(color);
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glPopAttrib();
}

#ifdef _WIN32
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}
#endif
static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
int trialMode = FIXATIONMODE;

void cleanup()
{   // Stop the optotrak (Optotrak class should be RAII but for every evenience we stop it...)
    optotrak.stopCollection();
    for (int i=0; i<3; i++)
        Beep(880,220);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void drawInfo()
{   switch ( headCalibrationDone )
    {   case 0:
        {   allVisibleHead = isVisible(markers[17].p) && isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
            if ( allVisibleHead )
                glClearColor(0.0,1.0,0.0,1.0); //green light
            else
                glClearColor(1.0,0.0,0.0,1.0); //red light
        }
        break;
        case 1:
        {   
		if ( fingerCalibrationDone==0 )
		{
			GLText text;
			text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
            text.enterTextInputMode();
            text.draw("EL  " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()));
            text.draw("ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()));
			text.draw("INDEX " + stringify<int>(realIndex.x() ) + " " + stringify<int>(realIndex.y() ) + " " + stringify<int>(realIndex.z()));
			text.draw("REFLECTED INDEX " + stringify<int>(realIndex.x() ) + " " + stringify<int>(realIndex.y() ) + " " + stringify<int>(realIndex.z()));
			text.leaveTextInputMode();
		}
		}
        break;
    }
    // end if ( headCalibrationDone )
}

void drawFixation()
{
}

void drawTrial()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POINT_BIT );
	whichColor ? glColor3fv(glRed) : glColor3fv(glRed);
	//glColor3fv(glRed);
	glPointSize(15);
	glBegin(GL_POINTS);
	glVertex3dv(realIndex.data());
	!whichColor ? glColor3fv(glRed) : glColor3fv(glRed);
	glVertex3dv(reflectedIndex.data());
	glEnd();
	glPopAttrib();
	/*
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0,0,focalDistance-100);
	glRotated(mirrorAngle,0,1,0);
	glScaled(-1,1,1);
	stimDrawer.draw();
	glPopMatrix();
	*/
}

void initVariables()
{   //interoculardistance = str2num<double>(parameters.find("IOD"));
	cylinder.setNpoints(150);
    cylinder.setAperture(0,2*M_PI);
    cylinder.setRadiusAndHeight(10,50);
    cylinder.setFluffiness(0.001);
    cylinder.compute();
	
	// update the stimulus drawer
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&cylinder);
    stimDrawer.initList(&cylinder,glRed,3);
}

void initStreams()
{   /*
    string parametersFileName = "parametersActiveTwoPlanes.txt";
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
    if ( !exists(baseDir) )
        create_directory(baseDir);
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string responseFileName =   "responseFile_" + subjectName + ".txt";
    string markersFileName =    "markersFile_"  + subjectName + ".txt";

    // Check for output file existence
    // Response file
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());
    if (!fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());
    */
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

void drawGLScene()
{   if (stereo)
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

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {       //Quit program
        case 'q':
        case 27:
        {   cleanup();
            exit(0);
        }
        break;
		case ' ':
        {   // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
            if ( headCalibrationDone==0 && allVisibleHead )
            {   headEyeCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
                headCalibrationDone=1;
                beepOk();
            }
			break;
        }
        break;
		case 13:
			whichColor=(bool)(rand()%2);
		break;

        case 'f':
        case 'F':
        {   // Here we record the finger tip physical markers
            if ( allVisiblePlatform && (fingerCalibrationDone==0) )
            {   platformIndex=markers.at(16).p;
                platformThumb=markers.at(15).p;
                fingerCalibrationDone=1;
                beepOk();
                break;
            }
            if ( (fingerCalibrationDone==1) && allVisibleFingers )
            {   thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
                indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
                fingerCalibrationDone=3;
                beepOk();
				RoveretoMotorFunctions::homeObjectAsynchronous(3500);
                break;
            }	
        }
        break;
		case '+':
			{
			mirrorAngle+=10;
			mirrorPlane = Eigen::Hyperplane<double,3>(Vector3d(sin(toRadians(mirrorAngle)),0,cos(toRadians(mirrorAngle))),focalDistance);
			cout << mirrorAngle << endl;
			}
			break;
		case '-':
			{
			mirrorAngle-=10;
			mirrorPlane = Eigen::Hyperplane<double,3>(Vector3d(sin(toRadians(mirrorAngle)),0,cos(toRadians(mirrorAngle))),focalDistance);
			cout << mirrorAngle << endl;
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

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{   
    double deltaT=1E-2;
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();

	allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p);
	// update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p);
	// update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p);
	
    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

	realThumb = thumbCoords.getP1();
	realIndex = indexCoords.getP1();

	reflectedIndex = getReflected(realIndex,mirrorPlane);
	reflectedThumb = getReflected(realThumb,mirrorPlane);
}

void initOptotrak()
{   optotrak.setTranslation(calibration);
    if ( optotrak.init("cameraFiles/Aligned20110823") != 0)
    {   cleanup();
        exit(0);
    }

    for (int i=0; i<10; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}

int main(int argc, char*argv[])
{
	RoveretoMotorFunctions::homeObject(3500);
	RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,3500);
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
        glutCreateWindow("EXP MIRROR FINGER");
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
    /* Application main loop */
    glutIdleFunc(idle);
    glutMainLoop();

    cleanup();

    return 0;
}
