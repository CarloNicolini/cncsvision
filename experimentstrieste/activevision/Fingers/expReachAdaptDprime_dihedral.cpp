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
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

#ifdef __linux__
#include "beep.h"
#endif

#include "Marker.h"
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "WedgePointsStimulus.h"
#include "EllipsoidPointsStimulus.h"

#include "TrialGenerator.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "ParametersLoader.h"

#include "Util.h"

#include "EulerExtractor.h"
#include "../../../experiments/calibrations/LatestCalibration.h"
#include "RoveretoMotorFunctions.h"

#define SCREENSPEED 4500
#define RODSPEED 4500
#define TOTALBLOCKS 3

#define CURTRIAL trial[blocks.at(blockIndex)]
#define CURFACTORS trial[blocks.at(blockIndex)].getCurrent().first
#define CURSTAIRCASE trial[blocks.at(blockIndex)].getCurrent().second

using namespace std;
using namespace Eigen;
using namespace util;

static const bool gameMode=true;
static const bool stereo=true;

Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords;
CoordinatesExtractor2 thumbCoordsReal;
CoordinatesExtractor2 thumbCoordsVisual;
CoordinatesExtractor2 indexCoords;
RigidBody rigidStartThumb;
RigidBody rigidCurrentThumb;
RigidBody rigidStartIndex;
RigidBody rigidCurrentThumbIndex;
EulerExtractor eulerAngles;
VRCamera cam;
ParametersLoader parameters;
TrialGenerator<double> trial[TOTALBLOCKS];
StimulusDrawer stimDrawer;

double focalDistance= -418.5;
double homeFocalDistance=-418.5;

Vector3d rodTipHome(344.95,-213.77,-824.6);
Vector3d rodAway(350,0,-800);
Vector3d relDisplacementCylinder(0,0,0);
Vector3d physicalRigidBodyTip(0,0,0);
Vector3d platformThumb;
Vector3d platformIndex;
Vector3d hapticRodCenter(0,0,focalDistance);
Vector3d visualRodCenter;
RowVector3d junk(9999,9999,9999);
vector <Marker> markers;

double interoculardistance=65;
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
bool indexInside[2];
bool indexCameBack=false;
static const int TRIAL_MODE_HANDS_ON_START=0;
static const int TRIAL_MODE_HANDS_AWAY=1;
static const int TRIAL_MODE_RESPONSE=2;

static const int PHASE_ADAPTATION=0;
static const int PHASE_TEST_GRASP=1;
static const int PHASE_TEST_PERC=2;
int trialMode = TRIAL_MODE_HANDS_ON_START;
int totalTrialNumber=0;

bool handAway=false;
bool isMovingRod=false;
bool trialOk=false;
double stimulusTime=2000;
double fingerDistance=0;
unsigned int blockIndex=0;
int blocksTrialNumber[TOTALBLOCKS]= {0,0,0};
int drawingTrialFrame=0;
int occludedFrames=0;
int totalFrame=0;
int trialFrame=0;

// Mask properties
double maskWidth=80;
double maskHeight=10;
bool hasMask=true;
bool showProbe=false;
int testFinger=0;

Timer delayedTimer;
Timer frameTimer;
Timer globalTimer;
Timer totalTime;

vector<int> blocks;
double adaptStimHeight;
double adaptStimRadius;
vector<double> adaptOffsets;
double ballRadius=10;
int paramIndex=0;
char *blockNames[] = {"Adapt","Grasp","Perc"};
char *parametersFileNames[]=
{
    "C:/workspace/cncsvisioncmake/data/parametersFiles/parametersExpReachAdaptDprimeDihedral.txt",
    "C:/workspace/cncsvisioncmake/data/parametersFiles/parametersExpReachAdaptDprimeDihedral_training.txt",
};

ofstream markersFile;
ofstream responseFile;
ofstream errorLog("C:\cncsvisiondata\errorlog\errors_log.txt");

void beepBad();
void beepOk();
void beepTrial();
void checkBounds();
void drawFingerTips();
void drawGLScene();
void drawInfo();
void drawStimulusWedge();
void drawTrial();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void idle();
void initOptotrak();
void advanceTrialAdapt();
void advanceTrialTest(bool);
void initProjectionScreen(double _focalDist);
void initRendering();
void initializeExperiment();
void moveRod(const Vector3d &p, int speed, bool hasFeedback=true);
void update(int value);
void updateMarkers();
void updateTrialStimulus();


