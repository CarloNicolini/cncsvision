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

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <boost/filesystem.hpp>
/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Marker.h"
#include "Util.h"
#include "Optotrak2.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "ExtrudedTrapezePointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;

static const GLfloat glRedDim[]      = { 0.7, 0.0, 0.0 };

// Function prototypes
void generateStimulus();
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);
static const int RODSPEED=3000;
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;
//double homeFocalDistance=-418.5;
double maxCoveredDistance=0;

Vector3d platformCalibration(350.0,-100.0,-300.0);
static const Eigen::Vector3d rodTipHome(344.95,-213.77,-824.6); //relativo calibration 29 maggio 2013 in coordinate OpenGL
/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;
RigidBody rigidStartIndex;
RigidBody rigidStartThumb;
RigidBody rigidCurrentIndex;
RigidBody rigidCurrentThumb;
Vector3d eyeLeft;
Vector3d eyeRight;
Vector3d thumb;
Vector3d index;
Vector3d physicalRigidBodyTip;
Vector3d platformThumb;
Vector3d platformIndex;
double cyclopeanY=0;
static double interoculardistance=65;

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
bool thumbProjectedInside=false;
bool indexProjectedInside=false;
bool infoDraw=true;
bool fingNow=false;
bool isSaving=true;

// CONSTANT OFFSETS
double constantOffsetX=0;
double constantOffsetY=0;
double constantOffsetZ=0;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
BalanceFactor<double> factors;
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;
double fingerDistance=0;
int trialMode = HANDONSTARTMODE;
int subjSpacebarPressed=0;
int isDrawing = 0;
ParametersLoader parameters;
int selectedFinger=-1;
#define ONLY_THUMB 0
#define ONLY_INDEX 1
#define BOTH_INDEX_AND_THUMB 2

/********* STIMULI *******/
ExtrudedTrapezePointsStimulus stim;
StimulusDrawer stimDrawer;
// Size of fingers cylinder
double cylBase=7;
double cylHeight=20;

/****** TIMING *****/
Timer globalTimer;
Timer fingersTimer;
Timer trialTimer;

unsigned int invisibleThumbFrames=0;
unsigned int invisibleIndexFrames=0;
unsigned int invisibleWristFrames=0;
unsigned int trialFrame=0;
unsigned int drawingTrialFrame=0;
unsigned int totalTrialNumber=0;

bool isMovingRod=false;

