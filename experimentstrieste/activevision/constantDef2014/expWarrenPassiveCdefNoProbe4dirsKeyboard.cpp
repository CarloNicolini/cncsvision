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
// Monitor tilt in mezzo
// Monitor zoom -23

#include <cstdlib>
#include <iostream>
//#include <iomanip>
//#include <fstream>
//#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "IncludeGL.h"
#include "LatestCalibrationTrieste.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "Timer.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"

#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using util::str2num;
using util::fileExists;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;

ObjLoader model;

static const double focalDistance= -418.5;
// A plane defining the virtual surface which we are projecting onto
Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,focalDistance), Vector3d(0,1,focalDistance),Vector3d(0,0,focalDistance) );

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft, eyeRight, fixationPoint, projPoint, fixationPointFull;
vector <Vector3d> markers(20);
static double interoculardistance=65;
Eigen::ParametrizedLine<double,3> pline;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/********* Timing variables  ************************/
// Timing variables
double timeFrame=0;
double oldvariable=0;
double variable =0;
//double BASEPERIOD=1000; // is the period of triangular wave
int stimulusEmiCycles= 0;
/*** Streams File ***/
ofstream transformationFile;
ofstream trialFile;
ofstream anglesFile;
ofstream eyePosFile;
ofstream responseFile;

ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
BoxNoiseStimulus redDotsPlane;
StimulusDrawer stimDrawer;
Vector3d eyeCalibration(0,0,0);
bool allVisibleHead=false;
double circleRadius=85;
// responses
double probeAngle=0;
double probeStartingAngle=0;

// Trial related things
int trialNumber=0;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
bool conditionInside=true;
bool wasInside=true;
deque<bool> signsX;
deque<bool> signsY;
int sumOutside=-1;

/***** SOUND THINGS *****/
void beepOk()
{
    //Beep(440,440);
    return;
}

static const int FIXATIONMODE=0;
static const int STIMULUSMODE=1;
static const int PROBEMODE=2;
int trialMode = FIXATIONMODE;

/********** CONTROLS AND DEVICES **********/
int mouseButton=-1;
Timer responseTimer;
Timer stimulusTimer;
/** EXPERIMENTAL VARIABLES **/
string subjectName;

/*************************** FUNCTIONS ***********************************/
void cleanup()
{   // Close all the file streams
    transformationFile.close();
    trialFile.close();
    anglesFile.close();
    responseFile.close();
}

/*** FUNCTIONS FOR TRIAL MODE DRAWING ***/
void advanceTrial()
{   trialMode++;
    trialMode=trialMode%3;
    if (trialMode==PROBEMODE)
        responseTimer.start();
    if ( trialMode!=PROBEMODE)
        stimulusTimer.start();
}

void keyPressed()
{
    double responseTime=responseTimer.getElapsedTimeInMilliSec();
    double clickDelay=100;	//milliseconds before the click is valid
    if ( responseTime > clickDelay )
    {
        timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in
        bool contraction = ( factors.at("Tilt")==90 ) || (factors.at("Tilt")==0);
        if ( trial.getRemainingTrials()==0 )
        {
            responseFile << setw(6) << left <<  trialNumber << " " << factors.at("Def") <<" " << factors.at("Tilt") <<" " << factors.at("Slant") << " " << factors.at("Anchored") <<  " " << factors.at("FollowingSpeed") << " " << factors.at("Onset") << " " << (focalDistance - eyeCalibration.z()) << " " << probeAngle << " " << responseTime << " " << contraction << endl;
            cleanup();
            exit(0);
        }
        else
        {
            responseFile << setw(6) << left <<  trialNumber << " " << factors.at("Def") <<" " << factors.at("Tilt") <<" " << factors.at("Slant") << " " << factors.at("Anchored") << " " << factors.at("FollowingSpeed") << " " << factors.at("Onset") << " " << (focalDistance - eyeCalibration.z()) << " " << probeAngle << " " << responseTime << " " << contraction << endl;
            factors = trial.getNext();
            trialNumber++;
        }
        advanceTrial();

        if ( atoi(parameters.find("DrawOccluder").c_str()) ==1 )
        {
            redDotsPlane.setNpoints(500);  //XXX controllare densita di distribuzione dei punti
            redDotsPlane.setDimensions(200,200,0.1);
        }
        else
        {
            redDotsPlane.setNpoints(75);  //XXX controllare densita di distribuzione dei punti
            redDotsPlane.setDimensions(50,50,0.1);
        }
        //redDotsPlane.setSlantTilt( factors.at("Slant"), (int)factors.at("Tilt") );
        redDotsPlane.compute();
        stimDrawer.initList(&redDotsPlane);
    }
}

