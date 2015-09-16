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
//f
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
#include <math.h>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __linux__
#include "beep.h"
#endif
#include "IncludeGL.h"

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Optotrak2.h"
#include "Marker.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "EulerExtractor.h"
#include "ObjLoader.h"
#include "CalibrationHelper.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "BoundChecker.h"
#include "ParametersLoader.h"
#include "Util.h"
#include "TrialGenerator.h"
#include "LatestCalibrationTrieste.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
//using namespace boost::filesystem;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam,camPassive;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;

static const double obs_dist_from_origin= 100;
static const double focalDistance= baseFocalDistance-200.0; // la distanza totale media è baseFocalDistance-100.0 - obs_dist_from_origin; -100 è la distanza schermo
static const Vector3d center(0,0,focalDistance);
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),center );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight,projPointEyeRight, eyeCalibration;
vector <Marker> markers;
static double interoculardistance=65;
///// AGGIUNTE
double x_dim[2]; //// era static
int num_dots= 100;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* Timing variables  ************************/
// Timing variables
Timer rythmMaker;
Timer stimulusDuration;
Timer frameTimer;
Timer responseTimer;
/********* RIGID BODIES VARIABLES *********/
EulerExtractor eulerAngles;

/*** Streams File ***/
ofstream markersFile;
ofstream responseFile;
ofstream anglesFile;
ofstream trajFile;
ofstream matrixFile;
ofstream stimFile;
ofstream velocityFile;
ofstream projPointFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane[2];
StimulusDrawer stimDrawer[2];
int answer=0;
int headCalibrationDone=0;
bool allVisibleHead=false;
bool passiveMode=false;

bool whichPlaneDrawUp=0; // 0 ruota il piano in basso, 1 in alto
Affine3d objectActiveTransformation[2];
Affine3d objectPassiveTransformation[2];
double alpha=0;
// Trial related things
int trialNumber=0;
ParametersLoader parameters;
TrialGenerator<double> trial;
map <string, double> factors;
pair<map <string, double>,ParStaircase*> factorStaircasePair;
bool conditionInside=true;
bool wasInside=true;
int sumOutside=0;
int stimulusFrames=0;
double stimulusTime=0;
int timeInsideCircle=0;
deque<bool> signs;
bool orthographicMode=false;
bool stimOutside=false;

double instantPlaneSlant=0;

void drawFixation();
void idle();

void rectangle(double width, double height, double distance)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT, GL_FILL);

    if (str2num<double>(parameters.find("OccluderDistance"))<0) {
        glColor4f (0.0, 1.0, 0.0, 0.5);
    }
    else {
        glColor4f (0.0, 1.0, 0.0, 1);
    }

    glBegin(GL_POLYGON);
    glVertex3f (-width/2, -height/2, distance);
    glVertex3f (width/2, -height/2, distance);
    glVertex3f (width/2, height/2, distance);
    glVertex3f (-width/2, height/2, distance);
    glEnd();
    glPopAttrib();
    glFlush ();

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_BLEND);
}


void drawCircle(double radius, double x, double y, double z, const GLfloat *color)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor3fv(color);
    glBegin(GL_LINE_LOOP);
    double deltatheta=toRadians(5);
    for (double i=0; i<2*M_PI; i+=deltatheta)
        glVertex3f( x+radius*cos(i),y+radius*sin(i),z);
    glEnd();
    glPopAttrib();
}


/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
    return;
}

void tweeter()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(840,440);
    return;
}

void woofer()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(240,440);
    return;
}

