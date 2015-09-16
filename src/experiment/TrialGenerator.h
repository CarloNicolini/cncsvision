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

#ifndef _TRIALGENERATOR_H_
#define _TRIALGENERATOR_H_

#include <algorithm>
#include <fstream>
#include <set>
#include <list>

#include <math.h>
#include "Mathcommon.h"
#include "ParametersLoader.h"
#include "Staircase.h"
#include "ParStaircase.h"
#include "BalanceFactor.h"

using namespace std;
using namespace mathcommon;

/**
* \class TrialGenerator
* \ingroup Experiment
* \brief TrialGenerator is an utility class which combines a ParStaircase procedure with balanced factors extractions.
*
* The class takes as input two references, the first to the BalanceFactor object which contains all the permutations
* of factors and levels, the second a pointer to a ParStaircase object initialized with specified parameters.
* The class internally multiply the ParStaircase object deep copying it and assigning it to a factors-level extraction.
* The result of the getCurrent() method is a pair which as first element has the factors list as map< string, Scalar > and as
* second element the corresponding ParStaircase object which in turn contains other N staircases and which stimulus intensity
* can reached by a call to the getCurrentStaircase()->getState() combination of methods.
* An example follows:
* \code
* BalanceFactor<double> balance;
* balance.init(4,true);
* vector<double> slantVals,tiltVals;
* for (int i=0; i<2; i++)
* {  slantVals.push_back(i);
*    tiltVals.push_back(i);
* }
* balance.addFactor( "Slant",  slantVals );
* balance.addFactor( "Tilt",  tiltVals );
*
* ParStaircase *parStairCase = new ParStaircase();
* parStairCase->init(4);
* parStairCase->setStartState(vlist_of<double>(1)(0.5)(-0.5)(-1));
* parStairCase->setAscending(vlist_of<bool>(false)(false)(true)(true));
* parStairCase->setMaxInversions(vlist_of<int>(10)(10)(10)(10));
* parStairCase->setMaxTrials(vlist_of<int>(300)(300)(300)(300));
* parStairCase->setCorrectAnswers(vlist_of<int>(2)(3)(3)(2));
* parStairCase->setStairStep(0.05,0.025,0.025);
*
* TrialGenerator<double> trials(balance,parStairCase);
* pair< map<string,double>,ParStaircase* > currentTrial = trials.getCurrent();
* double mean=0;
* double std=0.01;
* cout << fixed << currentTrial.first["Slant"] << "\t" << currentTrial.second->getCurrentStaircase()->getID() << "\t" << currentTrial.second->getCurrentStaircase()->getState() << endl;
*
* while ( !trials.isEmpty() )
* {
*   cout << fixed << currentTrial.first["Slant"] << "\t" << currentTrial.first["Tilt"] << "\t" <<currentTrial.second->getCurrentStaircase()->getID() << "\t" << currentTrial.second->getCurrentStaircase()->getState() << endl;
*   bool answer=true;
*   double decision = unifRand(0.0,1.0);
*   double observer = (erf(((currentTrial.second->getCurrentStaircase()->getState() - mean)/std)))/2+0.5;  // simulate an observer with a sigmoidal detection function (psychometric)
*   answer = decision<=observer;
*   trials.next(answer);
*   currentTrial = trials.getCurrent();
*
* }
* trials.next(true);   // will produce an assert because the set is now empty
*
* \endcode
*
**/

template <typename Scalar>
class TrialGenerator
{
public:
    TrialGenerator();
    TrialGenerator( BalanceFactor<Scalar> &balance, ParStaircase *parStairCase);
    ~TrialGenerator();
    void init( BalanceFactor<Scalar> &balance, ParStaircase *parStairCase);
    void init(const ParametersLoader &params);
    std::pair < std::map<std::string, Scalar>, ParStaircase* > getCurrent();
    bool next(bool answer);
    bool isEmpty();
    int getRemainingTrials()
	{
		return nExtractions;
	}

private:
    unsigned int nExtractions;
    std::list < std::map<std::string,Scalar> > allConditions;
    std::map < std::map<std::string, Scalar>, ParStaircase* > staircaseMap;

    //pair < map< string, Scalar>, ParStaircase* > trialPair;
    typename std::map < std::map<std::string, Scalar>, ParStaircase* >::iterator iterStaircaseMap;

};


