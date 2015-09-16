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


#ifndef _BALANCE_FACTOR2_H_
#define _BALANCE_FACTOR2_H_

#include <iostream>
#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <Eigen/Core>
#include <boost/function.hpp> // for the debrujin callback

#include "ParametersLoader.h"
#include "Util.h"
#include "BalanceFactor.h"
#include "ParametersLoader.h"
#include "Mathcommon.h"
#include "Graph.h"

using namespace std;
using namespace Eigen;
using util::stringify;
using util::str2num;

static std::vector<unsigned int> debrujinizedSequence;

inline bool mapcomparisonfunctor(const std::pair<string,string> &s1, const std::pair<string,string> &s2 )
{  if ( s1.first == "Repetition" || s1.first == "ReinsertedTimes" )
        return true;   // it ignores these two keys
    if ( s1.first == s2.first )
        return (s1.second == s2.second);
    else
        return false;
}

/**
* \class BalanceFactor
* @ingroup Experiment
* @brief BalanceFactor balances a set of factors given the factor levels.
* First thing, you have to specify number of trials and if you want to randomize the conditions.
* BalanceFactor can be either initialized via the addFactor method or directly passing a \ref ParametersLoader object.
* For example this code create a 3 factors design, the total number of trial is 1 x 1 x 4 x NTrial
\code
* BalanceFactor<int> balance;
* balance.init(1,false);
* balance.addFactor( "Slant",  vlist_of<int>(45) );
* balance.addFactor( "Tilt", vlist_of<int>(90) );
* balance.addFactor( "Anchored", vlist_of<int>(0)(1)(2)(3));
* \endcode
* If you just loaded a parameter file, via the \ref ParametersLoader class, BalanceFactor can be used much simpler, by exploiting the specific initializer
* \code
* ParametersLoader params;
* params.loadParameterFile(paramFile);

* BalanceFactor<int> balance;
* balance.init(params);
* balance.print();
*
* while ( !balance.isEmpty() )
* {
*      map<string,int> vals = balance.getNext();
*      for ( map<string,int>::iterator iter = vals.begin(); iter!=vals.end(); ++iter )
*      {
*      cout << iter->first << " " << iter->second << endl;
*      }
*      cout << endl << endl;
*     if ( someCondition )
      balance.reinsert(factors);
* }
* \endcode
*
* The class can be used also to reinsert a given factors map to the back of the current deck thanks to the reinsert method.
* If the parameters file contains the following keys:
* \code
* RandomizeWithoutRepetitions: 1
* MaxReinsertions: 10
* \endcode
* then the default values for this two keys are overwritten. RandomizeWithoutRepetitions means that no consecutive extractions can have the same factors although they are in random order.
* MaxReinsertions specifies the maximum number of reinsertion for a given factor in a way such that factor will never been reinserted if it has been extracted too many times.
* This new version of BalanceFactor can work with every stringifiable type (double, int, complex, etc...) thanks to its templatized implementation. As example:

\code
* BalanceFactor<string> balance;
* balance.init(1,true,true,true);
* balance.addFactor( "factor1",  vlist_of<string>("45")("pippo") );
* balance.addFactor( "factor2", vlist_of<string>("zzz")("22.7"));
*
* while ( !balance.isEmpty() )
* {
*      map<string,string> vals = balance.getNext(); // or equivalently
*      balance.next();
*      map<string,string> vals = balance.getCurrent();
*      for ( map<string,string>::iterator iter = vals.begin(); iter!=vals.end(); ++iter )
*      {
*      cout << iter->first << " " << iter->second << endl;
*      }
*      cout << endl << endl;
*     if ( someCondition )
      balance.reinsert(factors);
* }
* \endcode
* In this  case we work directly with strings and if a cast is needed one can use the \code str2num<typename T> \endcode function defined in the util namespace
**/

template < typename Scalar >
class BalanceFactor
{
private:
    int randomizationMethod;
    unsigned int repetitions;
    unsigned int currentTrial;
    unsigned int maxReinsertion;
    bool randomizeWithoutConsecutive;

