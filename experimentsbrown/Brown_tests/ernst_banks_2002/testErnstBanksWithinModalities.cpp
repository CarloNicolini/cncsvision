///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unistd.h>

/**** BOOOST MULTITHREADED LIBRARY *********/
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>	//include asio in order to avoid the "winsock already declared problem"

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
//#include "Optotrak.h"
#include "Mathcommon.h"
#include "GLUtils.h"
#include "CoordinatesExtractor.h"
#include "CylinderPointsStimulus.h"
#include "StimulusDrawer.h"
#include "GLText.h"
#include "TrialGenerator.h"
#include "ParametersLoader.h"
#include "Util.h"


/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;
using namespace util;

/********* #DEFINE DIRECTIVES **************************/
#define TIMER_MS 11 // 85 hz
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
static const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* 18 October 2011   CALIBRATION ON CHIN REST *****/
static const Vector3d calibration(160,179,-75);
//static const Vector3d objCalibration(199.1, -149.2, -319.6);
// Alignment between optotrak z axis and screen z axis
double alignmentX =  33.5;
double alignmentY =  33;
double focalDistance= -270.0, homeFocalDistance=-270.0;
double interoculardistance=0.0;
static const Vector3d center(0,0,focalDistance);
double mirrorAlignment=0.0, screenAlignmentY=0.0, screenAlignmentZ=0.0;	// to keep track of mirror and monitor calibration
Screen screen;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=true;

/*----------------------------------------------- THIS EXPERIMENT SPECIFIC VARIABLES --------------------------------------------------------*/

/********** VISUALIZATION AND STIMULI ***************/
StimulusDrawer draw_ernst_banks[3];
CylinderPointsStimulus ernst_banks[3];
Timer timer;
Timer globalTimer;

double *s_pointsX;
double *s_pointsY;
double *s_pointsZ;
double *c_pointsX;
double *c_pointsY;
double *c_pointsZ;
int numPoints=0;
double *sizePoints;
double sizePointsF = 0.0;
double max_c_pointX = 0.0;
double min_c_pointX = 0.0;
double max_c_pointZ = 0.0;
double min_c_pointZ = 0.0;

/********* TRIAL VARIABLES *********/
static const int THREERODS=0;
int trialMode = THREERODS;
int trialNumber = 0;

/* PARAMETERS FILES */
ParametersLoader parameters, parametersOpenLoop, parametersClosedLoop;

int condition = 0;

/* EXPERIMENT MATRIX */
TrialGenerator<double> trial;
int responsekey=0, overwrite=0;
int frameN=0;
double stimulus_height[2] = {0.0, 0.0};

/* OTHERS */
int first_interval = 0, second_interval = 0, whichStimulus = 0;
double absDepth = 0.0, stimulus_noise = 0.0;
bool showStimuli = false;

/********** STREAMS **************/
ofstream responseFile;

/*----------------------------------------------------------------------------------------------------*/
/*----------------------------------------- END OF VARIABLES -----------------------------------------*/
/*----------------------------------------------------------------------------------------------------*/

/*************************** EXPERIMENT SPECS ****************************/

// paramters file directory and name
string parametersFile_directory = "/home/carlo/workspace/cncsvisioncmake/experimentsbrown/Brown_tests/ernst_banks_2002/parameters_fall14ErnstBanks_test1.txt";

// experiment directory
//string experiment_directory = "/home/carlo/Copy/Shape Lab Shared/Fall14/ErnstBanks_test_1/";
string experiment_directory = "/media/carlo/DATA/Copy/Shape Lab Shared/Fall14/ErnstBanks_test_1/";

// response file name
string responseFile_name = "fall14_ernst_banks_test1_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tstairID\tbar_height\treversals\tstairAscending\tstepsDone\tfirst_stimulus\tsecond_stimulus\ttrialDuration";

/*----------------------------------------- FUNCTIONS -----------------------------------------*/

/********** FUNCTION PROTOTYPES *****/

/* MOSTLY FIXED FUNCTIONS */
void handleKeypress(unsigned char key, int x, int y);	// manage keyboard presses
void initVariables();	// initialize experiment matrix in a BalanceFactor- or TrialGenerator-type of variable (usually trial)
void initStreams();	// read from parameters file and create+open the output files
void initTrial();	// execute the current trial
void advanceTrial(bool response);	// move to the next trial
void repeatTrial();	// repeat trial if it went wrong
void initRendering();
void drawSquare();
void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres);
void drawStimulus();
void buildComparison(double height, double fluffiness, double distance);
void buildStandard(double fluffiness, double distance);
void drawInfo();
void drawStandardComparison();

/********** FUNCTION DEFINITIONS *************/

