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
#include "LatestCalibrationTrieste.h"
#include "TriesteMotorFunctions.h"

#define SCREENSPEED 2500
#define RODSPEED 5000
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

double focalDistance= baseFocalDistance;
double homeFocalDistance=baseFocalDistance;

const Vector3d rodTipHome(502.60, 185.92, -757.62);
const Vector3d rodAway(350,0,-800);
Vector3d relDisplacementCylinder(0,0,0);
Vector3d physicalRigidBodyTip(0,0,0);
Vector3d platformThumb;
Vector3d platformIndex;
Vector3d hapticRodCenter(0,0,focalDistance);
Vector3d visualRodCenter;
Vector3d visualThumb;
Vector3d visualIndex;
Vector3d eyeLeft;
Vector3d eyeRight;
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
bool TestHapticFeedback= false;
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
bool lastResponse=false;
double stimulusTime=2000;
double fingerDistance=0;
double probeoffset=0;
unsigned int blockIndex=0;
int blocksTrialNumber[TOTALBLOCKS]= {0,0,0};
int drawingTrialFrame=0;
int occludedFrames=0;
int totalFrame=0;
int trialFrame=0;

// Mask properties
double maskWidth=80;
double maskHeight=10;
double maskDepth=0.0;
double percstimulusonset=2000;
double timeprobe=2000;
bool hasMask=true;
bool showProbe=false;
double jitterProbeOffset=0.0;
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
double fingerOffset=0;
int paramIndex=0;
char *blockNames[] = {"Adapt","Grasp","Perc"};
char *parametersFileNames[]=
{
    "C:/Users/fantoni/Desktop/AAAparameters.txt",
    "C:/Users/fantoni/Desktop/AAAparameters.txt"
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
        {
            headEyeCoords.init(markers.at(18).p-Vector3d(ChinRestWidth,0,0),markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        // Second calibration
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headCalibrationDone=3;
            TriesteMotorFunctions::moveObjectAbsolute(Vector3d(300,0,-400),rodTipHome-calibration,RODSPEED);
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
            ///// Assegna la posizione giusta al rod appena premuto l'ultimo tasto F della calibrazione
            //// se c'è hapticfeedback, altrimenti va a casa. Se è Nella fase perce il probe rimane dove è
            //// come definito dopo la pressione del tasto ' '
            if (blocks.at(blockIndex) == PHASE_TEST_PERC)
            {
                TriesteMotorFunctions::moveMonitorAbsolute(CURFACTORS.at("Distances"),homeFocalDistance,SCREENSPEED);
            }
            else
            {
                TriesteMotorFunctions::moveMonitorAbsoluteAsynchronous(CURFACTORS.at("Distances"),homeFocalDistance,SCREENSPEED);
            }
            if (TestHapticFeedback && blocks.at(blockIndex) == PHASE_TEST_GRASP)
                moveRod(Vector3d(0,0,CURFACTORS.at("Distances")-fingerOffset),RODSPEED);
            else if (TestHapticFeedback==0 && blocks.at(blockIndex) == PHASE_TEST_GRASP)
                moveRod(Vector3d(350,0,-800),RODSPEED);
            break;
        }
        break;
    }
    case 't':
    {
        moveRod(Vector3d(0.0,0.0,homeFocalDistance-200),RODSPEED,true);
        break;
    }
    case '4':
        //// la domanda è il vertice appare più vicino a te o più lontano del probe? Se
        //// è più lontanola risposta corretta è 6 altrimenti è 4
    {
        if (( blocks.at(blockIndex) == PHASE_TEST_GRASP || blocks.at(blockIndex) == PHASE_TEST_PERC ) && (trialMode==2)) {
            beepOk();
            advanceTrialTest(true);

        }
        else {
            beepBad();
            // advanceTrialAdapt();
            break;
        }
    }
    case '6':
    {

        if (( blocks.at(blockIndex) == PHASE_TEST_GRASP || blocks.at(blockIndex) == PHASE_TEST_PERC ) && (trialMode==2)) {
            beepOk();
            advanceTrialTest(false);
        }
        else {
            beepBad();
            // advanceTrialAdapt();
            break;
        }
    }
    }
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
        double rodRadius=0;//13;    //mm
        double markerThickness=0;//3.5;
        Vector3d finalPosition=v-Vector3d(0,0,markerThickness+rodRadius/2.0);
        if ( !isVisible(finalPosition) )
        {   beepBad();
            errorLog << "ERROR in " << __FILE__ << " at line " <<  __LINE__<< "Can't move motors to infinity" << endl;
            return ;
        }
        // a precise feedback based misuration - 2 corrections
        for ( int i=0; i<2; i++)
        {   updateMarkers();
            TriesteMotorFunctions::moveObject(finalPosition-markers.at(15).p,speed);
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
    double angle = M_PI/180.0*CURFACTORS.at("WedgeAngle");
    double stimEdge = parameters.get("StimulusWidth");
    WedgePointsStimulus wedge;
    wedge.setPlanesSize(stimEdge,stimEdge);
    int nPoints = parameters.get("StimulusNPoints");
    wedge.setNpoints(nPoints);
    wedge.setAngle(angle); /*modificato Controllare se prende angoli o radianti*/
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
                                     parameters.get("CylinderHeight")*parameters.get("StimulusNPoints"));
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
    string markersFileName = baseDir+"markersFile_" + parameters.find("SubjectName") + ".txt";
    if ( fileExists(parameters.find("BaseDir")+markersFileName ) )
    {
        cerr << "!! " << markersFileName << " already exists" << endl;
    }
    markersFile.open(markersFileName.c_str());
    cerr << "Writing to markers file " << markersFileName << " OK" << endl;

    string responseFileName = baseDir+"responseFile_" + parameters.find("SubjectName") + ".txt";
    if ( fileExists(parameters.find("BaseDir")+responseFileName ) )
    {
        cerr << "!! " << responseFileName << " already exists" << endl;
    }
    responseFile.open(responseFileName.c_str());
    cerr << "Writing to response file " << responseFileName << " OK" << endl;

    // Write the headers for markers file
    markersFile << fixed << setprecision(3) <<
                // "SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tStimDiameter\tTrialFrame\tTotTime\tIndexInside\tshowprobe\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tVIndexXraw\tVIndexYraw\tVIndexZraw\tVThumbXraw\tVThumbYraw\tVThumbZraw" << endl;
                "SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tWedgeAngle\tResponse\tStairID\tStaitReversals\tStairsDone\tStairInversions\tStairState\tTrialFrame\tTotTime\tIndexInside\tshowprobe\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tVIndexXraw\tVIndexYraw\tVIndexZraw\tVThumbXraw\tVThumbYraw\tVThumbZraw" << endl;

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
    fingerOffset = parameters.get("FingerOffset");
    probeoffset = parameters.get("Probeoffset");
    stimulusTime = parameters.get("TestStimulusTime");
    maskWidth = parameters.get("MaskWidth");
    maskHeight = parameters.get("MaskHeight");
    hasMask = str2num<int>(parameters.find("UseMask"));
    maskDepth = parameters.get("MaskDepth");
    timeprobe = parameters.get("Timeprobe");
    percstimulusonset= parameters.get("Percstimulusonset");
    testFinger = str2num<int>(parameters.find("TestFinger"));
    TestHapticFeedback= parameters.get("testHapticFeedback");

    hapticRodCenter = Vector3d(0,0,focalDistance);
    visualRodCenter << 0,0, CURFACTORS.at("Distances")-fingerOffset;
    indexInside[0]=indexInside[1]=false;
    generateWedgeStimulus();
    generateCylinderStimulus();
    updateTrialStimulus();

    initProjectionScreen(CURFACTORS.at("Distances"));


    /*	if (TestHapticFeedback && blocks.at(blockIndex) == PHASE_TEST_GRASP)
    	    moveRod(Vector3d(0,0,CURFACTORS.at("Distances")-fingerOffset),3500);*/

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
    // if (blocks.at(blockIndex) == PHASE_TEST_PERC)
    // TriesteMotorFunctions::moveMonitorAbsolute(_focalDist,homeFocalDistance,SCREENSPEED);
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

    static bool firstTrial=true;
    if ( !firstTrial )
    {

        if (blocks.at(blockIndex) == PHASE_TEST_PERC)
        {
            TriesteMotorFunctions::moveMonitorAbsolute(CURFACTORS.at("Distances"),homeFocalDistance,SCREENSPEED);
        }
        else
        {
            TriesteMotorFunctions::moveMonitorAbsoluteAsynchronous(CURFACTORS.at("Distances"),homeFocalDistance,SCREENSPEED);
        }

        if (TestHapticFeedback && blocks.at(blockIndex) == PHASE_TEST_GRASP)
            moveRod(Vector3d(0,0,CURFACTORS.at("Distances")-fingerOffset),RODSPEED);
    }
    firstTrial=false;

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


    if ( trialMode == TRIAL_MODE_HANDS_ON_START )
    {
        beepBad();
        return;
    }

    bool cond = (trialMode > TRIAL_MODE_HANDS_ON_START) && (globalTimer.getElapsedTimeInMilliSec() > timeprobe+100);
    //// se la risposta è più veloce di timeprobe +100 non prende
    if (!cond )
    {
        beepBad();
        return;
    }

    lastResponse = resp;
    jitterProbeOffset = mathcommon::unifRand(-probeoffset,0.0);
    showProbe=false;

    responseFile << blocks.at(blockIndex) << "\t" <<
                 blocksTrialNumber[blocks.at(blockIndex)] << "\t" <<
                 totalTrialNumber << "\t" <<
                 resp << "\t" <<
                 CURFACTORS.at("Distances") << "\t" <<
                 CURFACTORS.at("WedgeAngle") << "\t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getID() << "\t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getReversals() << "\t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getTrialsDone() << "\t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getInversions() << " \t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getAscending() << "\t" <<
                 CURSTAIRCASE->getCurrentStaircase()->getState() << "\t" <<
                 globalTimer.getElapsedTimeInMilliSec() <<
                 endl;

    //TriesteMotorFunctions::moveMonitorAbsoluteAsynchronous(CURFACTORS.at("Distances"),homeFocalDistance,SCREENSPEED);
    // if (TestHapticFeedback && blocks.at(blockIndex) == PHASE_TEST_GRASP)
    // moveRod(Vector3d(0,0,CURFACTORS.at("Distances")-fingerOffset),3500);

    bool goToNextBlock = CURTRIAL.next(resp);

    if ( goToNextBlock )
    {
        if ( CURTRIAL.isEmpty() )
            exit(0);

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

void writeContinuosFile()
{
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
    outputfile << "Thumb: " << thumbCoordsReal.getP1().p.transpose() << endl;
    outputfile << "Index: " << indexCoords.getP1().p.transpose() << endl;
    outputfile << "Wrist: " << markers.at(6).p.transpose() << endl;
    // outputfile << "IsDrawing: " << isDrawing << " TrialOk: " << trialOk << " OccludedFrames: " << occludedFrames << " DrawingTrialFrames: " << drawingTrialFrame << endl;
    outputfile << "HapticRodCenter: " << hapticRodCenter.transpose() << endl;
    outputfile << "VisualRodCenter: " << visualRodCenter.transpose() << endl;
    outputfile << "Markers(5): " << markers.at(5).p.transpose() << endl;
    // outputfile << "AdaptOffset: " << adaptOffsets.at(block1TrialNumber) << endl;
    // outputfile << "Phase: " << block.at("Phase") << endl;
    // outputfile << "TrialNumber: " << thisBlockTrialNumber << endl;
    outputfile << "Total trials: " << totalTrialNumber << endl;
    outputfile << "Response: " << lastResponse << endl;
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

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;

    visualIndex= indexCoords.getP1().p + Vector3d(0,0,fingerOffset);
    visualThumb= thumbCoordsReal.getP1().p + Vector3d(0,0,fingerOffset);


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
            if (indexInside[0]==0 && visibleFinger)
            {
                if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY)
                {
                    boost::thread comebackbeepthread(beepComeBack);
                    trialMode = TRIAL_MODE_RESPONSE;
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
        }

        if ( blocks.at(blockIndex) == PHASE_TEST_PERC ) // Blocco PERC, lo stimolo è triggerato dal tempo
        {
            double delta = globalTimer.getElapsedTimeInMilliSec();
            if ( globalTimer.getElapsedTimeInMilliSec() > percstimulusonset && trialMode == TRIAL_MODE_HANDS_ON_START) // triggera l'apparizione
            {
                trialMode = TRIAL_MODE_HANDS_AWAY;
                globalTimer.start();
            }
            if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY)
            {
                trialMode = TRIAL_MODE_RESPONSE;
            }
            /*
            if ( indexInside[1]==1 && trialMode == TRIAL_MODE_HANDS_ON_START) // triggera l'apparizione
            {
            	trialMode = TRIAL_MODE_HANDS_AWAY;
            	globalTimer.start();
            }
            */
        }
        /*
        if (indexInside[0]==0 && visibleFinger)
        {
        	if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY)
            {
                boost::thread comebackbeepthread(beepComeBack);
        		trialMode = TRIAL_MODE_RESPONSE;
        	}
        }
        */

        /* // XXX da controllare
        if (indexInside[0]==1 && indexInside[1]==0)
            indexCameBack=true;
        else
            indexCameBack=false;

        if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == TRIAL_MODE_HANDS_AWAY && visibleFinger && indexCameBack )
        {
            globalTimer.start();
        }
        */
        //////////////////// WRITE TO FILE ////////////
        if ( globalTimer.getElapsedTimeInMilliSec() < stimulusTime && trialMode==TRIAL_MODE_HANDS_AWAY )
            handAway=true;
        else
            handAway=false;
        //"SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tWedgeAngle\tResponse\tStairID\tStaitReversals\tStairsDone\tStairInversions\tStairState\tTrialFrame\tTotTime\tIndexInside\tshowprobe\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tVIndexXraw\tVIndexYraw\tVIndexZraw\tVThumbXraw\tVThumbYraw\tVThumbZraw" << endl;
        char *currentBlockName = blockNames[blocks.at(blockIndex)];

        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    currentBlockName << "\t" <<
                    totalTrialNumber << "\t" <<
                    blocksTrialNumber[blocks.at(blockIndex)] << "\t" <<
                    CURFACTORS.at("RelDepth") << "\t" <<
                    CURFACTORS.at("Distances") << "\t" <<
                    CURFACTORS.at("WedgeAngle") << "\t" <<
                    lastResponse << "\t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getID() << "\t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getReversals() << "\t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getTrialsDone() << "\t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getInversions() << " \t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getAscending() << "\t" <<
                    CURSTAIRCASE->getCurrentStaircase()->getState() << "\t" <<
                    trialFrame << "\t" <<
                    totalTime.getElapsedTimeInMilliSec() << "\t" <<
                    indexInside[0] << "\t" <<
                    showProbe<<"\t"<<
                    handAway << "\t" <<
                    ( isVisible(markers.at(15).p) ? markers.at(15).p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getLeftEye().p) ? headEyeCoords.getLeftEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getRightEye().p) ? headEyeCoords.getRightEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                    ( isVisible(thumbCoordsReal.getP1().p) ? thumbCoordsReal.getP1().p.transpose() : junk ) << "\t" <<
                    ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) <<"\t"<<
                    visualIndex.transpose() <<"\t"<<
                    visualThumb.transpose() <<
                    endl;
        writeContinuosFile();
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
            text.draw("STAIRCASE " + stringify<double>(CURFACTORS.at("Distances")+CURSTAIRCASE->getCurrentStaircase()->getState()));
            text.draw("fDistance= " + stringify<double>(CURFACTORS.at("Distances")));
            text.draw("TotTrialNumber=" + stringify<int>(totalTrialNumber));
            text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("OccludedTrialFrame= " + stringify<int>(occludedFrames));
            text.draw("HandAway= " + stringify<int>(handAway));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("VisualIndex= " + stringify< Eigen::Matrix<double,1,3> > (visualIndex.transpose()));
            text.draw("MARKER15= " + stringify< Eigen::Matrix<double,1,3> >(markers.at(15).p ));
            text.draw("ThumbReal= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsReal.getP1().p ));
            text.draw("CurrentWedgeAngle= " + stringify<double>(CURFACTORS.at("WedgeAngle")));
            text.draw("EyeLeft(x,y,z)= "+stringify<int>(headEyeCoords.getLeftEye().p.x())+","+stringify<int>(headEyeCoords.getLeftEye().p.y())+"," + stringify<int>(headEyeCoords.getLeftEye().p.z()));
            text.draw("EyeRight(x,y,z)= "+stringify<int>(headEyeCoords.getRightEye().p.x())+","+stringify<int>(headEyeCoords.getRightEye().p.y())+"," + stringify<int>(headEyeCoords.getRightEye().p.z()));
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
    double currentFocalDistance = CURFACTORS.at("Distances");

    /* Modificato adesso presenta solo oltre una certa distanza*/
    double delta = probeoffset - fingerOffset + jitterProbeOffset;
    double z = indexCoords.getP1().p.z();
    if ( (indexCoords.getP1().p.z() > currentFocalDistance + delta ) && !showProbe )
        showProbe=false;
    else
        showProbe=true;
    /// presenta il probe dopo time probe
    if ( blocks.at(blockIndex) == PHASE_TEST_PERC && globalTimer.getElapsedTimeInMilliSec() > timeprobe) // durante il PERC il probe c'è SEMPRE
        showProbe=true;

    if ( showProbe )
    {
        glPushMatrix();
        drawCircle(10,-maskWidth/2-40,0,currentFocalDistance+CURSTAIRCASE->getCurrentStaircase()->getState());
        glPopMatrix();
    }

    glColor3fv(glRed);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,currentFocalDistance);
    stimDrawer.draw();
    glPopMatrix();

    if ( hasMask )
    {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glColor3fv(glGray25);
        glBegin(GL_QUADS);
        glVertex3d(-maskWidth/2,-maskHeight/2,currentFocalDistance+maskDepth);
        glVertex3d(maskWidth/2,-maskHeight/2,currentFocalDistance+maskDepth);
        glVertex3d(maskWidth/2,maskHeight/2,currentFocalDistance+maskDepth);
        glVertex3d(-maskWidth/2,maskHeight/2,currentFocalDistance+maskDepth);
        glEnd();
        glPopAttrib();
    }
}