void drawCircle()
{
    glBegin(GL_LINE_LOOP);
    for (int angle=0; angle<360; angle+=5)
    {
        float angle_radians = angle * (float)3.14159 / (float)180;
        float x =  circleRadius * (float)cos(angle_radians);
        float y =  circleRadius * (float)sin(angle_radians);
        glVertex3f(x,y,focalDistance);
    }
    glEnd();
}

void drawRedDotsPlane()
{   // Draw the stimulus ( red-dots plane )
    bool isInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius);

    glPushMatrix();
    // Reset every previous transformation, put the stimulus in (0,0,0) with no rotation ( fronto-parallel )
    glLoadIdentity();
    if ( (int) factors.at("Anchored")==0 )
        glTranslated(0,0,fixationPoint.z());
    else
        glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());
    //double k=0.34657;

    double theta = 0.0;
    double angle = 0.0;

    switch ( (int)factors.at("Tilt"))
    {
    case 0:
        theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")/2*(1-factors.at("Onset")*variable)));
        //old fantoni theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")*(1-variable)/2));// inclinazione uguale a slant quando variable= 0.5
        angle = toDegrees(theta);
        glRotated(angle,0,1,0);
        glScaled(1/sin(toRadians( -90-factors.at("Slant"))),1,1);	//backprojection phase
        break;
    case 90:
        theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")/2*(1-factors.at("Onset")*variable)));
        // old fantoni theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")*(1-variable)/2));
        angle = toDegrees(theta);

        glRotated( angle,1,0,0);
        glScaled(1,1/sin(toRadians( -90-factors.at("Slant") )),1); //backprojection phase
        break;
    case 180:
        theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")/2*(1+factors.at("Onset")*variable)));
        //old fantoni theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")*(1-(1-variable)/2)));
        angle = toDegrees(theta);

        glRotated(angle,0,1,0);
        glScaled(1/sin(toRadians( -90-factors.at("Slant") )),1,1); //backprojection phase
        break;
    case 270:
        theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")/2*(1+factors.at("Onset")*variable)));
        // old fantoni theta = acos(exp(-0.346574+0.5*factors.at("Def")-factors.at("Def")*(1-(1-variable)/2)));
        angle = toDegrees(theta);

        glRotated( angle,1,0,0);
        glScaled(1,1/sin(toRadians( -90-factors.at("Slant") )),1); //backprojection phase
        break;
    }
    //cout << angle << endl;

    if (isInside && sumOutside < 0 )
        stimDrawer.draw();
    glPopMatrix();

    // Draw fixation (Always @ full-speed)
    if (isInside && sumOutside < 0 )
    {
        glColor3fv(glRed);
        glPushMatrix();
        if ( (int) factors.at("Anchored")==0 )
            glTranslated(0,0,fixationPoint.z());
        else
            glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());
        glutSolidSphere(1,10,10);
        glPopMatrix();
    }
}


void drawProbe()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    glColor3fv(glWhite);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3f(0,0,focalDistance);
    glEnd();
    glColor3fv(glRed);
    glPointSize(1);

}

