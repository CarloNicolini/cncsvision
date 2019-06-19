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

#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>


#include "IncludeGL.h"
#ifdef __linux__
#include "beep.h"
#endif

#include "Util.h"
#include "Mathcommon.h"
#include "Optotrak2.h"
#include "Marker.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
//#include "LatestCalibration.h"
#include "LatestCalibrationTrieste.h"
/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Screen screen;
Optotrak2 optotrak;
CoordinatesExtractor headEyeCoords;
Eigen::Affine3d objectActiveTransformation;
BoxNoiseStimulus stimulus;
StimulusDrawer stimDrawer;
// Trial related things
ParametersLoader parameters;
BalanceFactor<double> trial;
/********** EYES AND MARKERS **********************/
vector <Marker> markers;
bool allVisibleHead=false;
bool allVisiblePatch=false;
bool readyToStart=false;
bool infoDraw=true;
int headCalibrationDone=0;
Vector3d eyeCalibration(0.0,0.0,0.0);
static const int PRECALIBRATION_MODE=-1;
static const int BLACK_MODE=0;
static const int PROBE_MODE=1;
static const int STIMULUS_MODE=2;
static const int RESPONSE_MODE=3;
int trialMode=PRECALIBRATION_MODE;
int trialNumber=0;
Timer trialTimer,globalTimer;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;
bool useCircularOcclusor=false;
double focalDistance=-418.5;
double interocularDistance=65;

/****** OUTPUT FILES AND STREAMS *****/
ofstream responseFile;
ofstream markersFile;
ofstream timingFile;
RowVector3d junk(9999,9999,9999);
/********* TIMING AND STIMULUS VARIABLES ***********/

double counter=0.0;
double frame=0.0;
/***** SOUND THINGS *****/
void beepOk()
{
    Beep(440,440);
    return;
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

/**
 * @brief drawInfo
 */
void drawInfo()
{
    if (infoDraw==false)
        return;
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();

    switch ( headCalibrationDone )
    {
    case 0:
    {
        if ( allVisibleHead )
            text.draw("PRESS SPACEBAR TO CALIBRATE");
        else
            text.draw("BE VISIBLE...");
        break;
    }
    case 1:
    {
        break;
    }
    }
    text.draw("HeadCalibration="+util::stringify<int>(headCalibrationDone));
    text.draw("TrialMode=" + util::stringify<int>(trialMode));
    text.draw("ReadyToStart="+util::stringify<int>(readyToStart) );
    text.draw("RightEye="+util::stringify< Eigen::Matrix<int,1,3> >(headEyeCoords.getRightEye().transpose().cast<int>()) );
    /*
    if ( !trial.getCurrent().empty() )
    {
    	for ( map<std::string,std::string>::iterator iter = trial.getCurrentAsMapToString().begin(); iter!=trial.getCurrentAsMapToString().end();++iter )
    	{
    		text.draw(iter->first + "= " + iter->second);
    	}
    }
    */
    text.leaveTextInputMode();
}

/**
 * @brief initOptotrak
 */
void initOptotrak()
{
    optotrak.setTranslation(calibration);
    if ( optotrak.init(LastAlignedFile) != 0)
    {
        exit(0);
    }

    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
}

/**
 * @brief idle
 */
void idle()
{
    // Update markers
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    allVisiblePatch = isVisible(markers[1].p) && isVisible(markers[2].p) && isVisible(markers[3].p);
    allVisibleHead = isVisible(markers[18].p) && allVisiblePatch;
    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p);

    if ( trialTimer.getElapsedTimeInMilliSec() >= parameters.get("WaitTime") && trialMode==BLACK_MODE )
    {
        frame=0.0;
        trialMode++;
        trialTimer.start();
    }

    if ( trialTimer.getElapsedTimeInMilliSec() >= parameters.get("ProbeTime") && trialMode==PROBE_MODE )
    {
        frame=0.0;
        trialMode++;
        trialTimer.start();
    }

    if ( trialTimer.getElapsedTimeInMilliSec() >= parameters.get("StimulusTime") && trialMode==STIMULUS_MODE )
    {
        Beep(660,660);
        frame=0.0;
        trialMode=RESPONSE_MODE;
        trialTimer.start();
    }

    markersFile << fixed << trialNumber << " " << headCalibrationDone << " " << trialMode << " " ;
    markersFile << setprecision(3) <<
                ( isVisible(markers[1].p) ? markers[1].p.transpose() : junk ) << " " <<
                ( isVisible(markers[2].p) ? markers[2].p.transpose() : junk ) << " " <<
                ( isVisible(markers[3].p) ? markers[3].p.transpose() : junk ) << " " <<
                ( isVisible(markers[17].p) ? markers[17].p.transpose() : junk ) << " " <<
                ( isVisible(markers[18].p) ? markers[18].p.transpose() : junk ) << " " <<
                ( isVisible(markers[18].p) ? markers[18].p.transpose() : junk ) << " " <<
                ( isVisible(headEyeCoords.getLeftEye()) ? headEyeCoords.getLeftEye().transpose() : junk ) << " " <<
                ( isVisible(headEyeCoords.getRightEye()) ? headEyeCoords.getRightEye().transpose() : junk ) << " " ;
    markersFile << setprecision(1) <<
                trial.getCurrent().at("ZWidth") << " " <<
                trial.getCurrent().at("Slant") << " " <<
                trial.getCurrent().at("Tilt") << " " <<
                trial.getCurrent().at("StimulusAnchored") << " " <<
                endl;
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
    char* argv[1];  //dummy argument list for glutinit()
    char dummyString[8];
    argv[0] = dummyString;  //hook up the pointer
    int argc = 1;
    glutInit(&argc, argv);

    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STEREO | GLUT_STENCIL );
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL );
    if (gameMode)
    {
        glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
    }
    else
    {
        glutInitWindowSize( SCREEN_WIDTH,SCREEN_HEIGHT);
        glutCreateWindow("Experiment Active Rigidity");
    }
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GLUT_STENCIL);

    glEnable(GL_STENCIL_TEST);
    // Increase quality of points
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief updateStimulus
 */
