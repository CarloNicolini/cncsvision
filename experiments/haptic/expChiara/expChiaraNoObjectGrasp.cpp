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

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <boost/filesystem.hpp>

#include "Marker.h"
#include "Optotrak2.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "EllipsoidPointsStimulus.h"

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
#define TOTALBLOCKS 5

using namespace std;
using namespace Eigen;

static const bool gameMode=true;
static const bool stereo=true;

Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords;
CoordinatesExtractor2 thumbCoordsReal;
CoordinatesExtractor2 thumbCoordsVisual;
CoordinatesExtractor2 indexCoords;
RigidBody rigidStartIndex;
RigidBody rigidStartThumb;
RigidBody rigidCurrentIndex;
RigidBody rigidCurrentThumb;
EulerExtractor eulerAngles;
VRCamera cam;
ParametersLoader parameters;
BalanceFactor<double> balanceFactor[TOTALBLOCKS];
StimulusDrawer stimDrawer;
EllipsoidPointsStimulus ellipseBaseCylinder;

double focalDistance= -418.5;
double homeFocalDistance=-418.5;

Vector3d rodTipHome(344.95,-213.77,-824.6);
Vector3d rodAway(350,0,-800);
Vector3d relDisplacementCylinder(0,0,0);
Vector3d object20RelDisplacement(16,68,44);
Vector3d object40RelDisplacement(12.5,-1,35);
Vector3d visualThumb;
Vector3d visualIndex;
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
bool thumbProjectedInside=false;
bool indexProjectedInside=false;
static const int HANDONSTARTMODE=0;
static const int HANDSAWAYMODE=1;
int trialMode = HANDONSTARTMODE;
int totalTrialNumber=0;
int comeBackBeepDone=0;
bool handAway=false;
bool isMovingRod=false;
bool trialOk=false;
double stimulusTime=2000;
double fingerDistance=0;
unsigned int blockIndex=0;
int blocksTrialNumber[TOTALBLOCKS]= {0,0,0,0,0};
int drawingTrialFrame=0;
int fingerMode=2;
int occludedFrames=0;
int totalFrame=0;
int trialFrame=0;
Timer delayedTimer;
Timer frameTimer;
Timer globalTimer;
Timer totalTime;
vector<int> blocks;

double ballRadius=10;
double cylDepth;
double cylHeight;
double cylWidth;
double stimulusDensity=0.025;	// points per mm^2, it means 150 points on a h=70, radius=15 cylinder area
double stimulusWidth=0;
double thumbSphereRadius=6;

int paramIndex=0;
char *blockNames[] = {"B1","B2","B3","B4","B5"};
char *parametersFileNames[]=
{
    "C:/cncsvisiondata/parametersFiles/Chiara/experimentNoObjectGrasp/parametersExpChiaraNoObjectGrasp.txt",
    "C:/cncsvisiondata/parametersFiles/Chiara/experimentNoObjectGrasp/parametersExpChiaraNoObjectGrasp_training.txt"
};

ofstream markersFile;
ofstream errorLog("C:\cncsvisiondata\errorlog\errors_log.txt");

void beepBad();
void beepOk();
void beepTrial();
void checkBounds();
void drawFingerTips();
void drawGLScene();
void drawInfo();
void drawStimulus();
void drawTrial();
void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void idle();
void initOptotrak();
void initProjectionScreen(double _focalDist);
void initRendering();
void initVariables();
void moveRod(const Vector3d &p, int speed, bool hasFeedback=true);
void update(int value);
void updateMarkers();
void updateTrialStimulus();

