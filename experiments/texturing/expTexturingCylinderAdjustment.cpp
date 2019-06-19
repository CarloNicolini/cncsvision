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
#include <boost/filesystem.hpp>

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

#include <Eigen/Core>
#include <Eigen/Geometry>

/********* #DEFINE DIRECTIVES **************************/
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace boost::filesystem;
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
static const bool gameMode=true;
static const bool stereo=true;
bool orthoMode=true;
bool isAntiAliased=true;
bool drawInfo=true;
/*** Streams File ***/
ofstream responseFile;
ifstream inputParameters;

/*** STIMULI and TRIAL variables ***/

// Trial related things
int trialNumber=0;
bool topView=false;
bool experimentStarted=false;
ParametersLoader parameters;
BalanceFactor<double> trial;
map <string, double> factors;
StimulusDrawer stimDrawerBlack,stimDrawerRed;
EllipsoidPointsStimulus cylinderPointsWhite,cylinderPointsRed;
static const int textureHeight = 1024;

int textureWidth;

static const int STIMULUSMODE=0;
static const int PROBEMODE=1;
int trialMode = STIMULUSMODE;

/*** EXPERIMENTAL VARIABLES ***/
string subjectName;

/*** GLSL Shaders thing ***/

glsl::glShader *shader;

GLint texId;
float radiusX=10.0;
float height=radiusX;
float scaleZ=1.0;

bool horizontal=true;
Timer trialTimer;
double stimulusTime=1000; //presentation time in milliseconds, is set to 1000 by default, must be specified in the parameters