void updateStimulus()
{
    double stimSize = parameters.get("StimulusSize");
    double stimPeriod = parameters.get("StimulusPeriod");
    double zWidth = trial.getCurrent().at("ZWidth");
    int nStimulusPoints = (int)(stimSize*stimSize*parameters.get("StimulusDensity"));
    // Generate the plane stimulus
    if (parameters.find("StimulusType")=="Plane")
    {
        stimulus.setDimensions(stimSize,stimSize,0.001);
        stimulus.setNpoints(nStimulusPoints);
        stimulus.setFluffiness(0.001);
        stimulus.compute();
    }
    else if (parameters.find("StimulusType")=="Sinusoid")
    {
        stimulus.setDimensions(stimSize,stimSize,0.001);
        stimulus.setNpoints(nStimulusPoints);
        stimulus.setFluffiness(0.001);
        for (PointsRandIterator iter = stimulus.pointsRand.begin(); iter!=stimulus.pointsRand.end(); ++iter)
        {
            (*iter)->x = mathcommon::unifRand(-stimSize/2,stimSize/2);
            (*iter)->y = mathcommon::unifRand(-stimSize/2,stimSize/2);
            (*iter)->z = 0.5*zWidth*sin(((*iter)->x)/stimPeriod*M_PI);
        }
    }
    else
    {
        throw std::runtime_error("Wrong argument! Must be Sinusoid or Plane");
    }

    // Generate the front stimulus
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&stimulus);
    stimDrawer.initList(&stimulus,glRed,parameters.get("PointsSize"));
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    // MUST BE CALLED WITHIN A VALID OPENGL CONTEXT
    //Screen screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH, alignmentX, alignmentY, focalDistance );
    screen.init(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH, alignmentX, alignmentY, focalDistance );
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);

    // Initialize all the streams for file output eccetera
    parameters.loadParameterFile("C:/cncsvisioncmake/experimentstrieste/activevision/microheadmovements/parametersExpMicroHeadMovementsActive.txt");
    // Initialize focal distance and interocular distance from parameter file
    focalDistance = parameters.get("FocalDistance");
    interocularDistance = parameters.get("IOD");
    infoDraw = (int)parameters.get("DrawInfo");
    useCircularOcclusor = (int)parameters.get("UseCircularOcclusor");
    // Initialize trials balance factors
    trial.init(parameters);
    trial.next();
    // Base directory, full path
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file

    string markersFileName = baseDir + "markersFile_MicroHeadMovementsActive_" + subjectName  + ".txt";
    string outputFileName  = baseDir + "responseFile_MicroHeadMovementsActive_" + subjectName +".txt";
    string timingFileName  = baseDir + "timingFile_MicroHeadMovementsActive_" + subjectName   + ".txt";
    // Check for output file existence
    if ( !util::fileExists((outputFileName)) )
        responseFile.open(outputFileName.c_str());
    cerr << "File " << outputFileName << " loaded successfully" << endl;

    // Check for output markers file existence
    if ( !util::fileExists((markersFileName)) )
        markersFile.open(markersFileName.c_str());
    cerr << "File " << markersFileName << " loaded successfully" << endl;

    // Check for output timing file existence
    if ( !util::fileExists((timingFileName)) )
        timingFile.open(( timingFileName ).c_str());
    cerr << "File " << timingFileName << " opened successfully" << endl;

    // Write the response file header
    responseFile << "# SubjectName\tTrialNumber\tFocalDistance\tKeyPressed\tResponseTime\tfZWidth\tfSlant\tfTilt\tfStimAnchored" << endl;

    // Finished loading parameter files

    // Update the stimulus
    updateStimulus();

    trialTimer.start();
    globalTimer.start();
}