void drawCircle(double radius, double x, double y, double z)
{
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x,y-radius,z);
    glVertex3f(x,y+radius,z);
    glVertex3f(x-radius,0,z);
    glVertex3f(x+radius,0,z);
    glEnd();
}
/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepInvisible()
{   boost::mutex::scoped_lock lock(beepMutex);
    Beep(1000,5);
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
void beepComeBack()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(550,200);
    Beep(330,200);
}

/**
 * @brief updateMarkers
 */
void updateMarkers()
{
    double deltaT = (double)TIMER_MS;
    optotrak.updateMarkers(deltaT);
    markers = optotrak.getAllMarkers();

    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,TIMER_MS);
    // Update thumb coordinates
    thumbCoordsReal.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,TIMER_MS);
    thumbCoordsVisual.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,TIMER_MS);

    // Try to figure out the transformation of disk
    rigidCurrentThumb.setRigidBody(markers.at(11).p,markers.at(12).p,markers.at(13).p);
    rigidCurrentThumbIndex.setRigidBody(markers.at(7).p,markers.at(8).p,markers.at(9).p);

    rigidStartThumb.computeTransformation(rigidCurrentThumb);
    rigidStartIndex.computeTransformation(rigidCurrentThumbIndex);
    // Update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,TIMER_MS);
}

/**
 * @brief moveRod
 * @param v
 * @param speed
 * @param hasFeedback
 */
void moveRod(const Vector3d& v, int speed, bool hasFeedback)
{
    if ( hasFeedback )
    {
        updateMarkers();
        // gestisce l'offset del marker rispetto al centro del paletto
        double rodRadius=13;    //mm
        double markerThickness=3.5;
        Vector3d finalPosition=v-Vector3d(0,0,markerThickness+rodRadius/2.0);
        if ( !isVisible(finalPosition) )
        {   beepBad();
            errorLog << "ERROR in " << __FILE__ << " at line " <<  __LINE__<< "Can't move motors to infinity" << endl;
            return ;
        }
        // a precise feedback based misuration - 2 corrections
        for ( int i=0; i<2; i++)
        {   updateMarkers();
            //RoveretoMotorFunctions::moveObject(finalPosition-markers.at(5).p,speed);
        }
    }
    //else
    //RoveretoMotorFunctions::moveObjectAbsolute(v,rodTipHome,speed);
}

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {   exit(0);
        break;
    }
    case 'p':
    {
        CURSTAIRCASE->getCurrentStaircase()->print(cerr);
        break;
    }
    case ' ':
    {   // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {   headEyeCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        // Second calibration
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
    {
        // Save platform markes, here we record the finger tip physical markers
        if ( allVisiblePlatform && (fingerCalibrationDone==0) )
        {   platformIndex=markers.at(16).p;
            platformThumb=markers.at(15).p;
            fingerCalibrationDone=1;
            beepOk();
            break;
        }
        // Initialize thumb and index with platform markers, then put the platform away
        if ( (fingerCalibrationDone==1) && allVisibleFingers )
        {
            Vector3d offset(0,0,5);
            thumbCoordsReal.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            thumbCoordsVisual.init(platformThumb+offset, markers.at(11).p, markers.at(12).p, markers.at(13).p);

            // We need an auxiliary rigidbody to get the relative transformation of a disk and computing the normals
            Vector3d centroid = platformThumb;
            Vector3d centroidIndex = platformIndex;
            rigidStartThumb.setRigidBody(markers.at(11).p-centroid,markers.at(12).p-centroid,markers.at(13).p-centroid);
            rigidStartIndex.setRigidBody(markers.at(7).p-centroidIndex,markers.at(8).p-centroidIndex,markers.at(9).p-centroidIndex);

            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            beepOk();
            //RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(rodAway,rodTipHome,RODSPEED);
            break;
        }

        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {
            beepTrial();
            infoDraw=false;
            drawGLScene();

            physicalRigidBodyTip = indexCoords.getP1().p;
            trialMode = TRIAL_MODE_HANDS_ON_START;
            fingerDistance = (indexCoords.getP1().p-thumbCoordsReal.getP1().p).norm();
            trialFrame=0;
            globalTimer.start();
            totalTime.start();
            fingerCalibrationDone=3;
            break;
        }
        break;
    }
    case '4':
    {
        if ( blocks.at(blockIndex) == PHASE_TEST_GRASP || blocks.at(blockIndex) == PHASE_TEST_PERC )
            advanceTrialTest(true);
        else
            advanceTrialAdapt();
        break;
    }
    case '6':
    {
        if ( blocks.at(blockIndex) == PHASE_TEST_GRASP || blocks.at(blockIndex) == PHASE_TEST_PERC )
            advanceTrialTest(false);
        else
            advanceTrialAdapt();
        break;
    }
    }
}