    // Primary data structures is a list of names and total factors
    deque < pair<string, vector<string> > > factorsLevelsPairList;
    deque < pair<string, vector<string> > > factorsCartesianProduct;

    deque < string > factorNames;
    map<string, string> currentFactorsMap;
    map<string, Scalar> currentFactors;
    map<string, Scalar> previousFactors;

    // Main data structure to extract factors maps
    deque < map <string, string> > extractions;

    // Main data structure to extract factors maps, it contains all extractions
    deque < map <string, string> > fullExtractions;

    void reinsertstring( const map<string, string> &factorsToReinsert );
    bool compareMaps( const map<string, string> &map1,  const map<string, string> &map2 );
    // This part takes in account the randomization without repetitions
    int computeDistanceBetweenConsecutiveExtractions( const deque< map< string,string > > &input );
    void maximimizeInnerDistance();
    void shuffleIntraBlockHeadTail(int firstBlockTail, int secondBlockHead, int secondBlockEnd);


public:
    BalanceFactor();
    ~BalanceFactor();
    void init( unsigned int _repetitions , int _randomizationMethod=0, bool _randomizeWithoutConsecutive=false, unsigned int _maxReinsertion=0 );
    void init( const ParametersLoader &, bool _randomizeWithoutConsecutive=false);
    void addFactor( const string &name, const vector< Scalar > &currentFactor);

    size_t size();
    unsigned int getRemainingTrials();
    bool hasNext();
    bool next();
    map< string, string > getNextAsMapToString();
    map< string, string > getCurrentAsMapToString();

    map< string, Scalar > getNext();
    map< string, Scalar > &getCurrent();
    map< string, Scalar>  &getPrevious();

    deque < map <string, Scalar > > getFullExtractions();

    void debrujinize(unsigned int subsequenceLength, unsigned int nDeBrujinRepetitions, bool prerandomize);
    void reinsert( const map<string, Scalar> & );

    bool isEmpty();
    void print();
    void print(ostream &os);
    void print(const std::string &);
    void pyprint(){print();} // to avoid impossible naming print in python
    static const int RANDOMIZATION_NONE=0;
    static const int RANDOMIZE_EXTRA_BLOCK=1;
    static const int RANDOMIZE_INTRA_BLOCK=2;
    static const int RANDOMIZATION_NO_CONSECUTIVES=3;
    static const int DEBRUJIN_PSEUDORANDOM=4;

};

/**
* @ingroup Experiment
* @brief Maximize the inner distance of factors extractions
**/
template < typename Scalar >
void BalanceFactor<Scalar>::maximimizeInnerDistance( )
{  int iterations=0;
    unsigned int M = extractions.size();
    if ( M == repetitions ) // to handle the single factor-single level case
    {  //cerr << "Can't shuffle this configuration, extractions total size= " << M << endl;
        return;
    }
    int dist = computeDistanceBetweenConsecutiveExtractions(extractions);
    int lowerBoundDist  = 0;

    while (  dist > lowerBoundDist )
    {  for (unsigned int i=0; i<M-2; i++ )
        {  unsigned int j=mathcommon::unifRand( M - 2 -i )+ i +1;
            if ( (i==0) || ( (!compareMaps(extractions.at(i-1),extractions.at(j)) )
                             && (!compareMaps(extractions.at(i+1),extractions.at(j)))
                             && (!compareMaps(extractions.at(i),extractions.at(j-1)))
                             && (j==(M-1) || !compareMaps(extractions.at(i), extractions.at(j+1)) ) )
                 )
                std::swap(extractions.at(i),extractions.at(j));
        }
        dist = computeDistanceBetweenConsecutiveExtractions(extractions);
        iterations++;
    }
}

/**
 * @ingroup Experiment
 * @brief Given begin and end of a sequence, chooses the head of the second sequence that is different from the tail of the first sequence
 */
