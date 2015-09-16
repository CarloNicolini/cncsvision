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

#include "Util.h"
#include "Mathcommon.h"
#include "Staircase.h"

using namespace std;

/**
* \ingroup Experiment
*  Constructor
**/
Staircase::Staircase(int id) :
    ascending(true),
    clampCountHitsDown(0),
    clampCountHitsUp(0),
    clampLowerBound(std::numeric_limits<double>::min()),
    clampMaxHitsDown(3),
    clampMaxHitsUp(3),
    clampUpperBound(std::numeric_limits<double>::max()),
    correctAnswersToPositiveStep(1),
    countReversals(0),
    direction(1),
    maxNtrials(30),
    maxReversals(3),
    ntrialsDone(0),
    state(0.0)
{
    this->id = id;
    for (int i=0; i<correctAnswersToPositiveStep; i++)
        lastAnswers.push_front(false);

}

/**
 * @brief Staircase::Staircase
 * @param id
 * @param ascending
 * @param state
 * @param correctAnswersToPositiveStep
 * @param maxNtrials
 * @param maxReversals
 */
Staircase::Staircase(int _id, bool _ascending, double _state, int _correctAnswersToPositiveStep, int _maxNtrials, int _maxReversals):
    id(_id), ascending(_ascending),state(_state),correctAnswersToPositiveStep(_correctAnswersToPositiveStep),maxNtrials(_maxNtrials),maxReversals(_maxReversals),
    clampCountHitsDown(0),
    clampCountHitsUp(0),
    clampLowerBound(std::numeric_limits<double>::min()),
    clampMaxHitsDown(3),
    clampMaxHitsUp(3),
    clampUpperBound(std::numeric_limits<double>::max()),
    countReversals(0),
    direction(1),
    ntrialsDone(0)
{
    for (int i=0; i<correctAnswersToPositiveStep; i++)
        lastAnswers.push_front(false);
    stepSizesPos = util::vlist_of<double>(1);
    stepSizesNeg = util::vlist_of<double>(1);
}

/**
 * \ingroup Experiment
 * Destructor
**/
Staircase::~Staircase()
{  lastAnswers.clear();
}

/**
* \ingroup Experiment
* Copy constructor
*   @param other The other staircase
**/

Staircase::Staircase(const Staircase &other) :
    state(other.state), maxReversals(other.maxReversals),
    ntrialsDone(other.ntrialsDone),
    maxNtrials(other.maxNtrials),
    ascending(other.ascending),
    id(other.id),
    correctAnswersToPositiveStep(other.correctAnswersToPositiveStep),
    direction(other.direction),
    countReversals(other.countReversals),
    clampLowerBound(other.clampLowerBound),
    clampUpperBound(other.clampUpperBound),
    clampCountHitsUp(other.clampCountHitsUp),
    clampCountHitsDown(other.clampCountHitsDown),
    clampMaxHitsUp(other.clampMaxHitsUp),
    clampMaxHitsDown(other.clampMaxHitsDown),
    lastAnswers(other.lastAnswers),
    answers(other.answers),
    stepSizesPos(other.stepSizesPos),
    stepSizesNeg(other.stepSizesNeg),
    states(other.states),
    reversalStates(other.reversalStates),
    reversalIndices(other.reversalIndices)
{
//    for (int i=0; i<correctAnswersToPositiveStep; i++)
//        lastAnswers.push_front(false);
}
/**
*  \ingroup Experiment
*  \brief Initialize the Staircase object with default values
*
*   maxReversals=3;
*   state=0;
*   maxNtrials=30;
*   countReversals=0;
*   ntrialsDone=0;
*   answer=true;
*   id = 0;
**/
void Staircase::init( )
{
    maxReversals=3;
    setStartState(0);
    maxNtrials=30;
    countReversals=0;
    ntrialsDone=0;       //because the first step that will be done is the 0
    direction=0;
    id = 0;
    ascending=true;
    clampLowerBound = -1e30;
    clampUpperBound = 1e30;
    clampCountHitsUp=clampCountHitsDown=0;
    clampMaxHitsUp=clampMaxHitsDown=3;

    correctAnswersToPositiveStep=3;
    lastAnswers.clear();
    for (int i=0; i<correctAnswersToPositiveStep; i++)
        lastAnswers.push_front(false);

}

