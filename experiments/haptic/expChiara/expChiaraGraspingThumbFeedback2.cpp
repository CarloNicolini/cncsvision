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

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

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

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace Eigen;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords,thumbCoordsReal, thumbCoordsVisual, indexCoords;
RigidBody rigidStart,rigidCurrent;
EulerExtractor eulerAngles;

double focalDistance= -418.5, homeFocalDistance=-418.5;
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
int totalTrialNumber=0;

ParametersLoader parameters;
BalanceFactor<double> balanceFactor[5];
vector<int> blocks;
int blocksTrialNumber[5]= {0,0,0,0,0};
int blockIndex=0;
bool isDrawing=false,trialOk=false;
int trialFrame=0,totalFrame=0,drawingTrialFrame=0,fingerMode=2;
Timer globalTimer,delayedTimer,totalTime, frameTimer;
double fingerDistance=0;
double drawStimTime=2000,delayTime=500;
int occludedFrames=0;


/********* STIMULI *******/
EllipsoidPointsStimulus ellipseBaseCylinder;
double cylWidth,cylHeight,cylDepth;
StimulusDrawer stimDrawer;
double stimulusWidth=0;	//width of the stimulus, to be set at each new trial...
double stimulusDensity=0.025;	// points per mm^2, it means 150 points on a h=70, radius=15 cylinder area
double ballRadius=10;
double thumbSphereRadius=6;

bool isMovingRod=false;
/******** STREAMS AND FILES *******/
ofstream markersFile;
ofstream errorLog("C:\cncsvisiondata\errorlog\errors_log.txt");
/******** FUNCTION PROTOTYPES ******/
void beepOk();
void beepBad();
void beepTrial();
//void advanceTrial();
void initVariables();
void initStreams();
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
void initOptotrak();
void initRendering();
void updateMarkers();
void updateTrialStimulus();

char *blockNames[] = {"NF","VF","HF","FF","NF2"};

int paramIndex=0;
char *parametersFileNames[]=
{
	"C:/cncsvisiondata/parametersFiles/Chiara/experimentThumbFeedback/parametersExpChiaraGraspingThumbFeedback.txt",
	"C:/cncsvisiondata/parametersFiles/Chiara/experimentThumbFeedback/parametersExpChiaraGraspingThumbFeedbackTraining.txt" 
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

void updateMarkers()
{
    double deltaT = (double)TIMER_MS;
    optotrak.updateMarkers(deltaT);
    markers = optotrak.getAllMarkers();

	headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,TIMER_MS);
    // update thumb coordinates
    thumbCoordsReal.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,TIMER_MS);
    thumbCoordsVisual.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,TIMER_MS);
    
	// Try to figure out the transformation of disk
	rigidCurrent.setRigidBody(markers.at(11).p,markers.at(12).p,markers.at(13).p);
	rigidStart.computeTransformation(rigidCurrent);
	// update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,TIMER_MS);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;
}

// Functions implementation
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
            RoveretoMotorFunctions::moveObject(finalPosition-markers.at(5).p,speed);
        }
    }
    else
        RoveretoMotorFunctions::moveObjectAbsolute(v,rodTipHome-calibration,speed);
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
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
		// Second calibration
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headCalibrationDone=3;
            beepOk();
        }
    }
    break;
    case 'i':
    {
        infoDraw=!infoDraw;
    }
    break;
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
			Vector3d centroid = platformThumb;
			rigidStart.setRigidBody(markers.at(11).p-centroid,markers.at(12).p-centroid,markers.at(13).p-centroid);

            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            beepOk();
			if (blocks.at(blockIndex)==0 || blocks.at(blockIndex)==1 || blocks.at(blockIndex)==4)
				RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(rodAway,rodTipHome-calibration,RODSPEED);
			else
				moveRod(visualRodCenter+Vector3d(0,0, balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth")/2 -13/2),RODSPEED);
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
    }
    break;
    case 13:    // handle the go-to-next-block
    {    
		break;
	}
	case '+':
		{
		fingerMode++;
		fingerMode=fingerMode%4;
		break;
		}
    }
}



void initOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init("C:/cncsvisiondata/cameraFiles/Aligned20110823.cam")!=0)
        errorLog << "ERROR in Optotrak initialization " << __FILE__ << " at line " << __LINE__ << endl;
}

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

void initVariables()
{
    // Initialize the parameters file
    parameters.loadParameterFile(parametersFileNames[paramIndex]);
    blocks = util::str2num<int>(parameters.find("BlocksSequence"),",");

    for ( int i=0; i<5; i++ )
    {
        balanceFactor[i].init(parameters);
        balanceFactor[i].next();
    }

    stimulusDensity = str2num<double>(parameters.find("StimulusDensity"));

    ballRadius = str2num<double>(parameters.find("BallRadius"));
	thumbSphereRadius = util::str2num<double>(parameters.find("ThumbSphereRadius"));
    drawStimTime = str2num<double>(parameters.find("StimulusDuration"));
	delayTime = str2num<double>(parameters.find("DelayTime"));
	hapticRodCenter = Vector3d(0,0,focalDistance);
	visualRodCenter << 0,0, balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances");
	indexInside[0]=indexInside[1]=false;

	stimulusWidth = mathcommon::unifRand(util::str2num<double>(parameters.find("StimWidthMin")),util::str2num<double>(parameters.find("StimWidthMax")));

	updateTrialStimulus();
}


void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( gameMode )
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    else
        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void initProjectionScreen(double _focalDist)
{
    //cerr << "::: Monitor moved to " << _focalDist << endl;
	RoveretoMotorFunctions::moveScreenAbsolute(_focalDist,homeFocalDistance,3500);	// xxx messa versione asincrona
	//cerr << "::: End monitor moved" << endl;
	screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    cam.init(screen);
}

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

		double trialFocalDistance = balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances");
		initProjectionScreen(trialFocalDistance);
		double relDepth= balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth");
		visualRodCenter.z() = trialFocalDistance;// il paletto sta nella parte anteriore dello stimolo
		generateStimulus(blocks.at(blockIndex),str2num<double>(parameters.find("StimulusHeight")),stimulusWidth, relDepth );

}

void checkBounds()
{
    if ( fingerCalibrationDone==3 && headCalibrationDone==3 )
    {
        Vector3d trueIndex = indexCoords.getP1().p;
        Vector3d trueThumb = thumbCoordsReal.getP1().p;
        double stimulusRodOffsetY=0;
        switch ( trialMode )
        {
        case STIMULUSMODE:
        {   // Check if the index tip stays inside the stimulus
            Vector2d indexOnPlane = Vector2d(trueIndex.x(),trueIndex.z());
            Vector2d hapticRodCenterOnPlane = Vector2d(hapticRodCenter.x(), hapticRodCenter.z()+10);
            double stimHeight = str2num<double>(parameters.find("StimulusHeight") );
            double stimRadius = 13;//str2num<double>(parameters.find("StimulusDiameter") );
            bool yInside = (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))<(stimHeight/2) && (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))> (-stimHeight/2) ;
            if ( ( (indexOnPlane - hapticRodCenterOnPlane).norm() <= stimRadius ) && yInside )
                indexInside[0]=true;
            else
                indexInside[0]=false;
        }
        break;
        case HANDONSTARTMODE: //handsonstart
        {   // Define a sphere around physicalRigidBodyTip in which the thumb must stay
            bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
			// We set occluded frames = 0 in this mode
			occludedFrames=0;
            if ( indexVisible )
            {   bool triggerCondition=((indexCoords.getP1().p  - physicalRigidBodyTip).norm() < ballRadius);
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
						delayedTimer.start();
						trialMode=STIMULUSMODE;
                    }
                }
            }
        }
        break;
        }
    }
}