/**** FILES VARIABLES ***/
ofstream markersFile;
int paramIndex=0;
char *parametersFileNames[] =
{
    "C:/cncsvisiondata/parametersFiles/Chiara/experimentTrapezoid/parametersExpChiaraTrapezoidMSE_training.txt",
    "C:/cncsvisiondata/parametersFiles/Chiara/experimentTrapezoid/parametersExpChiaraTrapezoidMSE.txt"
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
        text.draw("Marker5= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(5).p.transpose()) );
		text.draw("Marker15= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(15).p.transpose()) );
		text.draw("Marker16= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(16).p.transpose()) );
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
                text.draw("Move INDEX on platform markers to record ghost finger tips, then press F");
                break;
            case 2:
                text.draw("Move THUMB on platform markers to record ghost finger tips, then press F");
                break;
            case 3:
                text.draw("Press  F to move motors before positioning platform...");
				break;
			case 4:
                text.draw("Prepare to start experiment. Press F to start!");
                break;
            }
        }
            break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==4 )
        {   text.draw("Trial mode " + stringify<int>(trialMode ));
            text.draw("ThumbProjectedInside " + stringify<int>(thumbProjectedInside));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
                        
			text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("IsDrawing?= " + stringify<int>(isDrawing));
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("TrialTimer= " + stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
            text.draw("FingersTime= " + stringify<int>(fingersTimer.getElapsedTimeInMilliSec()));
			text.draw("(DX,DY,DZ= " + stringify< Eigen::Matrix<int,1,3> >(Vector3i((int)constantOffsetX,(int)constantOffsetY,(int)constantOffsetZ)) );
			text.draw("Marker5= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(5).p.transpose()));
		}
        //text.draw("Gain=" + stringify<double>(factors.getCurrent().at("Gain")));
        text.leaveTextInputMode();
    }
}

/**
* @brief move the marker 5 in the point specified by v. It corrects the position
**/
void moveStimulusObject(const Vector3d& v, int speed=3500)
{
    Vector3d currentPosition = markers.at(5).p;
    isSaving=false; // disattiva temporaneamente la scrittura su file
    idle();
    // Gestisce l'offset del marker rispetto al centro del paletto
    double rodRadius=13;    //mm
    double markerThickness=3.5;
	
	Vector3d finalPosition= v + Vector3d(constantOffsetX,
										constantOffsetY,
										-(markerThickness+rodRadius/2)
										+constantOffsetZ);
    if ( !isVisible(finalPosition) )
    {   beepBad();
        return ;
    }
    // a precise feedback based misuration - 2 corrections
    for ( int i=0; i<2; i++)
    {
        idle();
        RoveretoMotorFunctions::moveObject(finalPosition-markers.at(5).p,speed);
    }

    isSaving=true; //riattiva la scrittura su file
}

/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial()
{
    fingersTimer.start(); // Here we change trial mode

    if (trialMode == HANDONSTARTMODE )
    {
        // Wait to show the stimulus that the fingers points are projected in the screen
        trialMode = STIMULUSMODE;
    }
    else // trialMode == STIMULUSMODE
    {
        trialMode = HANDONSTARTMODE;
        paintGL();
        beepTrial();
        double oldDistance = factors.getCurrent().at("Distances");
        // Reinsert the trial if too much occluded frames
        double maxInvisibleFramesPercent = util::str2num<double>(parameters.find("MaxPercentOccludedFrames"))/100.0;
        if ( (invisibleIndexFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent ) ||
             (invisibleThumbFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent )||
             (invisibleWristFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent ) )
        {
            map<string,double> currentFactorsList = factors.getCurrent();
            beepBad();
            cerr << "Reinserted current trial (InvIndex,InvThumb,drawingTrialFrame,totTrialFrame)" << invisibleIndexFrames << " " << invisibleThumbFrames << " " << drawingTrialFrame << " " << trialFrame << endl;
            factors.reinsert(currentFactorsList);
        }
        if (factors.isEmpty())
        {
            beepLong();
            beepLong();
            beepLong();
            exit(0);
        }
        invisibleIndexFrames = invisibleThumbFrames = invisibleWristFrames = trialFrame = drawingTrialFrame = 0;
        totalTrialNumber++;
        factors.next();

        if (oldDistance == factors.getCurrent().at("Distances"))
        {
            Timer sleepTimer; sleepTimer.sleep(1000);
        }

        double marker5z = markers.at(5).p.z();
		generateStimulus();
        initProjectionScreen(factors.getCurrent().at("Distances"));
        //moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY")+cyclopeanY,factors.getCurrent().at("Distances")),3500);
    }
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

    // Compute the coordinates of visual thumb
    thumb = thumbCoords.getP1().p;
    index = indexCoords.getP1().p;

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;

    rigidCurrentThumb.setRigidBody(markers.at(11).p,markers.at(12).p,markers.at(13).p);
    rigidCurrentIndex.setRigidBody(markers.at(7).p,markers.at(8).p,markers.at(9).p);

    rigidStartThumb.computeTransformation(rigidCurrentThumb);
    rigidStartIndex.computeTransformation(rigidCurrentIndex);

    if (headCalibrationDone==3 && fingerCalibrationDone==4)
    {
        if ( !allVisibleIndex || !allVisibleThumb || !isVisible(markers.at(6).p))
            if ( (int)parameters.get("AudioFeedback") )
				boost::thread invisibleBeep( beepInvisible);

        Vector2d thumbProjection = cam.computeProjected(thumb);
        thumbProjectedInside = ( thumbProjection.x() >= 0 && thumbProjection.x() <= SCREEN_WIDTH ) && ( thumbProjection.y() >= 0 && thumbProjection.y() <= SCREEN_HEIGHT );
		Vector2d indexProjection = cam.computeProjected(index);
        indexProjectedInside = ( indexProjection.x() >= 0 && indexProjection.x() <= SCREEN_WIDTH ) && ( indexProjection.y() >= 0 && indexProjection.y() <= SCREEN_HEIGHT );
        trialFrame++;
        // Check when to advance trial
        fingNow = true;
        if (!isSaving)
            return;
        RowVector3d junk(9999,9999,9999);
        if ( trialMode == STIMULUSMODE && isSaving && subjSpacebarPressed==0 )
		{
			Vector3d thumbDir = rigidStartThumb.getFullTransformation().linear()*Vector3d(-1,0,0);
			Vector3d indexDir = rigidStartIndex.getFullTransformation().linear()*Vector3d(0,0,-1);
		markersFile << fixed << setprecision(3) <<
                       parameters.find("SubjectName") << "\t" <<
                       fingerDistance << "\t" <<
                       totalTrialNumber << "\t" <<
                       trialFrame << "\t" <<
                       globalTimer.getElapsedTimeInMilliSec() << "\t" <<
                       factors.getCurrent().at("Distances") << "\t" <<
					   factors.getCurrent().at("StimDepth") << "\t" <<
					   factors.getCurrent().at("StimCenterY") << "\t" <<
					   ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                       ( isVisible(eyeRight) ? eyeRight.transpose() : junk ) << "\t" <<
                       ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                       ( isVisible(thumb) ? thumb.transpose() : junk ) << "\t" <<
                       ( isVisible(index) ? index.transpose() : junk ) << "\t" <<
                       isDrawing << "\t" <<
                       thumbProjectedInside << "\t" <<
                       indexProjectedInside << "\t" <<
                       fingNow << "\t" <<
					   thumbDir.x() << "\t" <<
					   thumbDir.y() << "\t" <<
					   thumbDir.z() << "\t" <<
                       indexDir.x() << "\t" <<
					   indexDir.y() << "\t" <<
					   indexDir.z() << "\t" <<
					   endl;
		}
        // Switch to this function if you want cylindrical stimulus
        if (trialMode == STIMULUSMODE )
        {
            if ( !fingNow ) // always starts counting the time if the finger is outside
            {
                fingersTimer.start();
            }
            // only if the finger is inside the volume the counter has counted more that 1 second
            //if ( fingersTimer.getElapsedTimeInMilliSec() > util::str2num<double>(parameters.find("LeaningTime")) )
            bool thumbOnTip = ((thumb - physicalRigidBodyTip).norm()< 50);
			if ( subjSpacebarPressed>=1 )
			{
				// Clear the screen, swap buffers and then wait 500 ms
				glDrawBuffer(GL_BACK);
				// Draw left eye view
				glDrawBuffer(GL_BACK_LEFT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0,0.0,0.0,1.0);
				// Draw right eye view
				glDrawBuffer(GL_BACK_RIGHT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0,0.0,0.0,1.0);
				glutSwapBuffers();
				Timer spacebarTimer; spacebarTimer.sleep(500);
				subjSpacebarPressed=2;
			}
			if (thumbOnTip && subjSpacebarPressed==2)
			{
				advanceTrial();
				subjSpacebarPressed=0;
			}			

            trialTimer.start(); // continue keeping it starting during stimulus
            drawingTrialFrame++;
            invisibleIndexFrames+=!allVisibleIndex;
            invisibleThumbFrames+=!allVisibleThumb;
            invisibleWristFrames+=!isVisible(markers.at(6).p);
        }
        else
        {
            if ( trialTimer.getElapsedTimeInMilliSec() > util::str2num<double>(parameters.find("IntervalTime")) )
            {
                boost::thread trialBeep( beepTrial );
                trialMode=STIMULUSMODE;
            }
        }
        isDrawing = (trialMode == STIMULUSMODE);
    }
}

void drawFingerCylinder()
{
	// https://www.opengl.org/sdk/docs/man2/xhtml/gluCylinder.xml
	int slices=20;
	int stacks=1;
	glPushMatrix();
	GLUquadric* myQuad=gluNewQuadric();
	//gluQuadricDrawStyle(myQuad,GLU_LINE);
	gluCylinder( myQuad, cylBase, cylBase, cylHeight, slices, stacks);
	gluDeleteQuadric(myQuad);
	glPopMatrix();
}

void drawFingers()
{
    glPushMatrix();
	glPointSize(7);
	glColor3fv(glRedDim);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	//glTranslated(-10,0,0);
    glMultMatrixd(rigidStartThumb.getFullTransformation().data());
	glRotated(90,0,1,0);
	drawFingerCylinder();
	glPopMatrix();
	glPushMatrix();
	//glTranslated(5,0,-10);
	glMultMatrixd(rigidStartIndex.getFullTransformation().data());
	drawFingerCylinder();
	glPopMatrix();
	glPopAttrib();
}

void drawStimulus()
{
	drawFingers();
	glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0, 2.0);
    Vector3d *P = stim.getVertices();
    glColor3fv(glBlack);
    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[1].data());
    glVertex3dv(P[2].data());
    glVertex3dv(P[5].data());
    glVertex3dv(P[6].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[0].data());
    glVertex3dv(P[1].data());
    glVertex3dv(P[3].data());
    glVertex3dv(P[2].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[4].data());
    glVertex3dv(P[5].data());
    glVertex3dv(P[7].data());
    glVertex3dv(P[6].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[0].data());
    glVertex3dv(P[1].data());
    glVertex3dv(P[4].data());
    glVertex3dv(P[5].data());
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3dv(P[2].data());
    glVertex3dv(P[3].data());
    glVertex3dv(P[6].data());
    glVertex3dv(P[7].data());
    glEnd();
    glPopAttrib();
    stimDrawer.draw();
    glPopMatrix();
}