/**
 * @brief drawProbe
 */
void drawProbe()
{
    glColor3fv(glRed);
    glPushMatrix();     // PUSH MATRIX
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(2.5,10,10);
    glPopMatrix();
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    //////////////////////////////
    // Update stimuli
    double oscillationAmplitude=50.0;
    double oscillationPeriod=100.0;
    static double timeFrame=0.0;
    timeFrame+=1.0;
    // cerr << "EyeCoords: " << headEyeCoords.getLeftEye().transpose() << "\n"  << endl;
    if ( !trial.getCurrent().at("StimulusAnchored") )
    {
        // FISSO
        glPushMatrix();
        glLoadIdentity();
        Vector3d translationFactor(0,0,focalDistance);
        double instantPlaneSlant = trial.getCurrent().at("Slant");

        switch ( (int) trial.getCurrent().at("Tilt") )
        {
        case 0:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,0,1,0);
            glScaled(1/sin(toRadians( -90-trial.getCurrent().at("Slant"))),1,1);	//backprojection phase
            break;
        }
        case 90:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,1,0,0);
            glScaled(1,1/sin(toRadians( -90-trial.getCurrent().at("Slant") )),1); //backprojection phase
            break;
        }
        case 180:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,0,1,0);
            glScaled(1/sin(toRadians( -90-trial.getCurrent().at("Slant") )),1,1); //backprojection phase
            break;
        }
        case 270:
        {
            glTranslated( translationFactor.x(), translationFactor.y(), translationFactor.z());
            glRotated( -instantPlaneSlant ,1,0,0);
            glScaled(1,1/sin(toRadians( -90-trial.getCurrent().at("Slant") )),1); //backprojection phase
            break;
        }
        }
        glGetDoublev(GL_MODELVIEW_MATRIX,objectActiveTransformation.data());
        stimDrawer.draw();
        glPopMatrix();
    }
    else
    {
        // ANCORATO
        glPushMatrix();
        glLoadMatrixd( (headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance))).data() );
        glGetDoublev(GL_MODELVIEW_MATRIX,objectActiveTransformation.data());
        stimDrawer.draw();
        glPopMatrix();
    }

    // Draw the occlusor
    if ( useCircularOcclusor )
    {
        double maskRadius=parameters.get("CircularOcclusorRadius");
        glPushMatrix();
        glLoadIdentity();
        Vector3d projCoords;
        if ( trial.getCurrent().at("StimulusAnchored") )
            projCoords << project( (headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance))).translation() );
        else
            projCoords << project( Vector3d(0,0,focalDistance) );
        circleMask( projCoords.x(), projCoords.y(), maskRadius );
        glPopMatrix();
    }
    bool passiveMode=false;
    stimDrawer.projectStimulusPoints( objectActiveTransformation, headEyeCoords.getRigidStart().getFullTransformation(), cam, focalDistance, screen, eyeCalibration, passiveMode, true);
}

/**
 * @brief drawFixation
 */