/**
 * @brief Staircase::setCorrectAnswers
 * @param _correctAnswersToPositiveStep
 */
void Staircase::setCorrectAnswers(int _correctAnswersToPositiveStep )
{  // This set the number of correct answers to produce to step in the correct direction when at least 1 reversal is made
    correctAnswersToPositiveStep=_correctAnswersToPositiveStep;
    lastAnswers.clear();
    if ( ascending )
    {  for ( int i=0; i<correctAnswersToPositiveStep; i++)
            lastAnswers.push_front(true);
    }
    else
    {  for ( int i=0; i<correctAnswersToPositiveStep; i++)
            lastAnswers.push_front(false);
    }
}

/** \ingroup Experiment
*  \brief Set the maximum number of reverals for a single staircase before its end.
*  @param _nInv Maximum number of staircase reversals(default 4)
**/
void Staircase::setMaxReversals(int _nInv)
{  maxReversals=_nInv;
}

/**
*   \ingroup Experiment
*  \brief Set the maximum number of trials for the staircase to end.
*  @param _maxNtrials Maximum number of trials (default 30)
**/
void Staircase::setMaxNtrials( int _maxNtrials )
{  maxNtrials = _maxNtrials;
}
/**
*   \ingroup Experiment
*  \brief Set the start state of the staircases before its end.
*  @param _startstate Starting stimulus level (state)
**/

void Staircase::setStartState(double _startstate)
{  state = _startstate;
    states.clear();
    states.push_back(state);
}

/**
*   \ingroup Experiment
* Set if the staircase is ascending or descending. Ascending staircases increase their state by negative response, descending staircases lower their state by positive answers
* @param _ascending Ascending if true, descending if false
**/
void Staircase::setAscending(bool _ascending)
{  ascending=_ascending;
    lastAnswers.clear();
    if ( ascending )
    {  for (int i=0; i<3; i++)
            lastAnswers.push_front(true);

    }
    else
    {  for (int i=0; i<3; i++)
            lastAnswers.push_front(false);
    }
}

/**
*  \ingroup Experiment
*  \brief Set the staircase trialsize depending of the current number of reversal. If number of reversal is greater of _reversalStep.size() then all the positive step size after the last element
are assigned to the last element of reversaltrialsize
* This vector basically means: _reversaltrialsize[0] is the trials size of the positive step for the 0th step and so on...
*  @param _reversaltrialsize vector of trials size one for each reversal
* @param negStep Size of the negative step
**/
void Staircase::setStairStep(const vector<double> &trialsSizes, double negStep)
{
    //negativeStep=negStep;
    stepSizesPos.clear();
    stepSizesPos = trialsSizes;

    stepSizesNeg.clear();
    stepSizesNeg.push_back(negStep);

    this->setStairStep(this->stepSizesPos,this->stepSizesNeg);
}

/**
*  \ingroup Experiment
*  \brief Set the staircase trialsize depending of the current number of reversal. If number of reversal is greater of _reversalStep.size() then all the positive step size after the last element
are assigned to the last element of reversaltrialsize
* This vector basically means: _reversaltrialsize[0] is the trials size of the positive step for the 0th step and so on...
*  @param _reversaltrialsize vector of trials size one for each reversal
* @param negStep Size of the negative step
**/
void Staircase::setStairStep(const vector<double> &_trialsSizesPos, const vector<double> &_trialsizesNeg)
{
    this->stepSizesPos = _trialsSizesPos;
    this->stepSizesNeg=_trialsizesNeg;

    if ( (unsigned int)maxReversals >= stepSizesPos.size() )
    {  while ( stepSizesPos.size() != (unsigned int)maxReversals )
            this->stepSizesPos.push_back(stepSizesPos.back());
    }
    else
        throw std::logic_error("Error: Staircase " + util::stringify(id) + " has more positive reversal step sizes than number of allowed reversal, something wrong!");

    if ( (unsigned int)maxReversals >= this->stepSizesNeg.size() )
    {
        while ( this->stepSizesNeg.size() != (unsigned int)maxReversals )
            this->stepSizesNeg.push_back(this->stepSizesNeg.back());
    }
    else
        throw std::logic_error("Error: Staircase " + util::stringify(id) + " has more negative reversal step sizes than number of allowed reversal, something wrong!");
}

