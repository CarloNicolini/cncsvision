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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#include <windows.h>
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "GLMaterial.h"
#include "GLLight.h"
#include "GLUtils.h"
#include "GLMaterial.h"
#include "Util.h"
#include "Imageloader.h"
#include "GLSL.h"
#include "ObjLoader.h"
#include "VRCamera.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "BalanceFactor.h"
#include "GLText.h"
#include "Timer.h"
#include "EllipsoidPointsStimulus.h"
#include "StimulusDrawer.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace boost;
/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
static const double focalDistance= -500.0; //Millimeters

/********* REAL SCREEN POINTS ****/
Screen screen;

/********** EYES  **********************/
Vector3d eyeLeft, eyeRight;
static double interoculardistance=65;
double frame=0.0;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=false;
static const bool stereo=false;
bool orthoMode=true;

/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/
// Trial related things
int trialNumber=0;
ParametersLoader parameters;
TrialGenerator<double> trial;
map <string, double> factors;
pair<map <string, double>,ParStaircase*> factorStaircasePair;

// Random dots to draw overlay
StimulusDrawer stimDrawerBlack[2],stimDrawerRed[2];
EllipsoidPointsStimulus cylinderPointsWhite[2],cylinderPointsRed[2];

static const int textureHeight = 1024;
int textureWidthFactorStim,textureWidthStaircaseStim,textureWidthMaskStim;

static const int STIMULUSMODE0=0;   // presentation of factor stimulus
static const int MASKMODE=1;        // presentation of mask
static const int STIMULUSMODE1=2;   // presentation of staircase stimulus
static const int PROBEMODE=3;       // probe (white dot in the center)
int trialMode = STIMULUSMODE0;      // the first stimulus presented is the FACTOR!!
int prevTrialMode=STIMULUSMODE1;

bool drawInfo=true;
bool experimentStarted=false;

int trialmodes[4]={STIMULUSMODE0,MASKMODE,STIMULUSMODE1,PROBEMODE};
int trialIndex=0;

/*** EXPERIMENTAL VARIABLES ***/
string subjectName;

/*** GLSL Shaders thing ***/
glsl::glShader *shader;
GLuint texIdFactor,texIdStaircase, texIdMask;
float radiusX=50.0;
float height=radiusX;
float radiusZ=radiusX;
bool horizontal=false;
Timer trialTimer;
double stimulusTime=1000, maskTime=500; //presentation time in milliseconds, is set to 1000 by default, must be specified in the parameters

/*************************** FUNCTIONS ***********************************/
void updateStimulusRandomPoints(double radiusX, double height, double radiusZFactor,double radiusZStaircase)
{
    int nDots[2];
    nDots[0] =util::str2num<double> (parameters.find("RandomNoiseDensity"))*mathcommon::ellipseCircumferenceBetter(radiusX,radiusZFactor)*height;
    nDots[1] =util::str2num<double> (parameters.find("RandomNoiseDensity"))*mathcommon::ellipseCircumferenceBetter(radiusX,radiusZStaircase)*height;

    // First fill the things related to the factor stimulus
    // Cylinder staircase
    cylinderPointsRed[0].setNpoints(nDots[0]/2);
    cylinderPointsRed[0].setAperture(0,M_PI);
    cylinderPointsRed[0].setFluffiness(0.0);
    cylinderPointsRed[0].setAxesAndHeight(radiusX*2,radiusZFactor*2,height*2);
    cylinderPointsRed[0].compute();

    // Cylinder mask
    cylinderPointsWhite[0].setNpoints(nDots[0]/2);
    cylinderPointsWhite[0].setAperture(0,M_PI);
    cylinderPointsWhite[0].setFluffiness(0.0);
    cylinderPointsWhite[0].setAxesAndHeight(radiusX*2,radiusZFactor*2,height*2);
    cylinderPointsWhite[0].compute();

    stimDrawerRed[0].setStimulus(&cylinderPointsRed[0]);
    stimDrawerBlack[0].setStimulus(&cylinderPointsWhite[0]);

    stimDrawerRed[0].setSpheres(false);
    stimDrawerBlack[0].setSpheres(false);

	const GLfloat *color1,*color2;
	if ( parameters.find("DotsColor")=="BlackAndRed" )
	{
		color1=glRed;
		color2=glBlack;
	}
	if ( parameters.find("DotsColor")=="White" )
		color1=color2=glWhite;

    stimDrawerRed[0].initList(&cylinderPointsRed[0],color1,util::str2num<int>(parameters.find("RandomDotsSize")));
    stimDrawerBlack[0].initList(&cylinderPointsWhite[0],color2,util::str2num<int>(parameters.find("RandomDotsSize")));

    // Then those related to the staircase stimulus
    // First fill the things related to the factor stimulus
    // Cylinder staircase
    cylinderPointsRed[1].setNpoints(nDots[0]/2);
    cylinderPointsRed[1].setAperture(0,M_PI);
    cylinderPointsRed[1].setFluffiness(0.001);
    cylinderPointsRed[1].setAxesAndHeight(radiusX*2,radiusZStaircase*2,height*2);
    cylinderPointsRed[1].compute();

    // Cylinder mask
    cylinderPointsWhite[1].setNpoints(nDots[0]/2);
    cylinderPointsWhite[1].setAperture(0,M_PI);
    cylinderPointsWhite[1].setFluffiness(0.001);
    cylinderPointsWhite[1].setAxesAndHeight(radiusX*2,radiusZStaircase*2,height*2);
    cylinderPointsWhite[1].compute();

    stimDrawerRed[1].setStimulus(&cylinderPointsRed[1]);
    stimDrawerBlack[1].setStimulus(&cylinderPointsWhite[1]);

    stimDrawerRed[1].setSpheres(false);
    stimDrawerBlack[1].setSpheres(false);

    stimDrawerRed[1].initList(&cylinderPointsRed[1],color1,util::str2num<int>(parameters.find("RandomDotsSize")));
    stimDrawerBlack[1].initList(&cylinderPointsWhite[1],color2,util::str2num<int>(parameters.find("RandomDotsSize")));

}

