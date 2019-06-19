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

#define SAVEDATADAT

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>  //include asio in order to avoid the "winsock already declared problem"

#ifdef __linux__
#include "beep.h"
#endif

#include "IncludeGL.h"

#include "GLTexture.h"
#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "GLUtils.h"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
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
#include "TriesteMotorFunctions.h"
#include "EulerExtractor.h"
#include "LatestCalibrationTrieste.h"

#define SCREENSPEED 4500
#define RODSPEED 4500

/************* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;
using namespace TriesteMotorFunctions; //scommentato
//using namespace boost::filesystem;

//LS
Arcball arcball;
GLUquadricObj* myReusableQuadric;
GLTexture tex[9];
//

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Optotrak2 optotrak;
CoordinatesExtractor2 headEyeCoords,thumbCoords, indexCoords;
EulerExtractor eulerAngles;


double focalDistance= baseFocalDistance, homeFocalDistance=baseFocalDistance;
static const Vector3d center(0,0,focalDistance);

//static const Vector3d rodTipHome(331.77, -272.35, -805.97);
static const Vector3d rodTipHome(494.90, 101.75, -751.03); /// coordinate nel sistema di riferimento Optotrack dell'oggetto
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
int isDrawing = trialMode;
bool trialOk=true;
ParametersLoader parameters;
BalanceFactor<double> trial;
BalanceFactor<int> module;
map <std::string, double> factors;
map <std::string, int> block;
double fingerDistance=0;
int answer=0;

/********* STIMULI *******/
EllipsoidPointsStimulus ellipseBaseCylinder;
StimulusDrawer stimDrawer[2];
double stimulusDensity=0.025;	// points per mm^2, it means 150 points on a h=70, radius=15 cylinder area
// PRE-POST TEST PHASE
// ADAPTATION PHASE VARIABLES
double adaptStimHeight, adaptStimRadius;
vector<double> adaptOffsets;
double ballRadius;
double deltaXRods=0;

Timer globalTimer,delayedTimer,totalTime, frameTimer;
int trialFrame=0,totalFrame=0,drawingTrialFrame=0;
int occludedFrames=0;
int block0TrialNumber=0, block1TrialNumber=0,block2TrialNumber=0, totalTrialNumber=0;
bool isMovingRod=false;

int timeInsideCircle=0; // Modificato

/******** STREAMS AND FILES *******/
ofstream markersFile;
ofstream responseFile;
/******** FUNCTION PROTOTYPES ******/
void beepOk();
void beepBad();
void beepTrial();
void advanceTrial();
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
void initStimulus(double h, double rx, double rz);
void initOptotrak();
void initRendering();
void drawImage();

int paramIndex=0;
char *parametersFileNames[] = {"parametersExpFingerFace.txt"};

/********* SOUNDS ********/

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

void drawImage()
{
    /*
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(-1,-1,0);

    glTexCoord2d(1,0);
    glVertex3d(1,-1,0);

    glTexCoord2d(1,1);
    glVertex3d(1,1,0);

    glTexCoord2d(0,1);
    glVertex3d(-1,1,0);
    glEnd();
    tex.use();
    */
    //tex.drawFrame(50,50,0);
}


/*void drawFixation()
{
    glPushAttrib(GL_POINT_BIT | GL_COLOR_BUFFER_BIT );
    double circleRadius=5.0;
    glColor3fv(glRed);
	glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3d(0,0, factors.at("Distances"));
    glEnd();
    // timeInsideCircle++;

        //if ( timeInsideCircle > 200 )
        //{
			// timeInsideCircle= 0.0;
            // trialMode=STIMULUSMODE;
            boost::thread okBeep( beepOk );
            okBeep.detach();
          // }

    glPopAttrib();
}*/

void initStimulus(double height, double longAxis, double shortAxis )
{
    double ellipticCircumferenceApproximate = 4*3.1415*longAxis*shortAxis/(longAxis+shortAxis);
    // Evitiamo di calcolarci la circonferenza dell'ellisse con la mega formula tanto cambia ben poco alla fine...

    int nStimulusPoints=2*(int)floor(ellipticCircumferenceApproximate*height*stimulusDensity);

    ellipseBaseCylinder.setNpoints(nStimulusPoints);
    ellipseBaseCylinder.setAperture(0,2*M_PI);
    ellipseBaseCylinder.setAxesAndHeight(longAxis,shortAxis,height);
    ellipseBaseCylinder.setFluffiness(0.001);
    ellipseBaseCylinder.compute();

    // update the stimulus drawer
    stimDrawer[0].setSpheres(false);
    stimDrawer[0].setStimulus(&ellipseBaseCylinder);
    stimDrawer[0].initList(&ellipseBaseCylinder,glRed,3);

    stimDrawer[1].setSpheres(false);
    stimDrawer[1].setStimulus(&ellipseBaseCylinder);
    stimDrawer[1].initList(&ellipseBaseCylinder,glRed,3);
}

void keyPressed()
{
    if (totalTrialNumber==0)
    {
        responseFile << "TrialNumber Block Face DelayedT GlobalT Answer" << endl;
    }
    responseFile << fixed <<
                 totalTrialNumber << " " <<
                 block.at("Phase") << " " <<
                 (int)factors.at("Faces") << " " <<
                 delayedTimer.getElapsedTimeInMilliSec()<< " " <<
                 globalTimer.getElapsedTimeInMilliSec() << " " <<
                 answer << endl;
}