template < typename Scalar >
void BalanceFactor<Scalar>::shuffleIntraBlockHeadTail(int firstBlockTail, int secondBlockHead, int secondBlockEnd)
{
    if ( compareMaps(extractions.at(firstBlockTail),extractions.at(secondBlockHead) ))
    {
        int index=secondBlockHead;
        while ( compareMaps(extractions.at(secondBlockHead), extractions.at(index) ) && index < secondBlockEnd)
        {
            index++;
        }

        std::swap(extractions.at(secondBlockHead),extractions.at(index));
    }
}

/**
* @ingroup Experiment
* @brief Default constructor
**/
template < typename Scalar >
BalanceFactor<Scalar>::BalanceFactor() : randomizationMethod(0), repetitions(1), currentTrial(0), maxReinsertion(0), randomizeWithoutConsecutive(false)
{
}

/**
* @ingroup Experiment
* @brief Default destructor
**/
template < typename Scalar >
BalanceFactor<Scalar>::~BalanceFactor()
{
}

/**
* @ingroup Experiment
* @brief Init method
* @param _repetitions Number of repetition of each factor map (default 1)
* @param _randomize Randomize the extractions. Use 0 for No randomization, 1 for whole randomization, 2 for intra-trial randomization
* @param _randomizeWithoutConsecutive Choose to pick a extractions randomization that doesn't allow two equal factors to be extracted consecutively.
* @param _maxReinsertion Maximum number of reinsertions
**/
template < typename Scalar >
void BalanceFactor<Scalar>::init(unsigned int _repetitions, int _randomize, bool _randomizeWithoutConsecutive, unsigned int _maxReinsertion )
{
    this->repetitions=_repetitions;
    this->randomizationMethod = _randomize;
    this->randomizeWithoutConsecutive= _randomizeWithoutConsecutive;
    this->currentTrial=0;
    this->maxReinsertion=_maxReinsertion;
    this->factorsLevelsPairList.clear();
    this->factorsCartesianProduct.clear();
    this->factorNames.clear();
    this->currentFactorsMap.clear();
    this->extractions.clear();
    this->fullExtractions.clear();
}