void beepBad()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(200,200);
    Beep(400,200);
    Beep(200,200);
}
void outsideBeep()
{
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(660,66);
    Beep(330,33);
    return;
}

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
static const int CALIBRATIONMODE=3;
int trialMode = FIXATIONMODE;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
    markersFile.close();
    anglesFile.close();
    trajFile.close();
    matrixFile.close();
    stimFile.close();
    // Stop the optotrak
    optotrak.stopCollection();
    for (int i=0; i<3; i++)
        Beep(880,220);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void drawInfo()
{
    glPushMatrix();
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    switch ( headCalibrationDone )
    {
    case 0:
    {   allVisibleHead = isVisible(markers[18].p) && isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) ;
        if ( allVisibleHead )
            glClearColor(0.0,1.0,0.0,1.0); //green light
        else
            glClearColor(1.0,0.0,0.0,1.0); //red light
    }
    break;
    case 1:
    case 2:
        //case 3:
    {
        glPrintText(10, SCREEN_HEIGHT-40,SCREEN_WIDTH,SCREEN_HEIGHT, "EL  " + stringify<int>(eyeLeft.x() ) + " " + stringify<int>(eyeLeft.y() ) + " " + stringify<int>(eyeLeft.z()) );
        glPrintText(10, SCREEN_HEIGHT-60,SCREEN_WIDTH,SCREEN_HEIGHT, "ER " + stringify<int>(eyeRight.x() ) + " " + stringify<int>(eyeRight.y() ) + " " + stringify<int>(eyeRight.z()) );
        glPrintText(10, SCREEN_HEIGHT-120,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
        glPrintText(10, SCREEN_HEIGHT-140,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
        glPrintText(10, SCREEN_HEIGHT-160,SCREEN_WIDTH,SCREEN_HEIGHT, "ROLL " + stringify<int>(toDegrees(eulerAngles.getRoll())));
        glPrintText(10, SCREEN_HEIGHT-180,SCREEN_WIDTH,SCREEN_HEIGHT, "Press SPACEBAR to calibrate again or ENTER to confirm calibration.");
        glPrintText(10, SCREEN_HEIGHT-200,SCREEN_WIDTH,SCREEN_HEIGHT, "Delta " + stringify<int>(eyeRight.z()- eyeCalibration.z()));
        Vector3d angles = headEyeCoords.getRigidStart().getFullTransformation().rotation().eulerAngles(0,1,2);

        glPrintText(10, SCREEN_HEIGHT-220,SCREEN_WIDTH,SCREEN_HEIGHT, "YAW " + stringify<int>(toDegrees(eulerAngles.getYaw())));
        glPrintText(10, SCREEN_HEIGHT-240,SCREEN_WIDTH,SCREEN_HEIGHT, "PITCH " + stringify<int>(toDegrees(eulerAngles.getPitch())));
        if ( !passiveMode )
            glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "Active");
        else
            glPrintText(10, SCREEN_HEIGHT-260,SCREEN_WIDTH,SCREEN_HEIGHT, "Passive");
        glPrintText(10, SCREEN_HEIGHT-280,SCREEN_WIDTH,SCREEN_HEIGHT, "OBJ " + stringify<int>(projPointEyeRight.x() ) + " " + stringify<int>(projPointEyeRight.y() ) + " " + stringify<int>(projPointEyeRight.z()) );
        glPrintText(10, SCREEN_HEIGHT-340,SCREEN_WIDTH,SCREEN_HEIGHT, "GlassesR" + stringify<int>(markers[18].p.x() ) + " " + stringify<int>(markers[18].p.y() ) + " " + stringify<int>(markers[18].p.z()) );
    }
    break;
    }

    /*
    glPrintText(10, SCREEN_HEIGHT-360,SCREEN_WIDTH,SCREEN_HEIGHT, "StimTime " + stringify<int>(stimulusDuration.getElapsedTimeInMilliSec() ) );
    	glPrintText(10, SCREEN_HEIGHT-380,SCREEN_WIDTH,SCREEN_HEIGHT, "RespTime " + stringify<int>(responseTimer.getElapsedTimeInMilliSec() ) );
    */

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopMatrix();
    // end if ( headCalibrationDone )
}


void drawRedDotsPlane()
{
    // Draw the first plane
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();
    glMultMatrixd(objectActiveTransformation[0].data());
    // Tilt 0 backprojection
    glScaled(1/sin(toRadians( -90-(factors.at("DeltaSlant")+factors.at("StillPlaneSlant")))),1,1);	//backprojection phase
    // mettere backprojection
    stimDrawer[0].draw();

    glPopMatrix();	// POP MATRIX

    // Draw the second plane
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();
    glMultMatrixd(objectActiveTransformation[1].data());
    glScaled(1/sin(toRadians( -90-factors.at("StillPlaneSlant"))),1,1);
    stimDrawer[1].draw();
    glPopMatrix();	// POP MATRIX
}

void drawProbe()
{
    glPushAttrib(GL_POINT_BIT);
    glPointSize(5);

    double circleRadius=2.5;
    glColor3fv(glRed);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glEnd();
    glPopAttrib();

}

void drawCalibration()
{
    glPushAttrib(GL_POINT_BIT | GL_COLOR_BUFFER_BIT );
    double circleRadius=5.0;
    glColor3fv(glRed);
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3d(0,0,focalDistance);
    glVertex3dv(projPointEyeRight.data());
    glEnd();
    // Draw the calibration circle
    // if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius && abs(eyeRight.z()) < str2num<double>(parameters.find("MaxZOscillation")) )
    if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius && ((eyeRight.z()) < obs_dist_from_origin + str2num<double>(parameters.find("MaxZOscillation")) && (eyeRight.z()) > obs_dist_from_origin - str2num<double>(parameters.find("MaxZOscillation"))) )
    {   timeInsideCircle++;

        drawCircle(circleRadius,0,0,focalDistance,glGreen50);
        if ( timeInsideCircle > 20 )
        {
            trialMode=FIXATIONMODE;
            boost::thread okBeep( beepOk );
            okBeep.detach();
            sumOutside=0;
            rythmMaker.start();	// XXX
            signs.front()=false;	// XXX
            signs.back()=true;	// XXX
            timeInsideCircle=0;
        }
    }
    else
    {
        drawCircle(circleRadius,0,0,focalDistance,glRed);
    }
    glPopAttrib();
}

