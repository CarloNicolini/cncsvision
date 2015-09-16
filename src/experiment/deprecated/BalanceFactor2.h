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

#include "ParametersLoader.h"
#include "Util.h"
#include "BalanceFactor2.h"
#include "ParametersLoader.h"
#include "Mathcommon.h"

using namespace std;
using namespace Eigen;
using util::stringify;
using util::str2num;

inline bool mapcomparisonfunctor(const std::pair<string,string> &s1, const std::pair<string,string> &s2 )
{
 if ( s1.first == "Repetition" || s1.first == "ReinsertedTimes" )
 	return true;	// it ignores these two keys
 if ( s1.first == s2.first )
 	return (s1.second == s2.second);
 else
 	return false;
}


template < typename Scalar >
class BalanceFactor2
{
private:
   bool randomize;
   unsigned int repetitions;
   unsigned int currentTrial;
   unsigned int maxReinsertion;
   bool randomizeWithoutConsecutive;

   // Primary data structures is a list of names and total factors
   deque < pair<string, vector<string> > > factorsLevelsPairList;
   deque < pair<string, vector<string> > > factorsCartesianProduct;

   deque < string > factorNames;
   map<string, string> currentFactorsMap;
   
   // Main data structure to extract factors maps
   deque < map <string, string> > extractions;
   
   void reinsertstring( const map<string, string> &factorsToReinsert );
   bool compareMaps( const map<string, string> &map1,  const map<string, string> &map2 );
   // This part takes in account the randomization without repetitions
   int computeDistanceBetweenConsecutiveExtractions( const deque< map< string,string > > &input );
   void maximimizeInnerDistance();
   
public:
   BalanceFactor2();
   ~BalanceFactor2();
   void init( unsigned int _repetitions , bool _randomize, bool _randomizeWithoutConsecutive, unsigned int _maxReinsertion=0 );
   void init( const ParametersLoader &, bool _randomizeWithoutConsecutive=false);
   void addFactor( const std::string &name, const vector< Scalar > &currentFactor);

   size_t size();
   // for retrocompatibility
   unsigned int getRemainingTrials()
   {  return (unsigned int)size();
   };

   void next();
   map< std::string, string > getNextAsMapToString();
   map< std::string, string > getCurrentAsMapToString();
   
   map< std::string, Scalar > getNext();
   map< std::string, Scalar > getCurrent();


   void reinsert( const map<string, Scalar> & );

   bool isEmpty();
   void print(){ print(cerr); };
   void print(ostream &os);

};

// BEGIN IMPLEMENTATION
template < typename Scalar >
void BalanceFactor2<Scalar>::maximimizeInnerDistance( )
{
   int iterations=0;
   unsigned int M = extractions.size();
   if ( M == repetitions ) // to handle the single factor-single level case
   {
   	//cerr << "Can't shuffle this configuration, extractions total size= " << M << endl;
   	return;
   }
   int dist = computeDistanceBetweenConsecutiveExtractions(extractions);
   int lowerBoundDist  = 0;
   
   
   while (  dist > lowerBoundDist )
   { for (unsigned int i=0; i<M-2; i++ )
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
   cerr << "iterations= " << iterations << endl;
}

template < typename Scalar >
BalanceFactor2<Scalar>::BalanceFactor2()
{  currentTrial=0;
   maxReinsertion=0;
   randomizeWithoutConsecutive=false;
}


template < typename Scalar >
BalanceFactor2<Scalar>::~BalanceFactor2()
{
}

template < typename Scalar >
void BalanceFactor2<Scalar>::init(unsigned int _repetitions, bool _randomize, bool _randomizeWithoutConsecutive, unsigned int _maxReinsertion )
{  repetitions=_repetitions;
   randomize = _randomize;
   randomizeWithoutConsecutive= _randomizeWithoutConsecutive;
   currentTrial=0;
   maxReinsertion=_maxReinsertion;
   factorsLevelsPairList.clear();
   factorsCartesianProduct.clear();
   factorNames.clear();
   currentFactorsMap.clear();
   extractions.clear();
}

template < typename Scalar >
void BalanceFactor2<Scalar>::init( const ParametersLoader &parametersLoader, bool _randomizeWithoutConsecutive )
{
	repetitions	=	str2num<int>(parametersLoader.find("Repetitions"));
	randomize	=	str2num<int>(parametersLoader.find("Randomize"));
	if ( parametersLoader.exists(("MaxReinsertions")) )
		maxReinsertion = str2num<int>(parametersLoader.find("MaxReinsertions"));
	else
		maxReinsertion=0;
	if ( parametersLoader.exists(("RandomizeWithoutRepetitions")) )
		randomizeWithoutConsecutive = str2num<bool>(parametersLoader.find("RandomizeWithoutRepetitions"));
	else
		randomizeWithoutConsecutive = _randomizeWithoutConsecutive;
		
	init(repetitions,randomize,randomizeWithoutConsecutive,maxReinsertion);
	
   multimap <string, string> mmap = parametersLoader.getAllParameters();
   for ( multimap<string, string>::iterator iter = (mmap.begin()); iter!=mmap.end(); ++iter)
   {  vector<Scalar> tmpFactor;
      if ( iter->first[0] == 'f' )    // this is a factor! We must use it
      {  string factor =  string(iter->first).erase(0,1);
         // Here we proceed if and only if the factors has not been just added. Very important!
         if ( find(factorNames.begin(), factorNames.end(), factor) == factorNames.end() )    // insert the factor if it is not in the just known factors list
         {
            pair<multimap<string,string>::iterator, multimap<string,string>::iterator> ret;
            multimap<string,string>::iterator it;
            ret = mmap.equal_range((*iter).first);
            for ( it=ret.first; it!=ret.second; ++it )
            	tmpFactor.push_back( str2num<Scalar>((*it).second) ) ;
            	
          addFactor( factor, tmpFactor );
         }
       }
   }

}

template < typename Scalar >
void BalanceFactor2<Scalar>::addFactor( const std::string &name, const vector< Scalar > &currentFactorScalar)
{
   assert ( find(factorNames.begin(), factorNames.end(), name) == factorNames.end() );
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
      tmp.insert(pair<string,string>("Repetition",stringify<int>(i/Nbase ))  );
      tmp.insert(pair<string,string>("ReinsertedTimes",stringify<int>(0))  );
      i++;
      extractions.push_front(tmp);
   }
   
   if ( randomize )
   {
      random_shuffle( extractions.begin(), extractions.end() );
   	if ( randomizeWithoutConsecutive )
   		maximimizeInnerDistance();
   }
}