// Funzione di callback per gestire pressioni dei tasti
void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
		case 'Q':
		case 'q':
		case 27:	//corrisponde al tasto ESC
		{   
			exit(0);
		}
		break;
		case '4':
		{
			responsekey=4;
			if(timer.getElapsedTimeInMilliSec() > 2500)
			{
				if(stimulus_height[first_interval] > stimulus_height[second_interval])
					advanceTrial(true);
				else
					advanceTrial(false);
			}
		}
		break;
		case '6':
		{
			responsekey=6;
			if(timer.getElapsedTimeInMilliSec() > 2500)
			{
				if(stimulus_height[first_interval] > stimulus_height[second_interval])
					advanceTrial(false);
				else
					advanceTrial(true);
			}
		}
		case ' ':
		{
			initTrial();
		}
    }
}

void drawInfo()
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

	GLText text;

	text.init(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_HELVETICA_18);

	text.enterTextInputMode();

	glColor3fv(glWhite);
	text.draw("trialN: " +stringify<int>(trialNumber));
	text.draw("frameN: " +stringify<int>(frameN));
	text.draw("Timer= " + stringify<int>(timer.getElapsedTimeInMilliSec()) );
	text.draw("showStimuli: " +stringify<int>(showStimuli));
	text.draw("first_interval: " +stringify<int>(first_interval));
	text.draw("second_interval: " +stringify<int>(second_interval));
	text.draw("stim noise: " +stringify<int>(stimulus_noise));
	text.draw("stim height 1: " +stringify<double>(stimulus_height[0]));
	text.draw("stim height 2: " +stringify<double>(stimulus_height[1]));
	text.draw("numPoints: " +stringify<int>(numPoints));

	text.leaveTextInputMode();
}

void initStreams()
{
	// Initializza il file parametri partendo dal file parameters.txt, se il file non esiste te lo dice
	ifstream parametersFile;
	parametersFile.open(parametersFile_directory.c_str());
	parameters.loadParameterFile(parametersFile);

	// Subject name
	string subjectName = parameters.find("SubjectName");

	// Principal streams file
	string responseFileName = experiment_directory + responseFile_name + subjectName + ".txt";

	// Check for output file existence
	/// Response file
	if ( !util::fileExists(responseFileName) )
	        responseFile.open(responseFileName.c_str());

	responseFile << fixed << responseFile_headers << endl;

}

void initVariables()
{
	// initialize the trial matrix
	trial.init(parameters);
	// initialize the noise level
	stimulus_noise = str2num<double>(parameters.find("NoiseLevel"));
	// begin
	//initTrial();
}

void initTrial()
{
	showStimuli = false;

	// if the current is not an empty trial...
	if(!trial.isEmpty())
	{
		// random seed
		srand(time(NULL));
		// pick 0 or 1 at random and assign it to first_interval
		first_interval = rand() % 2;
		// pick the complementary (0 or 1) and assign it to second_interval
		second_interval = 1 - first_interval;

		// set the parameters to build the stimulus:
		// HEIGHT
		//--- standard
		stimulus_height[first_interval] = 55.0; 
		//--- comparison
		stimulus_height[second_interval] = trial.getCurrent().second->getCurrentStaircase()->getState(); 
		// EGOCENTRIC DISTANCE
		absDepth = trial.getCurrent().first["AbsDepth"];

		// build the stimuli		
		buildStandard(stimulus_noise, absDepth);		
		buildComparison(stimulus_height[second_interval], stimulus_noise, absDepth);

		// show the stimuli
		showStimuli = true;
		frameN = 0;
		timer.start();
	} else
	{	
		if(condition < 2)
		{
			condition++;
			trialNumber = 0;
			initTrial();
		} else
		{
			responseFile.close();
			exit(0);
		}
	}
}

void buildStandard(double fluffiness, double distance)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / abs(distance))*180/M_PI;
	// density is 9 dots per degree
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle);
	// the size of the spheres is also dependent on distance
//	sizePointsF = tan(8.0/60.0*M_PI/180.0)*abs(distance);
	
	s_pointsX = new double[numPoints];
	s_pointsY = new double[numPoints];
	s_pointsZ = new double[numPoints];
	sizePoints = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		s_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		s_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		s_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(30.0*fluffiness/100.0))) - (30.0*fluffiness/100.0)/2.0;
		if(s_pointsY[i] < 55.0/2.0 && s_pointsY[i] > -55.0/2.0)
			s_pointsZ[i] = s_pointsZ[i] + 30.0;
	}
	// the size of the spheres is also dependent on distance
	for(int i = 0; i < numPoints; ++i ) 
	{
		sizePoints[i] = tan(8.0/60.0*M_PI/180.0)*(abs(distance+s_pointsZ[i]));
	}

}

