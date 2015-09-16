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
#include <map>

#include "Util.h"
#include "ParametersLoader.h"
#include "Mathcommon.h"

//using namespace std;
//using namespace mathcommon;
/** \ingroup Experiment
 * \brief Default constructor
 *
 */
ParametersLoader::ParametersLoader()
{  // First set the null argument for each options

}

/**
 * @brief ParametersLoader::ParametersLoader
 * @param filename
 */
ParametersLoader::ParametersLoader(const string &filename)
{
    this->loadParameterFile(filename);
}

/**
 * \ingroup Experiment
 * \brief Load a file containing the parameters.
 * Here you must give the reference to input stream
 * \param is Reference to input stream file.
*/
void ParametersLoader::loadParameterFile(ifstream &is)
{  // Set an hashcode for string values
    assert(is.good()); // controls if the input file is valid

    string line;
    int numline=0;
    while ( getline(is,line) )
    {  if ( line.size()<=1 )
            continue;

        vector <string> tokens;

        util::tokenizeString(line, tokens, ":\t\r  ");
        //if the first part of the string is Basedir and we are on windows we must allow ABSOLUTE directory names such as C:

        if ( tokens.at(0).at(0) == '#' )
            continue; // comments

#ifdef _WIN32
        // Here we give a different treatment for base directories in window, because : are allowed to specify directories
        if ( tokens.at(0)=="BaseDir" )
        {
            //cerr << "Special token \"Basedir\" found, merging substrings when \":\" token is found!" << endl;
            for (unsigned int i=1; i<tokens.size()-1; i++)
            {
                if ( tokens.at(i).find(":") == string::npos )
                {
                    tokens.at(i) += ":" + tokens.at(i+1);
                    tokens.pop_back();
                }
            }
#ifdef DEBUG
            cerr << "BaseDir is set to \"";
            for (int i=1; i<tokens.size(); i++)
            {
                cerr << tokens.at(i) << " ";
            }
            cerr << "\"" << endl;
#endif
        }
#endif
        pair<string,string> couple;
        couple.first = tokens.at(0);

        for ( vector<string>::iterator iter = (++tokens.begin()); iter!=tokens.end(); ++iter)
        {
            if ( (*iter).at(0) == '#')  // to allow online comments
                break;
            if ( (*iter) != "\r" )  //to avoid double spaces and to insert comments!
            {  couple.second = (*iter);
                params.insert(couple);
            }
        }
        numline++;
        line.clear();
    }
}

/**
 * @brief ParametersLoader::addParameter
 * @param key
 * @param value
 */
void ParametersLoader::addParameter(const string &key, const string &value)
{
    if (!this->exists(key))
    {
        vector <string> tokens;
        util::tokenizeString(value, tokens, ":\t\r  ");

        pair<string,string> couple;
        couple.first = key;

        for ( vector<string>::iterator iter = tokens.begin(); iter!=tokens.end(); ++iter)
        {
            if ( (*iter) != "\r" )  //to avoid double spaces!
            {  couple.second = (*iter);
                params.insert(couple);
            }
        }
    }
    else
        throw std::runtime_error("Parameter \""+key + " \"already exists.");
}

/**
  \ingroup Experiment
  Load a parameters file from filename
  \param filename The filename to load
**/
void ParametersLoader::loadParameterFile(const std::string &filename)
{
    ifstream is;
    is.open(filename.c_str());
    loadParameterFile(is);
    is.close();
}
/*
 * \ingroup Experiment
 * \brief Returns all the parameters read in the file.
 * \return params The parameters of the experiment as multimap of strings.
*/
const multimap<string, string> & ParametersLoader::getAllParameters() const
{  return params;
}

/* \ingroup Experiment
 * \brief Print the parsed parameters file to standard output
*/
void ParametersLoader::print()
{  for ( multimap<string, string>::iterator iter = (params.begin()); iter!=params.end(); ++iter)
    {  std::cout << iter->first << "\t" << iter->second << endl;
    }
}

/** \ingroup Experiment
* \brief Check if a parameter identifier exists
* \return true if exists, false otherwise
**/
bool ParametersLoader::exists(const std::string &identifier) const
{  multimap<string, string>::const_iterator iter = params.find(identifier);
    if ( iter != params.end() )
        return true;
    else
        return false;
}

/* \ingroup Experiment
 * \brief Find a given parameters with its name as string.
 *
 * Return that parameter if found else print "Can't find such identifier" on error output and exit.
 * \param s String representing the parameter name as written in parameters file.
 * \return The parameter if found
*/
std::string ParametersLoader::find(const std::string &value ) const
{  multimap<string, string>::const_iterator iter = params.find(value);
    if ( iter != params.end() )
        return iter->second;
    else
    {
        string ex("Can't find such parameter \"" + value + "\"" );
        cerr << ex << endl;
        throw std::invalid_argument(ex);
    }
}

/**
 * @brief ParametersLoader::find
 * @param key
 * @param delimiter
 * @param index
 * @return
 */
std::string ParametersLoader::find(const std::string &value, int index) const
{
    multimap<string, string>::const_iterator iter = params.find(value);
    if (iter == params.end())
    {
        throw std::invalid_argument("Can't find such parameter \"" + value + "\"" );
    }
    std::advance(iter,index);

    if ( iter != params.end() )
    {
        return iter->second;
    }
    else
    {
        string ex("Can't advance to such token index for parameter \"" + value + "\"" );
        cerr << ex << endl;
        throw std::invalid_argument(ex);
    }
}

std::vector<std::string> ParametersLoader::findAsVector(const std::string &value) const
{
    vector<string> result;
	pair<multimap<string, string>::const_iterator, multimap<string, string>::const_iterator> range = params.equal_range(value);
    for (multimap<string, string>::const_iterator i = range.first; i != range.second; ++i)
        result.push_back(i->second);
    return result;
}

/* \ingroup Experiment
 * \brief Reset the current parameters list contained in the internal multimap.
 *
*/
void ParametersLoader::reset()
{  params.clear();
}

/**
 * @brief ParametersLoader::get
 * @param parameterName
 * @return
 */
double ParametersLoader::get(const string &parameterName) const
{
    return util::str2num<double>(this->find(parameterName));
}