/**
 * @brief initOptotrak
 */
void initOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile)!=0)
        errorLog << "ERROR in Optotrak initialization " << __FILE__ << " at line " << __LINE__ << endl;
}


/**
 * @brief initRendering
 */
void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void generateWedgeStimulus()
{
    double stimEdge = parameters.get("StimulusWidth");
    WedgePointsStimulus wedge;
    wedge.setPlanesSize(stimEdge,stimEdge);
    int nPoints = 2*parameters.get("StimulusDensity")*stimEdge*stimEdge;
    wedge.setNpoints(nPoints);
    wedge.setAngle(CURFACTORS.at("WedgeAngle"));
    wedge.compute();
    stimDrawer.setStimulus(&wedge);
    stimDrawer.initList(&wedge,glRed);
    stimDrawer.setSpheres(false);
}


void generateCylinderStimulus()
{
    double deltaXRods = unifRand(parameters.get("DeltaXMin"),parameters.get("DeltaXMax"));

    double ellipticCircumferenceApproximate = 4*3.1415*CURFACTORS.at("RelDepth")*deltaXRods/(CURFACTORS.at("RelDepth")+deltaXRods);
    int nStimulusPoints=2*(int)floor(ellipticCircumferenceApproximate*
                                     parameters.get("CylinderHeight")*parameters.get("StimulusDensity"));
    EllipsoidPointsStimulus ellipseBaseCylinder;
    ellipseBaseCylinder.setNpoints(nStimulusPoints);
    ellipseBaseCylinder.setAperture(0,2*M_PI);
    ellipseBaseCylinder.setAxesAndHeight(deltaXRods,CURFACTORS.at("RelDepth"),parameters.get("CylinderHeight"));
    ellipseBaseCylinder.setFluffiness(0.001);
    ellipseBaseCylinder.compute();

    // update the stimulus drawer
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&ellipseBaseCylinder);
    stimDrawer.initList(&ellipseBaseCylinder,glRed,3);
}


/**
 * @brief initVariables
 */
void initializeExperiment()
{
    // Load the parameters file from the selected input filename
    // Initialize the parameters file
    parameters.loadParameterFile(parametersFileNames[paramIndex]);
    interoculardistance = str2num<double>(parameters.find("IOD"));
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
    {
        cerr << "Directory " << baseDir << " doesn't exist, press enter to exit..." << endl;
        cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        //exit(0);
    }
    string markersFileName = baseDir + "markersFile_" + parameters.find("SubjectName") + ".txt";
    if ( fileExists(parameters.find("BaseDir")+markersFileName ) )
    {
        cerr << "!! " << markersFileName << " already exists" << endl;
    }

    markersFile.open(markersFileName.c_str());
    cerr << "Writing to markers file " << markersFileName << " OK" << endl;
    // Write the headers for markers file
    markersFile << fixed << setprecision(3) <<
                "SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tStimDiameter\tTrialFrame\tTotTime\tIndexInside\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw" << endl;

    string responseFileName = baseDir + "responseFile_" + parameters.find("SubjectName") + ".txt";
    if ( fileExists(parameters.find("BaseDir")+responseFileName ) )
    {
        cerr << "!! " << responseFileName << " already exists" << endl;
    }
    responseFile.open(responseFileName.c_str());

    // Write the headers for markers file
    responseFile << fixed << setprecision(3) <<
                 "SubjectNameBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tStimDiameter\tTrialFrame\tTotTime\tIndexInside\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw" << endl;


    blocks = util::str2num<int>(parameters.find("Phase"),",");

    // Initialize the staircase for the blocks
    for ( int i=0; i<TOTALBLOCKS; i++ )
    {
        trial[i].init(parameters);
    }

    // Initialize adaptation phase variables
    adaptStimHeight = str2num<double>(parameters.find("AdaptStimulusHeight"));
    adaptStimRadius = str2num<double>(parameters.find("AdaptStimulusRadius"));
    int nAdaptTrials = str2num<int>(parameters.find("AdaptTrials"));
    double adaptOffsetStart = str2num<double>(parameters.find("AdaptOffsetStart"));
    double adaptOffsetEnd = str2num<double>(parameters.find("AdaptOffsetEnd"));
    int adaptOffsetNSteps = str2num<int>(parameters.find("AdaptOffsetSteps"));
    double adaptOffsetDelta = (adaptOffsetEnd - adaptOffsetStart)/adaptOffsetNSteps;
    for (int i=0; i<= adaptOffsetNSteps ; i++)
        adaptOffsets.push_back(i*adaptOffsetDelta+adaptOffsetStart);
    while ( adaptOffsets.size() != nAdaptTrials+2 ) //+2 così non rogna nel markersFile
        adaptOffsets.push_back(adaptOffsetEnd );


    ballRadius = parameters.get("BallRadius");
    stimulusTime = parameters.get("TestStimulusTime");
    maskWidth = parameters.get("MaskWidth");
    maskHeight = parameters.get("MaskHeight");
    hasMask = str2num<int>(parameters.find("UseMask"));
    testFinger = str2num<int>(parameters.find("TestFinger"));

    hapticRodCenter = Vector3d(0,0,focalDistance);
    visualRodCenter << 0,0, CURFACTORS.at("Distances");
    indexInside[0]=indexInside[1]=false;
    generateWedgeStimulus();
    generateCylinderStimulus();
    updateTrialStimulus();
    initProjectionScreen(CURFACTORS.at("Distances"));
}


