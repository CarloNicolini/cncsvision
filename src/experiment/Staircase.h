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

#ifndef _STAIRCASE_H
#define _STAIRCASE_H

#include <fstream>
#include <queue>
#include <deque>
#include <map>

/**
* \class Staircase
* \ingroup Experiment
* \brief Staircase wraps the psycophysic method used to measure stimuli response threshold.
*
* Staircase sually begin with a high intensity stimulus, which is easy to detect. The intensity is then reduced until the observer makes a mistake,
* at which point the staircase reverses and intensity is increased until the observer responds correctly, triggering another reversal.
* The values for these 'reversals' are then averaged. There are many different types of staircase, utilising many different decision and termination rules.
* Step-size, up/down rules and the spread of the underlying psychometric function dictate where on the psychometric function they converge.
* Threshold values obtained from staircases can fluctuate wildly, so care must be taken in their design.
* Many different staircase algorithms have been modeled and some practical recommendations suggested by Garcia-Perez.
*
* This staircase implementation allows the user to set all the parameters, which are self-describing: nStepToChange is the number of correct trials before the task gets more difficult,
* nReversals is the maximum number of reversals of the staircase before the experiment finish, stairstep is the delta noise between each iteration, ntrialsDone the total number
* of trials done currently, state the current state.
**/

class Staircase
{
private:
    bool ascending;
    double clampLowerBound;
    double clampUpperBound;
    double state;        // the level of the stimulus
    int clampCountHitsDown;
    int clampCountHitsUp;
    int clampMaxHitsDown;
    int clampMaxHitsUp;
    int correctAnswersToPositiveStep;
    int countReversals;
    int direction;
    int id;
    int maxNtrials;
    int maxReversals;
    int ntrialsDone;

    std::deque<bool> lastAnswers;
    std::vector <int> answers;
    std::vector<double> stepSizesPos;
    std::vector<double> stepSizesNeg;
    std::vector<double> states;
    std::map<int,std::pair<int,double> > reversalStates;
    std::vector<int> reversalIndices;

public:
    Staircase(int id=0);
    Staircase(int id,bool ascending,double state,int correctAnswersToPositiveStep=1,int maxNtrials=10,int maxReversals=3);
    ~Staircase();
    Staircase(const Staircase &);
    void init( );

    void setMaxReversals(int maxReversals);
    void setStartState(double startState);
    void setMaxNtrials(int );
    void setMaxClampHits(int);
    void setStairStep(const std::vector<double> &trials, double negStep);
    void setStairStep(const std::vector<double> &trialsSizesPos, const std::vector<double> &trialsizesNeg);
    void print(std::ostream &os);
    void setAscending(bool _ascending);
    void setCorrectAnswers(int _correctAnswersToPositiveStep);
    void setClamp(double lower,double upper, int maxClampHits);
    double getState();
    int getTrialsDone();
    int step(bool);
    void setID(int);
    int getID( void );
    int getReversals();
    bool getAscending(void);
    const std::vector<double> &getStates();
    std::vector<std::pair<int, double> > getReversalStates();
    double getAverageReversals(int latestReversalsToAverage);
    const std::vector<int> &getAnswers();
    // For retrocompatibility, these two methods are deprecated
    int getStepsDone()
    {
        return getTrialsDone();
    }
    int getInversions()
    {
        return getReversals();
    }

};

#endif