void drawStimulus()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,2);
    // Draw the factor stimulus
    if (trialMode==STIMULUSMODE0)
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texIdFactor);

        glPushMatrix();

        if (horizontal)
            glRotated(90,0,0,1);
        shader->begin();
        shader->setUniform1i((GLcharARB*)"my_color_texture",0); //0 is the current active texture unit
        shader->setUniform1f((GLcharARB*)"textureWidth",(float)textureWidthFactorStim);
        shader->setUniform1f((GLcharARB*)"textureHeight",(float)textureHeight);
        drawCylinderElliptic( radiusX,height,radiusX*factors["ScaleZ"],1E-1);
        shader->end();
        glPopMatrix();
    }
    // Draw the central mask
    if (trialMode == MASKMODE )
    {
        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texIdMask);
        glPushMatrix();

        if (horizontal)
            glRotated(90,0,0,1);
        shader->begin();
        shader->setUniform1i((GLcharARB*)"my_color_texture",1); //0 is the current active texture unit
        shader->setUniform1f((GLcharARB*)"textureWidth",(float)textureWidthFactorStim);
        shader->setUniform1f((GLcharARB*)"textureHeight",(float)textureHeight);
        drawCylinderElliptic( radiusX,height,0,2.5*1E-1);
        shader->end();
        glPopMatrix();
    }
    // Draw the staircase stimulus
    if (trialMode==STIMULUSMODE1)
    {
        double staircaseScale = factorStaircasePair.second->getCurrentStaircase()->getState();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,texIdStaircase);
        glPushMatrix();

        if (horizontal)
            glRotated(90,0,0,1);
        shader->begin();
        shader->setUniform1i((GLcharARB*)"my_color_texture",2); //1 is the current active texture unit
        shader->setUniform1f((GLcharARB*)"textureWidth",(float)textureWidthStaircaseStim);
        shader->setUniform1f((GLcharARB*)"textureHeight",(float)textureHeight);
        drawCylinderElliptic( radiusX,height,radiusX*staircaseScale,2.5*1E-1);
        shader->end();
        glPopMatrix();
    }
    // Restore base state
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_POLYGON_OFFSET_FILL);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance+0.2 );
    if (horizontal)
        glRotated(90,0,0,1);
    if (trialMode==STIMULUSMODE0)
    {
        stimDrawerRed[0].draw();
        stimDrawerBlack[0].draw();
    }
    if (trialMode==STIMULUSMODE1)
    {
        stimDrawerRed[1].draw();
        stimDrawerBlack[1].draw();
    }
    glPopMatrix();
	
}