/**
 * @brief update
 * @param value
 */
void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
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
        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}


/**
 * @brief initProjectionScreen
 * @param _focalDist
 */
void initProjectionScreen(double _focalDist)
{
    //RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
    Screen screen;
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    cam.init(screen);
}


/**
 * @brief updateTrialStimulus
 */
void updateTrialStimulus()
{
    // Clean the buffers, clean the screen
    glDrawBuffer(GL_BACK);
    glDrawBuffer(GL_BACK_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glutSwapBuffers();
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glutSwapBuffers();

    // Clean the buffers, clean the screen
    glDrawBuffer(GL_FRONT);
    glDrawBuffer(GL_FRONT_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glutSwapBuffers();
    glDrawBuffer(GL_FRONT_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glutSwapBuffers();

    // Il paletto sta nella parte anteriore dello stimolo
    if ( blocks.at(blockIndex) == PHASE_ADAPTATION)
        generateCylinderStimulus();
    if ( blocks.at(blockIndex) == PHASE_TEST_GRASP || blocks.at(blockIndex) == PHASE_TEST_PERC )
        generateWedgeStimulus();
}

void advanceTrialAdapt()
{
    double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
    double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

    if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
        initProjectionScreen( mathcommon::unifRand(zadaptmin,zadaptmax) );
    else
        initProjectionScreen( (zadaptmin+zadaptmax)/2 );
}

void advanceTrialTest(bool resp)
{
    if ( trialMode != TRIAL_MODE_RESPONSE )
    {
        beepBad();
        return;
    }

    if ( CURTRIAL.next(resp) )
    {
        updateTrialStimulus();
        initProjectionScreen(CURFACTORS.at("Distances"));
        beepOk();
    }
    else
    {
        blockIndex++;
        if ( blockIndex >= blocks.size() )
        {
            Beep(220,880);
            Beep(220,880);
            Beep(220,880);
            exit(0);
        }
        updateTrialStimulus();
        Beep(330,880);
    }
    totalTrialNumber++;
    blocksTrialNumber[blocks.at(blockIndex)]++;
    trialMode = TRIAL_MODE_HANDS_ON_START;
    drawingTrialFrame=occludedFrames=trialFrame=0;
}


/**
 * @brief idle
 */
void idle()
{
    // Call to Optotrak motor functions and updating of markers position by deep copy.
    updateMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);
    //checkBounds();
    if ( fingerCalibrationDone==3 && headCalibrationDone==3 )
    {
        //trialMode = TRIAL_MODE_HANDS_AWAY;
        bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
        if (indexVisible)
        {
            // Update the old state of indexInside to new state of indexInside
            indexInside[1]=indexInside[0];
            double distance = (indexCoords.getP1().p  - physicalRigidBodyTip).norm();
            indexInside[0]=distance < ballRadius ;
        }
        bool visibleFinger=isVisible(thumbCoordsReal.getP1().p) && isVisible(indexCoords.getP1().p);
        if ( str2num<int>(parameters.find("CheckWrist"))==1 )
            visibleFinger = visibleFinger && isVisible(markers.at(6).p);
        if ( !visibleFinger && trialMode==TRIAL_MODE_HANDS_AWAY )
        {
            occludedFrames++;
            if ( str2num<int>(parameters.find("AudioFeedback") ) )
                boost::thread invisibleBeep( beepInvisible);
        }

        // Blocco GRASP, lo stimolo è triggerato dall'uscita del dito
        if ( blocks.at(blockIndex) == PHASE_TEST_GRASP )
        {
            if ( indexInside[0]==0 && indexInside[1]==1 && trialMode == TRIAL_MODE_HANDS_ON_START) // triggera l'apparizione
            {
                trialMode = TRIAL_MODE_HANDS_AWAY;
                globalTimer.start();
            }
        }

        if ( blocks.at(blockIndex) == PHASE_TEST_PERC ) // Blocco PERC, lo stimolo è triggerato dal tempo
        {
            if ( indexInside[1]==1 && trialMode == TRIAL_MODE_HANDS_ON_START) // triggera l'apparizione
            {
                trialMode = TRIAL_MODE_HANDS_AWAY;
                globalTimer.start();
            }
        }


        if (indexInside[0]==0 && visibleFinger)
        {
            if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY)
            {
                boost::thread comebackbeepthread(beepComeBack);
                trialMode = TRIAL_MODE_RESPONSE;
                globalTimer.start();
            }
        }

        if (indexInside[0]==1 && indexInside[1]==0)
            indexCameBack=true;
        else
            indexCameBack=false;

        if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY && visibleFinger && indexCameBack )
        {
            globalTimer.start();
        }

        //////////////////// WRITE TO FILE ////////////
        if ( globalTimer.getElapsedTimeInMilliSec() < stimulusTime && trialMode==TRIAL_MODE_HANDS_AWAY )
            handAway=true;
        else
            handAway=false;
        //"SubjectName\tFingerDist\tBlock\tTotalTrial\tTrialNumber\tfRelDepth\tfDistances\tStimWidth\tTrialFrame\tTotTime\tIndexInside\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw"
        char *currentBlockName = blockNames[blocks.at(blockIndex)];
        /*
        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    currentBlockName << "\t" <<
                    totalTrialNumber << "\t" <<
                    blocksTrialNumber[blocks.at(blockIndex)] << "\t" <<
                    CURFACTORS.at("RelDepth") << "\t" <<
                    CURFACTORS.at("Distances") << "\t" <<
                    trialFrame << "\t" <<
                    totalTime.getElapsedTimeInMilliSec() << "\t" <<
                    indexInside[0] << "\t" <<
                    handAway << "\t" <<
                    ( isVisible(markers.at(15).p) ? markers.at(15).p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getLeftEye().p) ? headEyeCoords.getLeftEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getRightEye().p) ? headEyeCoords.getRightEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                    ( isVisible(thumbCoordsReal.getP1().p) ? thumbCoordsReal.getP1().p.transpose() : junk ) << "\t" <<
                    ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) <<
                    endl;
        */
        trialFrame++;
        frameTimer.start();
    }
}


