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

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <boost/filesystem.hpp>
/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
//#include "Optotrak.h"
#include "Marker.h"
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"

#include "Util.h"
#include "VmxLinearMotor.h"
#include "RoveretoMotorFunctions.h"

#include "EulerExtractor.h"

#include "LatestCalibration.h"

#define SCREENSPEED 4500
#define RODSPEED 4500


/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace RoveretoMotorFunctions;
//using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 *optotrak;
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;
EulerExtractor eulerAngles;


double focalDistance= -568.5, homeFocalDistance=-418.5;
static const Vector3d center(0,0,focalDistance);
static const Vector3d mirrorHome(-415.5,210.13,205.16);
static const Vector3d rodTipHome(-73.6,327.82,-386.2);
static const Vector3d rodAway(350,0,-800);
/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, visualThumb, visualIndex, physicalRigidBodyTip(0,0,0), platformThumb, platformIndex, hapticRodCenter(0,0,focalDistance),visualRodCenter;
vector <Marker> markers;
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=0, fingerCalibrationDone=0, platformCalibrationDone=0;
bool allVisibleHead=false, allVisiblePatch=false, allVisibleThumb=false, allVisibleIndex=false, allVisibleFingers=false, allVisiblePlatform=false, infoDraw=true;
bool indexInside[2];

/********* TRIAL VARIABLES *********/
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;
int trialMode = HANDONSTARTMODE;
int isDrawing = trialMode;
bool trialOk=true;
ParametersLoader parameters;
BalanceFactor<double> trial;
BalanceFactor<int> module;
map <std::string, double> factors;
map <std::string, int> block;
double fingerDistance=0;
/********* STIMULI *******/
CylinderPointsStimulus cylinder[3];
StimulusDrawer stimDrawer[3];
// PRE-POST TEST PHASE
double testStimHeight, testStimRadius;
// ADAPTATION PHASE VARIABLES
double adaptStimHeight, adaptStimRadius;
vector<double> adaptOffsets;
double ballRadius;

Timer globalTimer,delayedTimer,totalTime, frameTimer;
int trialFrame=0,totalFrame=0,drawingTrialFrame=0;
int occludedFrames=0;
int block0TrialNumber=0, block1TrialNumber=0,block2TrialNumber=0, totalTrialNumber=0;
bool isMovingRod=false;
/******** STREAMS AND FILES *******/
ofstream responseFile;
ofstream markersFile;
/******** FUNCTION PROTOTYPES ******/
void beepOk();
void beepBad();
void beepTrial();
void advanceTrial();
void initVariables();
void initStreams();
void cleanup();
void drawInfo();
void drawStimulus();
void drawFingerTips();
void drawTrial();
void drawGLScene();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void initProjectionScreen(double _focalDist);
void update(int value);
void checkBounds();
void idle();
void moveRod(const Vector3d &p, int speed, bool hasFeedback=true);
void initStimulus(double h, double r, int npoints);
void initOptotrak();
void initRendering();


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

void initStimulus(double height, double radius )
{
    for (int i=0; i<3; i++)
    {
        cylinder[i].setNpoints(150);
        cylinder[i].setAperture(0,2*M_PI);
        cylinder[i].setRadiusAndHeight(radius,height);
        cylinder[i].setFluffiness(0.001);
        cylinder[i].compute();

        // update the stimulus drawer
        stimDrawer[i].setSpheres(false);
        stimDrawer[i].setStimulus(&cylinder[i]);
        stimDrawer[i].initList(&cylinder[i],glRed,3);
    }
}