void advanceTrial()
{   if ( trialMode == STIMULUSMODE || (block.at("Phase") != 1 && parameters.find("Task") == "Perc"))
    {   bool pretrialMode=trialMode;
        trialMode++;
        trialMode=trialMode%2;
        totalTrialNumber++;

        cerr << "Advance Trial 272" << endl;
        // controlla se il numero di frame in cui il dito è occluso eccede un certo valore
        if (block.at("Phase") != 1)// && (parameters.find("Task") == "Grasp" || parameters.find("Task") == "MSE")) /// Modificato
        {
            double percentOccludedFrames = ((double)occludedFrames/(double)drawingTrialFrame )*100.0;
            if ( percentOccludedFrames > str2num<double>(parameters.find("TestPercentOccludedFrames")) )
            {
                cerr << "Percent occluded frames is" << percentOccludedFrames << " " << occludedFrames << " over " << drawingTrialFrame << endl;
                trial.reinsert(factors);

            }
            drawingTrialFrame=0;
            occludedFrames=0;
        }
        trialFrame=0; /// Azzera il contatore dei frame nel trial

        switch( block.at("Phase") )
        {
        case 0:  //pre-test
        {
            if ( !trial.isEmpty() )	// ci son ancora trial
            {   beepTrial();
                deltaXRods = mathcommon::unifRand(str2num<double>(parameters.find("DeltaXMin")),str2num<double>(parameters.find("DeltaXMax")));
                initStimulus(factors.at("StimulusHeight"),deltaXRods,factors.at("RelDepth"));
                factors = trial.getNext();

                cerr << "Advance Trial 297" << endl;

                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter  = rodAway;
                initProjectionScreen(visualRodCenter.z()); // scommentato
                moveMonitorAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED); //scommentato
                beepTrial();
            }
            else // si prepara per la modalità 1 (adaptation)
            {   trialMode = HANDONSTARTMODE;
                block = module.getNext();
                initStimulus(factors.at("StimulusHeight"),adaptStimRadius*2,adaptStimRadius*2);
                double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter=Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter=Vector3d(0,0, (zadaptmin+zadaptmax)/2);
                initProjectionScreen( visualRodCenter.z()); //scommentato

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 ) {
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                }
                else {
                    hapticRodCenter = rodAway;
                    drawGLScene(); // Aggiunto //// Deve cancellare la schermata precedente prima di muovere i motori
                }

                moveRod(Vector3d(0,0,hapticRodCenter.z() ),RODSPEED); //scommentato

                checkBounds(); //TEMPORANEMENTE ELIMINATO
                //// Aggiunto
                if (block.at("Phase")!=1 && (parameters.find("Task") == "Perc"))
                {
                    indexInside[0]= false;
                    indexInside[1]= true;
                }
                beepLong();
            }
            block0TrialNumber++;
        }
        break;
        case 1:  // adaptation
        {
            block1TrialNumber++;
            if ( block1TrialNumber < str2num<int>(parameters.find("AdaptTrials")) )
            {   beepTrial();
                initStimulus(adaptStimHeight,adaptStimRadius*2,adaptStimRadius*2);
                double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter = Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter=Vector3d(0,0, (zadaptmin+zadaptmax)/2);

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 )
                {
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                    moveMonitorAbsoluteAsynchronous(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                    moveRod(hapticRodCenter,RODSPEED);
                }
                else
                {
                    hapticRodCenter = rodAway;
                    moveMonitorAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                }

                initProjectionScreen( visualRodCenter.z());
                beepTrial();
            }
            else
            {   beepLong();
                trialMode = HANDONSTARTMODE;
                block  = module.getNext();
                // reset the factors in order to prepare the next test phase
                trial.init(parameters);
                factors.clear();
                factors  = trial.getNext();
                deltaXRods = mathcommon::unifRand(str2num<double>(parameters.find("DeltaXMin")),str2num<double>(parameters.find("DeltaXMax")));
                //cerr << "DISTANCE= " << factors.at("Distances") << endl;
                initStimulus(factors.at("StimulusHeight"),deltaXRods,factors.at("RelDepth"));
                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter = rodAway;

                moveMonitorAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                moveRod(rodAway,RODSPEED);
                initProjectionScreen(visualRodCenter.z());

                beepTrial();
            }
        }
        break;
        case 2:  /// post-test
        {   if ( trial.isEmpty() )
            {   beepLong();
                exit(0);
            }
            else
            {   beepTrial();
                factors = trial.getNext();
                deltaXRods = mathcommon::unifRand(str2num<double>(parameters.find("DeltaXMin")),str2num<double>(parameters.find("DeltaXMax")));
                initStimulus(factors.at("StimulusHeight"),deltaXRods,factors.at("RelDepth"));
                visualRodCenter =  Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter  = rodAway;
                initProjectionScreen(visualRodCenter.z());
                moveMonitorAbsolute(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                beepTrial();
            }
            block2TrialNumber++;
        }
        break;
        }
        globalTimer.start();
    }
    // Mettere una idle così non perde il primo trial
    cerr << "Advance Trial 407" << endl;
    idle();
}