#ifdef _WIN32
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
#endif

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
    rigidCurrentIndex.setRigidBody(markers.at(7).p,markers.at(8).p,markers.at(9).p);

    rigidStartThumb.computeTransformation(rigidCurrentThumb);
    rigidStartIndex.computeTransformation(rigidCurrentIndex);
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
            Vector3d offset(0,0,thumbSphereRadius);
            thumbCoordsReal.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            thumbCoordsVisual.init(platformThumb+offset, markers.at(11).p, markers.at(12).p, markers.at(13).p);

            // We need an auxiliary rigidbody to get the relative transformation of a disk and computing the normals
            rigidStartThumb.setRigidBody(markers.at(11).p-platformThumb,markers.at(12).p-platformThumb,markers.at(13).p-platformThumb);
            rigidStartIndex.setRigidBody(markers.at(7).p-platformIndex,markers.at(8).p-platformIndex,markers.at(9).p-platformIndex);

            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            beepOk();
            RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(rodAway,rodTipHome,RODSPEED);
            break;
        }

        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {
            beepTrial();
            infoDraw=false;
            drawGLScene();

            physicalRigidBodyTip = indexCoords.getP1().p;
            trialMode = HANDONSTARTMODE;
            fingerDistance = (indexCoords.getP1().p-thumbCoordsReal.getP1().p).norm();
            trialFrame=0;
            globalTimer.start();
            totalTime.start();
            fingerCalibrationDone=3;
            cout << "::: Rigid body tip= " << physicalRigidBodyTip.transpose() << endl;
            break;
        }
        break;
    }
    case '6':
    {
        relDisplacementCylinder.x()+=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(1,0,0));
        break;
    }
    case '4':
    {
        relDisplacementCylinder.x()-=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(-1,0,0));
        break;
    }
    case '8':
    {
        relDisplacementCylinder.y()+=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,1,0));
        break;
    }
    case '2':
    {
        relDisplacementCylinder.y()-=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,-1,0));
        break;
    }
    case '3':
    {
        relDisplacementCylinder.z()-=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,0,-1));
        break;
    }
    case '9':
    {
        relDisplacementCylinder.z()+=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,0,1));
        break;
    }
    case '+':
    {
        relDisplacementCylinder.z()+=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,0,1));
        break;
    }
    case '-':
    {
        relDisplacementCylinder.z()-=1;
        //RoveretoMotorFunctions::moveObject(Vector3d(0,0,-1));
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

/**
 * @brief generateStimulus
 * @param block
 * @param height
 * @param axisX
 * @param axisZ
 */
void generateStimulus( int block, double height, double axisX, double axisZ )
{
    cerr << "::: Block " << block << " h= " << height << " axisX= " << axisX << " axisZ= " << axisZ << endl;
    double ellipticCircumferenceApproximate = mathcommon::ellipseCircumferenceBetter(axisX,axisZ);
    int nStimulusPoints=2*(int)floor(ellipticCircumferenceApproximate*height*stimulusDensity);

    ellipseBaseCylinder.setNpoints(nStimulusPoints);
    ellipseBaseCylinder.setAperture(0,2*M_PI);
    ellipseBaseCylinder.setAxesAndHeight(axisX,axisZ,height);
    ellipseBaseCylinder.setFluffiness(0.001);
    ellipseBaseCylinder.compute();

    cylWidth = axisX;
    cylHeight = height;
    cylDepth = axisZ;

    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&ellipseBaseCylinder);
    stimDrawer.initList(&ellipseBaseCylinder,glRed,3);
}

/**
 * @brief initVariables
 */
void initVariables()
{
    // Load the parameters file from the selected input filename
    // Initialize the parameters file
    parameters.loadParameterFile(parametersFileNames[paramIndex]);
    interoculardistance = util::str2num<double>(parameters.find("IOD"));
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
    {
        cerr << "Directory " << baseDir << " doesn't exist, press enter to exit..." << endl;
        cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        exit(0);
    }
    string markersFileName = baseDir+"markersFile_" + parameters.find("SubjectName") + ".txt";
    if ( util::fileExists(parameters.find("BaseDir")+markersFileName ) )
    {
		MessageBox(NULL, (LPCSTR)"MARKER FILE ALREADY EXIST\n",NULL, NULL);
		exit(0);
    }

    markersFile.open(markersFileName.c_str());
    cerr << "Writing to markers file " << markersFileName << " OK" << endl;
    // Write the headers for markers file
    markersFile << fixed << setprecision(3) <<
                "SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tStimDiameter\tTrialFrame\tTotTime\tIndexInside\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tThumProjInside\tIndexProjInside" << endl;

    blocks = util::str2num<int>(parameters.find("BlocksSequence"),",");

    for ( int i=0; i<TOTALBLOCKS; i++ )
    {
        balanceFactor[i].init(parameters);
        balanceFactor[i].next();
    }

    stimulusDensity = str2num<double>(parameters.find("StimulusDensity"));

    ballRadius = str2num<double>(parameters.find("BallRadius"));
    thumbSphereRadius = util::str2num<double>(parameters.find("ThumbSphereRadius"));
    stimulusTime = str2num<double>(parameters.find("StimulusTime"));
    hapticRodCenter = Vector3d(0,0,focalDistance);
    visualRodCenter << 0,0, balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances");
    indexInside[0]=indexInside[1]=false;

    stimulusWidth = mathcommon::unifRand(util::str2num<double>(parameters.find("StimWidthMin")),util::str2num<double>(parameters.find("StimWidthMax")));

    updateTrialStimulus();
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
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
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
    double trialFocalDistance = balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances");
    double relDepth= balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth");

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

    initProjectionScreen(trialFocalDistance);
    visualRodCenter.z() = trialFocalDistance;// il paletto sta nella parte anteriore dello stimolo
    generateStimulus(blocks.at(blockIndex),parameters.get("StimulusHeight"),stimulusWidth, relDepth );
}