// BEGIN IMPLEMENTATION
/**
* \ingroup Experiment
* Default constructor
**/
template <typename Scalar>
TrialGenerator<Scalar>::TrialGenerator() : nExtractions(0)
{

}

/**
* \ingroup Experiment
* Constructor
* \param balance An already initialized BalanceFactor object
* \param parStairCase  A pointer to an already initialized ParStaircase object to be deep-copied for every factors permutation
**/
template <typename Scalar>
TrialGenerator<Scalar>::TrialGenerator( BalanceFactor<Scalar> &balance, ParStaircase *parStairCase)
{  // Fill the set of factors extracted from balance factor
    while ( !balance.isEmpty() )
        allConditions.push_back(balance.getNext());
/*
    for (typename std::list< std::map < std::string, Scalar > >::iterator iter1= allConditions.begin(); iter1!=allConditions.end();++iter1)
    {
        for (typename  std::map < std::string, Scalar >::iterator iter = iter1->begin(); iter!=iter1->end();++iter )
        {
            cout << "xxx " << iter->first << "\t" <<iter->second ;
        }
        cout << endl;
    }
*/
    nExtractions=allConditions.size();
    // Put all the factors as key in a map to the given staircase
    while ( !allConditions.empty() )
    {  staircaseMap[allConditions.front()] = new ParStaircase(*parStairCase); //XXX probably a bug has been inserted here because of the copy constructor
        //staircaseMap[allConditions.front()]->selectRandomStaircase();   // just to set the current staircase inside this ParStaircase to be non NULL!
        allConditions.pop_front();
    }

    if (staircaseMap.empty())
        throw std::runtime_error("Can't start a TrialGenerator instance without a factor list");

    unsigned int size = staircaseMap.size();

    iterStaircaseMap = (staircaseMap.begin());
    // Pick a random factor combination
    std::advance( iterStaircaseMap, mathcommon::unifRand(size)%size );
}

/**
* \ingroup Experiment
* Init method, initialize this with reference to a preloaded ParametersLoader object
* \param params Reference to the preloaded parameters loader
* The parameters file must contain the following items in order to correctly initialize the staircase procedure for every factor permutation
\code
sStairNumber: 4
sStairStartStates: 10,5,-5,-10
sStairAscending: 0,0,1,1
sStairCorrectAnswers: 3,3,3,3
sStairMaxInversions: 10,10,10,10
sStairMaxTrials: 100,100,100,100
sStairPositiveStep: 0.25,10,5,0.1
sStairPositiveStepAfterFirstReversal: 0.01
sStairNegativeStep: 1
\endcode
**/
template <typename Scalar>
void TrialGenerator<Scalar>::init(const ParametersLoader &params)
{  BalanceFactor<string> balance;
    balance.init(params);

    // Fill the set of factors extracted from balance factor
    while ( !balance.isEmpty() ) // it insert in the list only the factors at 0 repetion!
    {  balance.next();
        map<string,string> factorsAsString = balance.getCurrentAsMapToString();
        map<string,Scalar> factors;
        for ( map<string,string>::iterator iter = factorsAsString.begin(); iter!=factorsAsString.end(); ++iter )
            factors.insert(make_pair<string,Scalar>(iter->first,util::str2num<Scalar>(iter->second) ));

        if ( factorsAsString.find("Repetition")!=factorsAsString.end() && factorsAsString["Repetition"] == "0" )
            allConditions.push_back(factors);
    }

    nExtractions=allConditions.size();
    //cerr << "nExtractions= " << nExtractions <<  endl;
    ParStaircase *parStairCase;
    parStairCase = new ParStaircase();
    parStairCase-> init(str2num<int>(params.find("sStairNumber")));
    parStairCase->setStartState(str2num<double>(params.find("sStairStartStates"),","));
    parStairCase->setAscending(str2num<bool>(params.find("sStairAscending"),","));
    if (params.exists("sStairMaxReversals")) // for back-compatibilty
        parStairCase->setMaxReversals(str2num<int>(params.find("sStairMaxReversals"),","));
    else
        parStairCase->setMaxReversals(str2num<int>(params.find("sStairMaxInversions"),","));
    parStairCase->setMaxTrials(str2num<int>(params.find("sStairMaxTrials"),","));
    parStairCase->setCorrectAnswers(str2num<int>(params.find("sStairCorrectAnswers"),","));
    // CLAMP VARIABLES SECTION
    double lowerClamp= -std::numeric_limits<double>::max(), upperClamp = std::numeric_limits<double>::max();
    int maxClampHits = std::numeric_limits<int>::max();
    if ( params.exists("sStairClampLower") )
        lowerClamp = str2num<double>(params.find("sStairClampLower"));
    if ( params.exists("sStairClampUpper") )
        upperClamp = str2num<double>(params.find("sStairClampUpper"));
    if ( params.exists("sStairClampLower") )
        maxClampHits = str2num<double>(params.find("sStairMaxClampHits"));
    parStairCase->setClamp(lowerClamp,upperClamp,maxClampHits);

    // If the positiveStep is more than 1 then it means that the other are
    vector<double> steps = str2num<double>(params.find("sStairPositiveStep"),",");
    for ( int i=0; i< str2num<int>(params.find("sStairNumber")) ; i++ )
        parStairCase -> setStairStep( i,steps,str2num<double>(params.find("sStairNegativeStep"),",") );

    while ( !allConditions.empty() )
    {  staircaseMap[allConditions.front()] = new ParStaircase(*parStairCase);
        staircaseMap[allConditions.front()] ->getCurrentStaircase();   // just to set the current staircase inside this ParStaircase to be non NULL!
        allConditions.pop_front();
    }

    unsigned int size = staircaseMap.size();
    if (size==0)
    {
        throw std::runtime_error(":::: Error in TrialGenerator::init(const ParametersLoader&)::: You should have at least 1 factor with 1 level to proceed! Modify your parameters file");
    }
    iterStaircaseMap = (staircaseMap.begin());
    // randomize the first factor list
    std::advance( iterStaircaseMap, mathcommon::unifRand(size)%size );

    //trialPair = make_pair(iterStaircaseMap->first,iterStaircaseMap->second);
    iterStaircaseMap->second->getCurrentStaircase(); //IMPORTANTE!!! serve per non rendere nullo il puntatore iniziale alla staircase ma comunque creare una lista dei fattori ed avere un primo stato della staircase
    delete parStairCase;
}