template < typename Scalar >
void BalanceFactor<Scalar>::init( const ParametersLoader &params, bool _randomizeWithoutConsecutive )
{
    this->repetitions  =  str2num<int>(params.find("Repetitions"));
    this->randomizationMethod   =  str2num<int>(params.find("Randomize"));

    if (this->randomizationMethod == this->DEBRUJIN_PSEUDORANDOM) //must ensure that N=1 when using De Brujin pseudorandomization
        this->repetitions=1;

    if ( randomizationMethod < 0 || randomizationMethod > 4 )
    {
        cerr << "Not accepted randomization method. Randomization method can be 0 for no randomization, 1 for extra block randomization, 2 for randomization intra blocks, 3 for randomization without consecutive, 4 for De Brujin sequence of trials" << endl;
        cerr << "For example, with 3 repetitions of factor with values {A,B,C,D} we have 3 blocks consisting of the distinct levels that must be repeated and randomized" << endl;
        cerr << "With no randomization the effect is that the values are extracted in the reading order from parameters file:" << endl;
        cerr << "No randomization:                    { A, B, C, D, A, B, C, D, A, B, C, D}" << endl;
        cerr << "With extra block randomization, the ordered blocks are joined head-tail-head and then the shuffling is done at whole level:" << endl;
        cerr << "Extra blocks randomization:    { A, C, D, B, A, B, A, D, C, C, D, B}" << endl;
        cerr << "With intra block randomization, the shuffling is done for every block at just block level and then the blocks are joined head-tail-head:" << endl;
        cerr << "Intra blocks randomization:     { B, A, C, B, A, D, C, A, D, C, D, B}" << endl;
        cerr << "Press Enter to exit" << endl;
        cin.ignore(1E6,'\n');
        exit(0);
    }
    if ( params.exists(("MaxReinsertions")) )
        maxReinsertion = str2num<int>(params.find("MaxReinsertions"));
    else
        maxReinsertion=0;
    if ( params.exists(("RandomizeWithoutRepetitions")) )
        randomizeWithoutConsecutive = str2num<bool>(params.find("RandomizeWithoutRepetitions"));
    else
        randomizeWithoutConsecutive = _randomizeWithoutConsecutive;

    init(repetitions,randomizationMethod,randomizeWithoutConsecutive,maxReinsertion);

    // Fill the extractions list
    multimap <string, string> mmap = params.getAllParameters();
    for ( multimap<string, string>::iterator iter = (mmap.begin()); iter!=mmap.end(); ++iter)
    {  vector<Scalar> tmpFactor;
        if ( iter->first[0] == 'f' )    // this is a factor! We must use it
        {  string factor =  string(iter->first).erase(0,1);
            // Here we proceed if and only if the factors has not been just added. Very important!
            if ( find(factorNames.begin(), factorNames.end(), factor) == factorNames.end() )    // insert the factor if it is not in the just known factors list
            {  pair<multimap<string,string>::iterator, multimap<string,string>::iterator> ret;
                multimap<string,string>::iterator it;
                ret = mmap.equal_range((*iter).first);
                for ( it=ret.first; it!=ret.second; ++it )
                    tmpFactor.push_back( str2num<Scalar>((*it).second) ) ;

                addFactor( factor, tmpFactor );
            }
        }
    }

    if ( randomizationMethod == DEBRUJIN_PSEUDORANDOM )
    {
        // Pick the DeBrujin specific parameters
        int  debrujinSubSequenceLenght = util::str2num<int>(params.find("DeBrujinSubsequenceLength"));
        int  debrujinRepetitions = util::str2num<int>(params.find("DeBrujinRepetitions"));
        bool debrujinPreRandomize =util::str2num<bool>(params.find("DeBrujinPreRandomize"));
        this->debrujinize(debrujinSubSequenceLenght,debrujinRepetitions,debrujinPreRandomize);
    }

    fullExtractions = extractions;
}