/*************************** FUNCTIONS ***********************************/
void beepOk(int tone)
{
#ifdef WIN32
    switch(tone)
    {
    case 0:
        // Remember to put double slash \\ to specify directories!!!
        PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-1.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case 1:
        PlaySound((LPCSTR) "C:\\cygwin\\home\\visionlab\\workspace\\cncsvision\\data\\beep\\beep-6.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    }
    return;
#endif
}

void updateStimulusRandomPoints(double radiusX, double height, double radiusZ)
{
    int nDots =util::str2num<double> (parameters.find("RandomNoiseDensity"))*mathcommon::ellipseCircumferenceBetter(radiusX,radiusZ)*height;

    // Cylinder staircase
    cylinderPointsRed.setNpoints(nDots/2);
    cylinderPointsRed.setAperture(0,M_PI);
    cylinderPointsRed.setFluffiness(0.0);
    cylinderPointsRed.setAxesAndHeight(radiusX*2,radiusZ*2,height*2);
    cylinderPointsRed.compute();

    // Cylinder mask
    cylinderPointsWhite.setNpoints(nDots/2);
    cylinderPointsWhite.setAperture(0,M_PI);
    cylinderPointsWhite.setFluffiness(0.0);
    cylinderPointsWhite.setAxesAndHeight(radiusX*2,radiusZ*2,height*2);
    cylinderPointsWhite.compute();

    stimDrawerRed.setStimulus(&cylinderPointsRed);
    stimDrawerBlack.setStimulus(&cylinderPointsWhite);

    stimDrawerRed.setSpheres(false);
    stimDrawerBlack.setSpheres(false);

	const GLfloat *color1,*color2;
	if ( parameters.find("DotsColor")=="BlackAndRed" )
	{
		color1=glRed;
		color2=glBlack;
	}
	if ( parameters.find("DotsColor")=="White" )
		color1=color2=glWhite;

    stimDrawerRed.initList(&cylinderPointsRed,color1,util::str2num<int>(parameters.find("RandomDotsSize")));
    stimDrawerBlack.initList(&cylinderPointsWhite,color2,util::str2num<int>(parameters.find("RandomDotsSize")));
}

void advanceTrial()
{
    if (!trial.next())
        exit(0);
    double oldScaleZ=factors["ScaleZ"];

    trialNumber++;
    textureWidth =  mathcommon::ellipseCircumferenceBetter(textureHeight*trial.getCurrent()["ScaleZ"]/2,textureHeight/2)*0.5;
    // We must first delete the texture on GPU!!!
    GLuint texToDelete = texId;
    glDeleteTextures(1,&texToDelete);

    int ncircles=util::str2num<double> (parameters.find("CirclesPerPixelSquare"))*textureWidth*textureHeight;
    int dotsRadiusMin = util::str2num<int>(parameters.find("DotsRadiusMin"));
    int dotsRadiusMax = util::str2num<int>(parameters.find("DotsRadiusMax"));


    //bool addRandomNoise = (bool)util::str2num<int>(parameters.find("AddRandomNoise"));
    //double noiseDensity = util::str2num<double>(parameters.find("RandomNoiseDensity"));
    bool usePolkaDots = (bool)util::str2num<int>(parameters.find("UsePolkaDots"));
    texId = generateCirclesTexture(textureWidth,textureHeight,ncircles,dotsRadiusMin,dotsRadiusMax,false,false,false,usePolkaDots);

    responseFile <<
                    trialNumber << "\t" <<
                    oldScaleZ << "\t" <<
                    scaleZ << endl;
    factors = trial.getCurrent();

    updateStimulusRandomPoints(radiusX,height,factors.at("ScaleZ")*radiusX);

    // Randomize scaleZ again
    scaleZ= 1 + (double)mathcommon::unifRand(-10,10)/10;    //This is needed to randomize between trials


}

void drawStimulus()
{

    glPushMatrix();
    shader->begin();
    shader->setUniform1f("textureWidth",(float)textureWidth);
    shader->setUniform1f("textureHeight",(float)textureHeight);
    if (horizontal)
        glRotated(90,0,0,1);
    drawCylinderElliptic( radiusX,height,radiusX*factors["ScaleZ"],2.5*1E-2);
    shader->end();
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance+0.5);
    if (horizontal)
		glRotated(90,0,0,1);
    stimDrawerRed.draw();
    stimDrawerBlack.draw();
    glPopMatrix();
}

void drawCylinderProbe()
{
    glPushMatrix();
    glLoadIdentity();
    if (horizontal)
        drawEllipse( radiusX*(scaleZ), radiusX,focalDistance,-M_PI/2,M_PI/2);
    else
        drawEllipse( radiusX, radiusX*(scaleZ),focalDistance,-M_PI,0);
    glPopMatrix();
}

void drawTrial()
{
    if (!experimentStarted)
        return;

    if (drawInfo)
    {
        GLText text;
        text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
        text.enterTextInputMode();
        for (map<string,double>::iterator iter = factors.begin(); iter!=factors.end(); ++iter)
        {
            text.draw(iter->first + " = " + util::stringify<double>(iter->second) );
        }
        text.draw("RandomBaseScaleZ = " + util::stringify<double>(scaleZ));
        text.leaveTextInputMode();
    }
    if ( trialMode == PROBEMODE )
        drawCylinderProbe();
    if (trialMode == STIMULUSMODE )
        drawStimulus();
}

void initVariables()
{
    cam.setOrthoGraphicProjection((bool)str2num<int>(parameters.find("OrthographicMode")));
    drawInfo = (bool)util::str2num<int>(parameters.find("DrawInfo"));
    //cam.setOrthoGraphicProjection(orthoMode);
    interoculardistance = str2num<double>(parameters.find("IOD"))*str2num<double>(parameters.find("IODFactor"));
    cerr << "COMPUTED IOD=" << interoculardistance  << endl;
    trial.init(parameters);

    radiusX = util::str2num<double>(parameters.find("RadiusX"));
    height = util::str2num<double>(parameters.find("CylinderHeight"));

    stimulusTime=util::str2num<double>(parameters.find("StimulusTime"));
    trialTimer.start();

    trial.next();
    factors = trial.getCurrent();
    textureWidth =  mathcommon::ellipseCircumferenceBetter(textureHeight*factors["ScaleZ"]/2,textureHeight/2)*0.5;

    int ncircles=util::str2num<double> (parameters.find("CirclesPerPixelSquare"))*textureWidth*textureHeight;
    int dotsRadiusMin = util::str2num<int>(parameters.find("DotsRadiusMin"));
    int dotsRadiusMax = util::str2num<int>(parameters.find("DotsRadiusMax"));


    bool usePolkaDots = (bool)util::str2num<int>(parameters.find("UsePolkaDots"));
    texId = generateCirclesTexture(textureWidth,textureHeight,ncircles,dotsRadiusMin,dotsRadiusMax,false,false,false,usePolkaDots);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);

    // Initialize the stimulus drawer with the proper cylinder
    updateStimulusRandomPoints(radiusX,height,factors.at("ScaleZ")*radiusX);
}