void drawFixation()
{
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    /** DRAW THE FIXATION POINT **/
    glColor3fv(glRed);
    glPushMatrix();
    if ( (int) factors.at("Anchored")==0 )
        glTranslated(0,0,fixationPoint.z());
    else
        glTranslated(fixationPoint.x(),fixationPoint.y(),fixationPoint.z());

    glutSolidSphere(1,10,10);
    glPopMatrix();

    //drawCircle();
}

void drawOcclusion()
{
    double holeRadius=25;
    double maxSlant=M_PI/4;
    double tanMaxSlant = tan(maxSlant);
    glColor3fv(glBlack);
    glPushMatrix();
    glLoadIdentity();
    if ( (int) factors.at("Anchored")==0 )
        glTranslated(0,0,focalDistance+100+holeRadius/tanMaxSlant );
    else
        glTranslated(fixationPoint.x(),fixationPoint.y(),focalDistance+100+holeRadius/tanMaxSlant );
    glScalef(25,25,2000); //diameter of inner hole is 5 cm @ focalDistance
    glRotated(90,1,0,0);
    model.draw();
    glPopMatrix();
}


void drawTrial()
{   switch ( trialMode )
    {
    case FIXATIONMODE:
    {   drawFixation();
    }
    break;
    case PROBEMODE :
    {   drawProbe();
    }
    break;
    case STIMULUSMODE:
    {
        drawRedDotsPlane();
        if ( atoi(parameters.find("DrawOccluder").c_str())==1 )
            drawOcclusion();
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
    stimulusEmiCycles= atoi(parameters.find("StimulusEmiCycles").c_str());
    trial.init(parameters);
    //trial.print();
    factors = trial.getNext(); // Initialize the factors in order to start from trial 1

    if ( atoi(parameters.find("DrawOccluder").c_str()) ==1 )
    {
        redDotsPlane.setNpoints(500);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane.setDimensions(200,200,0.1);
    }
    else
    {
        redDotsPlane.setNpoints(75);  //XXX controllare densita di distribuzione dei punti
        redDotsPlane.setDimensions(50,50,0.1);
    }
    //redDotsPlane.setSlantTilt( factors.at("Slant"), (int)factors.at("Tilt") );
    redDotsPlane.compute();
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane);

    /** Bound check things **/
    signsX.push_back(false);
    signsX.push_back(false);

    signsY.push_back(false);
    signsY.push_back(false);

    headEyeCoords.init(Vector3d(-32.5,0,0),Vector3d(32.5,0,0), Vector3d(0,0,0),Vector3d(0,10,0),Vector3d(0,0,10),interoculardistance );
    eyeCalibration=headEyeCoords.getRightEye();
    model.load("../../data/objmodels/occluder.obj");
}

void mouseFunc(int button, int state, int _x , int _y)
{
    if ( trialMode == PROBEMODE )
    {
        //HERE IS THE FUNCTION NEEDED TO USE MOUSE INSTEAD OF KEYBOARD
        /*
        	glutWarpPointer(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

        	if( button== GLUT_MIDDLE_BUTTON  )
        	{
        		double responseTime=responseTimer.getElapsedTimeInMilliSec();
        		double clickDelay=100;	//milliseconds before the click is valid
        		if ( responseTime > clickDelay )
        		{
        		timeFrame=0.0; //this put the stimulus in the center each central time mouse is clicked in
        		bool contraction = ( factors.at("Tilt")==90 ) || (factors.at("Tilt")==0);
        		double angleResponse = (mod(probeAngle+probeStartingAngle,360));
        		if ( trial.getRemainingTrials()==0 )
        		{
        			responseFile << setw(6) << left <<  trialNumber << " " << factors.at("Def") <<" " << factors.at("Tilt") <<" " << factors.at("Slant") << " " << factors.at("Anchored") <<  " " << factors.at("FollowingSpeed") << " " << factors.at("Onset") << " " << (focalDistance - eyeCalibration.z()) << " " << angleResponse << " " << responseTime << " " << contraction << endl;
        			cleanup();
        			exit(0);
        		}
        		else
        		{
        			responseFile << setw(6) << left <<  trialNumber << " " << factors.at("Def") <<" " << factors.at("Tilt") <<" " << factors.at("Slant") << " " << factors.at("Anchored") << " " << factors.at("FollowingSpeed") << " " << factors.at("Onset") << " " << (focalDistance - eyeCalibration.z()) << " " << angleResponse << " " << responseTime << " " << contraction << endl;
        			factors = trial.getNext();
        			trialNumber++;
        		}
        		advanceTrial();

        		double possibleAngles[]={0,90,180,270};
        		probeStartingAngle=possibleAngles[rand()%4];
        	    probeAngle=0;
                redDotsPlane.compute();
                stimDrawer.initList(&redDotsPlane);
        		mouseButton=0;
        		}
        	}
        */
    }
    glutPostRedisplay();
}

void initStreams()
{
    string parametersPassiveFileName("C:/cncsvisiondata/parametersFiles/parametersPassiveCdef.txt");
    inputParameters.open(parametersPassiveFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersPassiveFileName << " doesn't exist, enter a valid path" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

#ifdef _WIN32
    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    string baseDir = parameters.find("BaseDir");
    // Subject name
    string subjectName = parameters.find("SubjectName");
    // Principal streams file
    string transformationFileName("transformationFile_");
    string trialFileName("trialFile_");
    string anglesFileName("anglesFile_");
    string responseFileName("responseFile_");

    // Add the subject name to file extension
    transformationFileName	+=string(subjectName)+".txt";
    trialFileName			+=string(subjectName)+".txt";
    anglesFileName			+=string(subjectName)+".txt";
    responseFileName		+=string(subjectName)+".txt";

    // Check for output file existence
    /** Transformation file **/
    if ( !fileExists((baseDir+transformationFileName)) )
        transformationFile.open((baseDir+transformationFileName).c_str() );

    /** Trial file **/
    if ( !fileExists((baseDir+trialFileName)) )
        trialFile.open((baseDir+trialFileName).c_str());

    /** Angles file **/
    if ( !fileExists((baseDir+anglesFileName)) )
        anglesFile.open((baseDir+anglesFileName).c_str());

    /** Response file **/
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());
#endif

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

    /** LIGHTS **/
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, glWhite);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

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
        drawTrial();

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawTrial();

        glutSwapBuffers();
    }
    else	// MONOCULAR
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if ( (int) factors.at("Anchored") != 0 )
            cam.setEye(Vector3d(fixationPoint.x(),fixationPoint.y(),0));
        else
            cam.setEye(Vector3d::Zero());
        drawTrial();
        glutSwapBuffers();
    }
}