void initVariables()
{   trial.init(parameters);
    // module factor: 0 -> 1 -> 2
    module.init(1,false);
    ///LS: immagine utilizzate per la condizione Perc: Face
    // Tex può essere definito come Array tex[n] dove n è il numero di immagini-1
    // In questa maniera si creerà un fattore che va da 1 al numero di immagini utilizzate nel nostro
    // continuum di espressioni facciali e poi in DrawStimulus si andrà a presentare il tex[factor].drawFrame
    tex[0].loadBMP("C:/cncsvisioncmake/data/textures/Io-1.bmp");
    tex[1].loadBMP("C:/cncsvisioncmake/data/textures/Io-0.75.bmp");
    tex[2].loadBMP("C:/cncsvisioncmake/data/textures/Io-0.5.bmp");
    tex[3].loadBMP("C:/cncsvisioncmake/data/textures/Io-0.25.bmp");
    tex[4].loadBMP("C:/cncsvisioncmake/data/textures/Io0.bmp");
    tex[5].loadBMP("C:/cncsvisioncmake/data/textures/Io0.25.bmp");
    tex[6].loadBMP("C:/cncsvisioncmake/data/textures/Io0.5.bmp");
    tex[7].loadBMP("C:/cncsvisioncmake/data/textures/Io0.75.bmp");
    tex[8].loadBMP("C:/cncsvisioncmake/data/textures/Io1.bmp");

    ///

    module.addFactor("Phase",str2num<int>(parameters.find("Phase"),","));
    block = module.getNext();

    if ( parameters.exists("StimulusDensity") )
        stimulusDensity = str2num<double>(parameters.find("StimulusDensity"));
    else
        cerr << "Stimulus density parameters not found, set to 0.025..." << endl;

    adaptStimHeight = str2num<double>(parameters.find("AdaptStimulusHeight"));
    adaptStimRadius = str2num<double>(parameters.find("AdaptStimulusRadius"));

// crea la lista degli offset in adaptation dati start, end e numero steps
    int nTrials = str2num<int>(parameters.find("AdaptTrials"));
    double adaptOffsetStart = str2num<double>(parameters.find("AdaptOffsetStart"));
    double adaptOffsetEnd = str2num<double>(parameters.find("AdaptOffsetEnd"));
    int adaptOffsetNSteps = str2num<int>(parameters.find("AdaptOffsetSteps"));
    if ( adaptOffsetNSteps > nTrials )
    {   cerr << "too much steps, they must be at max " << nTrials << endl;
        cin.ignore(1e6,'\n');
        exit(0);
    }

    double adaptOffsetDelta = (adaptOffsetEnd - adaptOffsetStart)/adaptOffsetNSteps;
    for (int i=0; i<= adaptOffsetNSteps ; i++)
        adaptOffsets.push_back(i*adaptOffsetDelta+adaptOffsetStart);
    while ( adaptOffsets.size() != nTrials+2 ) //+2 così non rogna nel markersFile
        adaptOffsets.push_back(adaptOffsetEnd );

    ballRadius = str2num<double>(parameters.find("BallRadius"));
    initStimulus(adaptStimHeight,adaptStimRadius*2,adaptStimRadius*2);

    hapticRodCenter = Vector3d(0,0,focalDistance);

    indexInside[0]=indexInside[1]=false;
    globalTimer.start();
    frameTimer.start();

    trial.print(cerr);
}

void initStreams()
{   /*ifstream inputParameters;
    inputParameters.open(parametersFileNames[paramIndex]);
    if ( !inputParameters.good() )
    {   cerr << "File doesn't exist" << endl;
    //exit(0);
    }*/
    parameters.loadParameterFile("parametersExpFingerFace.txt");
    // WARNING:
    // Base directory and subject name, if are not
    // present in the parameters file, the program will stop suddenly!!!
    // Base directory where the files will be stored
    cerr << "Folders creation..." ;
    string baseDir = parameters.find("BaseDir");
    if ( !boost::filesystem::exists(baseDir) )
        boost::filesystem::create_directory(baseDir);
    // Subject name
    string subjectName = parameters.find("SubjectName");

    // Principal streams file
    string markersFileName  = "markersFile_" + subjectName + ".txt";
    string responseFileName =	"responseFile_"	+ subjectName + ".txt";

    // Check for output file existence
    if ( !fileExists((baseDir+markersFileName)) )
        markersFile.open((baseDir+markersFileName).c_str());
    if ( !fileExists((baseDir+responseFileName)) )
        responseFile.open((baseDir+responseFileName).c_str());
    cerr << " OK!" << endl;
}

