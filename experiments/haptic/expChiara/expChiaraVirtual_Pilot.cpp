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
#include "CylinderPointsStimulus.h"
#include "SpherePointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"
#include "PlatoGlasses.h"
#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;

// Function prototypes
void generateSphereStimulus(double radius);
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
double homeFocalDistance=-418.5;
double maxCoveredDistance=0;

Vector3d platformCalibration(250.0,-100.0,-500.0);
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
Vector3d visualThumb;
Vector3d visualIndex;
Vector3d physicalRigidBodyTip;
Vector3d platformThumb;
Vector3d platformIndex;
Vector3d visualStimCenter;
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


/****** OPTOTRAK AND MARKERS VARIABLES ***/
Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
BalanceFactor<double> factors;
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;
double fingerDistance=0;
int trialMode = HANDONSTARTMODE;
int isDrawing = 0;
ParametersLoader parameters;
int selectedFinger=-1;
#define ONLY_THUMB 0
#define ONLY_INDEX 1
#define BOTH_INDEX_AND_THUMB 2

double thumbSphereRadius=8;
double indexSphereRadius=8;

/********* STIMULI *******/
CylinderPointsStimulus cylinder;
SpherePointsStimulus sphere;
StimulusDrawer stimDrawer;
double stimulusDensity=0.025;
double thetaRod;
double phiRod;
double r = 40;
Vector3d rodStart;
Vector3d rodEnd;

/****** TIMING *****/
Timer globalTimer;
Timer fingersTimer;
Timer trialTimer;

unsigned int invisibleThumbFrames=0;
unsigned int invisibleIndexFrames=0;
unsigned int invisibleWristFrames=0;
unsigned int trialFrame=0;
int drawingTrialFrame=0;

int totalTrialNumber=0;
bool isMovingRod=false;

/**** FILES VARIABLES ***/
ofstream markersFile;
int paramIndex=0;
char *parametersFileNames[] =
{
"C:/cncsvisiondata/parametersFiles/Chiara/experimentVirtualPilot/parametersExpChiaraVirtualPilot_training.txt",
"C:/cncsvisiondata/parametersFiles/Chiara/experimentVirtualPilot/parametersExpChiaraVirtualPilot.txt"
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
                text.draw("Move BOTH INDEX AND THUMB to rigidbody tip, then press F");
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
            text.draw("VisualRod= " + stringify<Eigen::Matrix<double,1,3> >(visualStimCenter.transpose()) );
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("TrialTimer= " + stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
			text.draw("FingersTime= " + stringify<int>(fingersTimer.getElapsedTimeInMilliSec()));
        }
        //text.draw("Gain=" + stringify<double>(factors.getCurrent().at("Gain")));
        text.leaveTextInputMode();
    }
}

/**
* @brief fingersInCylindricalVolume Controls that both the fingers are in a given cylindrical volume
*/
bool fingersInCylindricalVolume(const Vector3d &thumb, const Vector3d &index, const Vector3d &cylinderCenter, double cylinderHeight, double innerRadius, double outerRadius, int _selectedFinger)
{
    // Control height
    bool heightIndexInside = (index.y() < cylinderCenter.y() + cylinderHeight/2 )&& (index.y() > cylinderCenter.y() - cylinderHeight/2);
    bool heightThumbInside = (thumb.y() < cylinderCenter.y() + cylinderHeight/2 )&& (thumb.y() > cylinderCenter.y() - cylinderHeight/2);

    // Distance of index from center of stimulus on XZ plane
    double indexDistanceFromCenter = ( Vector2d( index.x(),index.z() ) - Vector2d(cylinderCenter.x(),cylinderCenter.z()) ).norm();

    // Distance of thumb from center of stimulus on XZ plane
    double thumbDistanceFromCenter = ( Vector2d( thumb.x(),thumb.z() ) - Vector2d(cylinderCenter.x(),cylinderCenter.z()) ).norm();
    bool indexXZinside =  indexDistanceFromCenter > innerRadius && indexDistanceFromCenter < outerRadius;
    bool thumbXZinside =  thumbDistanceFromCenter > innerRadius && thumbDistanceFromCenter < outerRadius;

    switch (_selectedFinger)
    {
    case ONLY_THUMB:
    {
        return heightThumbInside && thumbXZinside;
    }
    break;
    case ONLY_INDEX:
    {
        return heightIndexInside && indexXZinside;
    }
    break;
    case BOTH_INDEX_AND_THUMB:
    {
        return heightIndexInside && indexXZinside && heightThumbInside && thumbXZinside;
    }
    break;
	case -1:
		{
			cerr << "Error you must set SelectedFinger in parameters file, press Enter to continue" << endl;
			cin.ignore(1E6,'\n');
			plato_stop();
			exit(0);
		}
		break;
    }
}