/** \ingroup Experiment
* \brief Print the actual staircase status to standard error
*/
void Staircase::print(ostream &os)
{  os << fixed << id << "\t" << ascending << "\t" << state << "\t" << ntrialsDone << "\t" << countReversals << "\t" << endl;

}


/**
*  \ingroup Experiment
*  \brief Step of a staircase function
*
*  Each time this method is called, it checks the answer using the pickAnswer() function, then it sets up a staircase procedure following the following rules:
*
*  state is increased by 2 if 3 consecutive correct answers are given AND if no reversals are made before
*  state is increased by 1 if 3 consecutive correct answers are given AND some reversals are done before
*  state is decreased by 2 if 1 wrong answer is given
*
*  \return 0 if not finished, 1 if finished
*
**/
bool Staircase::getAscending()
{  return ascending;
}

/**
 * @brief Staircase::getAnswers
 * @return
 */
const std::vector<int> &Staircase::getAnswers()
{
    return answers;
}

/**
 * @brief Staircase::getStates
 * @return
 */
const std::vector<double> &Staircase::getStates()
{
    return this->states;
}

/**
 * @brief Staircase::step
 * @param _answer
 * @return
 */
int Staircase::step( bool _answer )
{
    ntrialsDone++;
    if (ntrialsDone==1) direction = int(_answer);
    lastAnswers.pop_back();
    lastAnswers.push_front(_answer);
    // Trattamento della risposta 0 con staircase ascendente
    if ( ascending && (_answer == true) )
    {  int oldDir=direction;
        direction=1;
        countReversals += (int)(direction != oldDir) ;
        state-=this->stepSizesNeg.at(countReversals==maxReversals ? maxReversals-1 : countReversals/2);
    }

    if ( ascending && !lastAnswers.at(0) && (countReversals==0) )
    {  int oldDir=direction;
        direction=0;
        countReversals += int( direction != oldDir );

        for ( unsigned int i=0; i<lastAnswers.size(); i++)
            lastAnswers.at(i)=true;
        state+=stepSizesPos.at(0);
    }

    // We use a circular buffer of the last N answers (remember ! (OR)x_i == AND !x_i )
    bool retType=lastAnswers.at(0);
    for ( unsigned int i=1; i<lastAnswers.size(); i++)
    {  retType= retType || (lastAnswers.at(i));
    }
    retType=!retType;

    if ( ascending && ( retType ) && (ntrialsDone>1) && (countReversals>0) )
    {  int oldDir=direction;
        direction=0;
        countReversals += int( direction != oldDir );

        for ( unsigned int i=0; i<lastAnswers.size(); i++)
            lastAnswers.at(i)=true;
        state+=stepSizesPos.at(countReversals==maxReversals ? maxReversals-1 : countReversals/2);
    }

    if ( !ascending && (_answer == false) )
    {  int oldDir=direction;
        direction=0;
        countReversals += int( direction != oldDir );
        state+=this->stepSizesNeg.at(countReversals==maxReversals ? maxReversals-1 : countReversals/2);
    }

    // Only for the first trial
    if ( !ascending && lastAnswers.at(0) && (countReversals==0) )
    {  int oldDir=direction;
        direction=1;
        countReversals += int( direction != oldDir );
        for ( unsigned int i=0; i<lastAnswers.size(); i++)
            lastAnswers.at(i)=false;
        state-=stepSizesPos.at(0);
    }

    bool retType2=lastAnswers.at(0);
    for ( unsigned int i=1; i<lastAnswers.size(); i++)
    {  retType2= retType2 && (lastAnswers.at(i));
    }
    // We use a circular buffer of the last N answers
    if ( !ascending && (retType2) && (ntrialsDone>1) && (countReversals>0) )
    {  int oldDir=direction;
        direction=1;
        countReversals += (int)( direction != oldDir );

        for ( unsigned int i=0; i<lastAnswers.size(); i++)
            lastAnswers.at(i)=false;
        state-=stepSizesPos.at(countReversals==maxReversals ? maxReversals-1 : countReversals/2);
    }

    clampCountHitsUp += int(state > clampUpperBound);
    clampCountHitsDown += int(state < clampLowerBound);

    state = mathcommon::clamp<double>(state,clampLowerBound,clampUpperBound);
    bool retBecauseTooManytrials = ( ntrialsDone >= maxNtrials );
    bool retBecauseTooManyreversals = ( countReversals >= maxReversals );
    bool retBecauseTooManyClamps = ( clampCountHitsUp >= clampMaxHitsUp|| clampCountHitsDown >= clampMaxHitsDown );
    bool finished = ( retBecauseTooManytrials || retBecauseTooManyreversals  || retBecauseTooManyClamps );
    if (!finished)
    {
        this->answers.push_back(_answer);
        this->states.push_back(state);
        this->reversalStates[countReversals]=std::pair<int,double>(ntrialsDone,state);
    }
    return finished;
}