void drawTrial()
{
    switch ( trialMode )
    {
    case STIMULUSMODE:
    {
        drawStimulus();
    }
        break;
    case HANDONSTARTMODE:
    {
        //drawStimulus();
    }
        break;
    }
}

void generateStimulus()
{
    stim.setHeight(parameters.get("StimHeight"));
    stim.setWFar(parameters.get("StimWidthFar"));
    stim.setWNear(parameters.get("StimWidthNear"));
    stim.setZNear(factors.getCurrent().at("StimDepth")/2);
    stim.setZFar(-factors.getCurrent().at("StimDepth")/2);
    stim.setDensity(parameters.get("StimDensity"));
    stim.setCenter(Vector3d(0,
		factors.getCurrent().at("StimCenterY")+cyclopeanY,
		factors.getCurrent().at("Distances")));
	stim.compute(); // Update the stimulus points
    // Update the stimulus drawer with the new stimulus
	stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&stim);
    stimDrawer.initList(&stim,glRedDim);
}

/**
 * @brief paintGL
 */
void paintGL()
{
	if ( subjSpacebarPressed!=0 )
		return;

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
	
   // RoveretoMotorFunctions::homeScreen(3500);
    RoveretoMotorFunctions::homeObjectAsynchronous(3500);

    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {
        exit(0);
    }
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
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("EXPERIMENT GRASPING");
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

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
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
			//cyclopeanY = 0.5*(eyeRight.y() + eyeLeft.y());
            headCalibrationDone=3;
            RoveretoMotorFunctions::moveObjectAbsolute( platformCalibration+Vector3d(0,0,constantOffsetZ), rodTipHome, 3500);
            beepOk();
        }

		if (headCalibrationDone==3 && fingerCalibrationDone==4 && allVisiblePatch && allVisibleFingers )
		{
			if (subjSpacebarPressed==0)
				subjSpacebarPressed=1;
			break;
		}

    }
        break;
    case 'i':
        infoDraw=!infoDraw;
        break;
    case 'f':
    case 'F':
    {
        if (headCalibrationDone < 3 )
            break;
        // Here we record the finger tip physical markers
        if ( allVisiblePlatform && (fingerCalibrationDone==0) )
        {   platformIndex=markers.at(16).p;
            platformThumb=markers.at(15).p;
            fingerCalibrationDone=1;
            beepOk();
            break;
        }
        // Calibrate the index first
        if ( (fingerCalibrationDone==1) && allVisibleIndex )
        {
            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            platformIndex += Vector3d(cylBase,0,-cylHeight/2);
			rigidStartIndex.setRigidBody(markers.at(7).p-platformIndex,markers.at(8).p-platformIndex,markers.at(9).p-platformIndex);
            fingerCalibrationDone=2;
            beepOk();
            break;
        }
        // Calibrate the thumb then
        if ( (fingerCalibrationDone==2) && allVisibleThumb )
        {
			thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
			platformThumb += Vector3d(-cylHeight/2,0,cylBase);
			rigidStartThumb.setRigidBody(markers.at(11).p-platformThumb,markers.at(12).p-platformThumb,markers.at(13).p-platformThumb);
			fingerCalibrationDone=3;
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==3 )
        {
            fingerCalibrationDone=4;
            paintGL();
			// oggetto va in home
			RoveretoMotorFunctions::homeObject(4000);
            break;
        }
		// qui si mette il piano a mano (fra fingerCalibrationDone=3 e fingerCalibrationDone=4)
		if ( fingerCalibrationDone==4  && allVisibleFingers )
		{
			paintGL();
			fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
            trialMode = HANDONSTARTMODE;
			physicalRigidBodyTip = indexCoords.getP1().p;
			generateStimulus();
            beepTrial();
            trialFrame=0;
			infoDraw=false;
            drawingTrialFrame=0;
            globalTimer.start();
			break;
		}
    }
        break;

	case '4':
	{
		constantOffsetX-=1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
		break;
	}
	case '6':
	{
		constantOffsetX+=1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
		break;
	}
	case '8':
	{
		constantOffsetY+=1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
		break;
	}
	case '2':
	{
		constantOffsetY-=1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
		break;
	}
	case '+':
	{
		constantOffsetZ+=1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
		break;
	}
	case '-':
	{
		constantOffsetZ -= 1;
		moveStimulusObject(Vector3d(0,factors.getCurrent().at("StimCenterY"),factors.getCurrent().at("Distances")),3500);
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

void initProjectionScreen(double _focalDist)
{
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,_focalDist));
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
}