void drawFixation()
{
    switch ( headCalibrationDone )
    {
    case 1:
        // Fixed stimulus at (0,0,focalDistance)
        glPushAttrib(GL_POINT_BIT);
        glColor3fv(glRed);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glEnd();
        glPopAttrib();
        break;
    case 2:
        // Fixed stimulus + projected points
        glPushAttrib(GL_POINT_BIT | GL_COLOR_BUFFER_BIT );
        glColor3fv(glWhite);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glRed);
        glVertex3dv(projPointEyeRight.data());
        glColor3fv(glBlue);
        glVertex3d(eyeRight.x(),eyeRight.y(),focalDistance);
        glEnd();
        // Draw the calibration circle
        //if (abs(eyeRight.z()) < str2num<double>(parameters.find("MaxZOscillation")))
        if ((eyeRight.z()) < obs_dist_from_origin + str2num<double>(parameters.find("MaxZOscillation")) && (eyeRight.z()) > obs_dist_from_origin - str2num<double>(parameters.find("MaxZOscillation")))
            drawCircle(3,0,0,focalDistance,glWhite);
        else
            drawCircle(3,0,0,focalDistance,glRed);
        glPopAttrib();
        break;

    case 3:
        // Fixation point must be anchored to the orthogonal projection of eye right x coordinate
        glPushAttrib(GL_COLOR_BUFFER_BIT);	//Begin glPushAttrib colors
        const GLfloat *color;
        double circleRadius=str2num<double>(parameters.find("MaxCircleRadius"));	// millimeters
        //if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius && abs(eyeRight.z()) < str2num<double>(parameters.find("MaxZOscillation")) )
        if ( pow(projPointEyeRight.x(),2)+pow(projPointEyeRight.y(),2) <= circleRadius*circleRadius && ((eyeRight.z()) < obs_dist_from_origin + str2num<double>(parameters.find("MaxZOscillation")) && (eyeRight.z()) > obs_dist_from_origin - str2num<double>(parameters.find("MaxZOscillation"))))
            color = glGreen50;
        else
            color = glRed;

        drawCircle(5,0,0,focalDistance,color);
        glColor3fv(color);
        glPushMatrix();
        glTranslated(projPointEyeRight.x(),projPointEyeRight.y(),projPointEyeRight.z());
        glutSolidSphere(1,10,10);
        glPopMatrix();
        glPopAttrib();	// end glPopAttrib colors
        break;
    }
}

void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {
        drawFixation();
    }
    break;
    case PROBEMODE :
    {
        //drawProbe();
    }
    break;
    case STIMULUSMODE:
    {

        responseTimer.start();
        drawRedDotsPlane();
        ////// Temporaneo XXX /////
        if (str2num<int>(parameters.find("Occluder"))) {
            if (str2num<double>(parameters.find("OccluderDistance"))>0) {
                // if ((int) factors.at("Occluder")) /// if factor scommenta
                rectangle(str2num<double>(parameters.find("OccluderWidth")),str2num<double>(parameters.find("OccluderHeight")),focalDistance + str2num<double>(parameters.find("OccluderDistance")));
            } /// Added: Draw an occluding rectangle infront of the twsited structure
            ///// Riscala la grandezza del rettangolo i n maniera che la sua grandezza retinica sia uguale a quando funge da occlusore
            else {
                rectangle(str2num<double>(parameters.find("OccluderWidth"))*((fabs(focalDistance - str2num<double>(parameters.find("OccluderDistance"))) + 2*fabs(str2num<double>(parameters.find("OccluderDistance"))))/fabs(focalDistance - str2num<double>(parameters.find("OccluderDistance")))),str2num<double>(parameters.find("OccluderHeight"))*((fabs(focalDistance - str2num<double>(parameters.find("OccluderDistance"))) + 2*fabs(str2num<double>(parameters.find("OccluderDistance"))))/fabs(focalDistance - str2num<double>(parameters.find("OccluderDistance")))),focalDistance + str2num<double>(parameters.find("OccluderDistance")));
            } /// Added: Draw an occluding rectangle infront of the twsited structure

        }
    }
    break;
    case CALIBRATIONMODE:
    {
        drawCalibration();
    }
    break;
    default:
    {
        drawFixation();
    }
    }
}

