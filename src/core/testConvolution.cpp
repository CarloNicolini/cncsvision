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
#include <Eigen/Core>
#include <Eigen/Dense>
#include <algorithm>
#include <complex>

#include "MatrixStream.h"
#include "Convolution.h"
#include "Mathcommon.h"
#include "Timer.h"


using namespace std;
using namespace Eigen;


complex<float> cexp( complex<float> a )
{  return exp(a);
}

float carg( complex<float> a )
{  return arg(a);
}

float cabs( complex<float> a)
{  return abs(a);
}

std::pointer_to_unary_function  < std::complex<float>, float > complexphase (carg);
std::pointer_to_unary_function <  std::complex<float>, std::complex<float> > complexexp ( cexp );
std::pointer_to_unary_function <  std::complex<float>, float > complexnorm ( cabs );

std::pointer_to_unary_function < float , float > expz (exp);
std::pointer_to_unary_function < float, float > cosz (cos);
std::pointer_to_unary_function < float, float > sinz (sin);

int main(void)
{  //srand(time(0));

   Matrix<float,16,16> I;
   MatrixStream< Matrix<float,16,16> > stream;

   ifstream image;
   image.open("phantom.dat");
   I=stream.next(image);
   image.close();
   I.transposeInPlace();
   Matrix<float,3,3> K=Matrix<float,3,3>::Identity();
   Matrix<float,16,16> O;
   conv2d(I,K,O);
   conv2d(I,K);

   MatrixXf a,b;
   separateKernel(K,a,b);
   cout << a << "\n\n" << b << endl;

#ifdef MATLAB
   //It = I;
   // Here follows the equivalent matlab commands
   // MATLAB [X,Y]=meshgrid(-3:0.1:3,-3:0.1:3);
   Matrix<float,Dynamic,Dynamic> X,Y;

   mathcommon::meshgrid<Matrix<float,Dynamic,Dynamic> , float > (-M_PI, M_PI, M_PI/20, -M_PI, M_PI, M_PI/20, X, Y );
   cerr << X.size() << " " << X.rows() << " " << X.cols() << endl;

   mathcommon::meshgrid<Matrix<float,Dynamic,Dynamic> , float > (-3.0, 3.0, 0.1, -3.0, 3.0, 0.1, X, Y );

   float omega=2;
   Matrix< complex<float>, Dynamic, Dynamic > R(X.rows(),X.cols());

   double omega=10;
   // K=exp(-j*omega*(X.^2+Y.^2) )
   Matrix<  complex<float> ,Dynamic, Dynamic > K(X.rows(), X.cols());
   K.real() = ( omega*(-X.cwiseAbs2() - Y.cwiseAbs2()) ).unaryExpr(cosz);
   K.imag() = ( omega*(-X.cwiseAbs2() - Y.cwiseAbs2()) ).unaryExpr(sinz);

   R.real_impl = (X.cwiseProduct(Y));
#endif

#ifdef PROVA2
   Matrix<float,7,7> K;
   K << 0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
     0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
     0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
     0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
     0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
     0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
     0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067 ;

   MatrixXf a,b;
   separateKernel(K,a,b);
   cout << a << "\n\n" << b << endl;
#endif
   /*
       MatrixXf Kreal,Kimag;
       Kreal=K.real();
       Kimag=K.imag();

       Matrix< complex<float>, 16,16 > R;
       Timer time;
       time.start();

       // R=arg(conv2(phantom(16),K,'same'))
       R.real() = conv2d(I, Kreal );
       R.imag() = conv2d(I, Kimag );
       cerr << time.getElapsedTimeInMilliSec() << endl;
       //cout << R.unaryExpr( complexphase ) << endl;

       cout << R.unaryExpr( complexphase ) << endl;
       Matrix<float,7,7> K;
       K << 0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
       0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
       0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,q
       0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
       0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
       0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
       0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067 ;
   */

   return 0;
}
