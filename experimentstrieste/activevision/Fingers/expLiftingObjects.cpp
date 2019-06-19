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

#define SAVEDATADAT

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __linux__
#include "beep.h"
#endif

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

#include "IncludeGL.h"
#include "GLUtils.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Marker.h"
#include "Optotrak2.h"
#include "Mathcommon.h"

#include "VRCamera.h"
#include "CoordinatesExtractor2.h"

#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"

#include "Util.h"
#include "TriesteMotorFunctions.h"
#include "LatestCalibrationTrieste.h"

#define SCREENSPEED 2500
#define RODSPEED 5000

/************* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace TriesteMotorFunctions; //scommentato

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;

double focalDistance= baseFocalDistance, homeFocalDistance=baseFocalDistance;
static const Vector3d center(0,0,focalDistance);
int Dist=0;
//static const Vector3d rodTipHome(331.77, -272.35, -805.97);
static const Vector3d rodTipHome(502.60, 185.92, -757.62); /// Updated to the Aligned20130524_5.cam file: see LatestCalibrationTrieste
static const Vector3d rodAway(350,0,-800);
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, visualThumb, visualIndex, physicalRigidBodyTip(0,0,0), platformThumb, platformIndex, hapticRodCenter(0,0,focalDistance),visualRodCenter;
vector <Marker> markers;
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=0;
int fingerCalibrationDone=0;
int platformCalibrationDone=0;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool allVisibleThumb=false;
bool allVisibleIndex=false;
bool allVisibleFingers=false;
bool allVisiblePlatform=false;
bool infoDraw=true;
bool isSavingMarkers=false;
bool indexInside[2];

/********* TRIAL VARIABLES *********/
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;

ParametersLoader parameters;
BalanceFactor<double> trial;
// map <std::string, double> factors;

Timer globalTimer;
Timer deltaTimer;
int trialFrame=0,totalFrame=0,drawingTrialFrame=0,occludedFrames=0;
int totalTrialNumber=0;
double fingerDistance=0;

/******** STREAMS AND FILES *******/
ofstream markersFile;
ofstream responseFile;
int paramIndex=0;
char *parametersFileNames[] = {"parametersExpLiftingObjects.txt"};

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


void initializeExperiment()
{
    parameters.loadParameterFile("C:/cncsvisioncmake/build/Release/parametersExpLiftingObjects.txt");
    cerr << "Folders creation..." ;
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string markersFileName  =   "markersFile_" + subjectName + ".txt";
    string responseFileName =	"responseFile_"	+ subjectName + ".txt";

    // Check for output file existence
    if ( !util::fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());
    if ( !util::fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());

    // Initialize trial related variables
    trial.init(parameters);
    trial.print(cerr);
    trial.next();/// inizializza le variabili
    trial.getCurrent().at("Distance");
    // outp(0x378,255); /// Comunica con la porta parallela


    indexInside[0]=indexInside[1]=false;
    globalTimer.start();


    // Initialize camera and screen
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);

    // Initialize stream variables
    /*ifstream inputParameters;
    if ( !util::fileExists((string(parametersFileNames[paramIndex]))) )
       inputParameters.open(parametersFileNames[paramIndex]);
    cerr << "File " << parametersFileNames[paramIndex] << " loaded successfully" << endl;
    */

    /*   string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);*/

    // Subject name

}


/*************************** FUNCTIONS ***********************************/

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
                text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
                break;
            case 2:
            {
                text.draw("Move index and thumb to rigidbody tip to define starting position, then press F");
            }
            break;
            }
        }
        break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {
            text.draw("IS SAVING DATA" + stringify<int>((int)isSavingMarkers));
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
        }
        text.leaveTextInputMode();
    }
}

void drawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ((indexCoords.getP1().p  - physicalRigidBodyTip).norm() > util::str2num<double>(parameters.find("Ballradius")))
    {   glClearColor(1.0,0.0,0.0,1.0);
    } else {
        glClearColor(0.0,1.0,0.0,1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(eyeRight);
    drawInfo();
    glutSwapBuffers();

}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
    case ' ':
    {   // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            headEyeCoords.init(markers.at(18).p-Vector3d(ChinRestWidth,0,0),markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headCalibrationDone=3;
            beepOk();
        }
        break;
    }

    case 'i':
    {
        infoDraw=!infoDraw;
        break;
    }

    case 'f':
    case 'F':
    {   // Here we record the finger tip physical markers
        if ( allVisiblePlatform && (fingerCalibrationDone==0) )
        {
            platformIndex=markers.at(16).p;
            platformThumb=markers.at(15).p;
            fingerCalibrationDone=1;
            beepOk();
            break;
        }
        if ( (fingerCalibrationDone==1) && allVisibleFingers )
        {
            thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            // XXX sposto l'oggetto a home per permettere le dita di calibrarsi
            // TriesteMotorFunctions::homeObject(3500); //// XXX commentato: A TS no servi
            beepOk();
            break;
        }

        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {   infoDraw=false;
            drawGLScene();
            physicalRigidBodyTip = indexCoords.getP1().p;
            fingerCalibrationDone=3;
            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
            beepTrial();
            trialFrame=0;
            globalTimer.start();
            deltaTimer.start();
            break;
        }
        break;
    }
    // Experimenter press ENTER
    case 13:
    {
        if (deltaTimer.getElapsedTimeInMilliSec()<500)
            break;
        if ( trial.isEmpty() )
            exit(0);

        isSavingMarkers=!isSavingMarkers;
        if (isSavingMarkers==false)
        {
            beepOk();
            deltaTimer.start();
            trial.next();
            totalTrialNumber++;
        }
        /*
        else
        {
            if(trial.isEmpty())
                exit(0);
        }
        */
        break;
    }
    }
}

/**
* @brief Resize the OpenGL viewport
**/
void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}


void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void idle()
{
    // Set the time during which the stimulus is drawn, it depents on the phase (adaption or test)
    double deltaT = (double)TIMER_MS;
    optotrak.updateMarkers(deltaT);
    markers = optotrak.getAllMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(18).p);

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,deltaT);
    // update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,deltaT);
    // update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,deltaT);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;
    RowVector3d junk(9999,9999,9999);
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && isSavingMarkers )
    {
        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    totalTrialNumber << "\t" <<
                    trialFrame << "\t" <<
                    deltaT*1000 << "\t" <<
                    indexInside[0] << "\t" <<
                    trial.getCurrent().at("Distance")<<"\t"<<
                    ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                    ( isVisible(eyeRight) ? eyeRight.transpose() : junk) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk) << "\t" <<
                    ( isVisible(thumbCoords.getP1().p) ? thumbCoords.getP1().p.transpose() : junk) << "\t" <<
                    ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) << endl;
    }
    trialFrame++;
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && isSavingMarkers )
    {
#ifdef SAVEDATADAT
        ofstream outputfile;
        outputfile.open("data.dat");
        outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
        outputfile << "Total trials: " << totalTrialNumber << endl;
        outputfile << "Thumb: " << thumbCoords.getP1().p.transpose() << endl;
        outputfile << "Index: " << indexCoords.getP1().p.transpose() << endl;
        outputfile << "Wrist: " << markers.at(6).p.transpose() << endl;
        outputfile << "Distance: "<< trial.getCurrent().at("Distance")<<endl;
#endif
    }

}

/**
* @brief Initialize the Optotrak
**/
void initOptotrak()
{
    optotrak.setTranslation(frameOrigin);
    optotrak.init(LastAlignedFile,TS_N_MARKERS,TS_FRAMERATE,TS_MARKER_FREQ,TS_DUTY_CYCLE,TS_VOLTAGE);
    for (int i=0; i<100; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}

/**
* @brief Initialize the OpenGL+GLUT environment
**/
void initializeGL()
{
    int argc=1;
    char *argv[]= {""};
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode==false )
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("expLiftingObjects");
    }
    else
    {   glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char*argv[])
{
    randomizeStart();

    //// Temporaneo
    /*TriesteMotorFunctions::homeMirror(3500);
    TriesteMotorFunctions::homeMonitorAsynchronous(2000);
    TriesteMotorFunctions::homeObject(RODSPEED);
    TriesteMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,RODSPEED);*/

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();

    initializeGL();
    initializeExperiment();

    glutIdleFunc(idle);
    // They depend on markers positions so they need to stay AFTER the idle callback
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    // Application main loop
    glutMainLoop();


    return 0;
}
