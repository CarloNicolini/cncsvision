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
#include <complex>
#include <Eigen/Core>
#include <Eigen/LU>

using namespace Eigen;
using namespace std;

template <typename Scalar>
bool convolve2DFast(Scalar* in, Scalar* out, int dataSizeX, int dataSizeY,
                    Scalar* kernel, int kernelSizeX, int kernelSizeY)
{  int i, j, m, n, x, y, t;
   Scalar **inPtr, *outPtr, *ptr;
   int kCenterX, kCenterY;
   int rowEnd, colEnd;                             // ending indice for section divider
   Scalar sum;                                      // temp accumulation buffer
   int k, kSize;

   // check validity of params
   if(!in || !out || !kernel) return false;
   if(dataSizeX <= 0 || kernelSizeX <= 0) return false;

   // find center position of kernel (half of kernel size)
   kCenterX = kernelSizeX >> 1;
   kCenterY = kernelSizeY >> 1;
   kSize = kernelSizeX * kernelSizeY;              // total kernel size

   // allocate memeory for multi-cursor
   inPtr = new Scalar*[kSize];
   if(!inPtr) return false;                        // allocation error

   // set initial position of multi-cursor, NOTE: it is swapped instead of kernel
   ptr = in + (dataSizeX * kCenterY + kCenterX); // the first cursor is shifted (kCenterX, kCenterY)
   for(m=0, t=0; m < kernelSizeY; ++m)
   {  for(n=0; n < kernelSizeX; ++n, ++t)
      {  inPtr[t] = ptr - n;
      }
      ptr -= dataSizeX;
   }

   // init working  pointers
   outPtr = out;

   rowEnd = dataSizeY - kCenterY;                  // bottom row partition divider
   colEnd = dataSizeX - kCenterX;                  // right column partition divider

   // convolve rows from index=0 to index=kCenterY-1
   y = kCenterY;
   for(i=0; i < kCenterY; ++i)
   {  // partition #1 ***********************************
      x = kCenterX;
      for(j=0; j < kCenterX; ++j)                 // column from index=0 to index=kCenterX-1
      {  sum = 0;
         t = 0;
         for(m=0; m <= y; ++m)
         {  for(n=0; n <= x; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += (kernelSizeX - x - 1);         // jump to next row
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      // partition #2 ***********************************
      for(j=kCenterX; j < colEnd; ++j)            // column from index=kCenterX to index=(dataSizeX-kCenterX-1)
      {  sum = 0;
         t = 0;
         for(m=0; m <= y; ++m)
         {  for(n=0; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      // partition #3 ***********************************
      x = 1;
      for(j=colEnd; j < dataSizeX; ++j)           // column from index=(dataSizeX-kCenter) to index=(dataSizeX-1)
      {  sum = 0;
         t = x;
         for(m=0; m <= y; ++m)
         {  for(n=x; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += x;                             // jump to next row
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      ++y;                                        // add one more row to convolve for next run
   }

   // convolve rows from index=kCenterY to index=(dataSizeY-kCenterY-1)
   for(i= kCenterY; i < rowEnd; ++i)               // number of rows
   {  // partition #4 ***********************************
      x = kCenterX;
      for(j=0; j < kCenterX; ++j)                 // column from index=0 to index=kCenterX-1
      {  sum = 0;
         t = 0;
         for(m=0; m < kernelSizeY; ++m)
         {  for(n=0; n <= x; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += (kernelSizeX - x - 1);
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      // partition #5 ***********************************
      for(j = kCenterX; j < colEnd; ++j)          // column from index=kCenterX to index=(dataSizeX-kCenterX-1)
      {  sum = 0;
         t = 0;
         for(m=0; m < kernelSizeY; ++m)
         {  for(n=0; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++inPtr[t]; // in this partition, all cursors are used to convolve. moving cursors to next is safe here
               ++t;
            }
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
      }

      // partition #6 ***********************************
      x = 1;
      for(j=colEnd; j < dataSizeX; ++j)           // column from index=(dataSizeX-kCenter) to index=(dataSizeX-1)
      {  sum = 0;
         t = x;
         for(m=0; m < kernelSizeY; ++m)
         {  for(n=x; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += x;
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }
   }

   // convolve rows from index=(dataSizeY-kCenterY) to index=(dataSizeY-1)
   y = 1;
   for(i= rowEnd; i < dataSizeY; ++i)               // number of rows
   {  // partition #7 ***********************************
      x = kCenterX;
      for(j=0; j < kCenterX; ++j)                 // column from index=0 to index=kCenterX-1
      {  sum = 0;
         t = kernelSizeX * y;

         for(m=y; m < kernelSizeY; ++m)
         {  for(n=0; n <= x; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += (kernelSizeX - x - 1);
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      // partition #8 ***********************************
      for(j=kCenterX; j < colEnd; ++j)            // column from index=kCenterX to index=(dataSizeX-kCenterX-1)
      {  sum = 0;
         t = kernelSizeX * y;
         for(m=y; m < kernelSizeY; ++m)
         {  for(n=0; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];
      }

      // partition #9 ***********************************
      x = 1;
      for(j=colEnd; j < dataSizeX; ++j)           // column from index=(dataSizeX-kCenter) to index=(dataSizeX-1)
      {  sum = 0;
         t = kernelSizeX * y + x;
         for(m=y; m < kernelSizeY; ++m)
         {  for(n=x; n < kernelSizeX; ++n)
            {  sum += *inPtr[t] * kernel[t];
               ++t;
            }
            t += x;
         }

         // store output
         *outPtr = (Scalar)((Scalar)(sum) );
         ++outPtr;
         ++x;
         for(k=0; k < kSize; ++k) ++inPtr[k];    // move all cursors to next
      }

      ++y;                                        // the starting row index is increased
   }

   return true;
}

template <typename Scalar>
bool convolve2DSeparable(Scalar* in, Scalar* out, int dataSizeX, int dataSizeY,
                         Scalar* kernelX, int kSizeX, Scalar* kernelY, int kSizeY)
{  int i, j, k, m, n;
   Scalar *tmp, *sum;                               // intermediate data buffer
   Scalar *inPtr, *outPtr;                          // working pointers
   Scalar *tmpPtr, *tmpPtr2;                        // working pointers
   int kCenter, kOffset, endIndex;                 // kernel indice

   // check validity of params
   if(!in || !out || !kernelX || !kernelY) return false;
   if(dataSizeX <= 0 || kSizeX <= 0) return false;

   // allocate temp storage to keep intermediate result
   tmp = new Scalar[dataSizeX * dataSizeY];
   if(!tmp) return false;  // memory allocation error

   // store accumulated sum
   sum = new Scalar[dataSizeX];
   if(!sum) return false;  // memory allocation error

   // covolve horizontal direction ///////////////////////

   // find center position of kernel (half of kernel size)
   kCenter = kSizeX >> 1;                          // center index of kernel array
   endIndex = dataSizeX - kCenter;                 // index for full kernel convolution

   // init working pointers
   inPtr = in;
   tmpPtr = tmp;                                   // store intermediate results from 1D horizontal convolution

   // start horizontal convolution (x-direction)
   for(i=0; i < dataSizeY; ++i)                    // number of rows
   {

      kOffset = 0;                                // starting index of partial kernel varies for each sample

      // COLUMN FROM index=0 TO index=kCenter-1
      for(j=0; j < kCenter; ++j)
      {  *tmpPtr = 0;                            // init to 0 before accumulation

         for(k = kCenter + kOffset, m = 0; k >= 0; --k, ++m) // convolve with partial of kernel
         {  *tmpPtr += *(inPtr + m) * kernelX[k];
         }
         ++tmpPtr;                               // next output
         ++kOffset;                              // increase starting index of kernel
      }

      // COLUMN FROM index=kCenter TO index=(dataSizeX-kCenter-1)
      for(j = kCenter; j < endIndex; ++j)
      {  *tmpPtr = 0;                            // init to 0 before accumulate

         for(k = kSizeX-1, m = 0; k >= 0; --k, ++m)  // full kernel
         {  *tmpPtr += *(inPtr + m) * kernelX[k];
         }
         ++inPtr;                                // next input
         ++tmpPtr;                               // next output
      }

      kOffset = 1;                                // ending index of partial kernel varies for each sample

      // COLUMN FROM index=(dataSizeX-kCenter) TO index=(dataSizeX-1)
      for(j = endIndex; j < dataSizeX; ++j)
      {  *tmpPtr = 0;                            // init to 0 before accumulation

         for(k = kSizeX-1, m=0; k >= kOffset; --k, ++m)   // convolve with partial of kernel
         {  *tmpPtr += *(inPtr + m) * kernelX[k];
         }
         ++inPtr;                                // next input
         ++tmpPtr;                               // next output
         ++kOffset;                              // increase ending index of partial kernel
      }

      inPtr += kCenter;                           // next row
   }
   // END OF HORIZONTAL CONVOLUTION //////////////////////

   // start vertical direction ///////////////////////////

   // find center position of kernel (half of kernel size)
   kCenter = kSizeY >> 1;                          // center index of vertical kernel
   endIndex = dataSizeY - kCenter;                 // index where full kernel convolution should stop

   // set working pointers
   tmpPtr = tmpPtr2 = tmp;
   outPtr = out;

   // clear out array before accumulation
   for(i = 0; i < dataSizeX; ++i)
      sum[i] = 0;

   // start to convolve vertical direction (y-direction)

   // ROW FROM index=0 TO index=(kCenter-1)
   kOffset = 0;                                    // starting index of partial kernel varies for each sample
   for(i=0; i < kCenter; ++i)
   {  for(k = kCenter + kOffset; k >= 0; --k)     // convolve with partial kernel
      {  for(j=0; j < dataSizeX; ++j)
         {  sum[j] += *tmpPtr * kernelY[k];
            ++tmpPtr;
         }
      }

      for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
      {  *outPtr = sum[n];                       // store final result to output array
         sum[n] = 0;                             // reset to zero for next summing
         ++outPtr;                               // next element of output
      }

      tmpPtr = tmpPtr2;                           // reset input pointer
      ++kOffset;                                  // increase starting index of kernel
   }

   // ROW FROM index=kCenter TO index=(dataSizeY-kCenter-1)
   for(i = kCenter; i < endIndex; ++i)
   {  for(k = kSizeY -1; k >= 0; --k)             // convolve with full kernel
      {  for(j = 0; j < dataSizeX; ++j)
         {  sum[j] += *tmpPtr * kernelY[k];
            ++tmpPtr;
         }
      }

      for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
      {  *outPtr = sum[n];                       // store final result to output buffer
         sum[n] = 0;                             // reset before next summing
         ++outPtr;                               // next output
      }

      // move to next row
      tmpPtr2 += dataSizeX;
      tmpPtr = tmpPtr2;
   }

   // ROW FROM index=(dataSizeY-kCenter) TO index=(dataSizeY-1)
   kOffset = 1;                                    // ending index of partial kernel varies for each sample
   for(i=endIndex; i < dataSizeY; ++i)
   {  for(k = kSizeY-1; k >= kOffset; --k)        // convolve with partial kernel
      {  for(j=0; j < dataSizeX; ++j)
         {  sum[j] += *tmpPtr * kernelY[k];
            ++tmpPtr;
         }
      }

      for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
      {  *outPtr = sum[n];                       // store final result to output array
         sum[n] = 0;                             // reset to 0 for next sum
         ++outPtr;                               // next output
      }

      // move to next row
      tmpPtr2 += dataSizeX;
      tmpPtr = tmpPtr2;                           // next input
      ++kOffset;                                  // increase ending index of kernel
   }
   // END OF VERTICAL CONVOLUTION ////////////////////////

   // deallocate temp buffers
   delete [] tmp;
   delete [] sum;
   return true;
}

template <typename Derived, typename Derived2 >
Derived conv2d( MatrixBase<Derived>& I,   MatrixBase<Derived2> &kernel )
{  typedef typename Derived::Scalar Scalar;
   typedef typename Derived2::Scalar Scalar2;

   Derived output = Derived::Zero(I.rows(),I.cols());

   FullPivLU<Derived2> lu_decomp(kernel);

   if ( lu_decomp.rank() !=1 )
   {  //cerr << "Non Separable" << endl;
      convolve2DFast( &I(0,0), &output(0,0), I.cols(), I.rows(), &kernel(0,0), kernel.cols(), kernel.rows() );
   }
   else
   {  //cerr << "Separable" << endl;
      // Calcolare i due vettori il cui prodotto esterno forma il kernel con la SVD del kernel.
      JacobiSVD<Derived2> svd(kernel, ComputeFullU | ComputeFullV);
      Derived2 U = svd.matrixU();
      Derived2 V = svd.matrixV();

      Scalar2 s = sqrt(svd.singularValues()(0));
      Matrix< Scalar2, Dynamic, 1 > v = U.col(0)*s;
      Matrix< Scalar2, Dynamic, 1 > h = V.col(0)*s;
      convolve2DSeparable( &I(0,0), &output(0,0), I.cols(), I.rows(), &v(0,0), v.rows() , &h(0,0), h.rows() );
   }

   return output;
}

template <typename Derived, typename Derived2 >
void conv2d(  MatrixBase<Derived>& I,   MatrixBase<Derived2> &kernel, MatrixBase<Derived> &output )
{  typedef typename Derived::Scalar Scalar;
   typedef typename Derived2::Scalar Scalar2;

   if (output.size()==0)
      cerr << "Allocate output matrix before!" << endl;

   FullPivLU<Derived2> lu_decomp(kernel);

   if ( lu_decomp.rank() !=1 )
   {  convolve2DFast( &I(0,0), &output(0,0), I.cols(), I.rows(), &kernel(0,0), kernel.cols(), kernel.rows() );
   }
   else
   {  // Calcolare i due vettori il cui prodotto esterno forma il kernel con la SVD del kernel.
      JacobiSVD<Derived2> svd(kernel, ComputeFullU | ComputeFullV);
      Derived2 U = svd.matrixU();
      Derived2 V = svd.matrixV();

      Scalar2 s = sqrt(svd.singularValues()(0));
      Matrix< Scalar2, Dynamic, 1 > v = U.col(0)*s;
      Matrix< Scalar2, Dynamic, 1 > h = V.col(0)*s;
      convolve2DSeparable( &I(0,0), &output(0,0), I.cols(), I.rows(), &v(0,0), v.rows() , &h(0,0), h.rows() );
   }
}

template <typename Derived , typename RowDerived, typename ColumnDerived >
bool separateKernel(  MatrixBase<Derived>& kernel, MatrixBase<RowDerived> &rowVector, MatrixBase<ColumnDerived> &columnVector )
{  typedef typename Derived::Scalar Scalar;
   FullPivLU<Derived> lu_decomp(kernel);

   if ( lu_decomp.rank() ==1 )
   {  JacobiSVD<Derived> svd(kernel, ComputeFullU | ComputeFullV);
      Derived U = svd.matrixU();
      Derived V = svd.matrixV();

      Scalar s = sqrt(svd.singularValues()(0));
      rowVector      = (U.col(0)*s).transpose();
      columnVector   = V.col(0)*s;
      return true;
   }
   else
   {  cerr << "This kernel is not separable" << endl;
      return false;
   }
}

template <typename Derived, typename Derived2 >
void conv2dSeparable(  MatrixBase<Derived>& I,   MatrixBase<Derived2> &kernelX,  MatrixBase<Derived2> &kernelY, MatrixBase<Derived>&output )
{  typedef typename Derived::Scalar Scalar;
   typedef typename Derived2::Scalar Scalar2;


   if (output.size()==0)
      cerr << "Allocate output matrix before!" << endl;

   convolve2DSeparable( &I(0,0), &output(0,0), I.cols(), I.rows(), &kernelX(0,0), kernelX.rows() , &kernelY(0,0), kernelY.rows() );

}