void advanceTrial()
{   if ( trialMode == STIMULUSMODE )
    {   bool pretrialMode=trialMode;
        trialMode++;
        trialMode=trialMode%2;
        totalTrialNumber++;

        if (block.at("Phase") != 1 )
        {
            double percentOccludedFrames = ((double)occludedFrames/(double)drawingTrialFrame )*100.0;
            if ( percentOccludedFrames > str2num<double>(parameters.find("TestPercentOccludedFrames")) )
            {
                cerr << "Percent occluded frames is" << percentOccludedFrames << " " << occludedFrames << " over " << drawingTrialFrame << endl;
                trial.reinsert(factors);

            }
            drawingTrialFrame=0;
            occludedFrames=0;
        }
        trialFrame=0;
        switch( block.at("Phase") )
        {
        case 0:  //pre-test
        {
            if ( !trial.isEmpty() )	// ci son ancora trial
            {   beepTrial();
                initStimulus(testStimHeight,testStimRadius);
                factors = trial.getNext();
                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter  = rodAway;
                initProjectionScreen(visualRodCenter.z());
                moveScreenAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                beepTrial();
            }
            else // si prepara per la modalità 1 (adaptation)
            {   trialMode = HANDONSTARTMODE;
                block = module.getNext();
                initStimulus(testStimHeight,testStimRadius );
                double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter=Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter=Vector3d(0,0, (zadaptmin+zadaptmax)/2);
                initProjectionScreen( visualRodCenter.z());

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 )
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                else
                    hapticRodCenter = rodAway;

                moveRod(Vector3d(0,0,hapticRodCenter.z() ),RODSPEED);

                checkBounds();
                beepLong();
            }
            block0TrialNumber++;
        }
        break;
        case 1:  // adaptation
        {
            block1TrialNumber++;
            if ( block1TrialNumber < str2num<int>(parameters.find("AdaptTrials")) )
            {   beepTrial();
                initStimulus(testStimHeight,testStimRadius);
                double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter = Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter=Vector3d(0,0, (zadaptmin+zadaptmax)/2);

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 )
                {
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                    moveScreenAbsoluteAsynchronous(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                    moveRod(hapticRodCenter,RODSPEED);
                }
                else
                {
                    hapticRodCenter = rodAway;
                    moveScreenAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                }

                initProjectionScreen( visualRodCenter.z());
                beepTrial();
            }
            else
            {   beepLong();
                trialMode = HANDONSTARTMODE;
                block  = module.getNext();
                // reset the factors in order to prepare the next test phase
                trial.init(parameters);
                factors.clear();
                factors  = trial.getNext();
                //cerr << "DISTANCE= " << factors.at("Distances") << endl;
                initStimulus(testStimHeight,testStimRadius);
                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter = rodAway;

                moveScreenAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                moveRod(rodAway,RODSPEED);
                initProjectionScreen(visualRodCenter.z());

                beepTrial();
            }
        }
        break;
        case 2:  /// post-test
        {   if ( trial.isEmpty() )
            {   beepLong();
                cleanup();
                exit(0);
            }
            else
            {   beepTrial();
                initStimulus(testStimHeight,testStimRadius);
                factors = trial.getNext();
                cerr << "DISTANCE= " << factors.at("Distances") << endl;
                visualRodCenter =  Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter  = rodAway;
                initProjectionScreen(visualRodCenter.z());
                moveScreenAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                beepTrial();
            }
            block2TrialNumber++;
        }
        break;
        }
        globalTimer.start();
    }
}

void initVariables()
{   trial.init(parameters);
// module factor: 0 -> 1 -> 2
    module.init(1,false);

    module.addFactor("Phase",str2num<int>(parameters.find("Phase"),","));
    block = module.getNext();

    adaptStimHeight = str2num<double>(parameters.find("AdaptStimulusHeight"));
    adaptStimRadius = str2num<double>(parameters.find("AdaptStimulusRadius"));

// crea la lista degli offset in adaptation dati start, end e numero steps
    int nTrials = str2num<int>(parameters.find("AdaptTrials"));
    double adaptOffsetStart = str2num<double>(parameters.find("AdaptOffsetStart"));
    double adaptOffsetEnd = str2num<double>(parameters.find("AdaptOffsetEnd"));
    int adaptOffsetNSteps = str2num<int>(parameters.find("AdaptOffsetSteps"));
    if ( adaptOffsetNSteps > nTrials )
    {   cerr << "too much steps, they must be at max " << nTrials << endl;
        cin.ignore(1e6,'\n');
        exit(0);
    }

    double adaptOffsetDelta = (adaptOffsetEnd - adaptOffsetStart)/adaptOffsetNSteps;
    for (int i=0; i<= adaptOffsetNSteps ; i++)
        adaptOffsets.push_back(i*adaptOffsetDelta+adaptOffsetStart);
    while ( adaptOffsets.size() != nTrials+2 ) //+2 così non rogna nel markersFile
        adaptOffsets.push_back(adaptOffsetEnd );

    testStimHeight = str2num<double>(parameters.find("TestStimulusHeight"));
    testStimRadius = str2num<double>(parameters.find("TestStimulusRadius"));

    ballRadius = str2num<double>(parameters.find("BallRadius"));
    initStimulus(adaptStimHeight,adaptStimRadius);

    hapticRodCenter = Vector3d(0,0,focalDistance);

    indexInside[0]=indexInside[1]=false;
    globalTimer.start();
    frameTimer.start();

    //trial.print(cerr);
}

