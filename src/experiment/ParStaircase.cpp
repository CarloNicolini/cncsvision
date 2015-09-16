// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
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
#include <algorithm>
#include <vector>
#include <map>

#include "Util.h"
#include "Mathcommon.h"
#include "ParStaircase.h"

using std::map;
using std::set;
using std::vector;

using std::cout;
using std::cerr;
using util::str2num;

/** \ingroup Experiment
* Default constructor
**/
ParStaircase::ParStaircase()
{
    this->nStaircases=0;
    this->setClamp(-1E10,1E10,1E8);
    this->currentStaircaseIndex=0;
}

/** \ingroup Experiment
* Default constructor, make a deep copy of the staircases contained
**/

ParStaircase::ParStaircase(const ParStaircase &rhs)
{
    this->nStaircases = rhs.nStaircases;
    this->staircases = rhs.staircases;
    this->unfinishedStaircasesIDs = rhs.unfinishedStaircasesIDs;
    this->currentStaircaseIndex=rhs.currentStaircaseIndex;
}

/** \ingroup Experiment
* Default destructor, clear the memory of all instanciated Staircases
**/
ParStaircase::~ParStaircase()
{
}

/** \ingroup Experiment
* Initialize the ParStaircase with n internal staircase, and randomize them in the lower-upper interval as specified by other two parameters.
* @param params ParametersLoader object just initialized with the staircase properties, for a correct settings of the staircase parameters the parameters files loaded by params, should contain the following items (all starting with s to specify that are special parameters relative to the Parstaircase :
\code
sStairNumber: 4
sStairStartStates: 10,5,-5,-10
sStairAscending: 0,0,1,1
sStairCorrectAnswers: 3,3,3,3
sStairMaxreversals: 10,10,10,10
sStairMaxTrials: 100,100,100,100
sStairPositiveStep: 0.25,10,5,0.1
sStairPositiveStepAfterFirstReversal: 0.01
sStairNegativeStep: 1
\endcode
**/
void ParStaircase::init( ParametersLoader &params )
{
    this->currentStaircaseIndex=0;
    this-> init(str2num<int>(params.find("sStairNumber")));
    this->setStartState(str2num<double>(params.find("sStairStartStates"),","));
    this->setAscending(str2num<bool>(params.find("sStairAscending"),","));
    this->setMaxReversals(str2num<int>(params.find("sStairMaxReversals"),","));
    this->setMaxTrials(str2num<int>(params.find("sStairMaxTrials"),","));
    this->setCorrectAnswers(str2num<int>(params.find("sStairCorrectAnswers"),","));

    // Handle the case where the user want to specify different staircase steps for every single staircase, they are colon separated list of comma separated numbers
    if ( params.find("sStairPositiveStep").find(";")!=string::npos )	// a colon is found
    {
        // then try to stringify its content
        string content = params.find("sStairPositiveStep");
        //removeAllWhite(content);
        vector<string> stairstepsstrings;
        util::tokenizeString(content,stairstepsstrings,";");
        vector<string> negstepsstrings;

        util::tokenizeString(params.find("sStairNegativeStep"),negstepsstrings,";");
        vector<double> negSteps(negstepsstrings.size());
        for (unsigned int i=0; i<negstepsstrings.size(); i++)
            negSteps.at(i) = util::str2num<double>(negstepsstrings.at(i));

        unsigned int n = str2num<int>(params.find("sStairNumber"));
        if ( stairstepsstrings.size()!=n  || negstepsstrings.size()!=n )
        {	cerr << "Error here, number of list of staircase positive and negative steps size must equal the staircase number!" << endl;
            cerr << "Be sure to avoid spaces between the colon \";\"" << endl;
            exit(0);
        }

        for ( unsigned int i=0; i<stairstepsstrings.size(); i++)
        {
            vector<double> steps = str2num<double>(stairstepsstrings.at(i),",");
            this->setStairStep(i,steps,negSteps);
        }
    }
    else
    {
        vector<double> posSteps = str2num<double>(params.find("sStairPositiveStep"),",");
        vector<double> negSteps = str2num<double>(params.find("sStairNegativeStep"),",");
        for ( int i=0; i< str2num<int>(params.find("sStairNumber")) ; i++ )
            this -> setStairStep( i,posSteps, negSteps);
    }
    double lowerClamp=-1e30,upperClamp=1e30;
    int maxClampHits=3;
    if ( params.exists("sStairClampLower") )
        lowerClamp = str2num<double>(params.find("sStairClampLower"));
    if ( params.exists("sStairClampUpper") )
        upperClamp = str2num<double>(params.find("sStairClampUpper"));
    if ( params.exists("sStairMaxClampHits") )
        maxClampHits= str2num<int>(params.find("sStairMaxClampHits") );
    this->setClamp(lowerClamp,upperClamp,maxClampHits);
}