void initStreams()
{
    string parametersFileName("C:/cncsvisiondata/parametersFiles/Fulvio/expTexturing/parametersExpTexturingAdjustment.txt");
    //string parametersFileName("/home/carlo/Desktop/params/parametersExpTexturingAdjustment.txt");
    inputParameters.open(parametersFileName.c_str());
    if ( !inputParameters.good() )
    {
        cerr << "File " << parametersFileName << " doesn't exist, enter a valid path, press Enter to exit" << endl;
        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
        exit(0);
    }
    parameters.loadParameterFile(inputParameters);

    string subjectName = parameters.find("SubjectName");
    string outputFilename = parameters.find("BaseDir")+string("responseFileAdjustment_") + subjectName + string(".txt");
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
{   glClearColor(0.0,0.0,0.0,1.0);
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
    //cam.setOrthoGraphicProjection(true);
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
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    //Quit program
    case 'q':
    case 27:
    {
        GLuint texToDelete = texId;
        glDeleteTextures(1,&texToDelete);
        exit(0);
    }
        break;
    case 'i':
        drawInfo=!drawInfo;
        break;
    case 13:
    {
        if (!experimentStarted)
        {
            experimentStarted=true;
            beepOk(0);
            trialMode=STIMULUSMODE;
            trialTimer.start();
        }
        if (trial.isEmpty())
        {
            trialNumber++;
            responseFile <<
                            trialNumber << "\t" <<
                            factors["ScaleZ"] << "\t" <<
                            scaleZ << endl;
            exit(0);
        }
        if (trialMode==PROBEMODE)
        {
            trialTimer.start();
            beepOk(0);
            trialMode=STIMULUSMODE;
            advanceTrial();
        }
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
    eyeLeft = Vector3d(-interoculardistance/2,0,0);
    eyeRight = -eyeLeft;

    if (!experimentStarted)
        return;

    if ( trialMode == STIMULUSMODE && trialTimer.getElapsedTimeInMilliSec() > stimulusTime )
    {
        trialMode= PROBEMODE;
        /*
        if (trial.isEmpty())
        {
            trialNumber++;
            responseFile <<
                            trialNumber << "\t" <<
                            factors["ScaleZ"] << "\t" <<
                            scaleZ << endl;
        }
        */
        trialTimer.start();
    }
}

void initScreen()
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
	cam.setNearFarPlanes(300,600);
}

void loadShaders()
{
    glsl::glShaderManager *SM = new glsl::glShaderManager();
    
    const GLcharARB vertexshader[] =
            STATIC_STRINGIFY(
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
        gl_FragColor = mix(tmp2, tmp1, mixFactors.y);//*vec4(1.0,0.0,0.0,1.0); // color it in red
    }
    );
    shader = SM->loadfromMemory(vertexshader,fragmentshader);
    delete SM;
}

void arrowFunc(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
    {
        if (horizontal)
        {
            scaleZ+=util::str2num<double>(parameters.find("EllipseStepSize"));
        }
    }
        break;
    case GLUT_KEY_RIGHT:
    {
        if (horizontal)
        {
            scaleZ-=util::str2num<double>(parameters.find("EllipseStepSize"));
        }
    }
        break;
    case GLUT_KEY_UP:
    {
        if (!horizontal)
            scaleZ-=util::str2num<double>(parameters.find("EllipseStepSize"));
    }
        break;
    case GLUT_KEY_DOWN:
    {
        if (!horizontal)
            scaleZ+=util::str2num<double>(parameters.find("EllipseStepSize"));
    }
        break;
    }
}

int main(int argc, char*argv[])
{
    cout << "Select Horizontal 'o' or Vertical 'v' session" << endl;
    char val='h';
    cin >> val;
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
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO | GLUT_MULTISAMPLE );
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    if ( gameMode==false )
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("EXP TEXTURING ADJUSTMENT");
        //glutFullScreen();
    }
    else
    {   glutGameModeString("1024x768:32@100");
        glutEnterGameMode();
        glutFullScreen();
    }
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    
    initRendering();
    initScreen();
    initStreams();
    initVariables();

    loadShaders();

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(mouseFunc);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSpecialFunc(arrowFunc);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */

    glutMainLoop();

    return 0;
}
