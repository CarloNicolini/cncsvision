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
#include <vector>
#include "alp.h"
#include "ALPProjector.h"
#include "Timer.h"

#define N_TOTAL_SEQUENCES 10
using namespace std;

void fillSquare(vector<unsigned char> &buf, int width, int height, int centerX, int centerY, int nPixelsX, int nPixelsY)
{  for (int i=0; i<width; i++)
   {  for (int j=0; j<height; j++)
      {  if ( i >= centerX-nPixelsX && i <= centerX+nPixelsX && j >= centerY-nPixelsY && j <= centerY+nPixelsY &&
               centerX+nPixelsX <= width && centerY+nPixelsY<=height &&
               centerX-nPixelsX >= 0 && centerY-nPixelsY>=0 )
            buf.at(i+j*width)=255;
         else
            buf.at(i+j*width)=0;
      }
   }
}

void fillPixel(vector<unsigned char> &buf,  int width, int height, int x, int y)
{  buf.at(x+y*width)=255;
}

int main( int argc, char *argv[] )
{  unsigned long int nPictures=1;
   unsigned long int width = 1024;
   unsigned long int height = 768;
   unsigned long int length = width*height*nPictures;

   /*
   cerr << "Creating sequences" << endl;
   std::vector< std::vector<unsigned char> > allSequences(N_TOTAL_SEQUENCES);

   for ( int i=0; i<N_TOTAL_SEQUENCES; i++ )
   {
   	allSequences.at(i) = std::vector<unsigned char>(length,0);
   	// It fills a moving square
   	//fillPixel(allSequences.at(i),width,height,i*50,height/2);
   	fillSquare(allSequences.at(i),width,height,(i*50)%width,height/2,50,100);
   }

   ALPProjector alp;
   try
   {
   	alp.init();
   	alp.initLED();
   	alp.setLED(5000,100);
   	alp.inquire();
   	alp.setPicturesTimeus(1E6); //timing in hertz
   	for (int i=0; i<N_TOTAL_SEQUENCES;i++)
   		alp.loadSequence(nPictures,allSequences.back().data());
   	alp.inquire();
   	cerr << "Press enter to continue" << endl;

   	alp.setSequenceQueueMode();
   	Timer timer; timer.start();
   	for ( int i=0; i<10; i++ )
   		alp.loopAllSequences();
   	cerr << timer.getElapsedTimeInSec() << endl;
   	cerr << "Press enter to continue" << endl;
   	cin.ignore(1E6,'\n');
   }
   catch (const std::exception &e)
   {
   	cerr << e.what() << endl << "Press Enter to exit..." ;
   	cin.ignore(1E6,'\n');
   	alp.cleanup();
   }
   */
   std::vector<unsigned char> data;
   data.resize(length);
   fillSquare(data, width, height, width/2, height/2, 10,10);
   fillPixel(data,  width, height, width/2, height/2);
   //for (int i=0; i<length; i++)
   //data[i] =	data[i]==255 ? 0 : 255;
   ALPProjector alp;
   try
   {  alp.init();
      alp.initLED();
      alp.setLED(15000,100);
      alp.inquire();
      alp.setPicturesTimeus(80); //timing in hertz
      alp.loadSequence(1,data.data());
      alp.start();
      cin.ignore(1E6,'\n');
      alp.stop();
   }
   catch (const std::exception &e)
   {  cerr << e.what() << endl << "Press Enter to exit..." ;
      cin.ignore(1E6,'\n');
      alp.cleanup();
   }
   return 0;
}