/**
 * @brief drawInfo
 */
void drawInfo()
{
    if ( infoDraw )
    {
        GLText text;
        if ( gameMode )
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);
        else
            text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_12);
        text.enterTextInputMode();

        switch ( headCalibrationDone )
        {
        case 0:
        {   if ( allVisibleHead )
                text.draw("==== Head Calibration OK ==== Press Spacebar to continue");
            else
                text.draw("Be visible with the head and glasses");
            break;
        }
        case 1:
        case 2:
        {
            if ( allVisiblePatch )
                text.draw("Cyclopean(x,y,z)= "+stringify<int>(headEyeCoords.getCyclopeanEye().p.x())+","+stringify<int>(headEyeCoords.getCyclopeanEye().p.y())+"," + stringify<int>(headEyeCoords.getCyclopeanEye().p.z()));
            else
                text.draw("Be visible with the patch");
            break;
        }
        // When the head calibration is done then calibrate the fingers
        case 3:
        {
            switch ( fingerCalibrationDone )
            {
            case 0:
            {
                text.draw("Press F to record platform markers");
                break;
            }
            case 1:
            {
                text.draw("Move index and thumb on platform markers to record ghost finger tips, then press F");
                break;
            }
            case 2:
            {
                text.draw("Move index and thumb to rigidbody tip to define starting position, then press F");
                break;
            }
            }
        }
        break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {
            text.draw("CurrentBlock= " + stringify<int>(blocks.at(blockIndex)));
            text.draw("Trial mode " + stringify<int>(trialMode));
            text.draw("STAIRCASE " + stringify<double>(focalDistance+CURSTAIRCASE->getCurrentStaircase()->getState()));
            text.draw("fDistance= " + stringify<double>(CURFACTORS.at("Distances")));
            text.draw("TotTrialNumber=" + stringify<int>(totalTrialNumber));
            text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("OccludedTrialFrame= " + stringify<int>(occludedFrames));
            text.draw("HandAway= " + stringify<int>(handAway));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            //text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            //text.draw("ThumbReal= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsReal.getP1().p ));
            //text.draw("ThumbVisual= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsVisual.getP1().p ));
        }
        text.leaveTextInputMode();
    }
}

