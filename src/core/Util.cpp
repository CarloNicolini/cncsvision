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
/**
* \ingroup Util
* Cast a string to a vector of numbers if the string contains a delimiter
* \code
* string x("923,12,43,214,12");
* vector<int> y = str2num<int>(x,",");
* for (int i=0; i<y.size(); i++ )
* cout << y.at(i) << endl;
* \endcode
* Will print:
* \code
* 923
* 12
* 43
* 214
* 12
* \endcode
* \param s String to cast to vector
* \param delimiter String delimiter
* \return lexically casted vector
**/
template <class T> vector<T> str2num( const std::string& ss, const std::string &delimiter )
{
   string s=ss;
   boost::algorithm::trim(s);
   vector<string> tokenized;
   util::tokenizeString(s,tokenized,delimiter);
   vector<T> ret(tokenized.size());
   std::transform(tokenized.begin(),tokenized.end(), ret.begin(), boost::lexical_cast<T,string> );
   return ret;
}
