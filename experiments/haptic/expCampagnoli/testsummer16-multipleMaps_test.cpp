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
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;

// Function prototypes
void generateStimulus();
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);
void drawStimulus();
void drawFingers(bool _areFingersDrawn);
void calculate_object_3Dproperties();
void beepInvisible();
void beepLong();
void beepOk();
void beepBad();
void beepTrial();

/********* VISUALIZATION VARIABLES *****************/
#ifndef SIMULATION
	static const bool gameMode=true;
	static const bool stereo=true;
#else
	static const bool gameMode=false;
	static const bool stereo=false;
#endif
/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;

Vector3d platformCalibration(310.0,-100.0,-300.0); // position of the platform when pcalibrating the fingers at the beginning of the experiment
static const Eigen::Vector3d rodTipHome(313.96,-264.16,-795.48); //marker 5 when motors are at home -- 28 june 2016
/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;
RigidBody rigidStartIndex;
RigidBody rigidStartThumb;
RigidBody rigidCurrentIndex;
RigidBody rigidCurrentThumb;
Vector3d eyeLeft;
Vector3d eyeRight;
Vector3d thumb;
Vector3d ind;
Vector3d platformThumb;
Vector3d platformIndex;
double cyclopeanY=0;
static double interoculardistance=65;

/********* CALIBRATION VARIABLES *********/
int headCalibrationDone=2;
int fingerCalibrationDone=0;
int platformCalibrationDone=0;

#ifdef SIMULATION
	bool markers_status = true;
#else
	bool markers_status = false;
#endif
bool allVisibleHead=markers_status;
bool allVisiblePatch=markers_status;
bool allVisibleThumb=markers_status;
bool allVisibleIndex=markers_status;
bool allVisibleFingers=markers_status;
bool allVisiblePlatform=markers_status;
bool thumbProjectedInside=false;
bool indexProjectedInside=false;
bool infoDraw=true;
bool isSaving=true;
bool fingersOccluded=false;
bool isStimulusDrawn = true;
bool training = true;
bool grasping = true;
bool feedback = true;
int framesOccluded=0;

// CONSTANT OFFSETS
double constantOffsetX=19;
double constantOffsetY=31;
double constantOffsetZ=-227;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
BalanceFactor<double> factors;
static const int HANDONSTARTMODE=0;
static const int STIMULUSMODE=1;
double fingerDistance=0;
int trialMode = STIMULUSMODE;
int isDrawing = 0;
ParametersLoader parameters;
int selectedFinger=-1;
#define ONLY_THUMB 0
#define ONLY_INDEX 1
#define BOTH_INDEX_AND_THUMB 2

/********* STIMULI *******/
StimulusDrawer stimDrawer[2], trainingstim;
CylinderPointsStimulus cylinder[2];
// Size of fingers cylinder
double cylBase=7;
double cylHeight=20;
double objwidth=30.0;
double objdepth=0.0;
double objheight = 0.0;
double central_distance = 0.0;
double object_distance = 0.0, object_y = 0.0;

/****** TIMING *****/
Timer globalTimer;
Timer fingersTimer;
Timer trialTimer;

unsigned int invisibleThumbFrames=0;
unsigned int invisibleIndexFrames=0;
unsigned int invisibleWristFrames=0;
unsigned int frameN=0;
unsigned int drawingTrialFrame=0;
unsigned int trialNumber=0;

bool isMovingRod=false;
bool fingNow=false;

/**** FILES VARIABLES ***/
ofstream markersFile;
int paramIndex=0;

#ifndef SIMULATION
	string parametersFileNames = "C:/cncsvisiondata/parametersFiles/Campagnoli/parameters_multipleMaps.txt";
#else
	string parametersFileNames = "/home/carlo/workspace/cncsvisioncmake/experiments/haptic/expCampagnoli/parameters_multipleMaps.txt";
	//string parametersFileNames = "/home/carlo/Desktop/parameters_3Dcues-parabCylinder_grasp-texture.txt";
#endif

