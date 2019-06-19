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

#include "IncludeGL.h"

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Marker.h"
#include "Util.h"
#include "Optotrak2.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor2.h"
#include "CylinderPointsStimulus.h"
#include "EllipsoidPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "RoveretoMotorFunctions.h"

#include "LatestCalibration.h"

using namespace std;
using namespace Eigen;
using namespace RoveretoMotorFunctions;

// Function prototypes
void generateStimulus(double xAxis,double zAxis, double height);
void initProjectionScreen(double _focalDist);
void paintGL();
void idle(void);
/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/********* CAMERA AND CALIBRATIONS **********************/
VRCamera cam;
Screen screen;
double homeFocalDistance=-418.5;

Vector3d platformCalibration(250.0,-100.0,-500.0);
static const Eigen::Vector3d rodTipHome(344.95,-213.77,-824.6); //relativo calibration 29 maggio 2013 in coordinate OpenGL

/********** EYES AND MARKERS **********************/
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;
Vector3d eyeLeft;
Vector3d eyeRight;
Vector3d thumb;
Vector3d index;

Vector3d physicalRigidBodyTip;
Vector3d platformThumb;
Vector3d platformIndex;
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
bool debugRotated=false;
std::vector<double> previousAdjustMents;

int fingerInside=1;
int fingerWasInside=1;

/****** OPTOTRAK AND MARKERS VARIABLES ***/
Optotrak2 optotrak;
vector <Marker> markers;

/********* TRIAL VARIABLES *********/
ParametersLoader parameters;
BalanceFactor<double> factors;
std::map< vector<double>, double > heightTheta2AverageAdjust;

const int HANDONSTARTMODE=0;
const int STIMULUSMODE=1;
const int ONLY_THUMB=0;
const int ONLY_INDEX=1;
const int BOTH_INDEX_AND_THUMB=2;

int trialMode = HANDONSTARTMODE;
bool isDrawing = false;

double fingerDistance=0;
int selectedFinger=ONLY_THUMB;

/********* STIMULI *******/
EllipsoidPointsStimulus cylinder;
StimulusDrawer stimDrawer;

/****** TIMING *****/
Timer globalTimer;
Timer trialTimer;

unsigned int invisibleThumbFrames=0;
unsigned int invisibleIndexFrames=0;
unsigned int trialFrame=0;
int drawingTrialFrame=0;

int totalTrialNumber=0;
bool isMovingRod=false;

/**** FILES VARIABLES ***/
ofstream markersFile;
RowVector3d junk(9999,9999,9999);
int paramIndex=0;
char *parametersFileNames[] = {"C:/cncsvisiondata/parametersFiles/Campagnoli/parametersExpCampagnoliShapeConstancyGrasp.txt",
                               "C:/cncsvisiondata/parametersFiles/Campagnoli/parametersExpCampagnoliShapeConstancyGrasp_training.txt"
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

std::multimap< vector<double>, double > readFactorsAdjustmentZ(const std::string &filename)
{
	// Empty factors multimap
	std::multimap< vector<double>, double > m;
	cerr << filename << endl;
	// Load the previously measured adjustment files
	// this is a tricky way to put explicitly 
	//std::string adjustmentFileName( baseDir + "adjustmentFile_" + subjectName + ".txt");
	ifstream adjustmentFile(filename.c_str());
	
	std::string line;
	while ( getline(adjustmentFile,line) )
	{
		std::vector <string> tokens;
        util::tokenizeString(line, tokens, "\t\r  ");
		if ( tokens.at(0).at(0) == '#' )
            continue; // comments
		int trialNumber = util::str2num<int>(tokens.at(0));
		double cylHeight = util::str2num<double>(tokens.at(1));
		double cylOrient = util::str2num<double>(tokens.at(2));
		double screenDist = util::str2num<double>(tokens.at(3));
		double adjustedZ = util::str2num<double>(tokens.at(4));

		vector<double> key;
		key.push_back(cylHeight);
		key.push_back(cylOrient);
		key.push_back(screenDist);

		std::pair< vector<double> , double > keyValueKey(key,adjustedZ);
		m.insert( keyValueKey );
	}
	return m;
}

std::map< vector<double>, double > computeAverages( const std::multimap< vector<double>, double >	&m)
{
	std::map< vector<double>, double > averages;
	for ( std::multimap< vector<double>, double >::const_iterator iter = (m.begin()); iter!=m.end(); ++iter)
    {
		if ( averages.find(iter->first) == averages.end() )
			averages[iter->first]=0.0;
		averages[iter->first] += iter->second/m.count(iter->first);
    }

	for ( std::map< vector<double>, double >::iterator iter = averages.begin(); iter!=averages.end(); ++iter)
	{
		cout << "(h,theta,screenDist)= " << iter->first.at(0) << " " << iter->first.at(1) << " " << iter->first.at(2) << " --> average adjusted Z " << iter->second << endl; 
	}	
	return averages;
}
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
        {   //text.draw("Trial mode " + stringify<int>(trialMode ));
			//text.draw("Trial number" + stringify<int>(totalTrialNumber));
			text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("TrialFrame= " + stringify<int>(trialFrame));
            text.draw("InvThumbFrame= " + stringify<int>(invisibleThumbFrames));
            text.draw("InvIndexFrame= " + stringify<int>(invisibleIndexFrames));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            text.draw("TrialTimer= " + stringify<int>(trialTimer.getElapsedTimeInMilliSec()));
			text.draw("CylinderOrientation= " + stringify<int>(factors.getCurrent().at("CylinderOrientation")));
			text.draw("CylinderHeight= " + stringify<int>(factors.getCurrent().at("CylinderHeight")));
			text.draw("XDiameter= " + stringify<double>(cylinder.getXAxis()*2));
			text.draw("ZDiameter= " + stringify<double>(cylinder.getZAxis()*2));
			text.draw("IsDrawing= " + stringify<int>((int)isDrawing));
		}
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
    }
	return 0;
}