/**
 * @brief Staircase::getReversalStates
 * @return
 */
std::vector<std::pair<int,double> > Staircase::getReversalStates()
{
    std::vector<std::pair<int,double> > revStates;
    for (std::map<int,std::pair<int,double> >::iterator iter = this->reversalStates.begin();iter!=this->reversalStates.end();++iter)
        revStates.push_back( iter->second);

    return revStates;
}

/**
 * @brief Staircase::getAverageReversals
 * @param latestReversalsToAverage
 * @return
 */
double Staircase::getAverageReversals(int latestReversalsToAverage)
{
    std::vector<std::pair<int,double> > revStates = getReversalStates();
    double avg=0.0;
    for (unsigned int i=(revStates.size()-1); i>(revStates.size()-1-latestReversalsToAverage);  i--)
        avg+=revStates.at(i).second;
    avg/=latestReversalsToAverage;
    return avg;
}

/**
*   \ingroup Experiment
*   \brief Get the current state of this Staircase object
*   \return the current state of the staircase.
**/
double Staircase::getState()
{  return state;
}

/**
*   \ingroup Experiment
*   \brief Set the identification variable of this staircase (ID)
*   @param _id The ID of for this staircase
**/
void Staircase::setID( int _id )
{  id=_id;
}

/**
*  \ingroup Experiment
*  \brief Get this Staircase ID
*  \return The ID of this staircase
**/
int Staircase::getID(void )
{  return id;
}
/**
*  \ingroup Experiment
*  \brief Get this Staircase number of reversal
*  \return The number of reversals
**/
int Staircase::getReversals()
{  return countReversals;
}
/**
*  \ingroup Experiment
*  \brief Get this Staircase number of trials until now
*  \return The number of trials
**/
int Staircase::getTrialsDone()
{  return ntrialsDone;
}

void Staircase::setClamp(double lower, double upper, int _maxClampHits)
{  if ( lower >= upper )
    {  cerr << "Error, lower clamp must be < upperClamp " << endl;
        assert(false);
    }
    clampLowerBound=lower;
    clampUpperBound=upper;
    clampMaxHitsDown = clampMaxHitsUp = _maxClampHits;
    //cerr << "CLAMP SET TO [" << lowerClamp << ", " << upperClamp << "]" << " Max Clamp hits up= " << maxClampHitsUp << " " << maxClampHitsDown << endl;
}