/********* SOUNDS ********/
//#ifdef _WIN32
	/***** SOUND THINGS *****/
	boost::mutex beepMutex;
	void beepInvisible()
	{   
	#ifndef SIMULATION
		boost::mutex::scoped_lock lock(beepMutex);
	    Beep(1000,5);
	    return;
	#endif
	}
	void beepLong()
	{   
	#ifndef SIMULATION
		boost::mutex::scoped_lock lock(beepMutex);
	    Beep(220,880);
	    return;
	#endif
	}
	void beepOk()
	{   
	#ifndef SIMULATION
		boost::mutex::scoped_lock lock(beepMutex);
	    Beep(440,440);
	    return;
	#endif
	}
	void beepBad()
	{   
	#ifndef SIMULATION
		boost::mutex::scoped_lock lock(beepMutex);
	    Beep(200,200);
	#endif
	}

	void beepTrial()
	{   
	#ifndef SIMULATION
		boost::mutex::scoped_lock lock(beepMutex);
	    Beep(660,200);
	    return;
	#endif
	}
//#endif

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
			case 2:
			{   
				text.draw("Press spacebar to start calibration.");
			}
				break;
			case 3:  // When the head calibration is done then calibrate the fingers
			{   switch ( fingerCalibrationDone )
				{
					case 0:
					{
						text.draw("Press F to record platform markers");
						if(isVisible(markers.at(15).p) && isVisible(markers.at(16).p))
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Marker15= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(15).p.transpose()) );
						text.draw("Marker16= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(16).p.transpose()) );
					}	break;
					case 1:
					{
						glColor3fv(glWhite);
						text.draw("Move INDEX on platform markers to record ghost finger tips, then press F");
						if(allVisibleIndex)
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Marker7= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(7).p.transpose()) );
						text.draw("Marker8= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(8).p.transpose()) );
						text.draw("Marker9= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(9).p.transpose()) );
					}	break;
					case 2:
					{
						glColor3fv(glWhite);
						text.draw("Move THUMB on platform markers to record ghost finger tips, then press F");
						if(allVisibleThumb)
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Marker11= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(11).p.transpose()) );
						text.draw("Marker12= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(12).p.transpose()) );
						text.draw("Marker13= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(13).p.transpose()) );
					}	break;
					case 3:
					{
						glColor3fv(glWhite);
						text.draw("Move BOTH INDEX AND THUMB to rigidbody tip, then press F");
						if(allVisibleIndex)
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Index");
						if(allVisibleThumb)
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Thumb");
						if(isVisible(markers.at(6).p))
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Wrist");
						text.draw(" ");
						if(isVisible(markers.at(10).p))
							glColor3fv(glGreen);
						else
							glColor3fv(glRed);
						text.draw("Marker10= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(10).p.transpose()) );
					}	break;
				}
			}
				break;
        }
        if ( headCalibrationDone==3 && fingerCalibrationDone==4 )
        {   
			glColor3fv(glWhite);
			text.draw("subjName= " + parameters.find("SubjectName"));
			text.draw("trialNumber= " + stringify<int>(trialNumber));
			text.draw("frameN= " + stringify<int>(frameN));
			if(allVisibleIndex)
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
			if(allVisibleThumb)
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
			if(isVisible(markers.at(6).p))
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
            text.draw("Wrist= " + stringify< Eigen::Matrix<double,1,3> >(markers.at(6).p ));
			glColor3fv(glWhite);
            text.draw("TrialTimer= " + stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
			text.draw(" ");
			if(isVisible(markers.at(10).p))
				glColor3fv(glGreen);
			else
				glColor3fv(glRed);
			text.draw("Marker10= " + stringify<Eigen::Matrix<double,1,3> >(markers.at(10).p.transpose()) );
		}
        text.leaveTextInputMode();
    }
}