void initStreams()
{   ifstream inputParameters;
    inputParameters.open("parametersRobertGrasping_v4.txt");
    if ( !inputParameters.good() )
    {   cerr << "File doesn't exist" << endl;
//exit(0);
    }
    parameters.loadParameterFile(inputParameters);
	interoculardistance = util::str2num<double>(parameters.find("IOD"));
// WARNING:
// Base directory and subject name, if are not
// present in the parameters file, the program will stop suddenly!!!
// Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);
// Subject name
    string subjectName = parameters.find("SubjectName");

// Principal streams file
    string markersFileName  = "markersFile_" + subjectName + ".txt";
    string responseFileName =  "responseFile_"   + subjectName + ".txt";

// Check for output file existence
/// Response file
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());
    if ( !fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());
}

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Stop the optotrak
    optotrak->stopCollection();
    delete optotrak;
}

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
        {   text.draw("Phase 0 Trial number= " + stringify<int>(block0TrialNumber));
            text.draw("Phase 1 Trial number= " + stringify<int>(block1TrialNumber));
            text.draw("Phase 2 Trial number= " + stringify<int>(block2TrialNumber));
            text.draw("Trial mode " + stringify<int>(trialMode ));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("IsDrawing?= " + stringify<int>(isDrawing));
            text.draw("VisualRod= " + stringify<Eigen::Matrix<double,1,3> >(visualRodCenter.transpose()) );
            text.draw("HapticRod= " + stringify<Eigen::Matrix<double,1,3> >(hapticRodCenter.transpose()) );
            text.draw("AdaptOffsets= " + stringify< vector<double> >(adaptOffsets) );
            text.draw("ThisAdaptOffset= " + stringify< double >(adaptOffsets.at(block1TrialNumber)) );
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            if ( block.at("Phase")!=1 )
            {
                text.draw("CurrentFactor= " + stringify<int>(factors.at("Distances")) );
                text.draw("Remaining= " + stringify<int>( trial.getRemainingTrials() ));
            }
        }

        text.leaveTextInputMode();
    }
}

void drawStimulus()
{
    if ( isDrawing )
    {
        if ( block.at("Phase") == 1 )
        {
            glPushMatrix();
            glLoadIdentity();
            glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z());
            stimDrawer[0].draw();
            glPopMatrix();
        }
        else
        {
            glPushMatrix();
            glLoadIdentity();
            glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z()+factors.at("RelDepth")*0.5);
            stimDrawer[0].draw();
            glLoadIdentity();
            glTranslated(visualRodCenter.x()+factors.at("DeltaX")/2, visualRodCenter.y(), visualRodCenter.z()-factors.at("RelDepth")*0.5);
            stimDrawer[1].draw();
            glLoadIdentity();
            glTranslated(visualRodCenter.x()-factors.at("DeltaX")/2, visualRodCenter.y(), visualRodCenter.z()-factors.at("RelDepth")*0.5);
            stimDrawer[2].draw();
            glPopMatrix();
        }

    }
}

void drawFingerTips()
{   // ADAPTATION BLOCK
    if ( block.at("Phase") == 1 && headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE && isDrawing ) //adaptation
    {   glColor3fv(glRed);
        glPointSize(8);
        glBegin(GL_POINTS);
        switch ( str2num<int>(parameters.find("AdaptFinger")) )
        {
        case 0:
        {   glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
        }
        break;
        case 1:
        {   glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        }
        break;
        case 2:
        {   glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
            glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        }
        break;
        }
        glEnd();
        glPointSize(1);
    }
    // END ADAPTATION BLOCK
}