/**
* @brief initStreams
**/
void initStreams()
{
    ifstream inputParameters;
    inputParameters.open(parametersFileNames[paramIndex]);
    parameters.loadParameterFile(inputParameters);
    interoculardistance = util::str2num<double>(parameters.find("IOD"));
    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string markersFileName  = baseDir + "markersFile_" + subjectName + ".txt";

    // Check for output file existence
    if (util::fileExists(markersFileName ))
    {
        string error_on_file_io = markersFileName + string(" already exists");
        cerr << error_on_file_io << endl;
        MessageBox(NULL, (LPCSTR)"MARKER FILE ALREADY EXIST\n",NULL, NULL);
        exit(0);
    }
    else
    {
        markersFile.open(( markersFileName ).c_str());
        cerr << "File " << markersFileName << " loaded successfully" << endl;
        markersFile << fixed << setprecision(3) << "SubjectName\t\
			FingerDist\t\
			TrialNumber\t\
			TrialFrame\t\
			TotTime\t\
			fDistances\t\
			fStimDepth\t\
			fStimCenterY\t\
			EyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\t\
			EyeRightXraw\tEyeRightYraw\tEyeRightZraw\t\
			WristXraw\tWristYraw\tWristZraw\t\
			ThumbXraw\tThumbYraw\tThumbZraw\t\
			IndexXraw\tIndexYraw\tIndexZraw\t\
			IsDrawing\t\
			IsThumbProjected\t\
			IsIndexProjected\t\
			FingersLeaning\t\
			ThumbDirX\t\
			ThumbDirY\t\
			ThumbDirZ\t\
			IndexDirX\t\
			IndexDirY\t\
			IndexDirZ\t" << endl;
    }
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    initStreams();
    factors.init(parameters);
    factors.next();
	initProjectionScreen(factors.getCurrent().at("Distances"));

    vector<double> distances = util::str2num<double>(parameters.find("fDistances")," ");
    maxCoveredDistance = max(distances.begin(),distances.end()) - min(distances.begin(),distances.end());

    selectedFinger = util::str2num<int>(parameters.find("SelectedFinger"));
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