/**
* \ingroup Experiment
* Destructor, clear all memory
**/
template <typename Scalar>
TrialGenerator<Scalar>::~TrialGenerator()
{  // Clear all the remaining memory, automatically destroys all the staircases
    typename list < map< string, Scalar > >::iterator iterConditions = allConditions.begin();
    for ( iterConditions = allConditions.begin(); iterConditions!=allConditions.end(); ++iterConditions)
    {  delete staircaseMap[*iterConditions];
    }

    staircaseMap.clear();
}

/**
* \ingroup Experiment
* Next method, internally pick the next permutation of factors-staircase
* \param answer A boolean value to be coded separately for each experiment (a two-way option)
**/
template <typename Scalar>
bool TrialGenerator<Scalar>::next(bool answer)
{
    if ( staircaseMap.empty() )
        return false;

    //iterStaircaseMap->second->getCurrentStaircase();
    if ( iterStaircaseMap->second->step(answer) == 1 )	//this staircase is finished
    {
        delete iterStaircaseMap->second;
        staircaseMap.erase(iterStaircaseMap);
    }
    unsigned int size = staircaseMap.size();
    if ( size!=0 )
    {  iterStaircaseMap = staircaseMap.begin();
        std::advance( iterStaircaseMap, mathcommon::unifRand(size)%size );
        //trialPair = std::make_pair(iterStaircaseMap->first,iterStaircaseMap->second);
        nExtractions--;
    }
	return true;
}

/**
* \ingroup Experiment
* Next method, internally pick the next permutation of factors-staircase
* \return A pair containing as first element the current factor list, as second element the current ParStaircase
* An example :
* \code
* pair< factors<string, double >, ParStaircase* > trial = trialGenerator.getCurrent();
* \endcode
**/
template <typename Scalar>
pair < map< string, Scalar>, ParStaircase* > TrialGenerator<Scalar>::getCurrent()
{  return make_pair(iterStaircaseMap->first,iterStaircaseMap->second);
}


template <typename Scalar>
bool TrialGenerator<Scalar>::isEmpty()
{  
    return staircaseMap.empty();
}
// END IMPLEMENTATION

#endif