void drawTrial()
{   switch ( trialMode )
    {
    case HANDONSTARTMODE :
    {   // disegna le dita per debug
        if ( block.at("Phase")==1 && isDrawing==1 )
            drawFingerTips();
    }
    break;
    case STIMULUSMODE:
    {   // Disegna le dita solo durante la fase di adattamento
        if ( block.at("Phase")==1 && isDrawing==1 )
            drawFingerTips();
        drawStimulus();
    }
    break;
    }
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

void moveRod(const Vector3d& v, int speed, bool hasFeedback)
{   assert( (v.z() < homeFocalDistance) );	// too avoid overdisplacements!!!
    isMovingRod=true;
    if ( hasFeedback )
    {   // must be called 10 times!!!
        for ( int i=0; i<10; i++)
            idle();
        // gestisce l'offset del marker rispetto al centro del paletto
        double rodRadius=13;    //mm
        double markerThickness=3.5;

        Vector3d finalPosition=v-Vector3d(0,0,markerThickness+rodRadius/2);
        if ( !isVisible(finalPosition) )
        {   beepBad();
            cerr << "CAN'T MOVE MOTOR TO INFINITY!!!" << endl;
            return ;
        }
        // a precise feedback based misuration - 2 corrections
        for ( int i=0; i<2; i++)
        {   for ( int j=0; j<10; j++) // must be called 10 times!!!
                idle();
            moveObject(finalPosition-markers.at(5).p,speed);
        }
    }
    else
    {
        moveObjectAbsolute(v,rodTipHome-calibration,speed);
    }
    isMovingRod=false;
}


void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
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
            break;
        }
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headCalibrationDone=3;
            beepOk();
        }
    }
    break;
    case 'i':
        infoDraw=!infoDraw;
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
            fingerCalibrationDone=2;
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {   infoDraw=false;
            drawGLScene();
            physicalRigidBodyTip = indexCoords.getP1().p;
            fingerCalibrationDone=3;

            switch ( block.at("Phase") )
            {
            case 0:
            case 2:
            {   factors = trial.getNext();
                initStimulus(testStimHeight,testStimRadius);
                moveScreenAbsolute(factors.at("Distances"),homeFocalDistance,SCREENSPEED);
                initProjectionScreen(factors.at("Distances"));
                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter = rodAway;	// keep it away
                moveRod(rodAway,RODSPEED);
            }
            break;
            case 1:
            {   double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter = Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter = Vector3d(0,0,(zadaptmax+zadaptmin)/2);

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 )
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                else
                    hapticRodCenter = rodAway;

                initProjectionScreen( visualRodCenter.z() );
                initStimulus(str2num<double>(parameters.find("AdaptStimulusHeight")),str2num<double>(parameters.find("AdaptStimulusRadius")));
                moveScreenAbsoluteAsynchronous(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                moveRod(hapticRodCenter,RODSPEED);
            }
            break;
            }
            trialMode = HANDONSTARTMODE;
            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
            beepTrial();
            trialFrame=0;
            globalTimer.start();
            totalTime.start();
            break;
        }
    }
    break;
    }
}


void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void initProjectionScreen(double _focalDist)
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    cam.init(screen);
}


void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void checkBounds()
{   if ( fingerCalibrationDone==3 && headCalibrationDone==3 )
    {
        switch ( trialMode )
        {   /*
            Vector3d trueIndex = indexCoords.getP1().p;
            Vector3d trueThumb = thumbCoords.getP1().p;
            case STIMULUSMODE:
            {  // Check if the index tip stays inside the stimulus
            Vector2d indexOnPlane = Vector2d(trueIndex.x(),trueIndex.z());
            Vector2d hapticRodCenterOnPlane = Vector2d(hapticRodCenter.x(), hapticRodCenter.z()+10);
            double stimHeight=0, stimRadius=0;
            switch ( block.at("Phase") )
            {  case 0:
            case 2:
            {  stimHeight = str2num<double>(parameters.find("TestStimulusHeight") );
            stimRadius = str2num<double>(parameters.find("TestStimulusRadius") );
            }
            break;
            case 1:
            {  stimHeight = str2num<double>(parameters.find("AdaptStimulusHeight") );
            stimRadius = str2num<double>(parameters.find("AdaptStimulusRadius") );
            }
            break;
            }
            bool yInside = (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))<(stimHeight/2) && (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))> (-stimHeight/2) ;
            if ( ( (indexOnPlane - hapticRodCenterOnPlane).norm() <= stimRadius ) && yInside )
            indexInside[0]=true;
            else
            indexInside[0]=false;
            }

            break;
            */

        case HANDONSTARTMODE: //handsonstart
        {   // Define a sphere around physicalRigidBodyTip in which the thumb must stay
            bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
            if ( indexVisible )
            {   bool triggerCondition=false;
                if ( parameters.find("Task") == "MSE" && (block.at("Phase") != 1) )
                    triggerCondition = (globalTimer.getElapsedTimeInMilliSec() < 1500);
                else
                    triggerCondition=((indexCoords.getP1().p  - physicalRigidBodyTip).norm() < ballRadius);

                if ( triggerCondition ) //5 centimeters
                {   indexInside[0]=true;
                    if ( indexInside[1]==false)
                        globalTimer.start();
                }
                else
                {   indexInside[0]=false;

                    double distance = (indexCoords.getP1().p  - physicalRigidBodyTip).norm();
                    // IMPORTANTE: controlla che il dito sia uscito ma esclude i casi in cui il dito esce da invisibile e diventa visibile fuori dalla palla... una soglia decente è 10 mm
                    if ( indexInside[1]==true && distance<(ballRadius+10) ) //è appena uscito
                    {   globalTimer.start();
                        trialMode=STIMULUSMODE;
                    }
                }
            }
        }
        break;
        }
    }
}