/**
 * @brief checkBounds
 */
/*
void checkBounds()
{
    if ( fingerCalibrationDone==3 && headCalibrationDone==3 )
    {
        switch ( trialMode )
        {
        case HANDSAWAYMODE:
        {   // Check if the index tip stays inside the stimulus
            Vector2d indexOnPlane = Vector2d(indexCoords.getP1().p.x(),indexCoords.getP1().p.z());
            Vector2d hapticRodCenterOnPlane = Vector2d(hapticRodCenter.x(), hapticRodCenter.z()+10.0);
            double stimHeight = str2num<double>(parameters.find("StimulusHeight") );
            double stimRadius = 13.0;
            bool yInside = (indexCoords.getP1().p.y()-(hapticRodCenter.y()+stimulusRodOffsetY))<(stimHeight/2) && (indexCoords.getP1().p.y()-(hapticRodCenter.y()+stimulusRodOffsetY))> (-stimHeight/2) ;
            if ( ( (indexOnPlane - hapticRodCenterOnPlane).norm() <= stimRadius ) && yInside )
                indexInside[0]=true;
            else
                indexInside[0]=false;
            break;
        }
        case HANDONSTARTMODE:
        {   // Define a sphere around physicalRigidBodyTip in which the thumb must stay
            bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
            // We set occluded frames = 0 in this mode
            occludedFrames=0;
            if ( indexVisible )
			{
				// Here as trigger condition we use the time
				bool triggerCondition = globalTimer.getElapsedTimeInMilliSec() > stimulusTime;
                if ( triggerCondition ) //5 centimeters
                {   indexInside[0]=true;
                    if ( indexInside[1]==false)
                        globalTimer.start();
                }
                else
                {   indexInside[0]=false;
                    double distance = (indexCoords.getP1().p  - physicalRigidBodyTip).norm();
                    // IMPORTANTE: controlla che il dito sia uscito ma esclude i casi in cui il dito esce da invisibile e diventa visibile fuori dalla palla... una soglia decente  10 mm
                    if ( indexInside[1]==true && distance<(ballRadius+10) ) // appena uscito
                    {   globalTimer.start();
                        delayedTimer.start();
                        trialMode=HANDSAWAYMODE;
                    }
                }
            }
            break;
        }
        }
    }
}
*/
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
        bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
        if	(indexVisible)
        {
            // Update the old state of indexInside to new state of indexInside
            indexInside[1]=indexInside[0];
            double distance = (indexCoords.getP1().p  - physicalRigidBodyTip).norm();
            indexInside[0]=distance < ballRadius ;
        }
        bool visibleFinger=isVisible(thumbCoordsReal.getP1().p) && isVisible(indexCoords.getP1().p);
        if ( util::str2num<int>(parameters.find("CheckWrist"))==1 )
            visibleFinger = visibleFinger && isVisible(markers.at(6).p);
        if ( !visibleFinger && trialMode==HANDSAWAYMODE )
        {
            occludedFrames++;
            if ( str2num<int>(parameters.find("AudioFeedback") ) )
                boost::thread invisibleBeep( beepInvisible);
        }

        if ( indexInside[0]==0 && indexInside[1]==1 && trialMode == HANDONSTARTMODE) // triggera l'apparizione
        {
            trialMode = HANDSAWAYMODE;
            globalTimer.start();
        }
        if (indexInside[0]==1 && indexInside[1]==0)
            indexCameBack=true;
        else
            indexCameBack=false;

        
        if (indexInside[0]==0 && visibleFinger)
        {
            if ( globalTimer.getElapsedTimeInMilliSec() > stimulusTime )
            {
                if ( comeBackBeepDone==0 )
					boost::thread comebackbeepthread(beepComeBack);
				comeBackBeepDone=1;
            }
        }
		// Procede al prossimo trial quando è passato il tempo, le mani sono fuori il dito è visibile e l'indice è tornato indietro al rigidbody
		// e ha suonato il comeBackBeepDone
        if (globalTimer.getElapsedTimeInMilliSec() > stimulusTime && trialMode == HANDSAWAYMODE && visibleFinger && indexCameBack )
        {
			comeBackBeepDone=0;
			trialMode=HANDONSTARTMODE;
            stimulusWidth=mathcommon::unifRand(util::str2num<double>(parameters.find("StimWidthMin")),util::str2num<double>(parameters.find("StimWidthMax")));
            // Here we make a new trial
            totalTrialNumber++;
            blocksTrialNumber[blocks.at(blockIndex)]++;
            // Reinsert trial if number of occluded frames is bigger in percentual than a given threshold,
            // the wrong trial is reinsterted a maximum number of times given by parameter MaxReinsertedTimes
            if ( double(occludedFrames)/double(drawingTrialFrame)*100 > util::str2num<double> (parameters.find("MaxPercentOccludedFrames")))
            {
                map<std::string,double> factorsToReinsert = balanceFactor[blocks.at(blockIndex)].getCurrent();
                balanceFactor[blocks.at(blockIndex)].reinsert(factorsToReinsert);
            }
            if ( balanceFactor[blocks.at(blockIndex)].next() )
			{
				updateTrialStimulus();
				beepOk();
			}
            else
            {
                blockIndex++;
                if ( blockIndex >= blocks.size() )
                {
					Beep(220,880); Beep(220,880); Beep(220,880);
                    exit(0);
                }
				updateTrialStimulus();
                Beep(330,880);
            }
			occludedFrames=trialFrame=drawingTrialFrame=0;
            globalTimer.start();
        }

        //////////////////// WRITE TO FILE ////////////
        if ( globalTimer.getElapsedTimeInMilliSec() < stimulusTime && trialMode==HANDSAWAYMODE )
            handAway=true;
        else
            handAway=false;
        //"SubjectName\tFingerDist\tBlock\tTotalTrial\tTrialNumber\tfRelDepth\tfDistances\tStimWidth\tTrialFrame\tTotTime\tIndexInside\thandAway\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw"
        char *currentBlockName = blockNames[blocks.at(blockIndex)];
		
        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    currentBlockName << "\t" <<
                    totalTrialNumber << "\t" <<
                    blocksTrialNumber[blocks.at(blockIndex)] << "\t" <<
                    balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth") << "\t" <<
                    balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances") << "\t" <<
                    stimulusWidth << "\t" <<
                    trialFrame << "\t" <<
                    totalTime.getElapsedTimeInMilliSec() << "\t" <<
                    indexInside[0] << "\t" <<
                    handAway << "\t" <<
                    ( isVisible(markers.at(15).p) ? markers.at(15).p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getLeftEye().p) ? headEyeCoords.getLeftEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(headEyeCoords.getRightEye().p) ? headEyeCoords.getRightEye().p.transpose() : junk ) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                    ( isVisible(thumbCoordsReal.getP1().p) ? thumbCoordsReal.getP1().p.transpose() : junk ) << "\t" <<
                    ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) << "\t" <<
                    thumbProjectedInside << "\t" <<
					indexProjectedInside <<
					endl;
		
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
            text.draw("RelDisp=("+stringify<int>(relDisplacementCylinder.x())+","+stringify<int>(relDisplacementCylinder.y())+","+stringify<int>(relDisplacementCylinder.z()) );
            text.draw("fDistance= " + stringify<double>(balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances")));
            text.draw("fRelDepth= " + stringify<double>(balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth")));
            text.draw("CurrentBlock= " + stringify<int>(blocks.at(blockIndex)));

            text.draw("TotTrialNumber=" + stringify<int>(totalTrialNumber));
            text.draw("Remaining trials= " + stringify<int>( balanceFactor[blocks.at(blockIndex)].getRemainingTrials() ));
            text.draw("Trial mode " + stringify<int>(trialMode));
            text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("OccludedTrialFrame= " + stringify<int>(occludedFrames));
            text.draw("DelayedTime= " + stringify<int>(delayedTimer.getElapsedTimeInMilliSec()));
            text.draw("IndexInside= " + stringify<int>(indexInside[0]));
			text.draw("HandAway= " + stringify<int>(handAway));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("VisualRod= " + stringify<Eigen::Matrix<double,1,3> >(visualRodCenter.transpose()) );
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("ThumbReal= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsReal.getP1().p ));
            text.draw("ThumbVisual= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsVisual.getP1().p ));
			text.draw("Thumb/Index projected inside" + stringify<int>(thumbProjectedInside)+ "," + stringify<int>(indexProjectedInside));
		}
        text.leaveTextInputMode();
    }
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    GLUquadricObj *quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_FILL);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z());
    stimDrawer.draw();
    glLoadIdentity();
    glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z());
    glColor3d(0,0,0);
    glTranslated(0,cylHeight/2,0);
    glScaled(1,1,cylDepth/(cylWidth));
    glRotated(90,1,0,0);
    gluCylinder(quad,cylWidth/2-0.1,cylWidth/2-0.1,cylHeight,10,10);
    glPopMatrix();
    gluDeleteQuadric(quad);
}