void drawProbe()
{
    // Draw a central white dot, waiting for user response
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    glutSolidSphere(1,20,20);
    glPopMatrix();
}

void drawTrial()
{
    if ( drawInfo )
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            text.draw(iter->first + " = " + util::stringify<double>(iter->second) );
        }
        string stairstate =
                "ID= "  + util::stringify<int>(factorStaircasePair.second->getCurrentStaircase()->getID())
                + " State= " +util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getState())
                + " Ascending= " + util::stringify<int>(factorStaircasePair.second->getCurrentStaircase()->getAscending())
                + " Inversions= " + util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getInversions())
                + " Steps= " +(util::stringify<double>(factorStaircasePair.second->getCurrentStaircase()->getStepsDone())) ;
        text.draw(stairstate);
        text.draw(util::stringify<int>(trialTimer.getElapsedTimeInMilliSec() ));
        if (trialMode==STIMULUSMODE0)
            text.draw("Presenting REFERENCE (factor)");
        if (trialMode==STIMULUSMODE1)
            text.draw("Presenting TEST (staircase)");
        if (trialMode==MASKMODE)
            text.draw("Presenting MASK");
        if (trialMode==PROBEMODE)
        {
            text.draw("Presenting PROBE");
            if (prevTrialMode==STIMULUSMODE0)
                text.draw("Previous mode= REFERENCE (factor)");
            if (prevTrialMode==STIMULUSMODE1)
                text.draw("Previous mode= TEST (staircase)");
            if (prevTrialMode==MASKMODE)
                text.draw("Previous mode= MASK");
            if (prevTrialMode==PROBEMODE)
                text.draw("Previous mode= PROBEMODE");
        }
        text.leaveTextInputMode();
    }

    if (!experimentStarted)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    if (trialMode!=PROBEMODE)
        drawStimulus();
    else
        drawProbe();
    glPopAttrib();
    // Restore base state
}

void initVariables()
{
    drawInfo = (bool)(util::str2num<int>(parameters.find("DrawInfo")));
    cam.setOrthoGraphicProjection((bool)str2num<int>(parameters.find("OrthographicMode")));
    interoculardistance = str2num<double>(parameters.find("IOD"))*str2num<double>(parameters.find("IODFactor"));
    cerr << "COMPUTED IOD=" << interoculardistance  << endl;
    trial.init(parameters);
    factorStaircasePair = trial.getCurrent();
    factors = factorStaircasePair.first;

    radiusX = height = str2num<double>(parameters.find("RadiusX"));
    radiusZ = radiusX*factorStaircasePair.second->getCurrentStaircase()->getState();

    trialTimer.start();
    stimulusTime = util::str2num<double>(parameters.find("StimulusTime"));
    maskTime = util::str2num<double>(parameters.find("MaskTime"));

    textureWidthFactorStim =  mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factors["ScaleZ"]/2),textureHeight/2)*0.5;
    textureWidthStaircaseStim = mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factorStaircasePair.second->getCurrentStaircase()->getState()/2),textureHeight/2)*0.5;
    textureWidthMaskStim = mathcommon::ellipseCircumferenceBetter(0,textureHeight/2)*0.5;

    int ncirclesFactorStim =util::str2num<double> (parameters.find("CirclesPerPixelSquare"))*textureWidthFactorStim*textureHeight;
    int ncirclesStaircaseStim = util::str2num<double>(parameters.find("CirclesPerPixelSquare"))*textureWidthStaircaseStim*textureHeight;
    int ncirclesMask = util::str2num<int>(parameters.find("NumMaskCircles"));
    int dotsRadiusMin = util::str2num<int>(parameters.find("DotsRadiusMin"));
    int dotsRadiusMax = util::str2num<int>(parameters.find("DotsRadiusMax"));

    //bool addRandomNoise = (bool)util::str2num<int>(parameters.find("AddRandomNoise"));
    //double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));
    bool usePolkaDotsReference = (bool)util::str2num<int>(parameters.find("UsePolkaDotsReference"));
    bool usePolkaDotsStaircase = (bool)util::str2num<int>(parameters.find("UsePolkaDotsStaircase"));
    texIdFactor = generateCirclesTexture(textureWidthFactorStim,textureHeight,ncirclesFactorStim,dotsRadiusMin,dotsRadiusMax,false,false,0.0,usePolkaDotsReference);
    texIdStaircase = generateCirclesTexture(textureWidthStaircaseStim,textureHeight,ncirclesStaircaseStim,dotsRadiusMin,dotsRadiusMax,false,false,0.0,usePolkaDotsStaircase);
    texIdMask = generateCirclesTexture(textureWidthMaskStim,textureHeight,ncirclesMask,10,25);

    updateStimulusRandomPoints(radiusX,height,radiusX*factors.at("ScaleZ"),radiusZ);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
}