bool fingersInSphericalVolume(const Vector3d &thumb, const Vector3d &index, const Vector3d &center, double innerRadius, double outerRadius, int _selectedFinger)
{
    // Distance of index from center of stimulus on XZ plane
    double indexDistanceFromCenter = (index-center).norm();
    // Distance of thumb from center of stimulus on XZ plane
    double thumbDistanceFromCenter = (thumb-center).norm();

    bool indexInsideSphere =  indexDistanceFromCenter > innerRadius && indexDistanceFromCenter < outerRadius;
    bool thumbInsideSphere =  thumbDistanceFromCenter > innerRadius && thumbDistanceFromCenter < outerRadius;

    switch (_selectedFinger)
    {
    case ONLY_THUMB:
    {
        return  thumbInsideSphere;
    }
    break;
    case ONLY_INDEX:
    {
        return indexInsideSphere;
    }
    break;
    case BOTH_INDEX_AND_THUMB:
    {
        return thumbInsideSphere && indexInsideSphere;
    }
    break;
	case -1:
	{
		cerr << "Error you must set SelectedFinger in parameters file, press Enter to continue" << endl;
		cin.ignore(1E6,'\n');
		plato_stop();
		exit(0);
	}
		break;
    }
}

/**
* @brief move the marker 5 in the point specified by v. It corrects the position
**/
void moveStimulusObject(const Vector3d& v, int speed)
{
    assert( (v.z() < homeFocalDistance) );	// too avoid overdisplacements!!!
	Vector3d currentPosition = markers.at(5).p;
    isSaving=false; // disattiva temporaneamente la scrittura su file
	idle();
    // gestisce l'offset del marker rispetto al centro del paletto
    double rodRadius=13;    //mm
    double markerThickness=3.5;

	double constantOffsetX=-5;
	double constantOffsetY=-63;
	double constantOffsetZ=-4;
	
	Vector3d finalPosition=v+Vector3d(constantOffsetX,constantOffsetY, -(markerThickness+rodRadius/2));
    if ( !isVisible(finalPosition) )
    {   beepBad();
        cerr << "CAN'T MOVE MOTOR TO INFINITY!!!" << endl;
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
            plato_stop();
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
		generateSphereStimulus( factors.getCurrent().at("StimulusRadius") );
        initProjectionScreen(factors.getCurrent().at("Distances"));
        moveStimulusObject(visualStimCenter,3500);
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
			boost::thread invisibleBeep( beepInvisible);

        Vector2d thumbProjection = cam.computeProjected(thumb);
        thumbProjectedInside = ( thumbProjection.x() >= 0 && thumbProjection.x() <= SCREEN_WIDTH ) && ( thumbProjection.y() >= 0 && thumbProjection.y() <= SCREEN_HEIGHT );

        Vector2d indexProjection = cam.computeProjected(index);
        indexProjectedInside = ( indexProjection.x() >= 0 && indexProjection.x() <= SCREEN_WIDTH ) && ( indexProjection.y() >= 0 && indexProjection.y() <= SCREEN_HEIGHT );

        trialFrame++;
        // Work on fingers coordinates
        double wThumb=0.5, wIndex=0.5;
        Vector3d m = (wThumb*thumb+wIndex*index)/(wThumb+wIndex);
        Vector3d d = thumb-index;

        visualThumb = thumb;
        visualIndex = index;

        double clamp= 2*factors.getCurrent().at("StimulusRadius");
        double gain = 1.0;//in quello di Bob era invece: factors.getCurrent().at("Gain");
        if ( d.norm() > clamp )
        {
            visualThumb = m + 0.5*d.normalized()*(gain*d.blueNorm() - clamp*(gain-1));
            visualIndex = m - 0.5*d.normalized()*(gain*d.blueNorm() - clamp*(gain-1));
        }

        // Check when to advance trial
        double stimRadius = factors.getCurrent().at("StimulusRadius");
        double radiusTolerance = util::str2num<double>(parameters.find("RadiusTolerance"));
        fingNow = fingersInSphericalVolume(thumb,index, visualStimCenter, stimRadius-radiusTolerance, stimRadius+radiusTolerance, selectedFinger );

		// SubjectName\tFingerDist\tTrialNumber\tTrialFrame\tTotTime\tVisualStimX\tVisualStimY\tVisualStimZ\tfStimulusRadius\tfDistances\tfGain\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tVisualThumbXraw\tVisualThumbYraw\tVisualThumbZraw\tVisualIndexXraw\tVisualIndexYraw\tVisualIndexZraw\tIsDrawing\tIsThumbProjected\tIsIndexProjected
        if (!isSaving)
			return;
		RowVector3d junk(9999,9999,9999);
        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    totalTrialNumber << "\t" <<
                    trialFrame << "\t" <<
                    globalTimer.getElapsedTimeInMilliSec() << "\t" <<
                    visualStimCenter.x() << "\t" <<
                    visualStimCenter.y() << "\t" <<
                    visualStimCenter.z() << "\t" <<
                    factors.getCurrent().at("StimulusRadius") << "\t" <<
                    factors.getCurrent().at("Distances") << "\t" <<
                    factors.getCurrent().at("Gain") << "\t" <<
					factors.getCurrent().at("DisappearRadius") << "\t" << 
                    ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                    ( isVisible(eyeRight) ? eyeRight.transpose() : junk ) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                    ( isVisible(thumb) ? thumb.transpose() : junk ) << "\t" <<
                    ( isVisible(index) ? index.transpose() : junk ) << "\t" <<
                    ( isVisible(visualThumb) ? visualThumb.transpose() : junk ) << "\t" <<
                    ( isVisible(visualIndex) ? visualIndex.transpose() : junk ) << "\t" <<
                    isDrawing << "\t" <<
                    thumbProjectedInside << "\t" <<
                    indexProjectedInside << "\t" <<
					fingNow <<
                    endl;

        // Switch to this function if you want cylindrical stimulus
        //fingersInCylindricalVolume( thumb,index, Vector3d(0,0,factors.getCurrent().at("Distances")), util::str2num<double>(parameters.find("StimulusHeight")), stimRadius,stimRadius+20, selectedFinger );
        if (trialMode == STIMULUSMODE )
        {
            if ( !fingNow ) // always starts counting the time if the finger is outside
            {
                fingersTimer.start();
            }
			// only if the finger is inside the volume the counter has counted more that 1 second
            if ( fingersTimer.getElapsedTimeInMilliSec() > util::str2num<double>(parameters.find("LeaningTime")) )
            {
					plato_write(PLATO_LEFT_RIGHT_CLOSED);
                    advanceTrial();
					plato_write(PLATO_LEFT_RIGHT_OPEN);
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
void ddcylinder()
{
	GLUquadric* myQuad=gluNewQuadric();
	gluCylinder( myQuad, 7, 7, 20, 20, 1 );
	gluDeleteQuadric(myQuad);
}
void drawStimulus()
{
    double stimRadius = factors.getCurrent().at("StimulusRadius");

    glPushMatrix();
	glPointSize(7);
	glColor3fv(glRed);
	/*
	glPushMatrix();
    glLoadMatrixd(rigidStartThumb.getFullTransformation().data());
    glScaled(1,1,0.001);
    glutSolidSphere(thumbSphereRadius,15,15);
	glPopMatrix();

	glPushMatrix();
    glLoadMatrixd(rigidStartIndex.getFullTransformation().data());
    glScaled(0.001,1,1);
    glutSolidSphere(indexSphereRadius,15,15);
	glPopMatrix();
	*/
	glPushMatrix();
	glTranslated(-10,0,0);
    glMultMatrixd(rigidStartThumb.getFullTransformation().data());
	glRotated(90,0,1,0);
	ddcylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslated(5,0,-10);
	glMultMatrixd(rigidStartIndex.getFullTransformation().data());
	ddcylinder();
	glPopMatrix();
	////////// OLD ////////
	/*
	glPointSize(7);
    glBegin(GL_POINTS);
    glVertex3dv(visualThumb.data());
    glVertex3dv(visualIndex.data());
    glEnd();
	*/

    double insideRadius = factors.getCurrent().at("DisappearRadius");
	if ((thumb - visualStimCenter).norm() > insideRadius ) // disegna lo stimolo
	{
		glColor3fv(glRed);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(visualStimCenter.x(),visualStimCenter.y(),visualStimCenter.z());
		stimDrawer.draw();
		glScaled(0.99,0.99,0.99);
		glColor3fv(glBlack);
		glutSolidSphere(stimRadius,20,20);
		glPopMatrix();

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glLineWidth(5);
		glColor3fv(glRed);
		glBegin(GL_LINES);
		glVertex3dv(rodStart.data());
		glVertex3dv(rodEnd.data());
		glEnd();
		glPopAttrib();
	}
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

void generateSphereStimulus(double radius)
{
    int nStimulusPoints=(int)(stimulusDensity*4*M_PI*radius*radius);
    /*
    cylinder.setNpoints(nStimulusPoints);
    cylinder.setAperture(0,2*M_PI);
    cylinder.setRadiusAndHeight(radius,height);
    cylinder.setFluffiness(0.001);
    cylinder.compute();
    */
    sphere.setNpoints(nStimulusPoints);
    sphere.setRadius(radius);
    sphere.setFluffiness(0.001);
    sphere.compute();

    // Update the stimulus drawer
    stimDrawer.setSpheres(false);

    stimDrawer.setStimulus(&sphere);
    stimDrawer.initList(&sphere,glRed,3);
    //stimDrawer.setStimulus(&cylinder);
    //stimDrawer.initList(&cylinder,glRed,3);

    visualStimCenter << util::str2num<double>(parameters.find("OffsetX")),0,factors.getCurrent().at("Distances");
	rodStart = visualStimCenter + r*Vector3d(sin(thetaRod)*cos(phiRod),cos(thetaRod),sin(thetaRod)*sin(phiRod));
	rodEnd= visualStimCenter - r*Vector3d(sin(thetaRod)*cos(phiRod),cos(thetaRod),sin(thetaRod)*sin(phiRod));
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
	RoveretoMotorFunctions::homeObjectAsynchronous(3500);

    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {
		plato_stop();
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
		plato_stop();
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
            RoveretoMotorFunctions::moveObjectAbsolute( platformCalibration, rodTipHome, 3500);
            beepOk();
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
			rigidStartIndex.setRigidBody(markers.at(7).p-platformIndex,markers.at(8).p-platformIndex,markers.at(9).p-platformIndex);
			fingerCalibrationDone=2;
            beepOk();
            break;
        }
		// Calibrate the thumb then
		if ( (fingerCalibrationDone==2) && allVisibleThumb )
        {   
			rigidStartThumb.setRigidBody(markers.at(11).p-platformThumb,markers.at(12).p-platformThumb,markers.at(13).p-platformThumb);
            thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            fingerCalibrationDone=3;
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==3  && allVisibleFingers )
        {
			fingerCalibrationDone=4;
            infoDraw=false;
            paintGL();
            physicalRigidBodyTip = indexCoords.getP1().p;
            trialMode = HANDONSTARTMODE;
            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
            moveStimulusObject(visualStimCenter,3500);
            beepTrial();
            trialFrame=0;
            drawingTrialFrame=0;
            globalTimer.start();
			plato_write(PLATO_LEFT_RIGHT_OPEN);

            break;
        }
    }
    break;
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
	    markersFile << fixed << setprecision(3) << "SubjectName\tFingerDist\tTrialNumber\tTrialFrame\tTotTime\tVisualStimX\tVisualStimY\tVisualStimZ\tfStimulusRadius\tfDistances\tfGain\tDisappearRadius\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw\tVisualThumbXraw\tVisualThumbYraw\tVisualThumbZraw\tVisualIndexXraw\tVisualIndexYraw\tVisualIndexZraw\tIsDrawing\tIsThumbProjected\tIsIndexProjected\tFingersLeaning" << endl;
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
    generateSphereStimulus(factors.getCurrent().at("StimulusRadius") );
    initProjectionScreen(factors.getCurrent().at("Distances"));

	vector<double> distances = util::str2num<double>(parameters.find("fDistances")," ");
	maxCoveredDistance = max(distances.begin(),distances.end()) - min(distances.begin(),distances.end());

	selectedFinger = util::str2num<int>(parameters.find("SelectedFinger"));
	// For the rod initialization
	thetaRod = M_PI/180.0*parameters.get("RodLatitude");
	phiRod = M_PI/180.0*parameters.get("RodLongitude");
	r = parameters.get("RodRadius");
	rodStart = visualStimCenter + r*Vector3d(sin(thetaRod)*cos(phiRod),cos(thetaRod),sin(thetaRod)*sin(phiRod));
	rodEnd= visualStimCenter - r*Vector3d(sin(thetaRod)*cos(phiRod),cos(thetaRod),sin(thetaRod)*sin(phiRod));
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
	// Initialize Plato glasses communication
	plato_init();
    plato_write(PLATO_LEFT_RIGHT_CLOSED);
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
