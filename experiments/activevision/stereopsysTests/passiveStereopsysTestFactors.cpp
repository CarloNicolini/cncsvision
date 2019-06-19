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

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#endif

#include "Util.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "GLText.h"
#include "ParametersLoader.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "BalanceFactor.h"
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

// Trial related things
ParametersLoader parameters;
BalanceFactor<double> balanceFactor;
/********** EYES **********************/
Vector3d eyeLeft, eyeRight;
int trialMode=0,trialNumber=0;
double scaling=1.0;
const int StimulusMode=0, FixationMode=1, AccuracyMode=2;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;
double focalDistance=-418.5;
double interocularDistance=65;

std::map< double, std::vector< int > > accuracyPerDepth;

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
        glutCreateWindow("Experiment Stereopsys");
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
    double d = balanceFactor.getCurrent().at("ZDepth");//staircase.getCurrentStaircase()->getState();
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

    double f = util::str2num<double>(parameters.find("FocalDistance"));
    if ( parameters.find("StimulusType")=="Squares" )
    {

        for (PointsRand::iterator iter=frontStimulus.pointsRand.begin();iter!=frontStimulus.pointsRand.end();++iter)
        {
            Point3D *p = (*iter);
            if ( (p->x <=  w && p->x >= -w) && (p->y <=  w && p->y >= -w) )
            {
                p->z = f + (balanceFactor.getCurrent().at("ZDepth"));
            }
            else
                p->z = f;
        }
    }
    else
    {
        double R = util::str2num<double>(parameters.find("EllipsoidRadius"));
        double Rz = balanceFactor.getCurrent().at("ZDepth");
        for ( PointsRand::iterator iter=frontStimulus.pointsRand.begin();iter!=frontStimulus.pointsRand.end();++iter )
        {
            Point3D *p = (*iter);
            p->z = f + Rz*sqrt( 1 - (p->x*p->x + p->y*p->y)/(R*R) );
        }
    }

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
    parameters.loadParameterFile("C:/cncsvisiondata/parametersStereopsisTestFactor.txt");
#else
    parameters.loadParameterFile("data/parametersFiles/parametersStereopsisTestFactor.txt");
#endif
    balanceFactor.init(parameters);
    interocularDistance = util::str2num<double>(parameters.find("IOD"));
    
    string outputfilename(parameters.find("BaseDir")+ "stereopsisFactorOuput_" + parameters.find("SubjectName")+".txt");
    cerr << "Opening output file to: " << outputfilename << endl;
    outputfile.open( outputfilename.c_str() );

	RoveretoMotorFunctions::homeScreen();
    RoveretoMotorFunctions::moveScreenAbsolute( util::str2num<double>(parameters.find("FocalDistance")),-418.5,3500);
    balanceFactor.next();
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
}

/**
 * @brief drawFixation
 */
void drawAccuracy()
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();
    for (std::map< double, std::vector<int> >::iterator iter=accuracyPerDepth.begin(); iter!=accuracyPerDepth.end();++iter)
    {
        std::string line;
        line.append( std::string("Accuracy at depth ") + util::stringify<double>(iter->first) + std::string(" = "));
        line.append( util::stringify<double>( (double)(std::accumulate(iter->second.begin(),iter->second.end(),0.0))/(double)iter->second.size()*100.0 ) + std::string("%") );
        text.draw(line);
    }
    text.leaveTextInputMode();
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
    case AccuracyMode:
    {
        drawAccuracy();
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
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(eyeLeft);
        drawTrial();
        glutSwapBuffers();
    }
    else
    {
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        cam.setEye(Vector3d::Zero());
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

    case 13:
    {
        if (trialMode==AccuracyMode)
        {
            exit(0);
        }
    }
        break;
    case '4':
    {
        if (!balanceFactor.hasNext())
        {
            trialMode=AccuracyMode;
        }
        bool correctAnswer = balanceFactor. getCurrent().at("ZDepth") > 0;
        accuracyPerDepth[balanceFactor.getCurrent().at("ZDepth")].push_back(correctAnswer);
        outputfile << trialNumber << "\t" << balanceFactor.getCurrent().at("ZDepth") << "\t" << 4 << "\t" << correctAnswer << endl;
        if (!balanceFactor.hasNext())
            return;
        balanceFactor.next();
        nextTrial();
        trialNumber++;
        break;
    }
    case '6':
    {
        if (!balanceFactor.hasNext())
        {
            trialMode=AccuracyMode;
        }
        bool correctAnswer = balanceFactor. getCurrent().at("ZDepth") < 0;
        accuracyPerDepth[balanceFactor.getCurrent().at("ZDepth")].push_back(correctAnswer);
        outputfile << trialNumber << "\t" << balanceFactor.getCurrent().at("ZDepth") << "\t" << 6 << "\t" << correctAnswer << endl;
        if (!balanceFactor.hasNext())
            return;
        balanceFactor.next();
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
