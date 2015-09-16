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

/********* TRIAL VARIABLES *********/
static const int THREERODS=0;
int trialMode = THREERODS;
int trialNumber = 0;

/* PARAMETERS FILES */
ParametersLoader parameters, parametersOpenLoop, parametersClosedLoop;

int condition = 0;

/* EXPERIMENT MATRIX */
TrialGenerator<double> trial[3];
int responsekey=0, overwrite=0;
int frameN=0;
double stim_value = 0.0;
bool isStimulusDrawn = false;
double stimulus_height[2] = {0.0,0.0};
int first_interval = 0, second_interval = 0;

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
string experiment_directory = "/home/carlo/Copy/Shape Lab Shared/Fall14/ErnstBanks_test_1/";

// response file name
string responseFile_name = "fall14_ernst_banks_test1_";

// response file headers
string responseFile_headers = "subjName\tIOD\ttrialN\tAbsDepth\tstairID\tbar_height\treversals\tstairAscending\tstepsDone\ttrialDuration\tcondition\tremaining_trials";

/*----------------------------------------- FUNCTIONS -----------------------------------------*/

/********** FUNCTION PROTOTYPES *****/

/* MOSTLY FIXED FUNCTIONS */
void handleKeypress(unsigned char key, int x, int y);	// manage keyboard presses
void initVariables();	// initialize experiment matrix in a BalanceFactor- or TrialGenerator-type of variable (usually trial)
void initStreams();	// read from parameters file and create+open the output files
void initTrial();	// execute the current trial
void advanceTrial(bool response);	// move to the next trial
void repeatTrial();	// repeat trial if it went wrong

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
			advanceTrial(false); // longer than x? NO
		}
		break;
		case '6':
		{
			responsekey=6;
			advanceTrial(true); // longer than x? YES
		}
    }
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
	string responseFileName =  experiment_directory + responseFile_name + subjectName + ".txt";

	// Check for output file existence
	/// Response file
	if ( !util::fileExists(responseFileName) )
	        responseFile.open((responseFileName).c_str());


	responseFile << fixed << responseFile_headers << endl;

}

void initVariables()
{
	for(int i = 0; i < 3; i++)
	{
		trial[i].init(parameters);
	}
	//trial.next(false);
	initTrial();
}

void initTrial()
{
	if(!trial[condition].isEmpty())
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
		stimulus_height[0] = 55.0; 
		//--- comparison
		stimulus_height[1] = trial[condition].getCurrent().second->getCurrentStaircase()->getState(); 			
		//stim_value = trial[condition].getCurrent().second->getCurrentStaircase()->getState();
		// trial file	
		//cout << stim_value << endl;
		if(stimulus_height[first_interval] > stimulus_height[second_interval] && stimulus_height[1] == stimulus_height[first_interval])
		{
			if ( trial[condition].isEmpty() )
				exit(0);
			advanceTrial(true);
		}
		else
		{
			if ( trial[condition].isEmpty() )
				exit(0);
			advanceTrial(false);
		}
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
				trial[condition].getCurrent().first["AbsDepth"] <<"\t" <<
				trial[condition].getCurrent().second->getCurrentStaircase()->getID()  << "\t" <<
				trial[condition].getCurrent().second->getCurrentStaircase()->getState() << "\t" <<
				trial[condition].getCurrent().second->getCurrentStaircase()->getInversions() << "\t" <<
				trial[condition].getCurrent().second->getCurrentStaircase()->getAscending() << "\t" <<
				trial[condition].getCurrent().second->getCurrentStaircase()->getStepsDone() << "\t" <<
				timer.getElapsedTimeInMilliSec() << "\t" <<
				condition << "\t" <<
				trial[condition].getRemainingTrials() << 
				endl;
	}
	
	if(!overwrite)
		trialNumber++;

	trial[condition].next(response);
	initTrial();
}

void repeatTrial()
{
//	map<std::string,double> currentFactorsList = trial[condition].getCurrent();
//	trial[condition].reinsert(currentFactorsList);
	//initTrial();
}

//###########################################################################################

int main(int argc, char*argv[])
{
	mathcommon::randomizeStart();
	initStreams();
	initVariables();
	//glutInit(&argc, argv);
	//glutKeyboardFunc(handleKeypress);
	// Alla fine e dopo aver disegnato tutto nero inizializza in background
	//boost::thread initVariablesThread(&initVariables);
    return 0;
}