/**
 * @brief drawFingerTips
 */
void drawFingerTips()
{
    // Draw the visual feedback for thumb and index as oriented disks
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3fv(glRed);
    glPointSize(5);
    glBegin(GL_POINTS);
    switch (testFinger)
    {
    case 0:
    {
        break;
    }
    case 1:
    {
        // glVertex3dv(indexCoords.getP1().p.data());
        glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        break;
    }
    case 2:
    {
        // glVertex3dv(thumbCoordsReal.getP1().p.data());
        glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
        break;
    }
    case 3:
    {
        // glVertex3dv(indexCoords.getP1().p.data());
        // glVertex3dv(thumbCoordsReal.getP1().p.data());
        glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
        break;
    }
    }
    glEnd();
    glPopAttrib();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
//	  visualIndex= indexCoords.getP1().p + Vector3d(0,0,fingerOffset);
//    visualThumb= thumbCoordsReal.getP1().p + Vector3d(0,0,fingerOffset);

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
        if (trialMode == TRIAL_MODE_HANDS_AWAY)
            drawStimulusWedge();
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
        // cerr << headEyeCoords.getRightEye().p.transpose() << "\t" << headEyeCoords.getLeftEye().p.transpose() << endl;
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
    TriesteMotorFunctions::homeMonitor(3000);
    TriesteMotorFunctions::homeObject(3500);


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
        glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initializeExperiment();
    initRendering();

    // drawGLScene();

    initProjectionScreen(focalDistance);

    glutIdleFunc(idle);

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();
    return 0;
}