bool fingersInSphericalVolume(const Vector3d &v, const Vector3d &c, double radius)
{
	return (v-c).norm() < radius;
}

/**
 * @brief advanceTrial: This function to the next trial
 */
void advanceTrial()
{
//        paintGL();
        if (factors.isEmpty())
        {
            beepLong();
            beepLong();
            beepLong();
            exit(0);
        }
		
		// Reinsert the trial if too much occluded frames
        if ( (invisibleIndexFrames + invisibleThumbFrames) / (2.0*drawingTrialFrame) > util::str2num<double>(parameters.find("MaxPercentOccludedFrames"))/100.0 )
        {
            map<string,double> currentFactorsList = factors.getCurrent();
            beepBad();
            cerr << "Reinserted current trial (InvIndex,InvThumb,drawingTrialFrame,totTrialFrame)" << invisibleIndexFrames << " " << invisibleThumbFrames << " " << drawingTrialFrame << " " << trialFrame << endl;
            factors.reinsert(currentFactorsList);
         }
        invisibleIndexFrames = invisibleThumbFrames = trialFrame = drawingTrialFrame = 0;
        
        factors.next();
        double cylHeight = factors.getCurrent().at("CylinderHeight");
		double cylXAxis = cylHeight * util::str2num<double>(parameters.find("CylinderXAxisProportionToHeight"));
		double cylZAxis=0.0;
		if ( factors.getCurrent().at("Selection")==1 )
		{
			vector<double> key;
			key.push_back(factors.getCurrent().at("CylinderHeight"));
			key.push_back(factors.getCurrent().at("CylinderOrientation"));
			key.push_back(factors.getCurrent().at("Distances"));
			cylZAxis = heightTheta2AverageAdjust[key];
		}
		else
			cylZAxis = cylHeight;

        generateStimulus(cylXAxis,cylZAxis,cylHeight);
        initProjectionScreen(factors.getCurrent().at("Distances"));

		totalTrialNumber++;
		boost::thread trialBeepThread( beepTrial );
}

/**
 * @brief idle
 */
