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

#include "OpticFlowEstimator.h"
#include "Convolution.h"

using std::cout;

OpticFlowEstimator::OpticFlowEstimator()
{

}

OpticFlowEstimator::~OpticFlowEstimator()
{  delete colorDirection;
}

void OpticFlowEstimator::init(int w, int h)
{  width=w;
   height=h;

   frame1.setZero(w,h);
   frame2.setZero(w,h);
   frameDx.setZero(w,h);
   frameDy.setZero(w,h);
   frameDt.setZero(w,h);
   velX.setZero(w,h);
   velY.setZero(w,h);

   Kx << -0.25,0.25,-0.25,0.25;
   Kx=-Kx;
   Ky = Kx.transpose();
   Kt = Matrix<float,2,2>::Ones()/4.0;
   KtOpposite = -Kt;

   double a=1.0/12.0;
   double b=1.0/6.0;

   Ksmooth << a,b,a,b,0.0,b,a,b,a;

   ones.setOnes(width,height);
}

void OpticFlowEstimator::getPreBuffer()
{  if ( frame1.size() != 0 )
      glReadPixels((GLint)0,(GLint)0,frame1.rows(), frame1.cols(),GL_GREEN, GL_FLOAT, frame1.data());

   frame1 = (conv2d(frame1,Ksmooth));
   frame1 /= frame1.maxCoeff();
}

void OpticFlowEstimator::getPostBuffer()
{  if ( frame2.size() != 0 )
      glReadPixels((GLint)0,(GLint)0,frame2.rows(), frame2.cols(),GL_GREEN, GL_FLOAT, frame2.data());

   frame2 = conv2d(frame2,Ksmooth);
   frame2 /= frame1.maxCoeff();
}

template <typename Derived, typename Derived2 >
Derived conv2d( DenseBase<Derived>& I, DenseBase<Derived2> &K )
{  typedef typename Derived::Scalar Scalar;
   typedef typename Derived2::Scalar Scalar2;

   Derived O(I.rows(),I.cols());
   Scalar *ptrIn = &I(0,0);
   Scalar *ptrOut= &O(0,0);
   Scalar2*ptrKer= &K(0,0);

   convolve2DFast( ptrIn , ptrOut ,I.cols(),I.rows(),ptrKer ,K.cols(), K.rows());
   return O;
}

template <typename Derived, typename Derived2 >
bool conv2d( DenseBase<Derived>& I, DenseBase<Derived2> &K, DenseBase<Derived> &O )
{  typedef typename Derived::Scalar Scalar;
   typedef typename Derived2::Scalar Scalar2;

   if (I.size()==0 || K.size()==0 || O.size()==0)
      return false;
   Scalar *ptrIn = &I(0,0);
   Scalar *ptrOut= &O(0,0);
   Scalar2*ptrKer= &K(0,0);

   convolve2DFast( ptrIn , ptrOut ,I.cols(),I.rows(),ptrKer ,K.cols(), K.rows());
   return true;
}

void OpticFlowEstimator::computeHS(double alpha)
{
   frameDx.setZero(width,height);
   frameDy.setZero(width,height);
   frameDt.setZero(width,height);
   velX.setZero(width,height);
   velY.setZero(width,height);

   uAvg.setZero(width,height);
   vAvg.setZero(width,height);

   alphaOnes=(alpha*alpha)*ones;

   // Compute derivatives
   frameDx = conv2d( frame2, Kx )  + conv2d( frame1, Kx );
   frameDy = conv2d( frame2, Ky )  + conv2d( frame1, Ky );
   frameDt = conv2d( frame2, Kt )   + conv2d( frame1 ,KtOpposite);


   frameDx=conv2d(frameDx,Ksmooth);
   frameDy=conv2d(frameDy,Ksmooth);
   frameDt=conv2d(frameDt,Ksmooth);

   int nIterations=5;
   divisor = (alphaOnes + frameDx.cwiseAbs2() + frameDy.cwiseAbs2());
   for ( int i=0; i<nIterations; i++)
   {  // Super smoothing way
      /*
      conv2d(velX,Ksmooth,uAvg); // otherwise call uAvg = conv2d(velX,Ksmooth) but this way you creates temporaries
      conv2d(velY,Ksmooth,vAvg); // otherwise call uAvg = conv2d(velX,Ksmooth) but this way you creates temporaries
      velX = uAvg - frameDx.cwiseProduct( (frameDx.cwiseProduct(uAvg) + frameDy.cwiseProduct(vAvg) + frameDt )).cwiseQuotient( divisor );
      velY = vAvg - frameDy.cwiseProduct( (frameDx.cwiseProduct(uAvg) + frameDy.cwiseProduct(vAvg) + frameDt )).cwiseQuotient( divisor );
      */
      velX -= frameDx.cwiseProduct( (frameDx.cwiseProduct(uAvg) + frameDy.cwiseProduct(vAvg) + frameDt )).cwiseQuotient( divisor );
      velY -= frameDy.cwiseProduct( (frameDx.cwiseProduct(uAvg) + frameDy.cwiseProduct(vAvg) + frameDt )).cwiseQuotient( divisor );
   }

   vModule = (velX.cwiseAbs2() + velY.cwiseAbs2()).cwiseSqrt();
   vModule/= (0.5*vModule.maxCoeff());
   vModule-=ones;

   vAngle.setZero(width,height);
   for (int i=0; i<vAngle.size(); i++)
      vAngle.coeffRef(i) = atan(velY(i)/velX(i));
   vAngle/= (0.5*vAngle.maxCoeff());
   vAngle-=ones;

}

void OpticFlowEstimator::printFlow( std::ostream &os1 )
{
    os1 << vModule << endl;
}

void OpticFlowEstimator::draw()
{  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   //cout << vModule << endl;
   glDrawPixels(width,height,GL_LUMINANCE,GL_FLOAT,vModule.data());
}

void OpticFlowEstimator::computeEigenSolver()
{

}