/** @ingroup Experiment
* @brief Add a factor to the current factorial design
* @param name The name of the factor
* @param currentFactorScalar The levels of that factor
**/
template < typename Scalar >
void BalanceFactor<Scalar>::addFactor( const string &name, const vector< Scalar > &currentFactorScalar)
{  assert ( find(factorNames.begin(), factorNames.end(), name) == factorNames.end() );
    factorNames.push_back( name );  // a simple vector with random access is enough for storing names of factors in order without memleaks

    int Ntotal = 1;
    vector < string > currentFactor(currentFactorScalar.size());
    std::transform( currentFactorScalar.begin(), currentFactorScalar.end(), currentFactor.begin(), stringify<Scalar>  );

    factorsLevelsPairList.push_back( pair<string,vector<string> >(name, currentFactor) );

    typename deque< pair< string,vector<string> > >::iterator end(factorsLevelsPairList.end());

    for ( typename deque< pair< string,vector<string> > >::iterator iter = factorsLevelsPairList.begin(); iter!=end; ++iter )
        Ntotal*=(iter->second.size());
    int Nbase = Ntotal;
    Ntotal*=repetitions;

    factorsCartesianProduct.clear();

    size_t f=0;
    for ( typename deque< pair< string,vector<string> > >::iterator iter = factorsLevelsPairList.begin(); iter!=end; ++iter )
    {  size_t len1 =1;
        std::advance(iter,1);
        for ( typename deque < pair< string,vector<string> > >::iterator iter2 = iter; iter2!=end; (++iter2) )
        {  len1*= iter2->second.size();
        }
        std::advance(iter,-1);
        size_t len2=1;
        for ( typename deque< pair< string,vector<string> > >::iterator iter3 = factorsLevelsPairList.begin(); iter3!=iter; ++iter3 )       // multiplies for the size of the upper levels
            len2*=iter3->second.size();
        vector <string> tmpRow;
        for ( unsigned int r=0; r<repetitions; r++)
        {  for (size_t i=0; i<len2; i++)
            {  for ( typename vector<string>::iterator iter2 = iter->second.begin(); iter2!=iter->second.end(); ++iter2 )
                {  for ( size_t j=0; j< len1; j++ )
                        tmpRow.push_back(*iter2);
                }
            }
        }
        factorsCartesianProduct.push_back(pair<string,vector<string> > (factorNames.at(f++),tmpRow));
    }

    //Importante altrimenti ci si porta dietro l'ultimo deck di mappe
    extractions.clear();
    //print();
    // Ora riempie la lista delle estrazioni
    int i=0;
    while ( factorsCartesianProduct.front().second.size() !=0 )
    {  map<string,string> tmp;
        for ( typename deque< pair< string,vector<string> > >::iterator iter = factorsCartesianProduct.begin(); iter!=factorsCartesianProduct.end(); ++iter )
        {  tmp.insert( pair<string,string> (iter->first,iter->second.back()) );
            iter->second.pop_back();   // estrae la coda all'insieme del prodotto cartesiano dei fattori
        }
        // aggiunge due fattori alla mappa che servono ad identificare il numero di estrazioni dello stesso fattore ed il suo numero di ripetizione
        tmp.insert(pair<string,string>("Repetition",stringify<int>(i/Nbase ))  );
        tmp.insert(pair<string,string>("ReinsertedTimes",stringify<int>(0))  );
        i++;
        extractions.push_front(tmp);
    }

    if (randomizationMethod!=RANDOMIZATION_NONE)
        srand(time(0));// Force the randomization of seed!

    switch ( randomizationMethod )
    {
    case RANDOMIZE_EXTRA_BLOCK:
    {
        random_shuffle( extractions.begin(), extractions.end() );
        if ( randomizeWithoutConsecutive )
            maximimizeInnerDistance();
    }
        break;
    case RANDOMIZE_INTRA_BLOCK:
    {
        for (int i=0; i<Ntotal-Nbase;i+=Nbase)
        {
            random_shuffle(extractions.begin()+i,extractions.begin()+(i+Nbase));
        }
        // We use a simplified version of the maximizeInnerDistance algorithm which is needed to just extractions intra blocks when
        // tail of a block is equal to head of the next block
        if ( randomizeWithoutConsecutive )
        {
            for (int i=0; i<Ntotal-Nbase;i+=Nbase)
                shuffleIntraBlockHeadTail(i+Nbase-1, i+Nbase, i+2*Nbase-1);
        }
    }
        break;
    }
    // Keep a deep copy of the full extractions deque
    fullExtractions = extractions;
}

/**
* @ingroup Experiment
* @brief Get the size of the extractions deck
* \return The size of the extractions deck  (dynamically is the number of remaining trials)
**/
template < typename Scalar >
size_t BalanceFactor<Scalar>::size()
{  return extractions.size();
}

/**
* @ingroup Experiment
* @brief Reinsert a given factors map
* @param The factors to reinsert at the back of the extractions deck
**/
template < typename Scalar >
void BalanceFactor<Scalar>::reinsert(const map<string,Scalar> &factorMap )
{  map< string, string > mapToReinsert; // questa mappa avere i value castati a stringa partendo dalla mappa di ingresso.
    for ( typename map<string,Scalar>::const_iterator iter = factorMap.begin(); iter!=factorMap.end(); ++iter )
    {  mapToReinsert.insert( pair<string,string>( iter->first, stringify<Scalar>(iter->second) ) );
    }

    unsigned int nTimesThisMapHasBeenReinserted = str2num<unsigned int>(mapToReinsert["ReinsertedTimes"]);
    if ( nTimesThisMapHasBeenReinserted < maxReinsertion )
    {  mapToReinsert["ReinsertedTimes"] = stringify<unsigned int>(nTimesThisMapHasBeenReinserted+1);
        reinsertstring( mapToReinsert );
    }
}