/*************************** FUNCTIONS ***********************************/

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
        {
            text.draw("Phase 0 Trial number= " + stringify<int>(block0TrialNumber));
            text.draw("Phase 1 Trial number= " + stringify<int>(block1TrialNumber));
            text.draw("Phase 2 Trial number= " + stringify<int>(block2TrialNumber));
            text.draw("Trial mode " + stringify<int>(trialMode ));
            text.draw("DrawingTrialFrame= " + stringify<int>(drawingTrialFrame));
            text.draw("IsDrawing?= " + stringify<int>(isDrawing));
            text.draw("VisualRod= " + stringify<Eigen::Matrix<double,1,3> >(visualRodCenter.transpose()) );
            text.draw("HapticRod= " + stringify<Eigen::Matrix<double,1,3> >(hapticRodCenter.transpose()) );
            text.draw("AdaptOffsets= " + stringify< vector<double> >(adaptOffsets) );
            text.draw("ThisAdaptOffset= " + stringify< double >(adaptOffsets.at(block1TrialNumber)) );
            text.draw("Index= " + stringify< Eigen::Matrix<double,1,3> >(indexCoords.getP1().p ));
            text.draw("Thumb= " + stringify< Eigen::Matrix<double,1,3> >(thumbCoords.getP1().p ));
            text.draw("GlobTime= " + stringify<int>(globalTimer.getElapsedTimeInMilliSec()));
            if ( block.at("Phase")!=1 )
            {
                text.draw("CurrentFactor= " + stringify<int>(factors.at("Distances")) );
                text.draw("Remaining= " + stringify<int>( trial.getRemainingTrials() ));
            }
        }
        text.leaveTextInputMode();
    }
}

void drawStimulus()
{
    if ( isDrawing )
    {
        cerr << "IND " << visualIndex.z() << " " << visualRodCenter.z() << endl;
        if ( block.at("Phase") == 1 )
        {
            glPushMatrix();
            glLoadIdentity();
            glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z());
            stimDrawer[0].draw();
            glPopMatrix();
        }
        else
        {
            ///// Nella fase perc presenta prima un punto di fissazione e poi l'immagine texture
            if ( block.at("Phase")!=1 && isDrawing==1 && parameters.find("Task") == "Perc" && globalTimer.getElapsedTimeInMilliSec()<500) { //drawingTrialFrame<30){ //// Aggiunto
                drawCircle(5.0,0.0, 0.0, visualRodCenter.z(),glGreen50);
                // glLoadIdentity();
                // glTranslated(visualRodCenter.x()+deltaXRods/2, visualRodCenter.y(), visualRodCenter.z()-factors.at("RelDepth")*0.5);
            }
            else
            {
                //// LS
                if (parameters.find("Task") == "Perc")
                {
                    tex[(int)factors.at("Faces")].drawFrame(100,100,visualRodCenter.z());
                }
                else {
                    ////
                    glPushMatrix();
                    glLoadIdentity();
                    glTranslated(visualRodCenter.x(), visualRodCenter.y(), visualRodCenter.z());
                    stimDrawer[0].draw();
                    glPopMatrix();

                    glPushMatrix();
                    glLoadIdentity();
                    glTranslated(visualRodCenter.x()+deltaXRods/2, visualRodCenter.y(), visualRodCenter.z()-factors.at("RelDepth")*0.5);
                    stimDrawer[1].draw();
                }
                glPopMatrix();
            }
        }
    }
}


void drawFingerTips()
{   // ADAPTATION BLOCK

    //glBegin(GL_POINTS);
    //glVertex3dv(indexCoords.getP1().p.data());
    // glEnd();

    if ( block.at("Phase") == 1 && headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE && isDrawing ) //adaptation
    {
        glColor3fv(glRed);
        glPointSize(8);
        glBegin(GL_POINTS);
        switch ( str2num<int>(parameters.find("AdaptFinger")) )
        {
        case 0:
        {
            glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
        }
        break;
        case 1:
        {
            glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        }
        break;
        case 2:
        {
            glVertex3d(visualThumb.x(),visualThumb.y(), visualThumb.z());
            glVertex3d(visualIndex.x(),visualIndex.y(), visualIndex.z());
        }
        break;
        }
        glEnd();
        glPointSize(1);
    }
    // END ADAPTATION BLOCK
}

void drawTrial()
{   switch ( trialMode )
    {
    case HANDONSTARTMODE :
    {
        // disegna le dita per debug
        if ( block.at("Phase")==1 && isDrawing==1 )
            drawFingerTips();


    }
    break;
    case STIMULUSMODE:
    {
        // cerr<<globalTimer.getElapsedTimeInMilliSec()<<endl;
        // Disegna le dita solo durante la fase di adattamento
        if ( block.at("Phase")==1 && isDrawing==1 )
            drawFingerTips();
        drawStimulus();
    }
    break;
    }
}