/**
* @brief move the marker 5 in the point specified by v. It corrects the position
**/
void moveStimulusObject(const Vector3d& v, int speed=4500)
{
    Vector3d currentPosition = markers.at(5).p;
    isSaving=false; // disattiva temporaneamente la scrittura su file
    idle();
    // Gestisce l'offset del marker rispetto al centro del paletto
    //double rodRadius=13;    //mm
    double markerThickness=11.0;
	double y_offset = -(object_y+floor((objheight/2.0) - 17.0));
	double x_offset = -3.0;
	double z_offset = markerThickness;

	if(!feedback)
	{
		z_offset = 150.0;
		y_offset = currentPosition.y();
	} else 
		z_offset = z_offset+objdepth/2.0;
	
	Vector3d finalPosition= v + Vector3d(x_offset, y_offset, -z_offset);

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

void loadStimulus()
{
	isStimulusDrawn = false;
	paintGL();
	if(!training)
	{
		grasping = factors.getCurrent().at("AbsDepth") > 200.0;
		feedback = factors.getCurrent().at("Feedback");
	} 
	calculate_object_3Dproperties();
	generateStimulus();
	initProjectionScreen(object_distance);
	moveStimulusObject(Vector3d(0,cyclopeanY,object_distance),4800);
	isStimulusDrawn = true;
}

/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial()
{
    globalTimer.start(); // Here we change trial mode

    if (training)
    {
		loadStimulus();
    } else
    {
		isStimulusDrawn = false;
        paintGL();
        beepTrial();
        double oldDistance = factors.getCurrent().at("AbsDepth");
        // Reinsert the trial if too much occluded frames
        double maxInvisibleFramesPercent = util::str2num<double>(parameters.find("MaxPercentOccludedFrames"))/100.0;
        if ( (invisibleIndexFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent ) ||
             (invisibleThumbFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent ) ||
             (invisibleWristFrames / ((double)drawingTrialFrame) > maxInvisibleFramesPercent ) )
        {
            map<string,double> currentFactorsList = factors.getCurrent();
            beepBad();
            cerr << "Reinserted current trial (InvIndex,InvThumb,drawingTrialFrame,totTrialFrame)" << invisibleIndexFrames << " " << invisibleThumbFrames << " " << drawingTrialFrame << " " << frameN << endl;
            factors.reinsert(currentFactorsList);
        }
        if (factors.isEmpty())
        {
            beepLong();
            beepLong();
            beepLong();
            exit(0);
        }
        invisibleIndexFrames = invisibleThumbFrames = invisibleWristFrames = frameN = drawingTrialFrame = 0;
        trialNumber++;
        factors.next();

        if (oldDistance == factors.getCurrent().at("AbsDepth"))
        {
            Timer sleepTimer; sleepTimer.sleep(1000);
        }

        double marker5z = markers.at(5).p.z();
        loadStimulus();
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

    if ( allVisiblePatch )
    	headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,deltaT);
    if ( allVisibleThumb )
    	thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,deltaT);
    if ( allVisibleIndex )
    	indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,deltaT);

	#ifndef SIMULATION
	    // Compute the coordinates of visual thumb
	    thumb = thumbCoords.getP1().p;
	    ind = indexCoords.getP1().p;
	#endif

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
		Vector2d indexProjection = cam.computeProjected(ind);
        indexProjectedInside = ( indexProjection.x() >= 0 && indexProjection.x() <= SCREEN_WIDTH ) && ( indexProjection.y() >= 0 && indexProjection.y() <= SCREEN_HEIGHT );
        frameN++;

		if(ind.z() > -150.0)
			trialTimer.start();

        // Check when to advance trial
        if (!isSaving)
            return;
        RowVector3d junk(9999,9999,9999);
        if ( !training && isSaving )
		{
			Vector3d thumbDir = rigidStartThumb.getFullTransformation().linear()*Vector3d(-1,0,0);
			Vector3d indexDir = rigidStartIndex.getFullTransformation().linear()*Vector3d(0,0,-1);
			markersFile << fixed << setprecision(3) <<
                       parameters.find("SubjectName") << "\t" <<
                       fingerDistance << "\t" <<
                       trialNumber << "\t" <<
                       frameN << "\t" <<
                       globalTimer.getElapsedTimeInMilliSec() << "\t" <<
                       factors.getCurrent().at("AbsDepth") << "\t" <<
					   objdepth << "\t" <<
					   feedback << "\t" <<
					   grasping << "\t" <<
					   objheight << "\t" <<
                       markers.at(6).p.transpose() << "\t" <<
                       thumb.transpose() << "\t" <<
                       ind.transpose() << "\t" <<
					   thumbDir.x() << "\t" <<
					   thumbDir.y() << "\t" <<
					   thumbDir.z() << "\t" <<
                       indexDir.x() << "\t" <<
					   indexDir.y() << "\t" <<
					   indexDir.z() << "\t" <<
					   fingersOccluded << "\t" <<
					   framesOccluded << "\t" <<
					   endl;
		}

		double timeLimit = str2num<double>(parameters.find("experimentTime"));
		if(training)
			timeLimit = str2num<double>(parameters.find("trainingTime"));

        if ( trialTimer.getElapsedTimeInMilliSec() > timeLimit )
        {
            advanceTrial();
        }
        drawingTrialFrame++;

       	fingersOccluded = !allVisibleFingers;
        framesOccluded += !allVisibleFingers;

        invisibleIndexFrames+=!allVisibleIndex;
        invisibleThumbFrames+=!allVisibleThumb;
        invisibleWristFrames+=!isVisible(markers.at(6).p);
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