void initStreams()
{
    //string parametersFileName("C:/cncsvisiondata/parametersFiles/Fulvio/expTexturing/parametersExpTexturingDiscrimination.txt");
    string parametersFileName("/home/carlo/Desktop/params/parametersExpTexturingDiscrimination.txt");
    inputParameters.open(parametersFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersFileName << " doesn't exist, enter a valid path, press Enter to exit" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);
    string subjectName = parameters.find("SubjectName");

    string outputFilename = parameters.find("BaseDir")+string("responseFileDiscrimination_") + subjectName + string(".txt");
    if ( util::fileExists(outputFilename) )
    {
        int i=0;
        cerr << "file " << outputFilename << "already exists, press Enter to quit" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
    else
    {
        responseFile.open( outputFilename.c_str() );
    }
}

void initRendering()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_MULTISAMPLE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);
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
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(interoculardistance/2,0,0));
        glTranslated(0,0,focalDistance);
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(-interoculardistance/2,0,0));
        glTranslated(0,0,focalDistance);
        drawTrial();
        glutSwapBuffers();
    }
    else	// MONOCULAR
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
        glTranslated(0,0,focalDistance);
        drawTrial();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
    case 27:
    {
        glDeleteTextures(1,&texIdFactor);
        glDeleteTextures(1,&texIdMask);
        glDeleteTextures(1,&texIdStaircase);
        exit(0);
    }
        break;
    case '4':
    {
        int responsekey=4;
        if (trialMode==PROBEMODE)
        {
            bool resp=false;
            if (prevTrialMode==STIMULUSMODE0)   // ha appena presentato lo stimolo del factor
            {
                resp=true;
            }
            if (prevTrialMode==STIMULUSMODE1)   // ha appena presentato lo stimolo della staircase
            {
                resp=false;
            }

            responseFile << trialNumber << "\t" <<
                            factors["ScaleZ"] <<"\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getID()  << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getState() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getInversions() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getAscending()<< "\t" <<
                            responsekey << "\t" <<
                            resp << "\t" <<
                            trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                            prevTrialMode << endl;

            trial.next(resp);

            if ( trial.isEmpty() )  // The experiment finished
            {
                cerr << "Experiment finished!" << endl;
                exit(0);
            }

            factorStaircasePair = trial.getCurrent();
            factors = factorStaircasePair.first;
            trialNumber++;
            radiusZ = radiusX*factorStaircasePair.second->getCurrentStaircase()->getState();

            textureWidthFactorStim =  mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factors["ScaleZ"]/2),textureHeight/2)*0.5;
            textureWidthStaircaseStim = mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factorStaircasePair.second->getCurrentStaircase()->getState()/2),textureHeight/2)*0.5;
            textureWidthMaskStim = mathcommon::ellipseCircumferenceBetter(0,textureHeight/2)*0.5;

            // We must first delete the texture on GPU!!!
            glDeleteTextures(1,&texIdFactor);
            glDeleteTextures(1,&texIdStaircase);
            glDeleteTextures(1,&texIdMask);

            int ncirclesFactorStim =util::str2num<double> (parameters.find("CirclesPerPixelSquare"))*textureWidthFactorStim*textureHeight;
            int ncirclesStaircaseStim = util::str2num<double>(parameters.find("CirclesPerPixelSquare"))*textureWidthStaircaseStim*textureHeight;
            int ncirclesMask = util::str2num<int>(parameters.find("NumMaskCircles"));
            int dotsRadiusMin = util::str2num<int>(parameters.find("DotsRadiusMin"));
            int dotsRadiusMax = util::str2num<int>(parameters.find("DotsRadiusMax"));

            //bool addRandomNoise = (bool)util::str2num<int>(parameters.find("AddRandomNoise"));
            //double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));
            bool usePolkaDotsReference = (bool)util::str2num<int>(parameters.find("UsePolkaDotsReference"));
            bool usePolkaDotsStaircase = (bool)util::str2num<int>(parameters.find("UsePolkaDotsStaircase"));
            texIdFactor = generateCirclesTexture(textureWidthFactorStim,textureHeight,ncirclesFactorStim,dotsRadiusMin,dotsRadiusMax,false,false,false,usePolkaDotsReference);
            texIdStaircase = generateCirclesTexture(textureWidthStaircaseStim,textureHeight,ncirclesStaircaseStim,dotsRadiusMin,dotsRadiusMax,false,false,false,usePolkaDotsStaircase);
            texIdMask = generateCirclesTexture(textureWidthMaskStim,textureHeight,ncirclesMask,10,25,false);

            updateStimulusRandomPoints(radiusX,height,radiusX*factors.at("ScaleZ"),radiusZ);
            //if (rand()%2)
                //std::swap(trialmodes[0],trialmodes[2]);

            trialIndex=0;
            trialMode=trialmodes[trialIndex];

            trialTimer.start();
        }
    }
        break;
    case '6':
    {
        if (trialMode==PROBEMODE)
        {
            int responsekey=6;
            bool resp=false;
            if (prevTrialMode==STIMULUSMODE0)   // ha appena presentato lo stimolo del factor
            {
                resp=false;
            }
            if (prevTrialMode==STIMULUSMODE1)   // ha appena presentato lo stimolo della staircase
            {
                resp=true;
            }
            responseFile << trialNumber << "\t" <<
                            factors["ScaleZ"] <<"\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getID()  << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getState() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getInversions() << "\t" <<
                            factorStaircasePair.second->getCurrentStaircase()->getAscending() << "\t" <<
                            responsekey << "\t" <<
                            resp << "\t" <<
                            trialTimer.getElapsedTimeInMilliSec() << "\t" <<
                            prevTrialMode << endl;
            trial.next(resp);

            if ( trial.isEmpty() )  // The experiment finished
            {
                cerr << "Experiment finished!" << endl;
                exit(0);
            }

            factorStaircasePair = trial.getCurrent();
            factors = factorStaircasePair.first;

            trialNumber++;
            radiusZ = radiusX*factorStaircasePair.second->getCurrentStaircase()->getState();

            textureWidthFactorStim =  mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factors["ScaleZ"]/2),textureHeight/2)*0.5;
            textureWidthStaircaseStim = mathcommon::ellipseCircumferenceBetter(abs(textureHeight*factorStaircasePair.second->getCurrentStaircase()->getState()/2),textureHeight/2)*0.5;
            textureWidthMaskStim = mathcommon::ellipseCircumferenceBetter(0,textureHeight/2)*0.5;

            // We must first delete the texture on GPU!!!
            glDeleteTextures(1,&texIdFactor);
            glDeleteTextures(1,&texIdStaircase);
            glDeleteTextures(1,&texIdMask);

            int ncirclesFactorStim =util::str2num<double> (parameters.find("CirclesPerPixelSquare"))*textureWidthFactorStim*textureHeight;
            int ncirclesStaircaseStim = util::str2num<double>(parameters.find("CirclesPerPixelSquare"))*textureWidthStaircaseStim*textureHeight;
            int ncirclesMask = util::str2num<int>(parameters.find("NumMaskCircles"));
            int dotsRadiusMin = util::str2num<int>(parameters.find("DotsRadiusMin"));
            int dotsRadiusMax = util::str2num<int>(parameters.find("DotsRadiusMax"));

            //bool addRandomNoise = (bool)util::str2num<int>(parameters.find("AddRandomNoise"));
            //double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));
            bool usePolkaDotsReference = (bool)util::str2num<int>(parameters.find("UsePolkaDotsReference"));
            bool usePolkaDotsStaircase = (bool)util::str2num<int>(parameters.find("UsePolkaDotsStaircase"));

            texIdFactor = generateCirclesTexture(textureWidthFactorStim,textureHeight,ncirclesFactorStim,dotsRadiusMin,dotsRadiusMax,false,false,0.0,usePolkaDotsReference);
            texIdStaircase = generateCirclesTexture(textureWidthStaircaseStim,textureHeight,ncirclesStaircaseStim,dotsRadiusMin,dotsRadiusMax,false,false,false,usePolkaDotsStaircase);
            texIdMask = generateCirclesTexture(textureWidthMaskStim,textureHeight,ncirclesMask,10,25);

             updateStimulusRandomPoints(radiusX,height,radiusX*factors.at("ScaleZ"),radiusZ);
            //if (rand()%2)
                //std::swap(trialmodes[0],trialmodes[2]);

            trialIndex=0;
            trialMode=trialmodes[trialIndex];

            trialTimer.start();
            //trialMode=rand()%2;
        }
    }
        break;
    case 13:
    {
        if (experimentStarted)
            break;
        experimentStarted=true;
        trialTimer.start();
    }
        break;
    }
}