/**
* @ingroup Experiment
* @brief Reinsert a given factors map as map of string, the cast is done automatically after
* @param The factors to reinsert at the back of the extractions deck as map of strings
**/
template < typename Scalar >
void BalanceFactor<Scalar>::reinsertstring(const map<string,string> &factorMap )
{  extractions.push_back( factorMap );
}

/**
* @ingroup Experiment
* @brief Return true if no extractions are left
* @return True if no extractions left
**/
template < typename Scalar >
bool BalanceFactor<Scalar>::isEmpty()
{  return extractions.empty();

}

/**
* @ingroup Experiment
* @brief Peel off the extractions deck and fill the factors map
* @return False if no extractions left
**/
template < typename Scalar >
bool  BalanceFactor<Scalar>::next()
{
    if ( !extractions.empty() )
    {  if ( randomizeWithoutConsecutive && currentTrial!=0 )
        {  while ( compareMaps(extractions.front(), currentFactorsMap ) )
            {  extractions.push_back(extractions.front() );
                extractions.pop_front();
            }
        }
        else
        {
            currentFactorsMap=extractions.front();
            extractions.pop_front();
        }
        // Create the current factors map that with the getCurrent() method is called.
        // We deferred the creation of currentFactors from getCurrent() with copy constructor to here because it is a slow operation
        // if it is carried out in realtime.
        this->previousFactors = this->currentFactors;
        this->currentFactors.clear();
        for ( typename map<string,string>::const_iterator iter = currentFactorsMap.begin(); iter!=currentFactorsMap.end(); ++iter )
        {
            currentFactors.insert(pair<string,Scalar>(iter->first, str2num<Scalar>(iter->second) ));
        }

        return true;
    }
    else
        return false;
}

template < typename Scalar >
bool  BalanceFactor<Scalar>::hasNext()
{
    return !extractions.empty();
}

/**
* @ingroup Experiment
* @brief Get the current factors map as map of strings
* \return The current factors map as map of strings
**/
template < typename Scalar >
map<string , string > BalanceFactor<Scalar>::getCurrentAsMapToString()
{  return currentFactorsMap;
}

template <typename Scalar>
unsigned int BalanceFactor<Scalar>::getRemainingTrials()
{  return (unsigned int)this->size();
};


/**
* @ingroup Experiment
* @brief Same as getCurrentAsMapToString but with next called automatically
* \return The current factors map as map of strings
**/
template < typename Scalar >
map <string , string > BalanceFactor<Scalar>::getNextAsMapToString()
{  next();
    return  getCurrentAsMapToString();
}

/**
* @ingroup Experiment
* @brief Get the current factors map as map of strings-Scalar
* \return The current factors map as map of strings-Scalar
**/
template < typename Scalar >
map<string , Scalar > & BalanceFactor<Scalar>::getCurrent()
{
    return this->currentFactors;
}

/**
* @ingroup Experiment
* @brief Get the next factors map
* \return The current factors map
**/
template <typename Scalar>
map <string , Scalar > BalanceFactor<Scalar>::getNext()
{
    next();
    return  getCurrent();
}

template < typename Scalar >
map<string , Scalar > & BalanceFactor<Scalar>::getPrevious()
{
    return this->previousFactors;
}

/**
* @ingroup Experiment
* @brief Get the next factors map
* \return The current factors map
**/
template <typename Scalar>
deque < map <string, Scalar> > BalanceFactor<Scalar>::getFullExtractions()
{
    deque < map <string, Scalar> > allExtractions;

    for ( deque < map<string, string> >::iterator iter = fullExtractions.begin(); iter!=fullExtractions.end() ; ++iter)
    {
        map< string, Scalar > tmp;
        for ( map<string,string>::iterator iter2 = iter->begin(); iter2!=iter->end(); ++iter2 )
        {
            tmp[iter2->first] = util::str2num<Scalar>(iter2->second);
        }
        allExtractions.push_back(tmp);
    }

    return allExtractions;
}

