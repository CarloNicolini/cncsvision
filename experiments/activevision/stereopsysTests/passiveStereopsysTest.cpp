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
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"
#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>


#include "Util.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "ParStaircase.h"
#include "RoveretoMotorFunctions.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;

BoxNoiseStimulus frontStimulus;
StimulusDrawer stimDrawerFront;
ParStaircase staircase;
// Trial related things
ParametersLoader parameters;
/********** EYES **********************/
Vector3d eyeLeft, eyeRight;
bool allVisibleHead=false;
int headCalibrationDone=0,trialMode=0,mode=0,sign=1,trialNumber=0;
double scaling=1.0;
const int StimulusMode=0, FixationMode=1, ResponseMode=2;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;
double focalDistance=-418.5;
double interocularDistance=65;

/********* STREAMS ******/
ofstream outputfile;

/***** SOUND THINGS *****/
boost::mutex beepMutex;
void beepOk()
{
#ifdef _WIN32
    boost::mutex::scoped_lock lock(beepMutex);
    Beep(440,440);
#endif
    return;
}

/**
 * @brief drawInfo
 */
void drawInfo()
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
	text.enterTextInputMode();
    switch ( headCalibrationDone )
    {
    case 0:
    {
        /*
		if ( allVisibleHead )
            text.draw("PRESS SPACEBAR TO CALIBRATE");
        else
            text.draw("BE VISIBLE...");
        break;
		*/
    }
    case 1:
    {
        break;
    }
    }
	text.leaveTextInputMode();
}


/**
 * @brief idle
 */
void idle()
{
    eyeLeft = Vector3d(-interocularDistance/2,0,0);
    eyeRight = Vector3d(interocularDistance/2,0,0);

}

/**
 * @brief resizeGL
 * @param w
 * @param h
 */
void resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

/**
 * @brief initializeGL
 */
void initializeGL()
{
    int argc=1;
	char*argv[]={""};
    glutInit(&argc, argv);

    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    if (gameMode)
    {
		glutGameModeString(ROVERETO_GAME_MODE_STRING);
        glutEnterGameMode();
    }
    else
    {
        glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
        glutCreateWindow("Experiment Stereopsis");
    }

    glEnable(GL_BLEND);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief nexTrial
 */
void nextTrial()
{
    // update scaling of other stimulus
    double d = staircase.getCurrentStaircase()->getState();
    scaling = 1.0;//(focalDistance+d)/focalDistance;
    // Generate the frontStimulus

    double edgeFrontX=util::str2num<double>(parameters.find("StimulusEdgeLength"));
    double edgeFrontY=edgeFrontX;
    double w = util::str2num<double>(parameters.find("StimulusWindowEdgeLength"));

    double depthZ=0.001;
    int frontStimNPoints = util::str2num<double>(parameters.find("PointsDensity"))*edgeFrontX*edgeFrontX;

    frontStimulus.setNpoints(frontStimNPoints);
    frontStimulus.setDimensions(edgeFrontX*scaling,edgeFrontY*scaling,depthZ);
    frontStimulus.setFluffiness(0.001);
    frontStimulus.compute();

	sign = ((rand()%2)*2) -1;
	double f = util::str2num<double>(parameters.find("FocalDistance"));
	if ( parameters.find("StimulusType")=="Squares" )
	{
		for (PointsRand::iterator iter=frontStimulus.pointsRand.begin();iter!=frontStimulus.pointsRand.end();++iter)
		{
			Point3D *p = (*iter);
			if ( (p->x <=  w && p->x >= -w) && (p->y <=  w && p->y >= -w) )
			{
				 p->z = f + abs(staircase.getCurrentStaircase()->getState())*sign;
			}
			else
				p->z = f;
		}
	}
	else
	{
		double R = util::str2num<double>(parameters.find("EllipsoidRadius"));
		double Rz = staircase.getCurrentStaircase()->getState();
		for ( PointsRand::iterator iter=frontStimulus.pointsRand.begin();iter!=frontStimulus.pointsRand.end();++iter )
		{
			Point3D *p = (*iter);
			p->z = f + Rz*sign*sqrt( 1 - (p->x*p->x + p->y*p->y)/(R*R) );
		}
	}
    //cerr << "FrontStimDepth = " << focalDistance+staircase.getCurrentStaircase()->getState() << " Scaling= " << scaling << "sign= " << sign << endl;

    // Generate the front stimulus
    stimDrawerFront.setSpheres(false);
    stimDrawerFront.setStimulus(&frontStimulus);
    stimDrawerFront.initList(&frontStimulus,glRed,5);
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    // MUST BE CALLED WITHIN A VALID OPENGL CONTEXT
    Screen screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH, 0, 0, focalDistance );
    //    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);

    // Initialize experimental variables
#ifdef WIN32
    parameters.loadParameterFile("C:/cncsvisiondata/parametersStereopsisTest.txt");
   #else
    parameters.loadParameterFile("data/parametersFiles/parametersStereopsisTest.txt");
#endif
    staircase. init(parameters);
    interocularDistance = util::str2num<double>(parameters.find("IOD"));
    
	string outputfilename(parameters.find("BaseDir")+ "stereopsisOuput_" + parameters.find("SubjectName")+".txt");
	cerr << "Opening output file to: " << outputfilename << endl;
	outputfile.open( outputfilename.c_str() );

	RoveretoMotorFunctions::homeScreen();
	RoveretoMotorFunctions::moveScreenAbsolute( util::str2num<double>(parameters.find("FocalDistance")),-418.5,3500);

	// Advance and initialize the first trial
	nextTrial();
}

