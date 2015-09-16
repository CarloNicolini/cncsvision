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

/**
* \class ParametersLoader
* \ingroup Experiment
* \brief Allows you to load all the experimental parameters in a single file and access them via a simple std::multistd::map.

* You first have to initialize the object via the \verbatim ParametersLoader::loadParameterFile(ifstream &) \endverbatim    method.
* The parameters file must be a text file, with each row containing the parameter name and/or the factor you want to include in your factorial design.
* For example:
* \code
* SubjectName: Renato
* IOD: 65
* Repetitions: 3
* Randomize: 0
* fTilt: 0
* fSlant: 45
* fAnchored: 2
* fRotationSpeed: 1
* fStimSpeed: 4
* \endcode
*
* In this case you can get the parameter you need via the \verbatim getAllParameters() \endverbatim method.
* \code
* ifstream paramFile;
* paramFile.open("parameters.txt");
* ParametersLoader params;
* params.loadParameterFile(paramFile);
* // Here for example we want to set the variable iod to the value contained in the parameters file:
* int iod = params.getAllParameters().find("IOD")->second;
* Here the type is automatically casted to the correct precision.
* \endcode
**/

#ifndef _PARAMETERSLOADER_H
#define _PARAMETERSLOADER_H

#include <vector>
#include <map>
#include <string>

class ParametersLoader
{
private:
   std::multimap<std::string, std::string> params;
   ParametersLoader(const ParametersLoader&) {}; //prevent copying
public:
   ParametersLoader();
   ParametersLoader(const std::string &filename);
   void loadParameterFile(std::ifstream &);
   void loadParameterFile(const std::string &filename );
   void addParameter(const std::string &key, const std::string &value);
   void print();
   void reset();
   bool exists(const std::string& ) const;
   std::string find( const std::string &value) const ;
   std::string find(const std::string &value, int index) const;
   std::vector<std::string> findAsVector(const std::string &value) const;
   double get(const std::string &parameterName) const;
   const std::multimap<std::string, std::string>& getAllParameters() const;
};


#endif
