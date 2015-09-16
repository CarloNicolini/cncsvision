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

#ifndef __UTIL_H
#define __UTIL_H

// standard includes
#include <algorithm>
#include <iterator>
#include <cmath>
#include <vector>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <limits>
#include <iostream>

#include <sys/stat.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions.hpp>
//#include <boost/filesystem.hpp>

#define STATIC_STRINGIFY(A)  #A
using namespace std;
/**
* \defgroup Util
* \brief This namespace contains various utility functions which extend some C++ functionalities. They are all inlined.

* \namespace util
* \brief This namespace contains various utility functions which extend some C++ functionalities. They are all inlined.
**/

namespace util
{
/**
* \ingroup Util
* 
* Jump to a given line in input file 
**/
inline std::ifstream& goToLine(std::ifstream& file, unsigned int num)
{
    file.seekg(std::ios::beg);
    for(unsigned int i=0; i < num - 1; ++i)
    {
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}


/**
* \ingroup Util
* 
* Check if a file without the substring junk already exists.
**/
inline bool fileExists(const string &file)
{
	string junkSubString("junk");
	if (file.find(junkSubString) != string::npos)
	{
		return false; // if the substring "junk" is found then always return false
	}
    struct stat buf;
	int res = stat(file.c_str(), &buf);
    return (res == 0);
}

/**
 * @brief readFile
 * @param file
 * @return
 */
inline std::string readFile(const std::string &file)
{
    if (!fileExists(file))
    throw std::runtime_error(file + std::string("doesn't exist"));

    // Using this method
    // http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    std::ifstream t(file.c_str());
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
    return str;
}

/**
* \ingroup Util
* Tokenize a std::string using a string of delimiters and put the output as a vector of tokens (splitted string)
* \param str String to tokenize
* \param tokens Tokens in which the string is splitted
* \param delimiters Delimiters to split the input string
**/
inline void tokenizeString(const string& str, vector<string>& tokens, const string& delimiters)
{  tokens.clear();
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {  // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
* \ingroup Util
* Cast a string to a number via lexical cast, for example
* \code
* string x("923.12");
* double y = str2num<double>(x);
* cout << y << endl;
* \endcode
* \param str String to cast
* \return lexically casted type
**/
template <class T> T str2num( const std::string& s )
{  
    return boost::lexical_cast<T>(s);
}

/**
* \ingroup Util
 Explicit template specialization for conversion of strings to double which is 80x faster than boost::lexical_cast
 */
template <> inline double  str2num<double>( const std::string& s )
{
    const char *p = s.c_str();
    double r = 0.0;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        r = (r*10.0) + (*p - '0');
        ++p;
    }
    if (*p == '.') {
        double f = 0.0;
        int n = 0;
        ++p;
        while (*p >= '0' && *p <= '9') {
            f = (f*10.0) + (*p - '0');
            ++p;
            ++n;
        }
        r += f / std::pow(10.0, n);
    }
    if (neg) {
        r = -r;
    }
    return r;
}

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
    tokenizeString(s,tokenized,delimiter);
    vector<T> ret(tokenized.size());
    std::transform(tokenized.begin(),tokenized.end(), ret.begin(), boost::lexical_cast<T,string> );
    return ret;
}



/**
* \ingroup Util
* Cast a vector of strings to a vector of numbers
* \code
* string x("123");
* vector xv;
* xv.push_back(x);
* xv.push_back(x);
* xv.push_back(x);
* vector<int> y = str2num<int>(x);
* for (int i=0; i<y.size(); i++ )
* cout << y.at(i) << endl;
* \endcode
* Will print:
* \code
* 123
* 123
* 123
* \endcode
* \param s String to cast to vector
* \return A vector with each element casted from the correspondent element of the input string
**/
template <class T> vector<T> str2num( const std::vector<std::string>& s )
{  
    vector<T> ret;
    for (   vector<std::string>::const_iterator iter = s.begin(); iter!=s.end(); ++iter )
        ret.push_back(str2num<T>(*iter));
    return ret;
}

/**
* \ingroup Util
* Cast a number to string via lexical cast
* \code
* int a=10;
* string s = stringify<int>(a);
* double x=-1.435346;
* string s2 = stringify<int>(x);
* \endcode
* \param t The number to cast to string
* \return the output string
**/
template <class T> string stringify(const T &t)
{
    std::ostringstream o;
    o<< t;
    return o.str();
    // return boost::lexical_cast<string>(t);
}

/**
* \ingroup Util
* Cast a vector of type T to a vector of strings
* 
* \param t The vector to cast to vector of strings
* \return the output vector of strings string
**/
template <class T> vector<string> num2str( const std::vector< T >& s )
{  
    vector<string> ret(s.size());
    std::transform(s.begin(), s.end(), ret.begin(), boost::lexical_cast<string,T> );
    return ret;

}

/**
* \ingroup Util
* Useful initializer for vectors
* \code
* vector<int> x = vlist_of<int>(1)(2)(3)
* \endcode
**/
template <typename T>
struct vlist_of : public vector<T>
{  vlist_of(const T& t)
    {  (*this)(t);
    }
    vlist_of& operator()(const T& t)
    {  this->push_back(t);
        return *this;
    }
};

/** 
* \ingroup Util
* Assertion template for error checking
*
* \param assert     the assertion to check
*
* This template throws an exception of type Except when the assertion
* of type Assertion is false. It is shamelessly adapted (stolen) from
* Stroustrup, The C++ Programming Language, 3rd ed.
*
**/
template <class Except, class Assertion>
inline void Assert(Assertion assert)
{  if (!assert) throw Except();
}

/** 
* \ingroup Util
* Cast all elements of a vector from one type to another
*
* \param x   the vector with elements to cast
*
* This template will cast elements of type From to elements of type
* To and return the resulting vector
**/
template <class To, class From>
inline std::vector<To> vector_cast(std::vector<From>& x)
{  unsigned end = x.size();
    std::vector<To> y(end);
    for (unsigned i = 0; i < end; ++i)
    {  y[i] = static_cast<To>(x[i]);
    }
    return y;
}


/**
* \ingroup Util
* \class PrintForVector
*
* This template provides a helper class for the template function
* below for std::vector<T> operator <<
**/
template <class T>
class PrintForVector
{
public:

    explicit PrintForVector(std::ostream& os)
        : os_(os), first_(true)
    {}
    void operator()(const T& x)
    {  if (!first_)
        {  os_ << " ";
        }
        else
        {  first_ = false;
        }
        os_ << x;
    }
private:
    std::ostream& os_;
    bool first_;
};

/**
* \ingroup Util
* ostream& operator<< for vectors
*
* \param os  the ostream
* \param x   the vector
**/
template <class T>
std::ostream& operator<<(std::ostream& os, std::vector<T> x)
{
    std::for_each(x.begin(), x.end(), PrintForVector<T>(os));
    return os;
}

struct RetrieveKey
{
    template <typename T>
    typename T::first_type operator()(T keyValuePair) const
    {
        return keyValuePair.first;
    }
};

template<class ForwardIt>
ForwardIt max_element_nth(ForwardIt first, ForwardIt last, int n)
{
    if (first == last) {
        return last;
    }
    ForwardIt largest = first;
    first += n;
    for (; first < last; first += n) {
        if (*largest < *first) {
            largest = first;
        }
    }
    return largest;
}

template<class ForwardIt>
ForwardIt min_element_nth(ForwardIt first, ForwardIt last, int n)
{
    if (first == last)
    {
        return last;
    }

    ForwardIt smallest = first;
    first += n;
    for (; first < last; first += n) {
        if (*smallest > *first) {
            smallest = first;
        }
    }
    return smallest;
}

}
#endif
