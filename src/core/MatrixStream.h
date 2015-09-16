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

#ifndef MATRIX_LOADER
#define MATRIX_LOADER

#include <iostream>
#include <sstream>
#include <fstream>


#include <Eigen/Core>

/**
* \class MatrixStream
* \ingroup Geometry
* \brief Read a stream of matrices. Every line contains a matrix in column-major order.
* An example of how to use this class is the following: suppose you have a file with 60 rows of numbers on 20 columns and you want to load it in a 60x20 matrix:
* \code
* ifstream file;
* file.open("example.dat"); //this is the file containing the matrix
* MatrixStream< Matrix<double,60,20> > ms;
* Eigen::Matrix<double,60,20> = ms.next(file);
* \endcode
* Otherwise if you want to load the file into 2 different matrices 30x20 do the following.
* \code
* ifstream file;
* file.open("example.dat");
* MatrixStream< Matrix<double,30,20> > ms1;
* MatrixStream< Matrix<double,30,20> > ms2;
* Eigen::Matrix<double,30,20 > m1 = ms1.next(file);
* Eigen::Matrix<double,30,20> m2 = ms2.next(file);
* \endcode
* and so on, you can apply the \code MatrixStream::next(const ifstream &) \endcode method until the input stream reaches its EOF.
**/

using namespace std;
using namespace Eigen;

/**
 * @brief MatrixStream is a class to load matrices row by row from files and/or loading them
 **/
template <typename Derived >
class MatrixStream
{
public:
    /**
    * @brief MatrixStream<Derived> constructor, you must specify the Eigen matrix to use
    */
   MatrixStream < Derived > ();
   /**
    * @brief next method allows you to loop on a file stream and load N rows a time the matrix
    * @param is file stream to convert in matrix
    * @return
    */
   const MatrixBase< Derived >& next(istream &is);
   Derived m;
};

template <typename Derived >
MatrixStream< Derived >::MatrixStream()
{  m = MatrixBase<Derived>::Identity();

}

template <typename Derived >
const MatrixBase<Derived>& MatrixStream< Derived >::next(istream &is)
{  int lenght=m.rows()*m.cols();

   for (int i=0; i<lenght; i++)
      is >> m(i);

   return m;
}


#endif