void mouseFunc(int state, int button, int _x , int _y)
{
    glutPostRedisplay();
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
    frame+=(double)1.0/(double)TIMER_MS;
}

void idle()
{
    if (!experimentStarted)
        return;

    eyeLeft = Vector3d(-interoculardistance/2,0,0);
    eyeRight = -eyeLeft;

    prevTrialMode=trialmodes[2];
    if (trialTimer.getElapsedTimeInMilliSec() > stimulusTime  && (trialMode==STIMULUSMODE0 || trialMode==STIMULUSMODE1) )
    {
        trialIndex++;
        trialIndex %= 4;
        // cerr << "Stimulus " << trialMode <<" time= " << trialTimer.getElapsedTimeInMilliSec() << endl;
        trialMode=trialmodes[trialIndex];
        trialTimer.start();
        return;
    }

    if (trialTimer.getElapsedTimeInMilliSec() > maskTime  && (trialMode==MASKMODE) )
    {
        trialIndex++;
        trialIndex %= 4;
        trialMode=trialmodes[trialIndex];
        //cerr << "Mask time= " << trialTimer.getElapsedTimeInMilliSec() << endl;
        trialTimer.start();
        return;
    }
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
}

void loadShaders()
{
    glsl::glShaderManager *SM = new glsl::glShaderManager();
    const GLcharARB vertexshader[] =
            STATIC_STRINGIFY
            (
                varying vec2 texture_coordinate;
            uniform int textureWidth;
    uniform int textureHeight;
    void main()
    {
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        texture_coordinate = vec2(gl_MultiTexCoord0.st);
    }
    );
    const GLcharARB fragmentshader[] =
            STATIC_STRINGIFY(
                varying vec2 texture_coordinate;
            uniform sampler2D my_color_texture;
    uniform float textureWidth;
    uniform float textureHeight;
    void main(void) {
        vec2 Tile = vec2(1.0/textureWidth,1.0/textureHeight );
        vec2 Blur = vec2(0.5,0.5);
        vec2 tilePos = vec2(fract(texture_coordinate.x / Tile.x), fract(texture_coordinate.y / Tile.y));
        vec2 p0 = vec2(floor(texture_coordinate.x / Tile.x) * Tile.x, floor(texture_coordinate.y / Tile.y) * Tile.y);
        vec2 p1 = vec2(clamp(p0.x - Tile.x, 0.0, 1.0), p0.y);
        vec2 p2 = vec2(p0.x, clamp(p0.y - Tile.y, 0.0, 1.0));
        vec2 p3 = vec2(p1.x, p2.y);
        vec2 mixFactors;
        mixFactors.x =  min(tilePos.x / Blur.x, 1.0);
        mixFactors.y =  min(tilePos.y / Blur.y, 1.0);
        vec4 tmp1 = mix(texture2D(my_color_texture, p1+(Tile/2.0)), texture2D(my_color_texture, p0+(Tile/2.0)), mixFactors.x);
        vec4 tmp2 = mix(texture2D(my_color_texture, p3 +(Tile/2.0)), texture2D(my_color_texture, p2+(Tile/2.0)), mixFactors.x);
        gl_FragColor = mix(tmp2, tmp1, mixFactors.y);//*vec4(1.0,0,0.0,1.0); // color it in red;
    }
    );
    shader = SM->loadfromMemory(vertexshader,fragmentshader);
    delete SM;
}

int main(int argc, char*argv[])
{
    cout << "Select Horizontal 'h' or Vertical 'v' session" << endl;
    char val='h';
    //cin >> val;
    horizontal = val=='h';

    if ( val!='h' && val !='v' )
    {
        cerr << "You must specify 'h' or 'v' and then press enter to select horizontal or vertical" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }

    mathcommon::randomizeStart();

    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO | GLUT_MULTISAMPLE);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP TEXTURING STAIRCASE");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initStreams();
    initVariables();

    loadShaders();
    initScreen();
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();

    return 0;
}