void drawFingers(bool _areFingersDrawn)
{
	if(_areFingersDrawn)
	{
		glLoadIdentity();
		glPushMatrix();
		glPointSize(7);
		glColor3fv(glRed);
	
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
}

void generateStimulus()
{
	for (int i=0; i<2; i++)
	{
		cylinder[i].setNpoints(150);
		cylinder[i].setRadiusAndHeight(3.0,objheight); // raggio (mm) altezza (mm)
		cylinder[i].compute();
		stimDrawer[i].setStimulus(&cylinder[i]);
		stimDrawer[i].setSpheres(true);
		stimDrawer[i].initList(&cylinder[i], glRed);
	}
}

void drawStimulus()
{
	if(isVisible(markers.at(10).p))
		glColor3fv(glRed);
	else
		glColor3fv(glWhite);
	glLoadIdentity();
	glTranslated(0, 0, object_distance);

	if(!grasping)
	{
		if(factors.getCurrent().at("AbsDepth") == 0)
		{
			glPushMatrix();
			glTranslated(0, 12.5, objdepth/2.0);
			glutSolidSphere(3.0,10,10);
			glPopMatrix();
		} else
		{
			glPushMatrix();
			glTranslated(objwidth, 12.5, -objdepth/2.0);
			glutSolidSphere(3.0,10,10);
			glPopMatrix();
		}
	}
	
	// Right rear rod
	glPushMatrix();
	glTranslated(objwidth, 0, -objdepth/2.0);
	stimDrawer[1].draw();
	glPopMatrix();

	// Front rod
	glPushMatrix();
	glTranslated(0, 0, objdepth/2.0);
	stimDrawer[0].draw();
	glPopMatrix();
}

void drawTrial()
{
	if(isStimulusDrawn)
	{
		switch ( trialMode )
		{
			case STIMULUSMODE:
			{
				if(isVisible(markers.at(10).p))
				{
					if(abs(markers.at(10).v.x()) < 5.0)
					{
	    				drawFingers(feedback);
						drawStimulus();
					}
				}
			}
				break;
			case HANDONSTARTMODE:
			{
				//drawStimulus();
			}
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
        cam.setEye(Vector3d(interoculardistance/2.0,0.0,0.0));
		//drawStimulus();
        
		drawInfo();
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(Vector3d(-interoculardistance/2.0,0.0,0.0));
		//drawStimulus();

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
	
    RoveretoMotorFunctions::homeMirror(4500);
    RoveretoMotorFunctions::homeScreen(4500);
    RoveretoMotorFunctions::homeObjectAsynchronous(4500);
	
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
    {   glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
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
	    {   
	        headCalibrationDone = 3;
			RoveretoMotorFunctions::moveObjectAbsolute( platformCalibration, rodTipHome, 4500);
	    }
	        break;
		case 'p':
		{
			if(training)
				grasping = !grasping;
		} break;
		case 'h':
		{
			if(training)
			{
				feedback = !feedback;
				advanceTrial();
			}
		} break;
		case 'e':
		{
			training = false;
			loadStimulus();
		} break;
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
	        if ( fingerCalibrationDone==3  && allVisibleFingers )
	        {
	            fingerCalibrationDone=4;
	            infoDraw=false;
	            paintGL();
	            trialMode = STIMULUSMODE;
	            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
	            loadStimulus();

	            beepTrial();
	            frameN=0;
	            drawingTrialFrame=0;
	            globalTimer.start();
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
    RoveretoMotorFunctions::moveScreenAbsoluteAsynchronous(_focalDist,homeFocalDistance,4000);
}

/**
* @brief initStreams
**/
void initStreams()
{
    ifstream inputParameters;
    inputParameters.open(parametersFileNames.c_str()); 
    parameters.loadParameterFile(inputParameters);
    cerr << "sono qui!" << endl;
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
    #ifndef SIMULATION
        MessageBox(NULL, (LPCSTR)"MARKER FILE ALREADY EXIST\n",NULL, NULL);
    #endif
        exit(0);
    }
    else
    {
        markersFile.open(( markersFileName ).c_str());
        cerr << "File " << markersFileName << " loaded successfully" << endl;
        markersFile << fixed << setprecision(3) << "subjName\t\
			fingerDist\t\
			trialN\t\
			frameN\t\
			globalTime\t\
			AbsDepth\t\
			RelDepthObj\t\
			feedback\t\
			grasping\t\
			objheight\t\
			wristXraw\twristYraw\twristZraw\t\
			thumbXraw\tthumbYraw\tthumbZraw\t\
			indexXraw\tindexYraw\tindexZraw\t\
			thumbDirX\t\
			thumbDirY\t\
			thumbDirZ\t\
			indexDirX\t\
			indexDirY\t\
			indexDirZ\t\
			fingersOccluded\t\
			framesOccluded\t" << endl;
    }
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    initStreams();
    factors.init(parameters);
	factors.print();
	central_distance = str2num<double>(parameters.find("CentralDistance"));
    selectedFinger = 2;
	factors.next();
    //loadStimulus();
}

void calculate_object_3Dproperties()
{
	if(training)
	{
		objdepth = 40.0;
		objheight = 50.0;
		object_distance = -465.0;
		object_y = 120.0;
	} else
	{
		if( factors.getCurrent().at("AbsDepth") == 120.0 || 
			factors.getCurrent().at("AbsDepth") == 380.0 || 
			factors.getCurrent().at("AbsDepth") == 420.0 || 
			factors.getCurrent().at("AbsDepth") == 460.0)
		{
			objdepth = 40.0;
		} else if(factors.getCurrent().at("AbsDepth") == 0.0 || 
			factors.getCurrent().at("AbsDepth") == 40.0 || 
			factors.getCurrent().at("AbsDepth") == 80.0)
		{
			if(unifRand(-1.0, 1.0) > 0)
				objdepth = 40.0;
			else
				objdepth = 50.0;
		} else
		{	
			objdepth = 50.0;
		}
		//objheight = sqrt(pow(objdepth, 2) + pow(objwidth, 2));
		objheight = 50.0;

		double distance_code = factors.getCurrent().at("AbsDepth");

		if(distance_code < 200.0)
		{
			if(distance_code < 100.0)
			{
				object_distance = central_distance - (distance_code + objdepth/2.0);
			} else
				object_distance = central_distance - distance_code + objdepth/2.0;
		} else 
			object_distance = -distance_code;
		

		if(objdepth == 40.0)
			object_y = 120.0;
		else
			object_y = 0.0;
	}
}

/**
 * @brief main
 */
int main()
{
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