void idle()
{
	fingerWasInside=fingerInside;
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

    // Compute the coordinates of thumb
    thumb = thumbCoords.getP1().p;
    index = indexCoords.getP1().p;

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;

    if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
    {
        if ( !allVisibleIndex || !allVisibleThumb )
            boost::thread invisibleBeep( beepInvisible);

        Vector2d thumbProjection = cam.computeProjected(thumb);
        thumbProjectedInside = ( thumbProjection.x() >= 0 && thumbProjection.x() <= SCREEN_WIDTH ) && ( thumbProjection.y() >= 0 && thumbProjection.y() <= SCREEN_HEIGHT );

        Vector2d indexProjection = cam.computeProjected(index);
        indexProjectedInside = ( indexProjection.x() >= 0 && indexProjection.x() <= SCREEN_WIDTH ) && ( indexProjection.y() >= 0 && indexProjection.y() <= SCREEN_HEIGHT );

        trialFrame++;
		fingerInside = fingersInSphericalVolume( index, physicalRigidBodyTip, 100);
		
		// Condizione di uscita
		if ( fingerInside==0 && fingerWasInside==1 )
		{
			if ( trialMode == HANDONSTARTMODE )
			{
				trialMode=STIMULUSMODE;
				drawingTrialFrame=0;
				isDrawing=true;
				trialTimer.start();
			}
		}
		// Condizione di entrata, manda avanti il trial
		if ( fingerInside==1 && fingerWasInside==0 )
		{
			isDrawing=true;
			advanceTrial();
			trialMode = HANDONSTARTMODE;
		}

		// Condizione di fine stimolo, rende lo schermo nero
		if ( trialMode==STIMULUSMODE )
		{
			drawingTrialFrame++;
			
			if (trialTimer.getElapsedTimeInMilliSec() > util::str2num<double>(parameters.find("IntervalTime")) )
			{
				trialMode=HANDONSTARTMODE;
				isDrawing=false;
				boost::thread trialBeepThread( beepOk );
			}
		}

		invisibleThumbFrames+=!allVisibleThumb;
		invisibleIndexFrames+=!allVisibleIndex;

        markersFile << fixed << setprecision(3) <<
                    parameters.find("SubjectName") << "\t" <<
                    fingerDistance << "\t" <<
                    totalTrialNumber << "\t" <<
                    trialFrame << "\t" <<
                    globalTimer.getElapsedTimeInMilliSec() << "\t" <<
                    factors.getCurrent().at("Distances") << "\t" <<
					factors.getCurrent().at("CylinderOrientation") << "\t" <<
					factors.getCurrent().at("CylinderHeight") << "\t" <<
					factors.getCurrent().at("Selection") << "\t" <<
					cylinder.getZAxis()*2 << "\t" << 
                    ( allVisiblePatch ? eyeLeft.transpose() : junk ) << "\t" <<
                    ( allVisiblePatch ? eyeRight.transpose() : junk ) << "\t" <<
                    ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                    ( allVisibleThumb ? thumb.transpose() : junk ) << "\t" <<
                    ( allVisibleIndex ? index.transpose() : junk ) << "\t" <<
                    isDrawing << "\t" <<
                    thumbProjectedInside << "\t" <<
                    indexProjectedInside << "\t" <<
                    fingerInside << "\t" <<
					invisibleThumbFrames << "\t" << 
					invisibleIndexFrames << "\t" << 
					endl;
		}
		//isDrawing = (trialMode == STIMULUSMODE);
}

void drawStimulus()
{
	if ( !isDrawing )
		return;

    // Draw index and thumb
	if ( parameters.find("DrawingFinger")=="None"  )
	{
		glColor3fv(glBlack);
	} else
		glColor3fv(glRed);
	glBegin(GL_POINTS);
	if ( parameters.find("DrawingFinger")=="Thumb"  )
	{
		glVertex3dv(thumb.data());
	}
	if ( parameters.find("DrawingFinger")=="Index"  )
	{
		glVertex3dv(index.data());
	}
	if ( parameters.find("DrawingFinger")=="BothIndexThumb"  )
	{
		glVertex3dv(thumb.data());
		glVertex3dv(index.data());
	}
	glEnd();
	
    double theta = factors.getCurrent().at("CylinderOrientation") + 90;

    glPushMatrix();
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,factors.getCurrent().at("Distances"));
    glRotated(theta,0,1,0);
    if (debugRotated)
		glRotated(90,1,0,0);
    stimDrawer.draw();
    glPopMatrix();
	
    // Draw inner black cylinder, very complicate transformation!
    double cylHeight = cylinder.getHeight();
    double xAxis = cylinder.getXAxis();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,factors.getCurrent().at("Distances"));
    glColor3fv(glBlack);

    GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad,GLU_FILL);

	glRotated(90,1,0,0);
	glTranslated(0,0,-cylHeight/2);
	glRotated(-theta,0,0,1);
	glScaled(1,cylinder.getZAxis()/cylinder.getXAxis(),1);
	glScaled(0.99,0.99,0.99);
	gluCylinder(quad,xAxis,xAxis,cylHeight,40,2);
	
    gluDeleteQuadric(quad);
    glPopMatrix();
	
}