void idle()
{   // Set the time during which the stimulus is drawn, it depents on the phase (adaption or test)
    double drawStimTime = 0;
    if ( block.at("Phase") == 1 )
        drawStimTime = str2num<double>(parameters.find("AdaptStimulusDuration"));
    else
        drawStimTime = str2num<double>(parameters.find("TestStimulusDuration"));
    double deltaT = (double)TIMER_MS;
    optotrak->updateMarkers(deltaT);
    markers = optotrak->getAllMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);

    //if ( allVisiblePatch )
    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,deltaT);
// update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,deltaT);
// update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,deltaT);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;

    checkBounds();
    indexInside[1]=indexInside[0];

    // Controlla i frames occlusi, se il dito di test è fuori dalla sfera
    // ed ha superato una percentuale di tempo maggiore di
    // TestPercentOccludedFrames rispetto al tempo di presentazione dello
    // stimolo allora fa un beep

    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && !indexInside[0] && (int) block.at("Phase")!=1 && isDrawing )
    {
        bool visibleFinger=true;
        switch ( str2num<int>(parameters.find("TestFinger")) )
        {
        case 0:
            visibleFinger=isVisible(thumbCoords.getP1().p);
            break;
        case 1:
            visibleFinger=isVisible(indexCoords.getP1().p);
            break;
        case 2:
            visibleFinger=isVisible(thumbCoords.getP1().p) && isVisible(indexCoords.getP1().p);
            break;
        }
        // XXX scommentare per far si che anche il polso venga controllato per missing frames
        //visibleFinger = visibleFinger && isVisible(markers.at(6).p);
        if ( !visibleFinger )
        {   occludedFrames++;
            if ( str2num<int>(parameters.find("AudioFeedback") ) )
                boost::thread invisibleBeep( beepInvisible);
        }
        trialOk=true;	// mentre disegna lo stimolo
    }
    // Qui durante la fase di delay dopo la scomparsa dello stimolo controlla che il numero
    // di frames occlusi non abbia superato il limite consentito
    if ( isDrawing==0 && headCalibrationDone==3 && (block.at("Phase")!=1) && !indexInside[0] )
    {
        double percentOccludedFrames = ((double)occludedFrames/(double)drawingTrialFrame )*100.0;
        trialOk=(percentOccludedFrames < str2num<double>(parameters.find("TestPercentOccludedFrames")));
    }
    // Calcola le coordinate delle dita offsettate durante la fase di adattamento
    if ( block.at("Phase") == 1 )
    {   double fingerOffset =   adaptOffsets.at(block1TrialNumber) ;
        switch ( str2num<int>(parameters.find("AdaptFinger")) )
        {
        case 0:  //index
            visualThumb = thumbCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            break;
        case 1:  //thumb
            visualIndex = indexCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            break;
        case 2:
        {   visualThumb = thumbCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            visualIndex = indexCoords.getP1().p +  Vector3d(0,0,fingerOffset);
        }
        break;
        }
        drawingTrialFrame=0;
        occludedFrames=0;
    }
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE )
    {
        if ( globalTimer.getElapsedTimeInMilliSec() <= drawStimTime )
        {   isDrawing=1;
            delayedTimer.start();
        }
        else
        {   // non disegna nulla
            if ( delayedTimer.getElapsedTimeInMilliSec() < str2num<int>(parameters.find("DelayTime")) )
                isDrawing=0;
            else
            {   delayedTimer.start();
                advanceTrial();
            }
        }
    }
    if ( (int) block.at("Phase") != 1 && isDrawing==1 )
    {   drawingTrialFrame++;
    }