/** \ingroup Experiment
* Initialize the ParStaircase with n internal staircase, and randomize them in the lower-upper interval as specified by other two parameters.
* @param nStaircases Number of internal staircases
**/
void ParStaircase::init(int _nStaircases )
{
    this->nStaircases=_nStaircases;
    staircases.clear();
    unfinishedStaircasesIDs.clear();
    for ( int i=0; i<_nStaircases; i++)
        staircases.push_back(Staircase(i));

    for (int i=0; i<_nStaircases; i++)
        unfinishedStaircasesIDs.push_back(i);
    this->currentStaircaseIndex = 0;
}

/** \ingroup Experiment
* Get a pointer to the currently used singular staircase, check externally that the pointer is !=NULL
* \return Pointer to the current staircase we are working on
*/
Staircase *ParStaircase::getCurrentStaircase()
{
    return &(staircases.at(this->currentStaircaseIndex));
}


/** \ingroup Experiment
* Make a step.
* @param _ans the answer
* @return 1 if the step is not valid or ParStaircase experiment is finished, 0 otherwise
**/
int ParStaircase::step( bool _ans )
{
    Staircase *currStaircase = &staircases.at(currentStaircaseIndex);
    int currFinished = currStaircase->step(_ans);
    if ( currFinished )
    {
        // Remove the just finished staircase id from the list of unfinished staircases
        std::vector<int>::iterator iterCurrIndex = std::find(unfinishedStaircasesIDs.begin(),unfinishedStaircasesIDs.end(),currentStaircaseIndex);
        if ( iterCurrIndex != unfinishedStaircasesIDs.end() )
        {
            unfinishedStaircasesIDs.erase(iterCurrIndex);
        }
        if (unfinishedStaircasesIDs.empty())
            return 1;
    }
    random_shuffle(unfinishedStaircasesIDs.begin(),unfinishedStaircasesIDs.end());
    currentStaircaseIndex = unfinishedStaircasesIDs.front();

    return 0;
}

/**
 * @brief ParStaircase::save
 * @param os
 */
void ParStaircase::save(const std::string &filename)
{
    ofstream os;
    os.open(filename.c_str());
    os << "Intensities:"<< endl;
    for (unsigned int i=0; i<staircases.size();i++)
    {
        vector<double> states = staircases.at(i).getStates();
        std::copy(states.begin(), states.end(), ostream_iterator<double>(os, ", "));
        os << endl;
    }
    os << "Reversal Intensities:" << endl;
    for (unsigned int i=0; i<staircases.size();i++)
    {
        std::vector<std::pair<int,double> > revStates = staircases.at(i).getReversalStates();
        for (unsigned int i=0; i<revStates.size();i++)
        {
            os << revStates.at(i).first << ":" << revStates.at(i).second << ", ";
        }
        os << endl;
    }
    os << "Answers:" << endl;
    for (unsigned int i=0; i<staircases.size();i++)
    {
        vector<int> answers= staircases.at(i).getAnswers();
        std::copy(answers.begin(), answers.end(), ostream_iterator<int>(os, ", "));
        os << endl;
    }
    os.close();
}

/**
 * @brief ParStaircase::getAverageReversals
 * @param latestReversalToAverage
 * @return
 */
vector<double> ParStaircase::getAverageReversals(int latestReversalToAverage)
{
    if (latestReversalToAverage<1)
        throw std::logic_error("Can't average on a negative number of inversions or zero inversions");

    vector<double> avgRev(nStaircases);
    for (int i=0; i<nStaircases;i++)
    {
        avgRev.at(i)=staircases.at(i).getAverageReversals(latestReversalToAverage);
    }
    return avgRev;
}


/** \ingroup Experiment
* Set the maximum number of reversals for every internal staircase
* @param maxreversals Vector of maximum number of reversals to apply to staircases
**/
void ParStaircase::setMaxReversals(const vector<int> &maxreversals)
{
    if( maxreversals.size() != staircases.size() )
        throw std::range_error("Error, number of reversals must be equal to number of staircases " + util::stringify<int>(maxreversals.size()) + "!=" + util::stringify<int>(staircases.size()));

    for (unsigned int i=0; i<maxreversals.size();i++)
        staircases.at(i).setMaxReversals(maxreversals.at(i));
}

/** \ingroup Experiment
* Set the number of reversals for every internal staircase
* @param staircaseID The ID of the staircase to modify
* @param maxreversals The maximum number of reversal (reversals) in the answer before the staircase ends
**/
void ParStaircase::setMaxReversals(int staircaseID , int maxreversals)
{
    staircases.at(staircaseID).setMaxReversals(maxreversals);
}


/** \ingroup Experiment
* Set the number of trials for every internal staircase. It launches an assert if the size of maxTrials input parameter is different from the number of staircases declared in the init method
* @param maxTrials A vector of the same lenght as the number of staircases with the maximum number of steps for each staircase
**/
void ParStaircase::setMaxTrials(const vector<int> &maxTrials)
{
    if ( maxTrials.size() != staircases.size() )
        throw std::range_error("Error, number of max trials must be equal to number of staircases " + util::stringify<int>(maxTrials.size()) + "!=" + util::stringify<int>(staircases.size()));

    for (unsigned int i=0; i<maxTrials.size();i++)
        staircases.at(i).setMaxNtrials(maxTrials.at(i));
}