/**
 * @brief drawTrial
 */
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
		drawStimulus();
    }
    break;
    }
}

/**
 * @brief generateStimulus
 * @param xAxis
 * @param zAxis
 * @param height
 */
void generateStimulus(double xAxis,double zAxis, double height)
{
	double stimulusDensity = util::str2num<double>(parameters.find("StimulusDensity"));
    int nStimulusPoints=(int)(stimulusDensity*height*mathcommon::ellipseCircumferenceBetter(xAxis,zAxis));

    cylinder.setNpoints(nStimulusPoints);
    cylinder.setAperture(0,2*M_PI);
    cylinder.setAxesAndHeight(xAxis,zAxis,height);
    cylinder.setFluffiness(0.001);
    cylinder.compute();

    // Update the stimulus drawer
    stimDrawer.setSpheres(false);

    stimDrawer.setStimulus(&cylinder);
    stimDrawer.initList(&cylinder,glRed,3);
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
        exit(0);
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
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("Shape Constancy Experiment");
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

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 'Q':
    case 27:
    {   exit(0);
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
	case 'h':
	{
		debugRotated=!debugRotated;
		break;
	}
    case 'i':
    case 'I':
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
        if ( (fingerCalibrationDone==1) && allVisibleFingers )
        {   thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            // XXX sposto l'oggetto a home per permettere le dita di calibrarsi
            //RoveretoMotorFunctions::homeObject(3500);
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {
            infoDraw=false;

            paintGL();
            physicalRigidBodyTip = indexCoords.getP1().p;
            fingerCalibrationDone=3;
            trialMode = HANDONSTARTMODE;
            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
			RoveretoMotorFunctions::homeObject(3500);
            beepTrial();
            trialFrame=0;
            drawingTrialFrame=0;
			
			trialTimer.start();
            globalTimer.start();
			isDrawing=true;
            break;
        }
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

/**
* @brief initProjectionScreen
* @param _focalDist The focal distance to use
*/
void initProjectionScreen(double _focalDist)
{
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,alignmentX,alignmentY,_focalDist));
    RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);
}

/**
 * @brief initializeExperiment Set all the variables for the experiment, streams, parameter files and factors list
 */
void initializeExperiment()
{
    ifstream inputParameters(parametersFileNames[paramIndex]);
    parameters.loadParameterFile(inputParameters);

	interoculardistance = util::str2num<double>(parameters.find("IOD"));

    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string markersFileName  = baseDir + "markersFile_" + subjectName + ".txt";

    // Check for output file existence
    if ( !util::fileExists((markersFileName)) )
        markersFile.open(( markersFileName ).c_str());
    cerr << "File " << markersFileName << " loaded successfully" << endl;

	heightTheta2AverageAdjust = computeAverages(readFactorsAdjustmentZ( baseDir + "medians_" + subjectName + ".txt" ));

    markersFile << fixed << setprecision(3) <<
                "subjName\tfingerDist\ttrialN\tframeN\ttime\tAbsDepth\ttheta\theight\tcond\tadjZ\teyeLX\teyeLY\teyeLZ\teyeRX\teyeLY\teyeLZ\twristX\twristY\twristZ\tthumbX\tthumbY\tthumbZ\tindexX\tindexY\tindexZ\tIsDrawing\tIsThumbProjected\tIsIndexProjected\tFingersLeaning\tinvisibleThumbFrames\tinvisibleIndexFrames" << endl;

	// Initialize the factors list
    factors.init(parameters);
	//factors.print(cerr); // to see debug information
    factors.next();

    // Generate and initialize the stimulus with correct dimensions
	double cylHeight = factors.getCurrent().at("CylinderHeight");
	double cylXAxis = cylHeight * util::str2num<double>(parameters.find("CylinderXAxisProportionToHeight"));
	double cylZAxis=0.0;
	if ( factors.getCurrent().at("Selection")==1 )
	{
		vector<double> key;
		key.push_back(factors.getCurrent().at("CylinderHeight"));
		key.push_back(factors.getCurrent().at("CylinderOrientation"));
		key.push_back(factors.getCurrent().at("Distances"));
		cylZAxis = heightTheta2AverageAdjust[key];
	}
	else
		cylZAxis = cylHeight;

	generateStimulus(cylXAxis,cylZAxis,cylHeight);

    // Generate the correct focal distance
    initProjectionScreen(factors.getCurrent().at("Distances"));
    // Choose the finger to display
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