void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {   cleanup();
        exit(0);
    }
    break;
    case 'p':
    {
        // Print the current trial
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            cerr << iter->first << " " << iter->second << " ";
        }
        cerr << endl;
    }
    break;
    case '2':
    {
        if ( trialMode == PROBEMODE )
        {
            probeAngle=270;
            keyPressed();
        }
    }
    break;
    case '8':
    {
        if ( trialMode == PROBEMODE )
        {
            probeAngle=90;
            keyPressed();
        }
    }
    break;
    case '4':
    {
        if ( trialMode == PROBEMODE )
        {
            probeAngle=0;
            keyPressed();
        }
    }
    break;
    case '6':
    {
        if ( trialMode == PROBEMODE )
        {
            probeAngle=180;
            keyPressed();
        }
    }
    break;
    case 'f':
        glutFullScreen();
        break;
    }
}


void specialPressed(int key, int x, int y)
{   switch (key)
    {   /** Calibration movements **/
    case GLUT_KEY_PAGE_DOWN:
    {   screen.translate(0,0,-0.5);
        cam.updateMonitorPoint(screen);
        break;
    }
    break;
    case GLUT_KEY_PAGE_UP:
    {   screen.translate(0,0,0.5);
        cam.updateMonitorPoint(screen);
        break;
    }
    }
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void checkBounds()
{
    if ((trialMode!=PROBEMODE) )
    {
        conditionInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius-2.5) ; // here we subtract 4 to work with trianglewave precision
        // If this condition is met, then this means that the point is outside the screen area!
        if ( !( conditionInside || (!wasInside) ) )
        {
            switch ( (int) factors.at("Anchored") )
            {
            case 0:	//anchored x
                signsX.pop_back();
                signsX.push_front( factors.at("Onset")* projPoint.x() > 0 );
                if ( (signsX.front() != signsX.back() ) )
                {   beepOk();
                    sumOutside++;
                }
                break;
            case 1:	//anchored x
                signsX.pop_back();
                signsX.push_front( factors.at("Onset")* projPoint.x() > 0 );
                if ( (signsX.front() != signsX.back() ) )
                {   beepOk();
                    sumOutside++;
                }
                break;
            case 2:	//anchored y
                signsY.pop_back();
                signsY.push_front( factors.at("Onset")*projPoint.y() > 0 );
                if ( (signsY.front() != signsY.back() ) )
                {   beepOk();
                    sumOutside++;
                }
                break;
            }
        }
        wasInside = conditionInside;
    }
}