/**
 * @brief drawStimulus
 */
void drawStimulus()
{
    glPushMatrix();
    glLoadIdentity();
    // Draw background stimulus
    stimDrawerFront.draw();
    glPopMatrix();

    /*
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    double edgeX=util::str2num<double>(parameters.find("StimulusFrontEdgeLength"));
    double z = frontStimulus.pointsRand[0]->z;
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,focalDistance);
    switch( mode )
    {
    case 0:
    {
        glColor3fv(glWhite);
        glBegin(GL_LINE_LOOP);
    }
        break;
    case 1:
    {
        glColor3fv(glBlack);
        glBegin(GL_POLYGON);
    }
        break;
    }
    glVertex3d(-edgeX,-edgeX,z+1E-1);
    glVertex3d(-edgeX,edgeX,z+1E-1);
    glVertex3d(edgeX,edgeX,z+1E-1);
    glVertex3d(edgeX,-edgeX,z+1E-1);
    glEnd();
    glPopMatrix();
    glPopAttrib();
    */
}

/**
 * @brief drawFixation
 */
void drawFixation()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,focalDistance);
    glPointSize(8);
    glColor3d(1.0,0.0,0.0);
    glBegin(GL_POINTS);
    glVertex3d(0.0,0.0,focalDistance);
    glEnd();
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch (trialMode)
    {
    case StimulusMode:
    {
        drawStimulus();
        break;
    }
    case ResponseMode:
    {
        drawFixation();
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
    {
        glDrawBuffer(GL_BACK);
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
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(Vector3d::Zero());
        drawInfo();
        drawTrial();
        glutSwapBuffers();
    }
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
    {       //Quit program
    case 'q':
    case 27:
    {
        exit(0);
        break;
    }
    case 'm':
    {
        mode++;
        mode=mode%2;
        break;
    }
    case '4':
    {
		bool step = ( sign*abs(staircase.getCurrentStaircase()->getState()) > 0) ;
		outputfile << trialNumber << "\t" << staircase.getCurrentStaircase()->getID() << "\t" << staircase.getCurrentStaircase()->getStepsDone() << "\t" << sign*abs(staircase.getCurrentStaircase()->getState()) << "\t" << sign << "\t" << 4 << "\t" << step << endl; 
		int expFinish=0;
		if ( step )
			expFinish = staircase.step(true);
		else
			expFinish = staircase.step(false);
		if (expFinish)
			exit(0);
		nextTrial();
		trialNumber++;
        break;
    }
    case '6':
    {
		bool step = ( sign*abs(staircase.getCurrentStaircase()->getState()) > 0 );
		outputfile << trialNumber << "\t" << staircase.getCurrentStaircase()->getID() << "\t" << staircase.getCurrentStaircase()->getStepsDone() << "\t" << sign*abs(staircase.getCurrentStaircase()->getState()) << "\t" << sign << "\t" << 6 << "\t" << !step << endl;
		int expFinish=0;
		if ( step )
			expFinish = staircase.step(false);
		else
			expFinish = staircase.step(true);
		if (expFinish)
			exit(0);
		nextTrial();
		trialNumber++;
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

    // Initialize the OpenGL context
    initializeGL();
    initializeExperiment();
    // Setup the callback functions
    glutDisplayFunc(paintGL);
    glutReshapeFunc(resizeGL);
    glutKeyboardFunc(keyPressEvent);
    glutTimerFunc(TIMER_MS, update, 0);
    glutIdleFunc(idle);
    if (gameMode)
        glutSetCursor(GLUT_CURSOR_NONE);
    glutMainLoop();

}