void drawGLScene()
{   if (stereo)
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

void moveRod(const Vector3d& v, int speed, bool hasFeedback)
{   assert( (v.z() < homeFocalDistance) );	// too avoid overdisplacements!!!
    isMovingRod=true;
    if ( hasFeedback )
    {   // must be called 10 times!!!
        for ( int i=0; i<10; i++)
            idle();
        // gestisce l'offset del marker rispetto al centro del paletto
        double rodRadius=13;    //mm
        double markerThickness=3.5;

        Vector3d finalPosition=v-Vector3d(0,0,markerThickness+rodRadius/2);
        if ( !isVisible(finalPosition) )
        {   beepBad();
            cerr << "CAN'T MOVE MOTOR TO INFINITY!!!" << endl;
            return ;
        }
        // a precise feedback based misuration - 2 corrections
        for ( int i=0; i<2; i++)
        {   for ( int j=0; j<10; j++) // must be called 10 times!!!
                idle();
            moveObject(finalPosition-markers.at(15).p,speed);// sostituito markers.at(5) con markers.at(15); markers.at(15) marker oggetto
        }
    }
    else
    {
        moveObjectAbsolute(v,rodTipHome-calibration,speed);
    }
    isMovingRod=false;
}


void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {   exit(0);
    }
    break;
    case ' ':
    {   // Here we record the head shape - coordinates of eyes and markers, but centered in (0,0,0)
        if ( headCalibrationDone==0 && allVisibleHead )
        {
            headEyeCoords.init(markers.at(18).p-Vector3d(ChinRestWidth,0,0),markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interoculardistance );
            headCalibrationDone=1;
            beepOk();
            break;
        }
        if ( headCalibrationDone==1 && allVisiblePatch )
        {
            headCalibrationDone=3;
            beepOk();
        }
    }
    break;

    case 'i':
        infoDraw=!infoDraw;
        break;
    case '4':
    {
        if ( ( fingerCalibrationDone==3 && headCalibrationDone==3 ) && (block.at("Phase")  != 1) && (parameters.find("Task") == "Perc") && (globalTimer.getElapsedTimeInMilliSec()>1000))
        {
            cerr << "tasto 4 " << endl;
            cerr<< block0TrialNumber << endl;
            answer=true;
            // delayedTimer.start();
            beepOk();
            //// Fare funzione keyPressed dove salva il response file
            keyPressed();
            advanceTrial();
            beepOk();
            cerr << "tasto 4 post" << endl;
        }
    }
    break;
    case '6':
    {
        if ( ( fingerCalibrationDone==3 && headCalibrationDone==3 ) && (block.at("Phase")  != 1) && (parameters.find("Task") == "Perc") && (globalTimer.getElapsedTimeInMilliSec()>1000))
        {
            cerr << "tasto 6 " << endl;
            cerr<< block0TrialNumber << endl;
            answer=false;
            // delayedTimer.start();
            keyPressed();
            advanceTrial();
            cerr << "tasto 6 post" << endl;
        }
    }
    break;
    case 'f':
    case 'F':
    {   // Here we record the finger tip physical markers
        if ( allVisiblePlatform && (fingerCalibrationDone==0) )
        {
            platformIndex=markers.at(16).p;
            platformThumb=markers.at(15).p;
            fingerCalibrationDone=1;
            beepOk();
            break;
        }
        if ( (fingerCalibrationDone==1) && allVisibleFingers )
        {
            thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
            indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p );
            fingerCalibrationDone=2;
            // XXX sposto l'oggetto a home per permettere le dita di calibrarsi
            // TriesteMotorFunctions::homeObject(3500); //// XXX commentato: A TS no servi
            beepOk();
            break;
        }
        if ( fingerCalibrationDone==2  && allVisibleFingers )
        {   infoDraw=false;
            drawGLScene();
            physicalRigidBodyTip = indexCoords.getP1().p;
            fingerCalibrationDone=3;

            switch ( block.at("Phase") )
            {
            case 0:
            case 2:
            {   factors = trial.getNext();
                deltaXRods = mathcommon::unifRand(str2num<double>(parameters.find("DeltaXMin")),str2num<double>(parameters.find("DeltaXMax")));
                initStimulus(factors.at("StimulusHeight"),deltaXRods,factors.at("RelDepth"));
                moveMonitorAbsolute(factors.at("Distances"),homeFocalDistance,SCREENSPEED);
                initProjectionScreen(factors.at("Distances"));
                visualRodCenter = Vector3d(0,0,factors.at("Distances"));
                hapticRodCenter = rodAway;	// keep it away
                // moveRod(rodAway,RODSPEED); //// XXX
            }
            break;
            case 1:
            {   double zadaptmin = str2num<double>(parameters.find("AdaptZMin"));
                double zadaptmax = str2num<double>(parameters.find("AdaptZMax"));

                if (str2num<int>(parameters.find("AdaptMoveMonitor"))==1)
                    visualRodCenter = Vector3d(0,0,mathcommon::unifRand(zadaptmin,zadaptmax));
                else
                    visualRodCenter = Vector3d(0,0,(zadaptmax+zadaptmin)/2);

                if ( str2num<int>(parameters.find("AdaptHapticFeedback"))==1 )
                    hapticRodCenter = visualRodCenter - Vector3d(0,0,adaptOffsets.at(block1TrialNumber));
                else
                    hapticRodCenter = rodAway;

                initProjectionScreen( visualRodCenter.z() );
                initStimulus(str2num<double>(parameters.find("AdaptStimulusHeight")),adaptStimRadius*2,adaptStimRadius*2);
                moveMonitorAbsoluteAsynchronous(visualRodCenter.z(),homeFocalDistance,SCREENSPEED);
                moveRod(hapticRodCenter,RODSPEED);
            }
            break;
            }
            trialMode = HANDONSTARTMODE;
            fingerDistance = (indexCoords.getP1().p-thumbCoords.getP1().p).norm();
            beepTrial();
            trialFrame=0;
            globalTimer.start();
            totalTime.start();
            break;
        }
    }
    break;
    }
}


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
{   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(_focalDist);
    cam.init(screen);
}