void update(int value)
{
    // Timing things
    if ( trialMode != PROBEMODE )
    {
        oldvariable = variable;
        variable = -factors.at("Onset")*mathcommon::trianglewave( timeFrame , factors["StimulusDuration"]/(TIMER_MS*factors.at("FollowingSpeed")) );

        timeFrame+=1;
        bool isInside = ((projPoint - Vector3d(0,0,focalDistance) ).norm()) <= (circleRadius);
        // permette di avanzare se siamo in stimulusMode e lo stimolo ha fatto un semiciclo ( una passata da dx a sx o da su a giù )
        //bool nextMode = ( sumOutside == 1 ) && (trialMode==STIMULUSMODE);
        bool nextMode=false;
        if ( isInside && ( sumOutside == stimulusEmiCycles ) || (nextMode) )
        {
            sumOutside=-1;
            advanceTrial();
        }
    }

    // Simulate head translation
    // Coordinates picker
    markers[1] = Vector3d(0,0,0);
    markers[2] = Vector3d(0,10,0);
    markers[3] = Vector3d(0,0,10);

    Vector3d translation(0,0,0);
    switch ( (int) factors.at("Anchored") )
    {
    case 0:
        translation = Vector3d((circleRadius+1)*variable,0,0);
        break;
    case 1:
        translation = Vector3d((circleRadius+1)*variable,0,0);
        break;
    case 2:
        translation = Vector3d(0,(circleRadius+1)*variable,0);
        break;
    }

    markers[1]+=translation;
    markers[2]+=translation;
    markers[3]+=translation;

    headEyeCoords.update(markers[1],markers[2],markers[3]);

    eyeLeft = headEyeCoords.getLeftEye();
    eyeRight = headEyeCoords.getRightEye();



    fixationPoint = (headEyeCoords.getRigidStart().getFullTransformation() * ( Vector3d(0,0,focalDistance) ) );
    // Projection of view normal on the focal plane
    pline = Eigen::ParametrizedLine<double,3>::Through(eyeRight,fixationPoint);
    projPoint = pline.intersection(focalPlane)*((fixationPoint - eyeRight).normalized()) + eyeRight;

    checkBounds();

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
}

int main(int argc, char*argv[])
{
    mathcommon::randomizeStart();
    initScreen();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("Experiment Passive");
    }
    else
    {
        glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();

    initStreams();
    initVariables();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(specialPressed);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    cleanup();

    return 0;
}