/**
* @ingroup Experiment
* @brief Print the extractions deck to output stream
* @param  os Output stream
**/
template < typename Scalar >
void BalanceFactor<Scalar>::print(ostream &os)
{  for ( deque < map<string, string> >::iterator iter = extractions.begin(); iter!=extractions.end() ; ++iter)
    {  for ( map<string,string>::iterator iter2 = iter->begin(); iter2!=iter->end(); ++iter2 )
        { // if ( iter2->first != "Repetition" && iter2->first != "ReinsertedTimes")
            os << iter2->first << ": " << iter2->second << "\t";
        }
        os << "----" << endl;
    }
}

/**
 *
 */
template < typename Scalar >
void BalanceFactor<Scalar>::print()
{  print(cerr);
}

/**
 *
 */
template < typename Scalar >
void BalanceFactor<Scalar>::print(const std::string &filename)
{  
    ofstream factorslist;
    factorslist.open(filename.c_str());
    print(factorslist);
    factorslist.close();
}

/**
* @ingroup Experiment
* @brief Compare two maps but ignoring the keys "Repetition" and "ReinsertedTimes"
* @param map1 The first element to compare
* @param map2 The second element to compare
* \return True if they are equal (except the keys "Repetition" and "ReinsertedTimes")
**/
template < typename Scalar >
bool BalanceFactor<Scalar>::compareMaps( const map<string,string> &map1, const map<string,string> &map2 )
{  return std::equal(map1.begin(), map1.end(), map2.begin(), mapcomparisonfunctor );
}

/**
* @ingroup Experiment
* @brief Compute the total distance between extractions in the extractions deck
* @param input An extractions deck
* \return A number specifying the number of consecutive equal pairs
**/
template < typename Scalar >
int BalanceFactor<Scalar>::computeDistanceBetweenConsecutiveExtractions( const deque< map< string,string > > &input )
{  int distance=0;
    deque < map<string,string> >::const_iterator end = input.end()-1;
    for ( deque< map<string,string> >::const_iterator iter = input.begin(); iter!=end; ++iter )
    {  deque< map<string,string> >::const_iterator iter2 = iter;
        if ( iter2 != input.end()-1 )
            std::advance(iter2,1);

        if ( compareMaps(*iter, *iter2 ) )
        {  distance++;
        }
    }
    return distance;
}

static void debruijn(unsigned int t, unsigned int p, const unsigned int k, const unsigned int n, unsigned int* a, boost::function<void (unsigned int*,unsigned int*)> printerFunction)
{
    if (t > n)
    {
        // we want only necklaces, not pre-necklaces or Lyndon words
        if (n % p == 0) {
            printerFunction(a+1, a+p+1);
        }
    }
    else
    {
        a[t] = a[t-p];
        debruijn(t+1, p, k, n, a, printerFunction);
        for (unsigned int j = a[t-p]+1; j < k; ++j)
        {
            a[t] = j;
            debruijn(t+1, t, k, n, a, printerFunction);
        }
    }
}

/**
 * @brief The seq_printer struct
 */
struct seq_printer
{
    const std::vector<unsigned int>& _alpha;
    seq_printer(const std::vector<unsigned int>& alpha) : _alpha(alpha) {}
    void operator() (unsigned int* a, unsigned int* a_end) const
    {
        for (unsigned int* i = a; i < a_end; ++i)
        {
            //cout << _alpha[*i] << endl;
            debrujinizedSequence.push_back(_alpha[*i]);
            //debrujinizedSequence.push_back(*i);
        }
    }
};


/**
 * Makes the trial sequence a De Brujin sequence of subsequence length N
 */