template < typename Scalar >
size_t BalanceFactor2<Scalar>::size()
{  return extractions.size();
}

template < typename Scalar >
void BalanceFactor2<Scalar>::reinsert(const map<string,Scalar> &factorMap )
{  map< string, string > mapToReinsert; // questa mappa avere i value castati a stringa partendo dalla mappa di ingresso.
   for ( typename map<string,Scalar>::const_iterator iter = factorMap.begin(); iter!=factorMap.end(); ++iter )
   {  mapToReinsert.insert( pair<string,string>( iter->first, stringify<Scalar>(iter->second) ) );
   }
   
   unsigned int nTimesThisMapHasBeenReinserted = str2num<unsigned int>(mapToReinsert["ReinsertedTimes"]);
   if ( nTimesThisMapHasBeenReinserted < maxReinsertion )
   {
   	mapToReinsert["ReinsertedTimes"] = stringify<unsigned int>(nTimesThisMapHasBeenReinserted+1);
   	reinsertstring( mapToReinsert );
   }
}

template < typename Scalar >
void BalanceFactor2<Scalar>::reinsertstring(const map<string,string> &factorMap )
{
      extractions.push_back( factorMap );
}

template < typename Scalar >
bool BalanceFactor2<Scalar>::isEmpty()
{  return extractions.empty();

}

template < typename Scalar >
void  BalanceFactor2<Scalar>::next()
{  
   if ( !extractions.empty() )
   {
   if ( randomizeWithoutConsecutive && currentTrial!=0 )
   {	
   	while ( compareMaps(extractions.front(), currentFactorsMap ) )
   	{
   		extractions.push_back(extractions.front() );
   		extractions.pop_front();
   	}
   }
   else
   {
   	currentFactorsMap=extractions.front();
   	extractions.pop_front();
   }
   
   }
}

template < typename Scalar >
map<std::string , string > BalanceFactor2<Scalar>::getCurrentAsMapToString()
{  return currentFactorsMap;
}


template < typename Scalar >
map <std::string , string > BalanceFactor2<Scalar>::getNextAsMapToString()
{  next();
   return  getCurrentAsMapToString();
}


template < typename Scalar >
map<std::string , Scalar > BalanceFactor2<Scalar>::getCurrent()
{
	map<string,Scalar> tmpFactors;
	
	for ( typename map<string,string>::const_iterator iter = currentFactorsMap.begin(); iter!=currentFactorsMap.end(); ++iter )
   	{
   		tmpFactors.insert(pair<string,Scalar>(iter->first, str2num<Scalar>(iter->second) ));
   	}
   	
	return tmpFactors;
}

template <typename Scalar>
map <std::string , Scalar > BalanceFactor2<Scalar>::getNext()
{  next();
   return  getCurrent();
}


template < typename Scalar >
void BalanceFactor2<Scalar>::print(ostream &os)
{  
   for ( deque < map<string, string> >::iterator iter = extractions.begin(); iter!=extractions.end() ; ++iter)
   {  for ( map<string,string>::iterator iter2 = iter->begin(); iter2!=iter->end(); ++iter2 )
      {  
      	if ( iter2->first != "Repetition" && iter2->first != "ReinsertedTimes")
      	os << iter2->first << ": " << iter2->second << "\t";
      }
      os << "----" << endl;
   }
}

template < typename Scalar >
bool BalanceFactor2<Scalar>::compareMaps( const map<string,string> &map1, const map<string,string> &map2 )
{
return std::equal(map1.begin(), map1.end(), map2.begin(), mapcomparisonfunctor );
}

template < typename Scalar >
int BalanceFactor2<Scalar>::computeDistanceBetweenConsecutiveExtractions( const deque< map< string,string > > &input )
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


// END IMPLEMENTATION

#endif