/**
 * @brief drawFingerTips
 */
void drawFingerTips()
{
    double indexSphereRadius=thumbSphereRadius;
	glColor3fv(glRed);
    // Draw the visual feedback for thumb and index as oriented disks
	glPushMatrix();
    glLoadMatrixd(rigidStartThumb.getFullTransformation().data());
    glScaled(1,1,0.001);
    glutSolidSphere(thumbSphereRadius,15,15);
	glPopMatrix();

	glPushMatrix();
    glLoadMatrixd(rigidStartIndex.getFullTransformation().data());
    //glScaled(1,1,0.001);
	glScaled(0.001,1,1);
    glutSolidSphere(indexSphereRadius,15,15);
	glPopMatrix();

	
	Vector2d thumbProjection = cam.computeProjected(thumbCoordsReal.getP1().p);
    thumbProjectedInside = ( thumbProjection.x() >= 0 && thumbProjection.x() <= SCREEN_WIDTH )
		&& ( thumbProjection.y() >= 0 && thumbProjection.y() <= SCREEN_HEIGHT );

    Vector2d indexProjection = cam.computeProjected(indexCoords.getP1().p);
    indexProjectedInside = ( indexProjection.x() >= 0 && indexProjection.x() <= SCREEN_WIDTH )
		&& ( indexProjection.y() >= 0 && indexProjection.y() <= SCREEN_HEIGHT );

}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    if (headCalibrationDone + fingerCalibrationDone < 6)
        return;

    switch ( blocks.at(blockIndex) )
    {
		case 0:	// old style
		{
			if (trialMode == HANDSAWAYMODE)
				drawStimulus();
		}
		break;
		case 1:
		{
			switch(trialMode)
			{
				case HANDONSTARTMODE:
				{
					drawStimulus();
				}
				break;
				case HANDSAWAYMODE:
				{
				}
				break;
			}
			break;
		}
		break;
		case 2:
		{
			switch(trialMode)
			{
			case HANDONSTARTMODE:
			{
				drawStimulus();
			}
			break;
			case HANDSAWAYMODE:
			{
				drawFingerTips();
			}
			break;
			}
		}
		break;
		case 3:
		{
			if (comeBackBeepDone==0)
				drawStimulus();
		}
		break;
		case 4:
		{
			drawStimulus();
			drawFingerTips();
		}
		break;
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
    RoveretoMotorFunctions::homeMirror(3500);
    // Move the monitor at home
    RoveretoMotorFunctions::homeScreen(3500);
    // Move the object at home
    RoveretoMotorFunctions::homeObject(RODSPEED);
    // Move the object in starting position
    RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome,RODSPEED);

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
        glutCreateWindow("EXP CHIARA NO OBJECT GRASP");
    }
    else
    {
        glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initVariables();
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