/* =============== DEPRECATED IT GENERATES A SMALL SUBSET OF ALL POSSIBLE DEBRUIJN
 * SEQUENCES ==================
*/
/*
template < typename Scalar >
void BalanceFactor<Scalar>::debrujinize(unsigned int subSequenceLength, unsigned int nDeBrujinRepetitions, bool prerandomize )
{
    // With this function we transform the trial sequence contained in 'extractions'
    // deque with a DeBrujin sequence of trial, where every combination of trial_i and trial_(j) (for subsequence lenght=2 for example) is present
    // We rely on the implementation of the algorithm of Sawada and Ruskey
    // http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.13.2152
    // Joe Sawada and Frank Ruskey, "An Efficient Algorithm for Generating Necklaces with Fixed Density", SIAM Journal of Computing 29:671-684, 1999.

    unsigned int nIndices = extractions.size()*nDeBrujinRepetitions/repetitions; //normalize with the total number of extractions
    std::vector<unsigned int> indices(nIndices);
    for (unsigned int i=0; i<nIndices; i++ )
        indices.at(i)=i;

    if (prerandomize)
        std::random_shuffle(indices.begin(),indices.end());
    // Clear any previous result collected and prepare to run the debrujin recursion,
    debrujinizedSequence.clear();
    // Prepare the vector a for the debrujin fuction
    vector<unsigned int> a(subSequenceLength+1);
    a.assign(a.size(),0);
    a[0] = 0;
    debruijn(1,1,indices.size(),subSequenceLength,&a[0],seq_printer(indices));

    for (unsigned int i=0; i<debrujinizedSequence.size();++i)
    {
        debrujinizedSequence.at(i)=debrujinizedSequence.at(i)%(extractions.size());
    }

    // add the front element to the end of the sequence to make it cyclic
    debrujinizedSequence.push_back(debrujinizedSequence.front());

    // Use a temporary extractionsDeque containing the trial list with the order from the debrujinized sequence
    deque < map<string, string> > extractionsDeBrujinized;
    for (unsigned int i=0; i<debrujinizedSequence.size();++i )
        extractionsDeBrujinized.push_back(extractions.at(debrujinizedSequence.at(i)));
    // Copy the debrujinized deque to the original extractions deque
    extractions = extractionsDeBrujinized;

    //cerr << "Sequence length = " << extractions.size() << endl;
}
*/

/**
 * @brief BalanceFactor::generateDebrujinGraph
 * @param subSequenceLength
 * @param nDeBrujinRepetitions
 * @param prerandomize
 */
#pragma warning ( disable : 4250 )
template < typename Scalar >
void BalanceFactor<Scalar>::debrujinize(unsigned int subSequenceLength, unsigned int nDeBrujinRepetitions, bool prerandomize )
{
    int m = extractions.size()*nDeBrujinRepetitions/repetitions;// number of symbols
    int n = subSequenceLength - 1; // effective subsequence length

    int nvert = (int) std::pow((double)m, (double)n);

    Eulercircuit<DIRECTED> debruijnGraph(nvert);
    debruijnGraph.randomizeEulerianCircuits = prerandomize;

    cerr << "m=" << m << " n=" << n << endl;
    //MatrixXi M(nvert,nvert);
    //M.setZero(nvert,nvert);
    for (int i=0; i<nvert; ++i)
    {
        for (int mm=(i*m); mm<(i+1)*m; ++mm)
        {
            //M(i,mm%nvert)=1;
            debruijnGraph.addEdge(i,mm%nvert);
        }
    }

    //cout << M << endl;

    // Generate the Eulerian cycle from vertex 0
    debruijnGraph.generate(0);
    // Push the first n sequences on bottom
    for (int i=0; i<n; ++i)
        debruijnGraph.circuit.push_back(debruijnGraph.circuit.at(i));

    //Use the eulerian circuit path to index the extractions and create the final list of factors
    for (unsigned int i=0; i<debruijnGraph.circuit.size();++i)
    {
        debruijnGraph.circuit.at(i)=debruijnGraph.circuit.at(i)%(extractions.size());
    }

    // Use a temporary extractionsDeque containing the trial list with the order from the debrujinized sequence
    deque < map<string, string> > extractionsDeBrujinized;
    for (unsigned int i=0; i<debruijnGraph.circuit.size();++i )
        extractionsDeBrujinized.push_back(extractions.at(debruijnGraph.circuit.at(i)));
    // Copy the debrujinized deque to the original extractions deque
    extractions = extractionsDeBrujinized;
}

#endif