void initStreams()
{
	char *parametersFileName = "C:/cncsvisiondata/parametersExpChiaraGraspingThumbFeedback.txt";
    ifstream inputParameters(parametersFileName);
    if ( !inputParameters.good() )
    {   errorLog << "Parameters file "<< parametersFileName <<" doesn't exist, press enter to exit" << endl;
		cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
		exit(0);
    }
    parameters.loadParameterFile(inputParameters);
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
		cerr << "!! " << markersFileName << " already exists" << endl;
	}

	markersFile.open(markersFileName.c_str());
	cerr << "Writing to markers file " << markersFileName << " OK" << endl;
	    // Write the headers for markers file
    markersFile << fixed << setprecision(3) <<
                "SubjectName\tFingerDist\tBlock\tTotalTrialNumber\tTrialNumber\tfRelDepth\tfDistances\tStimDiameter\tTrialFrame\tTotTime\tIndexInside\tIsDrawing\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw" << endl;
}


void idle()
{
    // call to Optotrak motor functions and updating of markers position by deep copy.
    updateMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(17).p) && isVisible(markers.at(18).p);


    checkBounds();
    // Update the old state of indexInside to new state of indexInside
    indexInside[1]=indexInside[0];

    // Controlla i frames occlusi, se il dito di test  fuori dalla sfera
    // ed ha superato una percentuale di tempo maggiore di
    // TestPercentOccludedFrames rispetto al tempo di presentazione dello
    // stimolo allora fa un beep
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && !indexInside[0] )
    {
        bool visibleFinger=isVisible(thumbCoordsReal.getP1().p) && isVisible(indexCoords.getP1().p);
        if ( util::str2num<int>(parameters.find("CheckWrist"))==1 )
            visibleFinger = visibleFinger && isVisible(markers.at(6).p);
        if ( !visibleFinger && trialMode==STIMULUSMODE )
        {
            occludedFrames++;
            if ( str2num<int>(parameters.find("AudioFeedback") ) )
                boost::thread invisibleBeep( beepInvisible);
        }
        trialOk=true;	// mentre disegna lo stimolo
    }

    // Meccanismo di trigger. L'uscita della mano dalla sfera, provoca l'avanzata del trial
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE )
    {
	    double trialTime = globalTimer.getElapsedTimeInMilliSec();
        if ( trialTime >= drawStimTime )
        {
			if (delayedTimer.getElapsedTimeInMilliSec() > delayTime )
			{
				beepOk();
				cerr << "Drawing trial frame" << drawingTrialFrame << " in " << trialTime << " [sec], means framerate= " << drawingTrialFrame/(trialTime*1E-3) << " [frames/second] " << endl;

				stimulusWidth = mathcommon::unifRand(util::str2num<double>(parameters.find("StimWidthMin")),util::str2num<double>(parameters.find("StimWidthMax")));
				cerr << "::: Setting new stimulus width for this trial= " << stimulusWidth << endl;
				// Here we make a new trial
				totalTrialNumber++;
				blocksTrialNumber[blocks.at(blockIndex)]++;
				// Reinsert trial if number of occluded frames is bigger in percentual than a given threshold, the wrong trial is reinsterted a maximum number of times given by parameter MaxReinsertedTimes
				if ( double(occludedFrames)/double(drawingTrialFrame)*100 > util::str2num<double> (parameters.find("MaxPercentOccludedFrames")))
				{
					map<std::string,double> factorsToReinsert = balanceFactor[blocks.at(blockIndex)].getCurrent();
					balanceFactor[blocks.at(blockIndex)].reinsert(factorsToReinsert);
				}
				int oldDistance = (int)(balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances"));
				bool hasOtherFactors = balanceFactor[blocks.at(blockIndex)].next();
				// Ricalcola dove sta e quanto largo è lo stimolo
				if ( hasOtherFactors )
				{
					// We are in the same block but with a new trial
					updateTrialStimulus();
					// only for blocks 2 and 3 we also have to move the rod away
					if ( blocks.at(blockIndex) == 2 || blocks.at(blockIndex) == 3 )
						moveRod(visualRodCenter+Vector3d(0,0,balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth")/2 -13/2),RODSPEED);
					else
						RoveretoMotorFunctions::moveObjectAbsolute(rodAway,rodTipHome-calibration,RODSPEED);
				// Put a pause if old distance and new distance are equal
				if (oldDistance == (int)(balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances")) && 
					(blocks.at(blockIndex)==0 || blocks.at(blockIndex)==1 || blocks.at(blockIndex)==4 ) )
				{
					Timer sleeper;sleeper.start();
					sleeper.sleep(2000);
				}
				beepOk(); // beep inizio trial
				}
				else // else we go to the next block
				{
					Beep(200,600); //beep lungo nuovo blocco
					// Modify the stimulus depth
					// Blocks and factors of the last block are finished, experiment is finished
					blockIndex++;//go to the next block
					if ( blockIndex >= blocks.size() )	// exceeded the total blocks
					{
						glutLeaveGameMode();
						exit(0);
					}
					updateTrialStimulus();
					map<std::string,double> factor = balanceFactor[blocks.at(blockIndex)].getCurrent();
					if ( blocks.at(blockIndex) == 2 || blocks.at(blockIndex) == 3 )
						moveRod(visualRodCenter+Vector3d(0,0, 
						balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth")/2 -13/2),RODSPEED);
					else
						RoveretoMotorFunctions::moveObjectAbsolute(rodAway,rodTipHome-calibration,RODSPEED);
					beepOk(); // beep inizio trial
				}

				globalTimer.start();
				trialMode=HANDONSTARTMODE;
				delayedTimer.start();
				trialFrame=drawingTrialFrame=occludedFrames=0;
				updateMarkers();	// elimina il problema che lascia la coordinata all'ultimo frame del trial appena fatto uguale alla prima coordinata del trial a venire
			}
        }
		else
		{
			delayedTimer.start();
		}
	}

    if ( !isMovingRod )
    {
        RowVector3d junk(9999,9999,9999);

        // Write to file
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {
			if ( globalTimer.getElapsedTimeInMilliSec() < drawStimTime && trialMode==STIMULUSMODE )
				isDrawing=true;
			else
				isDrawing=false;
		//"SubjectName\tFingerDist\tBlock\tTotalTrial\tTrialNumber\tfRelDepth\tfDistances\tStimWidth\tTrialFrame\tTotTime\tIndexInside\tIsDrawing\tHapticRodX\tHapticRodY\tHapticRodZ\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw"
			
            markersFile << fixed << setprecision(3) <<
                        parameters.find("SubjectName") << "\t" <<
                        fingerDistance << "\t" <<
						blockNames[blocks.at(blockIndex)] << "\t" <<
						totalTrialNumber << "\t" <<
						blocksTrialNumber[blocks.at(blockIndex)] << "\t" <<
						balanceFactor[blocks.at(blockIndex)].getCurrent().at("RelDepth") << "\t" <<
						balanceFactor[blocks.at(blockIndex)].getCurrent().at("Distances") << "\t" <<
						stimulusWidth << "\t" <<
                        trialFrame << "\t" <<
                        totalTime.getElapsedTimeInMilliSec() << "\t" <<
                        indexInside[0] << "\t" <<
			            isDrawing << "\t" <<
                        ( isVisible(markers.at(15).p) ? markers.at(15).p.transpose() : junk ) << "\t" <<
                        ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                        ( isVisible(eyeRight) ? eyeRight.transpose() : junk ) << "\t" <<
                        ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                        ( isVisible(thumbCoordsReal.getP1().p) ? thumbCoordsReal.getP1().p.transpose() : junk ) << "\t" <<
                        ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) <<
                        endl;
        }
        trialFrame++;
		if ( trialMode==STIMULUSMODE )
			drawingTrialFrame++;
    }
    frameTimer.start();
}


void drawInfo()
{   if ( infoDraw )
    {   GLText text;

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
			text.draw("IsDrawing= " + stringify<int>(isDrawing));
			
			text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
			text.draw("VisualRod= " + stringify<Eigen::Matrix<double,1,3> >(visualRodCenter.transpose()) );
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("ThumbReal= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsReal.getP1().p ));
			text.draw("ThumbVisual= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoordsVisual.getP1().p ));
			
        }
        text.leaveTextInputMode();
    }
}

// ::::::::: DRAW STIMULUS ::::::::::::
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

// :::::: DRAWFINGERTIPS ::::::::
void drawFingerTips()
{
	// Only show the visual feedback thumb (the offset version of real physical thumb
	if ( blocks.at(blockIndex) == 1 || blocks.at(blockIndex) == 3 )
	{
		Vector3d visualThumb = thumbCoordsVisual.getP1().p;
		
		switch ( fingerMode )
			{
		case 0: // Visual thumb
			{	glPushMatrix();
			glLoadIdentity();
			glTranslated(visualThumb.x(),visualThumb.y(), visualThumb.z());
			glColor3fv(glRed);
			glutWireSphere(thumbSphereRadius,5,20);
			glPopMatrix();
			break;
			}
		case 1: // REAL THUMB
			{
			Vector3d realThumb = thumbCoordsReal.getP1().p;
			glPushMatrix();
			glLoadIdentity();
			glTranslated(realThumb.x(),realThumb.y(), realThumb.z());
			glColor3fv(glRed);
			glutWireSphere(thumbSphereRadius,20,20);
			glPopMatrix();
			break;
			}
		case 2: // SUL POLPASTRELLO
			{
			glPushMatrix();
			glMultMatrixd(rigidStart.getFullTransformation().data());
			glColor3fv(glRed);
			glScaled(1,1,0.001);
			glutSolidSphere(thumbSphereRadius,15,15);
			glPopMatrix();
			break;
			}
		case 3: // ALIGNEDPHALANX CYLINDER
			{
			GLUquadricObj* quad = gluNewQuadric();
			glPushMatrix();
			glMultMatrixd(rigidStart.getFullTransformation().data());
			glColor3fv(glRed);
			glScaled(1,1,0.001);
			gluCylinder(quad,4,4,10,15,20); 
			glPopMatrix();
			gluDeleteQuadric(quad);
			break;
			}
			}
	}
}

// :::::::::::: DRAW TRIAL ::::::::::::
void drawTrial()
{
	if (!isDrawing)
		return;
	switch ( trialMode )
    {
        // disegna le dita per debug
    case HANDONSTARTMODE :
    {
        //drawFingerTips();
    }
    break;
    case STIMULUSMODE:
    {   // Disegna le dita solo durante la fase di adattamento
        drawFingerTips();
		drawStimulus();
    }
    break;
    }
}

// :::::: DRAWGLSCENE :::::::
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

std::string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

int main(int argc, char*argv[])
{
    cout << "Please select the parametersFileName" << endl;
	for (int i=0; i<2; i++)
		cout << i << ") " << parametersFileNames[i] << endl;
	cin >> paramIndex;
	cout << "Selected " << parametersFileNames[paramIndex] << endl;

    cout << "::: Current executable path= " <<  ExePath() << endl;
    // Randomize the seed of random number generators
    randomizeStart();
    // Move the mirrors at home
    RoveretoMotorFunctions::homeMirror(3500);
    // Move the monitor at home
    RoveretoMotorFunctions::homeScreen(3500);
    // Move the object at home
    RoveretoMotorFunctions::homeObject(RODSPEED);
    // Move the object in starting position
    RoveretoMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,RODSPEED);

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH,SCREEN_HEIGHT);
        glutCreateWindow("EXP CHIARA");
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    initVariables();    // must stay here because they explicitly contain call to gl functions
    initRendering();
    initStreams();
	
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