void update(int value)
{   glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

void checkBounds()
{   if ( fingerCalibrationDone==3 && headCalibrationDone==3 )
    {
        switch ( trialMode )
        {   /*
            Vector3d trueIndex = indexCoords.getP1().p;
            Vector3d trueThumb = thumbCoords.getP1().p;
            case STIMULUSMODE:
            {  // Check if the index tip stays inside the stimulus
            Vector2d indexOnPlane = Vector2d(trueIndex.x(),trueIndex.z());
            Vector2d hapticRodCenterOnPlane = Vector2d(hapticRodCenter.x(), hapticRodCenter.z()+10);
            double stimHeight=0, stimRadius=0;
            switch ( block.at("Phase") )
            {  case 0:
            case 2:
            {  stimHeight = str2num<double>(parameters.find("TestStimulusHeight") );
            stimRadius = str2num<double>(parameters.find("TestStimulusRadius") );
            }
            break;
            case 1:
            {  stimHeight = str2num<double>(parameters.find("AdaptStimulusHeight") );
            stimRadius = str2num<double>(parameters.find("AdaptStimulusRadius") );
            }
            break;
            }
            bool yInside = (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))<(stimHeight/2) && (trueIndex.y()-(hapticRodCenter.y()+stimulusRodOffsetY))> (-stimHeight/2) ;
            if ( ( (indexOnPlane - hapticRodCenterOnPlane).norm() <= stimRadius ) && yInside )
            indexInside[0]=true;
            else
            indexInside[0]=false;
            }

            break;
            */

        case HANDONSTARTMODE: //handsonstart
        {   // Define a sphere around physicalRigidBodyTip in which the thumb must stay
            bool indexVisible = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
            if ((parameters.find("Task") == "Perc") && (block.at("Phase") != 1))
                indexVisible=true;
            if ( indexVisible )
            {   bool triggerCondition=false;
                if ( (parameters.find("Task") == "MSE" || parameters.find("Task") == "Perc") && (block.at("Phase") != 1) )
                    triggerCondition = (globalTimer.getElapsedTimeInMilliSec() < 1500);
                /*else if  ( (parameters.find("Task") == "Perc") && (block.at("Phase") != 1) )
                	triggerCondition = true;*/
                else
                    triggerCondition=((indexCoords.getP1().p  - physicalRigidBodyTip).norm() < ballRadius);

                if ( triggerCondition ) //5 centimeters
                {   indexInside[0]=true;
                    if ( indexInside[1]==false)
                        globalTimer.start();
                }
                else
                {
                    indexInside[0]=false;

                    double distance = (indexCoords.getP1().p  - physicalRigidBodyTip).norm();
                    // IMPORTANTE: controlla che il dito sia uscito ma esclude i casi in cui il dito esce da invisibile e diventa visibile fuori dalla palla... una soglia decente è 10 mm
                    if ( indexInside[1]==true && distance<(ballRadius+10) ) //è appena uscito
                    {   globalTimer.start();
                        trialMode=STIMULUSMODE;
                    }
                }
            }
        }
        break;
        }
    }
}

void idle()
{   // Set the time during which the stimulus is drawn, it depents on the phase (adaption or test)
    double drawStimTime = 0;
    if ( block.at("Phase") == 1 )
        drawStimTime = str2num<double>(parameters.find("AdaptStimulusDuration"));
    else
        drawStimTime = str2num<double>(parameters.find("TestStimulusDuration"));
    double deltaT = (double)TIMER_MS;
    optotrak.updateMarkers(deltaT);
    markers = optotrak.getAllMarkers();
    // Coordinates picker
    allVisiblePlatform = isVisible(markers.at(15).p) && isVisible(markers.at(16).p);
    allVisibleThumb = isVisible(markers.at(11).p) && isVisible(markers.at(12).p) && isVisible(markers.at(13).p);
    allVisibleIndex = isVisible(markers.at(7).p) && isVisible(markers.at(8).p) && isVisible(markers.at(9).p);
    allVisibleFingers = allVisibleThumb && allVisibleIndex;

    allVisiblePatch = isVisible(markers.at(1).p) && isVisible(markers.at(2).p) && isVisible(markers.at(3).p);
    allVisibleHead = allVisiblePatch && isVisible(markers.at(18).p);

    //cerr << markers.at(15).p.transpose() << endl; // stampa la coordinata del marker 15 preso come oggetto

    //if ( allVisiblePatch )
    headEyeCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,deltaT);
    // update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,deltaT);
    // update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,deltaT);

    eyeLeft = headEyeCoords.getLeftEye().p;
    eyeRight = headEyeCoords.getRightEye().p;

    checkBounds();

    indexInside[1]=indexInside[0];

    ///// Aggiunto
    if (block.at("Phase")!=1 && (parameters.find("Task") == "Perc"))
    {
        indexInside[0]= false;
        indexInside[1]= true;

    }

    // Controlla i frames occlusi, se il dito di test è fuori dalla sfera
    // ed ha superato una percentuale di tempo maggiore di
    // TestPercentOccludedFrames rispetto al tempo di presentazione dello
    // stimolo allora fa un beep

    // Aggiunto

    if (block.at("Phase")!=1 && (parameters.find("Task") == "MSE" || parameters.find("Task") == "Grasp")) {
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 && !indexInside[0] && (int) block.at("Phase")!=1 && isDrawing )
        {
            bool visibleFinger=true;
            //if  (parameters.find("Task") == "Grasp" || parameters.find("Task") == "MSE") // Aggiunto elimina il check se task è PERC
            //{
            switch ( str2num<int>(parameters.find("TestFinger")) )
            {
            case 0:
                visibleFinger=isVisible(thumbCoords.getP1().p);
                break;
            case 1:
                visibleFinger=isVisible(indexCoords.getP1().p);
                break;
            case 2:
                visibleFinger=isVisible(thumbCoords.getP1().p) && isVisible(indexCoords.getP1().p);
                break;
            }
            // XXX scommentare per far si che anche il polso venga controllato per missing frames
            //visibleFinger = visibleFinger && isVisible(markers.at(6).p);
            if ( !visibleFinger )
            {   occludedFrames++;
                if ( str2num<int>(parameters.find("AudioFeedback") ) )
                    boost::thread invisibleBeep( beepInvisible);
            }
            //}
            cerr << "Idle 1001 pre trialOK" << endl;
            trialOk=true;	// mentre disegna lo stimolo
            cerr << "Idle 1003 after trialOK" << endl;
        }

        // Qui durante la fase di delay dopo la scomparsa dello stimolo controlla che il numero
        // di frames occlusi non abbia superato il limite consentito

        if ( (isDrawing==0 && headCalibrationDone==3 && (block.at("Phase")!=1) && !indexInside[0]) && (parameters.find("Task") == "Grasp" || parameters.find("Task") == "MSE") ) // Modificato
        {
            double percentOccludedFrames = ((double)occludedFrames/(double)drawingTrialFrame )*100.0;
            trialOk=(percentOccludedFrames < str2num<double>(parameters.find("TestPercentOccludedFrames")));

        }
    }

    if (block.at("Phase")!=1 && parameters.find("Task") == "Perc") {
        occludedFrames=0;
        bool indexVisible = true;
        bool visibleFinger=true;
        trialOk=true; ///// Aggiunto
        // cerr << "Idle 1018 after trialOK" << endl;
    }

    // Calcola le coordinate delle dita offsettate durante la fase di adattamento
    if ( block.at("Phase") == 1 )
    {   double fingerOffset =   adaptOffsets.at(block1TrialNumber) ;
        switch ( str2num<int>(parameters.find("AdaptFinger")) )
        {
        case 0:  //index
            visualThumb = thumbCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            break;
        case 1:  //thumb
            visualIndex = indexCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            break;
        case 2:
        {   visualThumb = thumbCoords.getP1().p +  Vector3d(0,0,fingerOffset);
            visualIndex = indexCoords.getP1().p +  Vector3d(0,0,fingerOffset);
        }
        break;
        }
        drawingTrialFrame=0;
        occludedFrames=0;
    }
    if ( headCalibrationDone==3 && fingerCalibrationDone==3 && trialMode==STIMULUSMODE )
    {
        if ( globalTimer.getElapsedTimeInMilliSec() <= drawStimTime )
        {   isDrawing=1;
            delayedTimer.start();
        }
        else
        {   // non disegna nulla
            if ( delayedTimer.getElapsedTimeInMilliSec() < str2num<int>(parameters.find("DelayTime")) ) {
                isDrawing=0;
            }
            else
            {
                delayedTimer.start();
                ///// Avanza automaticamente solo se GRASP, MSE o in adattamento
                if ( (int) block.at("Phase") == 1 || (parameters.find("Task") == "Grasp" || parameters.find("Task") == "MSE"))
                    advanceTrial();
            }
        }
    }
    if ( (int) block.at("Phase") != 1 && isDrawing==1 )
    {   drawingTrialFrame++;
    }