/**
 * @brief drawStimulusCylinder
 */
void drawStimulusCylinder()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    stimDrawer.draw();
    glPopMatrix();
}

/**
 * @brief drawStimulusWedge
 */
void drawStimulusWedge()
{
    static int x=0;
    if ( indexCoords.getP1().p.z() < focalDistance-50 && x==0)
    {
        glPushMatrix();
        drawCircle(10,maskWidth+10,0,focalDistance+CURSTAIRCASE->getCurrentStaircase()->getState());
        glPopMatrix();
        x=1;
    }

    glColor3fv(glRed);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glRotated(180,0,1,0);
    stimDrawer.draw();
    glPopMatrix();

    if ( hasMask )
    {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glColor3fv(glGray25);
        glBegin(GL_QUADS);
        glVertex3d(-maskWidth/2,-maskHeight/2,focalDistance);
        glVertex3d(maskWidth/2,-maskHeight/2,focalDistance);
        glVertex3d(maskWidth/2,maskHeight/2,focalDistance);
        glVertex3d(-maskWidth/2,maskHeight/2,focalDistance);
        glEnd();
        glPopAttrib();
    }
}

/**
 * @brief drawFingerTips
 */
void drawFingerTips()
{
    if (testFinger==0)
        return
            glColor3fv(glRed);
    // Draw the visual feedback for thumb and index as oriented disks
    glBegin(GL_POINTS);
    switch (testFinger)
    {
    case 1:
    {
        glVertex3dv(indexCoords.getP1().p.data());
        break;
    }
    case 2:
    {
        glVertex3dv(thumbCoordsReal.getP1().p.data());
        break;
    }
    case 3:
    {
        glVertex3dv(indexCoords.getP1().p.data());
        glVertex3dv(indexCoords.getP1().p.data());
        break;
    }
    }
    glEnd();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    glColor3fv(glRed);
    if (headCalibrationDone + fingerCalibrationDone < 6)
        return;
    switch (blocks.at(blockIndex))
    {
    case PHASE_ADAPTATION:
    {
        if (trialMode == TRIAL_MODE_HANDS_AWAY)
        {
            drawStimulusWedge();
            drawFingerTips();
        }
        break;
    }
    case PHASE_TEST_GRASP:
    {
        if (trialMode == TRIAL_MODE_HANDS_AWAY)
        {
            drawStimulusWedge();
            drawFingerTips();
        }
        break;
    }
    case PHASE_TEST_PERC:
    {
        drawStimulusCylinder();
        break;
    }
    }
}

/**
 * @brief drawGLScene
 */
void drawGLScene()
{
    if (stereo)
    {
        // Draw left eye view
        glDrawBuffer(GL_BACK);
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(headEyeCoords.getRightEye().p);
        drawInfo();
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(headEyeCoords.getLeftEye().p);
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
        cam.setEye(headEyeCoords.getRightEye().p);
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char*argv[])
{
    cout << "Please select the parametersFileName: Enter a number [0,1] then press Enter:\n:::::" << endl;
    for (int i=0; i<2; i++)
        cout << i << ") " << parametersFileNames[i] << endl;
    cout << "::::::::::\n";
    cin >> paramIndex;
    cout << "Selected " << parametersFileNames[paramIndex] << endl;

    // Randomize the seed of random number generators
    randomizeStart();
    // Move the mirrors at home
    //RoveretoMotorFunctions::homeMirror(3500);
    // Move the monitor at home
    //RoveretoMotorFunctions::homeScreen(3500);
    // Move the object at home
    //RoveretoMotorFunctions::homeObject(RODSPEED);
    // Move the object in starting position
    //RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome,RODSPEED);

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if ( stereo )
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {
        glutInitWindowSize( SCREEN_WIDTH,SCREEN_HEIGHT);
        glutCreateWindow("EXP Reach Adapt Dprime Dihedral");
    }
    else
    {
        glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initializeExperiment();
    initRendering();
    glutIdleFunc(idle);

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}