void initVariables()
{
    interoculardistance = str2num<double>(parameters.find("IOD"));
    // Create the factors-staircase object TrialGenerator
    trial.init(parameters);
    factorStaircasePair = trial.getCurrent();
    factors = factorStaircasePair.first;

    for (int i=0; i<2; i++)
    {
        x_dim[i]= str2num<double>(parameters.find("planeXdimension")) + mathcommon::unifRand(0.0,str2num<double>(parameters.find("randXdimension")));
        num_dots= int(floor((x_dim[i]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
        redDotsPlane[i].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane[i].setDimensions(x_dim[i],str2num<double>(parameters.find("planeYdimension")),0.01);
        redDotsPlane[i].compute();
        stimDrawer[i].setStimulus(&redDotsPlane[i]);
        stimDrawer[i].setSpheres(true);
        stimDrawer[i].setRadius(0.5);
        stimDrawer[i].initList(&redDotsPlane[i]);
    }

    /** Bound check things **/
    signs.push_back(false);
    signs.push_back(true);
    rythmMaker.start();
    stimulusDuration.start();
    responseTimer.start();



    if ( str2num<int>(parameters.find("RandomizePlaneY")))
        whichPlaneDrawUp = rand()%2;
    else
        whichPlaneDrawUp = 0;
    /*
    if ( str2num<int>(parameters.find("RandomizeOverallTilt")))
    	overallTilt= rand()%2;
    else
    	overallTilt = 1;
    */
}

void keyPressed()
{
    if ( trialNumber==0)
    {
        responseFile << "TrialNumber DeltaSlant StillPlaneSlant PlanesCentersYDistance Onset WhichPlaneUp  StairID StairState StairInversion StairAscending EyeCalx EyeCaly EyeCalz StimFrames StimDuration ResponseTime Answer OverallTilt" << endl;
    }
    responseFile << fixed <<
                 trialNumber << " " <<
                 factors.at("DeltaSlant") << " " <<
                 factors.at("StillPlaneSlant") << " " <<
                 factors.at("PlanesCentersYDistance") << " " <<
                 factors.at("Onset") << " " <<
                 whichPlaneDrawUp << " " <<
                 factorStaircasePair.second->getCurrentStaircase()->getID() << " " <<
                 factorStaircasePair.second->getCurrentStaircase()->getState() << " " <<
                 factorStaircasePair.second->getCurrentStaircase()->getInversions() << " " <<
                 factorStaircasePair.second->getCurrentStaircase()->getAscending() << " " <<
                 eyeCalibration.transpose() << " " <<
                 stimulusFrames << " " <<
                 stimulusTime << " " <<
                 responseTimer.getElapsedTimeInMilliSec() << " " <<
                 answer << " " <<
                 factors.at("OverallTilt") << endl;
    // go to next trial things
    responseTimer.start();
    stimulusDuration.start();
    trial.next(answer);
    factorStaircasePair = trial.getCurrent();
    factors.clear();
    factors = factorStaircasePair.first;
    trialNumber++;

    trialMode++;
    trialMode=trialMode%4;

    for (int i=0; i<2; i++)
    {
        x_dim[i]= str2num<double>(parameters.find("planeXdimension")) + mathcommon::unifRand(0.0,str2num<double>(parameters.find("randXdimension")));
        num_dots= int(floor((x_dim[i]*str2num<double>(parameters.find("planeYdimension")))*(str2num<int>(parameters.find("numberdots"))/(str2num<double>(parameters.find("planeXdimension"))*str2num<double>(parameters.find("planeYdimension"))))));
        redDotsPlane[i].setNpoints(num_dots);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane[i].setDimensions(x_dim[i],str2num<double>(parameters.find("planeYdimension")),0.01);
        redDotsPlane[i].compute();
        /// stimDrawer[i].drawSpecialPoints(); % Disegna i punti speciali
        stimDrawer[i].initList(&redDotsPlane[i]);
    }
    signs.clear();
    signs.push_back(false);
    signs.push_back(true);
    idle();
    rythmMaker.start();
    boost::thread beepOkThread(beepOk);

    // cerr << "YEHHHH " << factorStaircasePair.second->getCurrentStaircase()->getState() << endl;

}

void initStreams()
{
    string parametersFileName = "C:/cncsvisiondata/parametersFiles/parametersActiveTwoPlanesOccRand.txt";
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
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);
    // Subject name
    string subjectName = parameters.find("SubjectName");

    // Principal streams file
    string responseFileName =	"responseFile_"	+ subjectName + ".txt";
    string markersFileName =	"markersFile_"	+ subjectName + ".txt";
    string anglesFileName =		"anglesFile_"	+ subjectName + ".txt";
    string trajFileName =		"trajFile_"		+ subjectName + ".txt";
    string matrixFileName =		"matrixFile_"	+ subjectName + ".txt";
    string stimFileName	  =		"stimFile_"		+ subjectName + ".txt";
    string velocityFileName = "velocityFile_"+subjectName +".txt";
    string projPointFileName = "projPointFile_" + subjectName + ".txt";

    // Check for output file existence
    /** Response file **/
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());

    if (!fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());

    if (!fileExists((baseDir+anglesFileName)) )
        anglesFile.open((baseDir+anglesFileName).c_str());

    if (!fileExists((baseDir+trajFileName)) )
        trajFile.open((baseDir+trajFileName).c_str());

    if ( !fileExists(baseDir+matrixFileName))
        matrixFile.open((baseDir+matrixFileName).c_str());

    if ( !fileExists(baseDir+stimFileName))
        stimFile.open((baseDir+stimFileName).c_str());

    if ( !fileExists(baseDir+velocityFileName))
        velocityFile.open((baseDir+velocityFileName).c_str());

    if ( !fileExists(baseDir+projPointFileName))
        projPointFile.open((baseDir+projPointFileName).c_str());
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
    {
        //glDrawBuffer(GL_BACK);
        // Draw left eye view
        //glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    {   //Quit program
    case 'o':
        orthographicMode=!orthographicMode;
        cam.setOrthoGraphicProjection(orthographicMode);
        break;
    case 'p':
        passiveMode=!passiveMode;
        break;
    case 'q':
    case 27:
    {
        if ( str2num<int>(parameters.find("DebugVersion")) == 1 )
        {
            cleanup();
            exit(0);
        }
        else
            beepBad();

    }
    break;
    case 'w':
    {
        whichPlaneDrawUp=!whichPlaneDrawUp;
    }
    break;
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            headEyeCoords.init( markers[18].p-Vector3d(ChinRestWidth,0,0),markers[18].p, markers[1].p,markers[2].p,markers[3].p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        // Second calibration, you must look a fixed fixation point
        if ( headCalibrationDone==1 )
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=2;
            break;
        }
        if ( headCalibrationDone==2 )
        {   headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            break;
        }
    }
    break;
    // Enter key: press to make the final calibration, only if the right eyez is in [-maxZoscillation, maxZoscillation]
    case 13:
    {
        //if ( headCalibrationDone == 2 && abs(eyeRight.z()) <= str2num<double>(parameters.find("MaxZOscillation")))
        if ( headCalibrationDone == 2 && ((eyeRight.z()) < obs_dist_from_origin + str2num<double>(parameters.find("MaxZOscillation")) && (eyeRight.z()) > obs_dist_from_origin - str2num<double>(parameters.find("MaxZOscillation"))))
        {
            headEyeCoords.init( headEyeCoords.getP1(),headEyeCoords.getP2(), markers[1].p, markers[2].p,markers[3].p,interoculardistance );
            eyeCalibration=headEyeCoords.getRightEye();
            headCalibrationDone=3;
        }
    }
    break;
    case '4':
    {
        if ( trialMode == PROBEMODE )
        {
            // sopra

            if (whichPlaneDrawUp==0)
                answer=true;
            else
                answer=false;

            /*if( (int) factors.at("OverallTilt") == 1 )
            	answer=false;
            else
            	answer=true;*/

            if ( str2num<int>(parameters.find("RandomizePlaneY")))
                whichPlaneDrawUp = rand()%2;
            else
                whichPlaneDrawUp = 0;
            keyPressed();
        }
    }
    break;
    case '6':
    {
        if ( trialMode == PROBEMODE )
        {
            // sotto

            if (whichPlaneDrawUp==0)
                answer=false;
            else
                answer=true;

            /*if( (int) factors.at("OverallTilt") == 1 )
            	answer=true;
            else
            	answer=false;*/

            if ( str2num<int>(parameters.find("RandomizePlaneY")))
                whichPlaneDrawUp = rand()%2;
            else
                whichPlaneDrawUp = 0;
            keyPressed();
        }
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

void checkStimulusBounds()
{
    // Check the if both the stimuli stay inside the visualization area for both active and passive conditions
    BoundChecker stimBoundariesActive0(&cam, &redDotsPlane[0]);
    BoundChecker stimBoundariesPassive0(&camPassive, &redDotsPlane[0]);
    BoundChecker stimBoundariesActive1(&cam, &redDotsPlane[1]);
    BoundChecker stimBoundariesPassive1(&camPassive, &redDotsPlane[1]);
    stimOutside = ( stimBoundariesActive0.checkOutside(objectActiveTransformation[0]) || stimBoundariesPassive0.checkOutside(objectActiveTransformation[0]))
                  ||  ( stimBoundariesActive1.checkOutside(objectActiveTransformation[0]) || stimBoundariesPassive1.checkOutside(objectActiveTransformation[0]));

}

void checkHeadBounds()
{
// The projected point stays inside a 5 cm radius circle
    bool conditionProjPointInsideCircle = (pow(projPointEyeRight.x(),2) + pow(projPointEyeRight.y(),2)) <= pow(str2num<double>(parameters.find("MaxCircleRadius")),2);

    double maxOscTime = str2num<double>(parameters.find("MaxOscillationTime"));
    double minOscTime = str2num<double>(parameters.find("MinOscillationTime"));
    double MaxXOscillation=str2num<double>(parameters.find("MaxXOscillation"));

    if ( (headCalibrationDone > 2) && (trialMode!=PROBEMODE) )
    {
        double val = (eyeRight.x());

        conditionInside = abs(val) <= (MaxXOscillation); //50 to avoid that 1.5 speed stimulus goes outside
        // If this condition is met, then this means that the point is outside the screen area!
        if ( !( conditionInside || (!wasInside) ) )
        {
            signs.pop_back();
            signs.push_front( val > 0 );
            rythmMaker.stop();
            if ( signs.front() != signs.back() )
            {   // se siam in stimulusMode bippa sempre quando vengon raggiunti i limiti
                if ( trialMode == STIMULUSMODE )
                {
                    boost::thread beepOkThread(beepOk);
                    beepOkThread.detach();
                    sumOutside++;
                }

                double timeElapsed = rythmMaker.getElapsedTimeInMilliSec();
                if ( (timeElapsed > minOscTime) && (timeElapsed < maxOscTime) && (trialMode!=STIMULUSMODE && (trialMode!=CALIBRATIONMODE)) )
                {
                    boost::thread beepOkThread(beepOk);
                    beepOkThread.detach();
                    double v = markers[3].v.x();
                    if ( (factors.at("Onset") *v < 0 ) )
                    {   // prima era eulerAngles.getYaw()
                        //if (( abs(projPointAngleX)< maxAllowedTranslationYaw )&& (sumOutside>0))
                        // Controlla che il punto projPointEyeRightX stia vicino al centro
                        if ( conditionProjPointInsideCircle && sumOutside>0 )
                            sumOutside++;
                        else
                            sumOutside=0;
                    }
                    else
                    {
                        //if ( abs(projPointAngleX)< maxAllowedTranslationYaw )
                        // Controlla che il punto projPointEyeRightX stia vicino al centro
                        if ( conditionProjPointInsideCircle )
                            sumOutside++;
                        else
                            sumOutside=0;
                    }
                }

                if ( (timeElapsed < minOscTime) && ( trialMode==FIXATIONMODE ) )
                {
                    boost::thread tweeterThread(tweeter);
                    tweeterThread.detach();
                    sumOutside=0;
                }
                if ( (timeElapsed > maxOscTime ) && ( trialMode==FIXATIONMODE ) )
                {
                    boost::thread wooferThread(woofer);
                    wooferThread.detach();
                    sumOutside=0;
                }
            }
            rythmMaker.start();
        }
        wasInside = conditionInside;
    }

    if ( (stimOutside) && (trialMode==STIMULUSMODE) )
    {
        outsideBeep();
        stimOutside=false;
        trialMode=CALIBRATIONMODE;
        sumOutside=0;
    }
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

Vector2d getPlaneDimensions(int i)
{
    Point3D *pUpCenter = redDotsPlane[i].specialPointsRand[5];
    Point3D *pLowCenter = redDotsPlane[i].specialPointsRand[6];

    Point3D *pRightCenter = redDotsPlane[i].specialPointsRand[7];
    Point3D *pLeftCenter = redDotsPlane[i].specialPointsRand[8];

    Vector3d vUpCenter = objectActiveTransformation[i]*Vector3d( pUpCenter->x, pUpCenter->y, pUpCenter->z);
    Vector3d vLowCenter = objectActiveTransformation[i]*Vector3d( pLowCenter->x, pLowCenter->y, pLowCenter->z);
    Vector3d vRightCenter = objectActiveTransformation[i]*Vector3d( pRightCenter->x, pRightCenter->y, pRightCenter->z);
    Vector3d vLeftCenter = objectActiveTransformation[i]*Vector3d( pLeftCenter->x, pLeftCenter->y, pLeftCenter->z);

    double junkz=0;
    double vUpCenterWinx,vUpCenterWiny, vLowCenterWinx,vLowCenterWiny, vRightCenterWinx,vRightCenterWiny, vLeftCenterWinx,vLeftCenterWiny;

    gluProject(vUpCenter.x(),vUpCenter.y(),vUpCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vUpCenterWinx,&vUpCenterWiny,&junkz);
    gluProject(vLowCenter.x(),vLowCenter.y(),vLowCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLowCenterWinx,&vLowCenterWiny,&junkz);

    gluProject(vRightCenter.x(),vRightCenter.y(),vRightCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vRightCenterWinx,&vRightCenterWiny,&junkz);
    gluProject(vLeftCenter.x(),vLeftCenter.y(),vLeftCenter.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &vLeftCenterWinx,&vLeftCenterWiny,&junkz);

    double planeHeight = abs( vUpCenterWiny - vLowCenterWiny);
    double planeWidth = abs( vLeftCenterWinx - vRightCenterWinx );

    return Vector2d(planeHeight,planeWidth);
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void writeContinuosFile()
{
    ofstream outputfile;
    outputfile.open("data.dat");
    outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
    outputfile << "Passive matrix:" << endl << objectPassiveTransformation[0].matrix() << endl;
    outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl;
    outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().transpose() << endl;
    outputfile << "EyeRight: " << headEyeCoords.getRightEye().transpose() << endl << endl;
    outputfile << "Rotating plane (0=down, 1=up): " << whichPlaneDrawUp << endl;
    outputfile << "(Width,Height) Plane0 [px]: " << getPlaneDimensions(0).transpose() << " " << endl;
    outputfile << "(Width,Height) Plane1 [px]: " << getPlaneDimensions(1).transpose() << " " << endl;
    outputfile << "Factors:" << endl;
    for (map<string,double>::iterator iter=factors.begin(); iter!=factors.end(); ++iter)
    {
        outputfile << "\t\t" << iter->first << "= " << iter->second << endl;
    }
    outputfile << "Trials done: " << trialNumber << endl;
    outputfile << "Last response: " << answer << endl;
    outputfile << "Sumoutside: " << sumOutside << endl;
    outputfile << "StimCenter: " << objectActiveTransformation[0].translation().transpose() << endl;
    outputfile << "Staircase ID" << factorStaircasePair.second->getCurrentStaircase()->getID() << " " << factorStaircasePair.second->getCurrentStaircase()->getState()<< " " << answer << endl;
    outputfile << "WhichPlaneDrawUp: " << whichPlaneDrawUp << endl;
    outputfile << "OverallTilt: " << factors.at("OverallTilt") << endl;
    outputfile << "Fhz: " << 1000/(stimulusTime/stimulusFrames) << endl;

    // now rewind the file
    outputfile.clear();
    outputfile.seekp(0,ios::beg);
}

void idle()
{
    if ( trial.isEmpty() )
    {
        cleanup();
        exit(0);
    }
    frameTimer.start();
    double optotime=0;
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    if (conditionInside && (sumOutside > str2num<int>(parameters.find("FixationCycles"))) && (trialMode ==FIXATIONMODE )  )
    {
        sumOutside=0;
        trialMode++;
        trialMode=trialMode%4;
        stimulusDuration.reset();
        stimulusDuration.start();
    }

    // Conta i cicli di presentazione dello stimolo
    if ( (sumOutside > str2num<int>(parameters.find("StimulusCycles")) ) &&  (trialMode == STIMULUSMODE) )
    {
        sumOutside=0;
        trialMode++;
        trialMode=trialMode%4;
        stimulusDuration.stop();
        stimulusTime = stimulusDuration.getElapsedTimeInMilliSec();
    }

    if ( trialMode == STIMULUSMODE )
        stimulusFrames++;

    if ( trialMode == FIXATIONMODE )
        stimulusFrames=0;

    Screen screenPassive;
    screenPassive.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screenPassive.setOffset(alignmentX,alignmentY);
    screenPassive.setFocalDistance(0);
    screenPassive.transform(headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(center));
    camPassive.init(screenPassive);
    camPassive.setDrySimulation(true);
    camPassive.setEye(eyeRight);
    objectPassiveTransformation[0] = ( camPassive.getModelViewMatrix()*objectActiveTransformation[0] );
    objectPassiveTransformation[1] = ( camPassive.getModelViewMatrix()*objectActiveTransformation[1] );
    // Coordinates picker
    if ( isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p) )
        headEyeCoords.update(markers[1].p,markers[2].p,markers[3].p);
    Affine3d active = headEyeCoords.getRigidStart().getFullTransformation();

    eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();

    // Projection of view normal on the focal plane
    Vector3d directionOfSight = (active.rotation()*Vector3d(0,0,-1)).normalized();
    Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( eyeRight , eyeRight+directionOfSight );

    double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);

    //double lenghtOnZ = (active*(center-eyeCalibration )+eyeRight).z();
    projPointEyeRight = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
    // second projection the fixation point computed with z non constant but perfectly parallel to projPointEyeRight
    lineOfSightRightDistanceToFocalPlane= (( active.rotation()*(center)) - eyeRight).norm();
    Vector3d secondProjection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (eyeRight);
    projPointEyeRight=secondProjection ;

    // XXX
    double alphaMultiplier = factorStaircasePair.second->getCurrentStaircase()->getState();
    if ( trialMode == STIMULUSMODE )
    {


        // IMPORTANT Reset the previous status of transformations
        objectActiveTransformation[0].setIdentity();
        objectActiveTransformation[1].setIdentity();
        // PLANE 0 Transformation QUELLO CHE STA SOTTO
        alpha = atan( eyeRight.x()/abs(projPointEyeRight.z()) );
        /*
        instantPlaneSlant = alphaMultiplier*alpha+(-(overallTilt*2-1))*toRadians(factors.at("DeltaSlant")+factors.at("StillPlaneSlant"));
        AngleAxis<double> aa0( instantPlaneSlant,Vector3d::UnitY());
        objectActiveTransformation[0]*=aa0;
        double planesYOffset = factors.at("PlanesCentersYDistance")*(whichPlaneDrawUp ? 1 : -1);
        objectActiveTransformation[0].translation() = Vector3d(0,planesYOffset,focalDistance);

        // PLANE 1 Transformation QUELLO CHE STA SOPRA
        AngleAxis<double> aa1((-(overallTilt*2-1))*toRadians(factors.at("StillPlaneSlant")),Vector3d::UnitY());
        */
        if ( (int)factors.at("OverallTilt") )
        {
            instantPlaneSlant = alphaMultiplier*alpha+toRadians(-factors.at("DeltaSlant")-factors.at("StillPlaneSlant"));
            AngleAxis<double> aa0( instantPlaneSlant,Vector3d::UnitY());
            objectActiveTransformation[0]*=aa0;
            double planesYOffset = factors.at("PlanesCentersYDistance")*(whichPlaneDrawUp ? 1 : -1);
            objectActiveTransformation[0].translation() = Vector3d(0,planesYOffset,focalDistance);

            // PLANE 1 Transformation QUELLO CHE STA SOPRA
            AngleAxis<double> aa1(-toRadians(factors.at("StillPlaneSlant")),Vector3d::UnitY());
            objectActiveTransformation[1]*=aa1;
            objectActiveTransformation[1].translation() = Vector3d(0,-planesYOffset,focalDistance);
        }
        else
        {
            instantPlaneSlant = alphaMultiplier*alpha+toRadians(factors.at("DeltaSlant")+factors.at("StillPlaneSlant"));
            AngleAxis<double> aa0( instantPlaneSlant,Vector3d::UnitY());
            objectActiveTransformation[0]*=aa0;
            double planesYOffset = factors.at("PlanesCentersYDistance")*(whichPlaneDrawUp ? 1 : -1);
            objectActiveTransformation[0].translation() = Vector3d(0,planesYOffset,focalDistance);

            // PLANE 1 Transformation QUELLO CHE STA SOPRA
            AngleAxis<double> aa1(toRadians(factors.at("StillPlaneSlant")),Vector3d::UnitY());
            objectActiveTransformation[1]*=aa1;
            objectActiveTransformation[1].translation() = Vector3d(0,-planesYOffset,focalDistance);
        }
    }
    // end XXX
    if ( passiveMode )
        initProjectionScreen(0,headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance)));
    else
        initProjectionScreen(focalDistance,Affine3d::Identity());

    checkHeadBounds();

    /**** Save to file part ****/
    // Markers file save the used markers and time-depending experimental variable to a file
    // (Make sure that in passive experiment the list of variables has the same order)
    markersFile << fixed << trialNumber << " " << headCalibrationDone << " " << trialMode << " " ;
    markersFile <<markers[1].p.transpose() << " " << markers[2].p.transpose() << " " << markers[3].p.transpose() << " " << 0.0 << " " << 0.0 << " " << 0.0 << " " << markers[18].p.transpose() << " " ;

    markersFile <<	fixed <<
                factors.at("StillPlaneSlant") << " " <<
                factors.at("DeltaSlant") << " " <<
                factors.at("PlanesCentersYDistance") << " " <<
                alphaMultiplier << " " <<
                factors.at("Onset") << " " <<
                whichPlaneDrawUp << " " <<
                factorStaircasePair.second->getCurrentStaircase()->getAscending() << " " <<
                (int)factors.at("OverallTilt") << " " << // se overalltilt=0 prende i valori di tilt dal file parametri altrimenti li inverte
                x_dim[0] << " " <<
                x_dim[1] <<
                endl;
    // Scrittura su data file da leggere in tempo reale
    // write data.dat file
    writeContinuosFile();

    // Write down frame by frame the trajectories and angles of eyes and head
    if ( trialMode == STIMULUSMODE && headCalibrationDone > 2 )
    {
        velocityFile << fixed << objectActiveTransformation[0].translation().transpose() << endl;

        velocityFile << frameTimer.getElapsedTimeInMilliSec() << " " << optotime*1000 << endl;
        trajFile << setw(6) << left <<
                 trialNumber << " " <<
                 stimulusFrames << " " <<
                 eyeRight.transpose() << endl;

        anglesFile << setw(6) << left <<
                   trialNumber << " " <<
                   stimulusFrames << " " <<
                   toDegrees(eulerAngles.getPitch()) << " " <<
                   toDegrees(eulerAngles.getRoll()) << " " <<
                   toDegrees(eulerAngles.getYaw()) << " " <<
                   toDegrees(instantPlaneSlant) << " " <<
                   endl;

        matrixFile << setw(6) << left <<
                   trialNumber << " " <<
                   stimulusFrames << " " ;
        for (int i=0; i<3; i++)
            matrixFile << objectPassiveTransformation[0].matrix().row(i) << " " ;
        matrixFile << endl;

        // Write the 13 special extremal points on stimFile
        stimFile << setw(6) << left <<
                 trialNumber << " " <<
                 stimulusFrames << " " ;
        double winx=0,winy=0,winz=0;

        for (PointsRandIterator iRand = redDotsPlane[0].specialPointsRand.begin(); iRand!=redDotsPlane[0].specialPointsRand.end(); ++iRand)
        {   Point3D *p=(*iRand);
            Vector3d v = objectActiveTransformation[0]*Vector3d( p->x, p->y, p->z);

            gluProject(v.x(),v.y(),v.z(), (&cam)->getModelViewMatrix().data(), (&cam)->getProjectiveMatrix().data(), (&cam)->getViewport().data(), &winx,&winy,&winz);
            stimFile << winx << " " << winy << " " << winz << " ";
        }
        stimFile << endl;

        projPointFile << fixed << trialNumber << "\t" << projPointEyeRight.transpose() << endl;
    }
}

void initOptotrak()
{
    optotrak.setTranslation(frameOrigin);
    optotrak.init(LastAlignedFile,TS_N_MARKERS,TS_FRAMERATE,TS_MARKER_FREQ,TS_DUTY_CYCLE,TS_VOLTAGE);

    for (int i=0; i<10; i++)
    {
        optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
}

int main(int argc, char*argv[])
{
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
        glutCreateWindow("EXP WEXLER");
        glutFullScreen();
    }
    else
    {   glutGameModeString(TS_GAME_MODE_STRING);
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