#define WRITE

    int thisBlockTrialNumber=0;
    switch ( block.at("Phase") )
    {
    case 0:
        thisBlockTrialNumber=block0TrialNumber;
        break;
    case 1:
        thisBlockTrialNumber=block1TrialNumber;
        break;
    case 2:
        thisBlockTrialNumber=block2TrialNumber;
        break;
    }

    if ( !isMovingRod )
    {
        RowVector3d junk(9999,9999,9999);
// Write to file
        if ( headCalibrationDone==3 && fingerCalibrationDone==3 )
        {


            if ( totalFrame==0)
            {
                markersFile << fixed << setprecision(3) <<
                            "SubjectName\tAdaptFinger\tTestFinger\tAdaptOffset\tAdaptHapticFeedback\tFingerDist\tTotalTrial\tPhase\tTrialNumber\tTrialFrame\tdT\tTotTime\tIndexInside\tIsDrawing\tTrialOk\tVisualStimX\tVisualStimY\tVisualStimZ\tHapticRodX\tHapticRodY\tHapticRodZ\tfRelDepth\tfStimulusHeight\tfDistances\tDeltaXRods\tEyeLeftXraw\tEyeLeftYraw\tEyeLeftZraw\tEyeRightXraw\tEyeRightYraw\tEyeRightZraw\tWristXraw\tWristYraw\tWristZraw\tThumbXraw\tThumbYraw\tThumbZraw\tIndexXraw\tIndexYraw\tIndexZraw" << endl;
                totalFrame++;
            }
            markersFile << fixed << setprecision(3) <<
                        parameters.find("SubjectName") << "\t" <<
                        parameters.find("AdaptFinger") << "\t" <<
                        parameters.find("TestFinger") << "\t" <<
                        adaptOffsets.at(block1TrialNumber) << "\t" <<
                        parameters.find("AdaptHapticFeedback") << "\t" <<
                        fingerDistance << "\t" <<
                        totalTrialNumber << "\t" <<
                        block.at("Phase") << "\t" <<
                        thisBlockTrialNumber << "\t" <<
                        trialFrame << "\t" <<
                        deltaT*1000 << "\t" <<
                        totalTime.getElapsedTimeInMilliSec() << "\t" <<
                        indexInside[0] << "\t" <<
                        isDrawing << "\t" <<
                        trialOk << "\t" <<
                        visualRodCenter.transpose() << "\t" <<
                        hapticRodCenter.transpose() << "\t" <<
                        ((block.at("Phase")!=1) ? factors.at("RelDepth") : 9999) << "\t" <<
                        ((block.at("Phase")!=1) ? factors.at("StimulusHeight") : 9999) << "\t" <<
                        ((block.at("Phase")!=1) ? factors.at("Distances") : 9999) << "\t" <<
                        deltaXRods << "\t" <<
                        ( isVisible(eyeLeft) ? eyeLeft.transpose() : junk ) << "\t" <<
                        ( isVisible(eyeRight) ? eyeRight.transpose() : junk ) << "\t" <<
                        ( isVisible(markers.at(6).p) ? markers.at(6).p.transpose() : junk ) << "\t" <<
                        ( isVisible(thumbCoords.getP1().p) ? thumbCoords.getP1().p.transpose() : junk ) << "\t" <<
                        ( isVisible(indexCoords.getP1().p) ? indexCoords.getP1().p.transpose() : junk ) << endl;

        }
        trialFrame++;
    }
