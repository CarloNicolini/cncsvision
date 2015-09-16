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

#ifndef _PARSTAIRCASE_H
#define _PARSTAIRCASE_H

//#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "Staircase.h"
#include "ParametersLoader.h"

/**
* \defgroup Experiment
* \brief Experiment is the module for help the experimenter to accomplish typical psycophysic experiments. It contains a full staircase and a very easy to
* use factorial design implementations.
* The classes involved in the Experiment module are the following:
* <ul>
* <li> \ref Staircase </li>
* <li> \ref ParStaircase </li>
* <li> \ref BalanceFactor </li>
* <li> \ref ParametersLoader </li>
* </ul>
* The typical interface of an Experiment class is the standard:
* <ul>
* <li> Constructor </li>
* <li> Init method </li>
* <li> Next method </li>
* </ul>
* The destructor of the class then takes care of removing everything from the memory and be ready to begin with another experiment.
*
* \class ParStaircase
* \ingroup Experiment
* \brief A wrapper around the main Staircase class, needed to work with multiple staircases.
* It allows the user to keep many Staircase and for each trial select a random staircase, in order to better average the results.
*
* An example of usage of a staircase experiment is the following
* \code
*  ParStaircase *parStairCase;
*  parStairCase = new ParStaircase();
*  parStairCase->init(4);
*  parStairCase->setStartState(vlist_of<double>(1)(0.5)(-0.5)(-1));
*  parStairCase->setAscending(vlist_of<bool>(false)(false)(true)(true));
*  parStairCase->setMaxreversals(vlist_of<int>(10)(4)(4)(10));
*  parStairCase->setMaxTrials(vlist_of<int>(1000)(1000)(1000)(1000));
*  parStairCase->setCorrectAnswers(vlist_of<int>(3)(2)(2)(3));
*  parStairCase->setStairStep(0.05,0.025,0.025);
*  parStairCase->setStairStep(0,vlist_of<double>(1)(0.5),5);
*  \endcode
* To look for some examples on how to use this class, please take a look to testStaircase.cpp and testStaircase2.cpp
 **/
using std::vector;

class ParStaircase
{
public:
    ParStaircase();
    ParStaircase(const ParStaircase&);
    ~ParStaircase();
    void init(int nStaircases );
    void init( ParametersLoader &params);
    int step( bool );

    // Setter methods
    Staircase *getCurrentStaircase();
    void setStartState(const std::vector<double> &);
    void setAscending(const std::vector<bool> &);
    void setCorrectAnswers(const std::vector<int> &correctAnswersToPositiveStep);
    void setCorrectAnswers(int staircaseID, int correctAnswersToPositiveStep);
    void setStairStep(int staircaseID, const vector<double > &posStepSizes, const vector<double > &negativeStepSizes);
    void setStairStep(int staircaseID, const vector<double > &posStepSizes, double negativeStepSize);
    void setMaxReversals(const vector<int> &maxreversals);
    void setMaxReversals(int staircaseID, int maxreversals);
    void setMaxTrials(const vector<int> &maxreversals);
    void setMaxTrials(int staircaseID, int maxTrials);
    void setClamp(double lowerBound, double upperBound, int maxClampHits);
    void setClamp(int id, double lowerBound, double upperBound, int maxClampHits);

    // Outputs
    std::vector<double> getAverageReversals(int nRev);
    void save(const std::string &filename);

public:
    int nStaircases;
    int currentStaircaseIndex;

    std::vector<Staircase> staircases;
    std::vector<int> unfinishedStaircasesIDs;
};

#endif