/** \ingroup Experiment
* Set the number of trials for every internal staircase.
* @param staircaseID The ID of the staircase to modify
* @param maxTrials The maximum number of trials before the staircase ends
**/
void ParStaircase::setMaxTrials(int staircaseID , int maxTrials)
{
    staircases.at(staircaseID).setMaxNtrials(maxTrials);
}

/** \ingroup Experiment
* Set the start state for every internal staircase. It launches an assert if the size of startStates input parameter is different from the number of staircases declared in the init method
* @param startStates Vector of n starting states for every internal staircase. Size of startStates must be equal to number of staircases
**/
void ParStaircase::setStartState(const vector<double> &startStates)
{
    if (startStates.size() != staircases.size() )
        throw std::range_error("Error, number of start states must be equal to number of staircases " + util::stringify<int>(startStates.size()) + "!=" + util::stringify<int>(staircases.size()));

    for (unsigned int i=0; i<startStates.size();i++)
        staircases.at(i).setStartState(startStates.at(i));
}

/** \ingroup Experiment
* Modify each staircase to be ascending or descending. It launches an assert if the size of ascending input parameter is different from the number of staircases declared in the init method
* @param ascending Vector of booleans, set if the i-th staircase is ascending (true) or descending (false). Size of startStates must be equal to number of staircases
**/
void ParStaircase::setAscending(const vector<bool> &ascending)
{
    if (ascending.size() != staircases.size() )
        throw std::range_error("Error, number of ascending must be equal to number of staircases " + util::stringify<int>(ascending.size()) + "!=" + util::stringify<int>(staircases.size()));

    for (unsigned int i=0; i<ascending.size();i++)
        staircases.at(i).setAscending(ascending.at(i));
}

/** \ingroup Experiment
* Set the number of correct answers to do a negative (if descending) or positive (if ascending) step. It launches an assert if the size of correctAnswersToPositiveStep input parameter is different from the number of staircases declared in the init method
* @param correctAnswersToPositiveStep Vector of n correct answers for every internal staircase. Size of correctAnswersToPositiveStep must be equal to number of staircases
**/
void ParStaircase::setCorrectAnswers(const vector<int> &correctAnswersToPositiveStep)
{
    if (correctAnswersToPositiveStep.size() != staircases.size() )
        throw std::range_error("Error, number of ascending must be equal to number of staircases " + util::stringify<int>(correctAnswersToPositiveStep.size()) + "!=" + util::stringify<int>(staircases.size()));

    for (unsigned int i=0; i<correctAnswersToPositiveStep.size();i++)
        staircases.at(i).setCorrectAnswers(correctAnswersToPositiveStep.at(i));
}

/** \ingroup Experiment
* Set the number of correct answers to do a negative (if descending) or positive (if ascending) step
* @param staircaseID Number of the staircase of which you want to modifiy the number of correct answers to negative (if descending) or positive (if ascending) step
* @param correctAnswersToPositiveStep Number of correct answers to negative (if descending) or positive (if ascending) step
**/
void ParStaircase::setCorrectAnswers(int staircaseID, int correctAnswersToPositiveStep)
{
    staircases.at(staircaseID).setCorrectAnswers(correctAnswersToPositiveStep);
}

/**
 * @brief ParStaircase::setStairStep
 * @param staircaseID
 * @param posStepSizes
 * @param negativeStepSize
 */
void ParStaircase::setStairStep(int staircaseID, const vector<double > &posStepSizes, double  negativeStepSizes)
{
    vector<double>negStepsSizes;
    negStepsSizes.push_back(negativeStepSizes);
    this->setStairStep(staircaseID,posStepSizes,negStepsSizes);
}

/** \ingroup Experiment
* Set the step size for a specific staircase
* @param staircaseID The staircase to modify the steps
* @param reversalStep A vector containing the step size for every consecutive reversal, if number of reversal is greater of reversalStep.size() then all the positive step size after the last element are assigned to the last element of reversalStep
* @param negStep Size of the negative step
**/
void ParStaircase::setStairStep(int staircaseID, const vector<double> &posStepsSizes, const vector<double> & negStepSizes )
{
    this->staircases.at(staircaseID).setStairStep(posStepsSizes,negStepSizes);
}

/**
 * @brief ParStaircase::setClamp
 * @param lowerBound
 * @param upperBound
 * @param _maxClampHits
 */
void ParStaircase::setClamp(double lowerBound, double upperBound, int _maxClampHits)
{
    for (unsigned int i=0; i<staircases.size();i++)
        staircases.at(i).setClamp(lowerBound,upperBound,_maxClampHits);
}

/** \ingroup Experiment
* Set the clamps for a specific staircase
* @param staircaseID The staircase to modify the steps
* @param reversalStep A vector containing the step size for every consecutive reversal, if number of reversal is greater of reversalStep.size() then all the positive step size after the last element are assigned to the last element of reversalStep
* @param negStep Size of the negative step
**/
void ParStaircase::setClamp(int staircaseID,  double lowerBound, double upperBound, int _maxClampHits)
{
    staircases.at(staircaseID).setClamp(lowerBound,upperBound,_maxClampHits);
}