#ifdef SAVEDATADAT
    eulerAngles.init( headEyeCoords.getRigidStart().getFullTransformation().rotation() );
    // Write datafile to real-time streaming of data
    ofstream outputfile;
    outputfile.open("data.dat");
    outputfile << "Subject Name: " << parameters.find("SubjectName") << endl;
    outputfile << "AdaptHapticFeedback: " << parameters.find("AdaptHapticFeedback") << endl;
    outputfile << "AdaptFinger: " << parameters.find("AdaptFinger") << endl;
    outputfile << "TestFinger: " << parameters.find("TestFinger") << endl;
    outputfile << "IndexInside: " << indexInside[0] << endl;
    //outputfile << "Yaw: " << toDegrees(eulerAngles.getYaw()) << endl <<"Pitch: " << toDegrees(eulerAngles.getPitch()) << endl << "Roll: " << toDegrees(eulerAngles.getRoll()) << endl;
    outputfile << "EyeLeft: " <<  headEyeCoords.getLeftEye().p.transpose() << endl;
    outputfile << "EyeRight: " << headEyeCoords.getRightEye().p.transpose() << endl << endl;
    outputfile << "Thumb: " << thumbCoords.getP1().p.transpose() << endl;
    outputfile << "Index: " << indexCoords.getP1().p.transpose() << endl;
    outputfile << "Wrist: " << markers.at(6).p.transpose() << endl;
    outputfile << "IsDrawing: " << isDrawing << " TrialOk: " << trialOk << " OccludedFrames: " << occludedFrames << " DrawingTrialFrames: " << drawingTrialFrame << endl;
    outputfile << "HapticRodCenter: " << hapticRodCenter.transpose() << endl;
    outputfile << "VisualRodCenter: " << visualRodCenter.transpose() << endl;
    outputfile << "Markers(5): " << markers.at(5).p.transpose() << endl;
    outputfile << "AdaptOffset: " << adaptOffsets.at(block1TrialNumber) << endl;
    outputfile << "Phase: " << block.at("Phase") << endl;
    outputfile << "TrialNumber: " << thisBlockTrialNumber << endl;
    outputfile << "Total trials: " << totalTrialNumber << endl;
#endif
    frameTimer.start();
}

void initOptotrak()
{
    optotrak.setTranslation(frameOrigin);
    optotrak.init(LastAlignedFile,TS_N_MARKERS,TS_FRAMERATE,TS_MARKER_FREQ,TS_DUTY_CYCLE,TS_VOLTAGE);
    for (int i=0; i<100; i++)
    {   optotrak.updateMarkers();
        markers = optotrak.getAllMarkers();
    }
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

int main(int argc, char*argv[])
{
    randomizeStart();

    //// Temporaneo
    TriesteMotorFunctions::homeMirror(3500);
    TriesteMotorFunctions::homeMonitorAsynchronous(2000);
    TriesteMotorFunctions::homeObject(RODSPEED);
    TriesteMotorFunctions::moveObjectAbsoluteAsynchronous(Vector3d(300,0,-400),rodTipHome-calibration,RODSPEED);

    // Initializes the optotrak and starts the collection of points in background
    initOptotrak();
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( 640,480 );
        glutCreateWindow("EXP WEXLER");
        //glutFullScreen();
    }
    else
    {   glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initStreams();
    initVariables();    // must stay here because they explicitly contain call to gl functions
    drawGLScene();

    initProjectionScreen(focalDistance);

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