#define WRITE

    int thisBlockTrialNumber=0;
    switch ( block.at("Phase") )
    {
    case 0:
        thisBlockTrialNumber=block0TrialNumber;
        break;
    case 1:
        thisBlockTrialNumber=block1TrialNumber;
        break;
    case 2:
        thisBlockTrialNumber=block2TrialNumber;
        break;
    }

    if ( !isMovingRod )
    {
        RowVector3d junk(9999,9999,9999);
// Write to file
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {


            if ( totalFrame==0)
            {
                markersFile << fixed << setprecision(3) <<
                            "SubjectName\tAdaptFinger\tTestFinger\tAdaptOffset\tAdaptHapticFeedback\tFingerDist\tTotalTrial\tPhase\tTrialNumber\tTrialFrame\tdT\tTotTime\tIndexInside\tIsDrawing\tTrialOk\tVisualStimX\tVisualStimY\tVisualStimZ\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw" << endl;
                totalFrame++;
            }
            markersFile << fixed << setprecision(3) <<
                        parameters.find("SubjectName") << "\t" <<
                        parameters.find("AdaptFinger") << "\t" <<
                        parameters.find("TestFinger") << "\t" <<
                        adaptOffsets.at(block1TrialNumber) << "\t" <<
                        parameters.find("AdaptHapticFeedback") << "\t" <<
                        fingerDistance << "\t" <<
                        totalTrialNumber << "\t" <<
                        block.at("Phase") << "\t" <<
                        thisBlockTrialNumber << "\t" <<
                        trialFrame << "\t" <<
                        deltaT*1000 << "\t" <<
                        totalTime.getElapsedTimeInMilliSec() << "\t" <<
                        indexInside[0] << "\t" <<
                        isDrawing << "\t" <<
                        trialOk << "\t" <<
                        visualRodCenter.transpose() << "\t" <<
                        hapticRodCenter.transpose() << "\t" <<
                        ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                        ( isVisible(eyeRight) ? eyeRight.transpose() : junk ) << "\t" <<
                        ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                        ( isVisible(thumbCoords.getP1().p) ? thumbCoords.getP1().p.transpose() : junk ) << "\t" <<
                        ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) << endl;

        }
        trialFrame++;
    }
#ifdef SAVEDATADAT
    eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );
    // Write datafile to real-time streaming of data
    ofstream outputfile;
    outputfile.open("data.dat");
    outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
    outputfile << "AdaptHapticFeedback: " << parameters.find("AdaptHapticFeedback") << endl;
    outputfile << "AdaptFinger: " << parameters.find("AdaptFinger") << endl;
    outputfile << "TestFinger: " << parameters.find("TestFinger") << endl;
    outputfile << "IndexInside: " << indexInside[0] << endl;
    //outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl << "Roll: " << toDegrees(eulerAngles.getRoll()) << endl;
    outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().p.transpose() << endl;
    outputfile << "EyeRight: " << headEyeCoords.getRightEye().p.transpose() << endl << endl;
    outputfile << "Thumb: " << thumbCoords.getP1().p.transpose() << endl;
    outputfile << "Index: " << indexCoords.getP1().p.transpose() << endl;
    outputfile << "Wrist: " << markers.at(6).p.transpose() << endl;
    outputfile << "IsDrawing: " << isDrawing << " TrialOk: " << trialOk << " OccludedFrames: " << occludedFrames << " DrawingTrialFrames: " << drawingTrialFrame << endl;
    outputfile << "HapticRodCenter: " << hapticRodCenter.transpose() << endl;
    outputfile << "VisualRodCenter: " << visualRodCenter.transpose() << endl;
    outputfile << "Markers(5): " << markers.at(5).p.transpose() << endl;
    outputfile << "AdaptOffset: " << adaptOffsets.at(block1TrialNumber) << endl;
    outputfile << "Phase: " << block.at("Phase") << endl;
    outputfile << "TrialNumber: " << thisBlockTrialNumber << endl;
    outputfile << "Total trials: " << totalTrialNumber << endl;
#endif
    frameTimer.start();
}

void initOptotrak()
{   optotrak=new Optotrak2();
    optotrak->setTranslation(calibration);
    if ( optotrak->init(LastAlignedFile) != 0)
    {   cleanup();
        exit(0);
    }
    for (int i=0; i<100; i++)
    {   optotrak->updateMarkers();
        markers = optotrak->getAllMarkers();
    }
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

    /** LIGHTS **/
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /** END LIGHTS **/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char*argv[])
{   randomizeStart();
    RoveretoMotorFunctions::homeScreenAsynchronous(2000);
    RoveretoMotorFunctions::homeObject(RODSPEED);
    RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,RODSPEED);

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("EXP WEXLER");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initStreams();
    initVariables();    // must stay here because they explicitly contain call to gl functions
    drawGLScene();

    glutIdleFunc(idle);
    // They depend on markers positions so they need to stay AFTER the idle callback
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    // Application main loop
    glutMainLoop();

    cleanup();

    return 0;
}