void drawFixation()
{
    double circleRadius = parameters.get("MaxCircleRadius");	// millimeters
    double zBoundary    = parameters.get("MaxZOscillation"); // millimeters
    // Projection of view normal on the focal plane
    Vector3d directionOfSight = (headEyeCoords.getRigidStart().getFullTransformation().linear()*Vector3d(0,0,-1)).normalized();
    Eigen::ParametrizedLine<double,3> lineOfSightRight = Eigen::ParametrizedLine<double,3>::Through( headEyeCoords.getRightEye() , headEyeCoords.getRightEye()+directionOfSight );
    Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );
    double lineOfSightRightDistanceToFocalPlane = lineOfSightRight.intersection(focalPlane);
    Vector3d opticalAxisToFocalPlaneIntersection = lineOfSightRightDistanceToFocalPlane *(directionOfSight)+ (headEyeCoords.getRightEye());

    switch ( headCalibrationDone )
    {
    case 1:
    {
        // STIM_FIXED stimulus at (0,0,focalDistance)
        glPushAttrib(GL_POINT_BIT);
        glColor3fv(glRed);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3d(0,0,focalDistance);
        glVertex3d(headEyeCoords.getRightEye().x(),headEyeCoords.getRightEye().y(),focalDistance);
        glEnd();
        glPopAttrib();
        break;
    }
    case 2:
    {
        // STIM_FIXED stimulus + projected points
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glPointSize(5);
        glLineWidth(2);

        glBegin(GL_POINTS);
        glColor3fv(glRed);
        glVertex3d(0,0,focalDistance);
        glColor3fv(glBlue);
        glVertex3dv(opticalAxisToFocalPlaneIntersection.data());
        glColor3fv(glWhite);
        glVertex3d(headEyeCoords.getRightEye().x(),headEyeCoords.getRightEye().y(),focalDistance);
        glEnd();

        double r2EyeRight = pow(headEyeCoords.getRightEye().x(),2)+pow(headEyeCoords.getRightEye().y(),2);
        // Draw the calibration circle
        if ( pow(opticalAxisToFocalPlaneIntersection.x(),2)+pow(opticalAxisToFocalPlaneIntersection.y(),2) <= circleRadius*circleRadius && abs(headEyeCoords.getRightEye().z()) < zBoundary && r2EyeRight<circleRadius*circleRadius )
        {
            readyToStart=true;
            drawCircle(circleRadius,0,0,focalDistance,glGreen);
        }
        else
        {
            drawCircle(circleRadius,0,0,focalDistance,glRed);
        }
        glPopAttrib();
        break;
    }
    }
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch (trialMode)
    {
    case PRECALIBRATION_MODE:
    {
        drawFixation();
        break;
    }
    case BLACK_MODE:
    {
        break;
    }
    case PROBE_MODE:
    {
        drawProbe();
        break;
    }
    case STIMULUS_MODE:
    {
        drawStimulus();
        // GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        // text.enterTextInputMode();
        // text.setXY(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
        // text.draw(headEyeCoords.getLeftEye().transpose());
        // text.leaveTextInputMode();
        break;
    }
    case RESPONSE_MODE:
    {
        GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        text.setXY(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
        text.draw("PRESS 4 OR 6");
        text.leaveTextInputMode();
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
        cam.setEye(headEyeCoords.getRightEye());
        drawInfo();
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(headEyeCoords.getLeftEye());
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
        cam.setEye(headEyeCoords.getRightEye());
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
}

/**
 * @brief advanceTrial
 * @param response
 */
void advanceTrial(int response)
{
    responseFile <<
                 parameters.find("SubjectName") << "\t" <<
                 trialNumber << "\t" <<
                 focalDistance << "\t" <<
                 response << "\t" <<
                 trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                 trial.getCurrent().at("ZWidth") << "\t" <<
                 trial.getCurrent().at("Slant") << "\t" <<
                 trial.getCurrent().at("Tilt") << "\t" <<
                 trial.getCurrent().at("StimulusAnchored") << " " << endl;
    if ( trial.isEmpty() )
    {
        exit(0);
    }
    trial.next();

    updateStimulus();
    beepOk();
    trialMode=BLACK_MODE;
    trialNumber++;
    trialTimer.start();
    frame=0.0;
}

/**
 * @brief keyPressEvent
 * @param key
 * @param x
 * @param y
 */
void keyPressEvent(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
    case 'i':
    {
        infoDraw=!infoDraw;
        break;
    }
    case ' ':
    {
        // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead && trialMode==PRECALIBRATION_MODE)
        {
            headEyeCoords.init( markers[18].p-Vector3d(ChinRestWidth,0,0),markers[18].p, markers[1].p,markers[2].p,markers[3].p,interocularDistance );
            //headEyeCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        // Second calibration the user must be in the red circle
        if ( headCalibrationDone==1 && allVisiblePatch && trialMode==PRECALIBRATION_MODE)
        {
            headEyeCoords.init(headEyeCoords.getLeftEye(),headEyeCoords.getRightEye(), markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
            headCalibrationDone=2;
            beepOk();
            break;
        }
        break;
    }
    case 13:
    {
        // Third calibration the user must look toward the green/red circle and be in the correct z position, then the experiment starts
        if ( readyToStart && allVisiblePatch )
        {
            headEyeCoords.init(headEyeCoords.getLeftEye(),headEyeCoords.getRightEye(), markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
            eyeCalibration = headEyeCoords.getRightEye();
            headCalibrationDone=3;
            trialMode=BLACK_MODE;
            beepOk();
            trialTimer.start();
            break;
        }
    }
    case '4':
    {
        if (trialMode==RESPONSE_MODE)
        {
            advanceTrial(4);
        }
        break;
    }
    case '6':
    {
        if (trialMode==RESPONSE_MODE)
        {
            advanceTrial(6);
        }
        break;
    }
    }
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
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char*argv[])
{
    // Initialize the random seed
    randomizeStart();
    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();

    // Initialize the OpenGL context
    initializeGL();
    initializeExperiment();
    // Setup the callback functions
    glutDisplayFunc(paintGL);
    glutKeyboardFunc(keyPressEvent);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();

}