void buildComparison(double height, double fluffiness, double distance)
{
	// the stimulus subtends an overall area of 15*15 cm on the screen
	// an area of 225 cm^2 projects a certain angle on the retina depending on distance
	double stimulus_visual_angle = atan(150.0 / abs(distance))*180/M_PI;
	// density is 9 dots per degree
	double density = 9;
	// therefore the total number of points is density per degree
	numPoints = ceil(density * stimulus_visual_angle);
	// the size of the spheres is also dependent on distance
//	sizePointsF = tan(8.0/60.0*M_PI/180.0)*abs(distance);
	
	c_pointsX = new double[numPoints];
	c_pointsY = new double[numPoints];
	c_pointsZ = new double[numPoints];
	sizePoints = new double[numPoints];
	
	for(int i = 0; i < numPoints; ++i ) 
		c_pointsX[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsY[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/150.0)) - 75.0;
	}
	for(int i = 0; i < numPoints; ++i ) 
	{
		c_pointsZ[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(30.0*fluffiness/100.0))) - (30.0*fluffiness/100.0)/2.0;
		if(c_pointsY[i] < height/2.0 && c_pointsY[i] > -height/2.0)
			c_pointsZ[i] = c_pointsZ[i] + 30.0;
	}
	// the size of the spheres is also dependent on distance
	for(int i = 0; i < numPoints; ++i ) 
	{
		sizePoints[i] = tan(8.0/60.0*M_PI/180.0)*abs(distance+c_pointsZ[i]);
	}

}

void advanceTrial(bool response)
{
	if(responsekey == 5)
	{
		repeatTrial();
		overwrite = 1;
	}

	if(!overwrite)
	{
		//responseFile.precision(3);
		responseFile << parameters.find("SubjectName") << "\t" <<
				interoculardistance << "\t" <<
				trialNumber << "\t" <<
				absDepth <<"\t" <<
				trial.getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
				trial.getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
				trial.getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
				trial.getCurrent().second->getCurrentStaircase()->getAscending() << "\t" <<
				trial.getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
				stimulus_height[first_interval] << "\t" <<
				stimulus_height[second_interval] << "\t" <<
				timer.getElapsedTimeInMilliSec() <<
				endl;
	}
	
	if(!overwrite)
		trialNumber++;

	trial.next(response);
	initTrial();
}

void repeatTrial()
{
//	map<std::string,double> currentFactorsList = trial.getCurrent();
//	trial.reinsert(currentFactorsList);
//	initTrial();
}

void handleResize(int w, int h) 
{

    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (float)w / (float)h, absDepth-150, absDepth+150);

/*    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
*/
}

void drawGLScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawStimulus();
	drawInfo();
	glutSwapBuffers();
}

void drawErnstBanksSpheres(double *px, double *py, double *pz, int numSpheres, double *sizeSpheres)
{
	glColor3fv(glRed);

	for (int i = 0; i < numSpheres; ++i )
	{ 
		glPushMatrix();
		glTranslated(px[i], py[i], pz[i]);
		glutSolidSphere(1.5*sizeSpheres[i],10,10);
		glPopMatrix();
	}
}

void drawStandardComparison()
{
	switch(whichStimulus)
	{
		case 0: // standard
		{
			drawErnstBanksSpheres(s_pointsX, s_pointsY, s_pointsZ, numPoints, sizePoints);
		}
		break;
		case 1: // comparison
		{
			drawErnstBanksSpheres(c_pointsX, c_pointsY, c_pointsZ, numPoints, sizePoints);
		}
		break;
	}
}

void drawStimulus()
{
	if(showStimuli)
	{	
		glLoadIdentity();
		glTranslatef(0, 0, absDepth);
		//glRotated(90, 0, 1, 0);
		
		if(timer.getElapsedTimeInMilliSec() >= 150 && timer.getElapsedTimeInMilliSec() <= 1150)
		{
			whichStimulus = first_interval;
			drawStandardComparison();
		} else if(timer.getElapsedTimeInMilliSec() >= 1550 && timer.getElapsedTimeInMilliSec() <= 2500)
		{
			whichStimulus = second_interval;
			drawStandardComparison();
		} 
	}
}

void drawSquare()
{
	glColor3fv(glRed);
	glBegin(GL_QUADS);
	glVertex3f(-75.0f, 75.0f, 0.0f);		// Top Left
	glVertex3f( 75.0f, 75.0f, 0.0f);		// Top Right
	glVertex3f( 75.0f,-75.0f, 0.0f);		// Bottom Right
	glVertex3f(-75.0f,-75.0f, 0.0f);		// Bottom Left
	glEnd();
}

void update(int value) {

    glutPostRedisplay();
    glutTimerFunc(17, update, 0);
}

void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void idle()
{
	frameN++;
}


//###########################################################################################

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	initStreams();
	initRendering();
	initVariables();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("ErnstBanks2002 Stimulus");
	glutDisplayFunc(drawGLScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutIdleFunc(idle);
	glutTimerFunc(15, update, 0);
	boost::thread initVariablesThread(&initVariables);
	/* Application main loop */
	glutMainLoop();

	return 0;
}
