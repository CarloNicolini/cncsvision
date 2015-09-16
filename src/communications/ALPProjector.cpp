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

#include <stdexcept>
#include <algorithm>
#include "ALPProjector.h"

using namespace std;

/**
 * @brief ALPProjector::ALPProjector
 */
ALPProjector::ALPProjector() :
   m_bAlpInit(false),
   m_bAlpLEDInit(false),
   m_AlpLED_id(0),
   m_AlpSeqDisp(-1),
   m_Timing_PictureTime(1000),
   m_pData(NULL),
   m_bDisp(false)
{  m_BitNum = 1;
   m_PicNum = 16;
   sequenceID=0;
}

/**
 * @brief ALPProjector::inquire
 */
void ALPProjector::inquire()
{  if (!m_bAlpInit)	// check for initialization
   {  throw std::runtime_error("[ALP ERROR] "+std::string(ALP_ERROR_ALP_NOT_INIT) );
   }

   // get device information and simply show it
   long int	Info;
   std::string	Text;

   AlpDevInquire( this->m_AlpId, ALP_VERSION, &Info);
   cerr << "[ALP LOG] ALP Device Version: " << Info << endl;

   AlpDevInquire( this->m_AlpId, ALP_DEVICE_NUMBER, &Info);
   cerr << "[ALP LOG] ALP Serial Number: " << Info << endl;

   AlpDevInquire( this->m_AlpId, ALP_AVAIL_MEMORY, &Info);
   cerr << "[ALP LOG] ALP Memory: "<<  Info << " of " << this->m_FrameMemory << " frames free"<< endl;

   if (this->m_bDisp)
   {  cerr << "[ALP LOG] ALP Active sequence: " << this->m_AlpSeqDisp + 1 << endl;

      AlpSeqInquire( this->m_AlpId, this->m_AlpSeqId.at(this->m_AlpSeqDisp), ALP_PICTURE_TIME, &Info);
      cerr << "[ALP LOG] ALP Picture time: " << Info << "[us]" << endl;

      AlpSeqInquire( this->m_AlpId, this->m_AlpSeqId.at(this->m_AlpSeqDisp), ALP_MIN_PICTURE_TIME, &Info);
      cerr << "[ALP LOG] ALP Minimum picture time: " << Info << " [us]" << endl;
   }
}

/**
 * getMinimumPictureTime
**/
long int ALPProjector::getMinimumPictureTime() const
{  if (!m_bAlpInit)	// check for initialization
   {  throw std::runtime_error("[ALP ERROR] "+std::string(ALP_ERROR_ALP_NOT_INIT) );
   }
   long int minimumPictureTime;
   AlpSeqInquire( this->m_AlpId, this->m_AlpSeqDisp, ALP_MIN_PICTURE_TIME, &minimumPictureTime);

   return minimumPictureTime;
}

/**
 * @brief ALPProjector::init
 */
void ALPProjector::init()
{  cerr << "[ALP LOG] Initializing ALP..." << endl;
   this->m_AlpSeqId.clear();
   if (m_bAlpInit)	// avoid re-initialization
   {  throw std::runtime_error("[ALP ERROR] ALP already initialized \""+std::string(ALP_ERROR_ALP_REINIT) );
   }

   // Allocate device
   long alpDevAllocResult = AlpDevAlloc(ALP_DEFAULT, ALP_DEFAULT, &m_AlpId);
   long alpDevEnquireResult = AlpDevInquire(m_AlpId, ALP_DEV_DMDTYPE, &m_nDmdType);
   if ( alpDevAllocResult== ALP_OK && alpDevEnquireResult== ALP_OK)
   {  switch (m_nDmdType)
      {  case ALP_DMDTYPE_XGA_055A :
         case ALP_DMDTYPE_XGA_055X :
         case ALP_DMDTYPE_XGA_07A :
         {  m_nSizeX = 1024;
            m_nSizeY = 768;
            m_bAlpInit = true;
            cerr << "[ALP LOG] DMD Type ALP_DMDTYPE_XGA_07A || ALP_DMDTYPE_XGA_055X || ALP_DMDTYPE_XGA_055A" << endl;
            break;
         }
         case ALP_DMDTYPE_DISCONNECT :	// API emulates 1080p
         case ALP_DMDTYPE_1080P_095A :
         {  m_nSizeX = 1920;
            m_nSizeY = 1080;
            m_bAlpInit = true;
            cerr << "[ALP LOG] DMD Type ALP_DMDTYPE_1080P_095A" << endl;
            break;
         }
         case ALP_DMDTYPE_WUXGA_096A :
         {  m_nSizeX = 1920;
            m_nSizeY = 1200;
            m_bAlpInit = true;
            cerr << "[ALP LOG] DMD Type ALP_DMDTYPE_WUXGA_096A" << endl;
            break;
         }
         default :
            throw std::runtime_error("[ALP ERROR] Unsupported DMD format\""+std::string(ALP_ERROR_DMD_TYPE) +"\"");
      }

      // inquire device memory
      if (m_bAlpInit)
      {  AlpDevInquire(m_AlpId, ALP_AVAIL_MEMORY, &m_FrameMemory);
      }
      cerr << "[ALP LOG] ALP ID=" << m_AlpId << endl;
      cerr << "[ALP LOG] ALP Resolution " << m_nSizeX << "x" << m_nSizeY << endl;
      cerr << "[ALP LOG] ALP initialization status=" << m_bAlpInit << endl;
      cerr << "[ALP LOG] ALP maximum number of frames= " << (unsigned long int)m_FrameMemory << endl;
      cerr << "[ALP LOG] ALP maximum memory allocable= " << (unsigned long int)m_nSizeX*(unsigned long int)m_nSizeY*(unsigned long int)m_FrameMemory << " bytes= " << (unsigned long int)m_nSizeX*(unsigned long int)m_nSizeY*(unsigned long int)m_FrameMemory/1E6 << " [ MB]" <<endl;
   }
   else
   {  switch (alpDevAllocResult)
      {  case ALP_ADDR_INVALID:
            throw std::runtime_error("[ALP ERROR] AlpDevAlloc ALP Address is invalid");
            break;
         case ALP_NOT_ONLINE:
            throw std::runtime_error("[ALP ERROR] AlpDevAlloc ALP device is not online");
            break;
         case ALP_NOT_READY:
            throw std::runtime_error("[ALP ERROR] AlpDevAlloc Specified ALP already allocated");
            break;
         case ALP_ERROR_INIT:
            throw std::runtime_error("[ALP ERROR] AlpDevAlloc initialization error");
            break;
         case ALP_LOADER_VERSION:
            throw std::runtime_error("[ALP ERROR] AlpDevAlloc (ALP-4.1 only) This DLL requires the driver file VlxUsbLd.sys of at least version 0.1.0.22. Please update it and restart the ALP device.");
            break;
      }

      switch (alpDevEnquireResult)
      {  case ALP_NOT_AVAILABLE:
            throw std::runtime_error("[ALP ERROR] AlpDevInquire The specified ALP identifier is not valid");
         case ALP_ADDR_INVALID:
            throw std::runtime_error("[ALP ERROR] AlpDevInquire ALP user data access not valid");
            break;
         case ALP_PARM_INVALID:
            throw std::runtime_error("[ALP ERROR] AlpDevInquire parameters non correct");
            break;
         case ALP_DEVICE_REMOVED:
            throw std::runtime_error("[ALP ERROR] AlpDevInquire ALP device has been disconnected");
            break;
      }
   }

}

/**
 * @brief setSequenceQueueMode
**/
void ALPProjector::setSequenceQueueMode()
{  long int projControlQueueModeReturn = AlpProjControl(m_AlpId, ALP_PROJ_QUEUE_MODE, ALP_PROJ_SEQUENCE_QUEUE);
   long int projInquireSequenceMode;
   long int projInquireSequenceModeReturnValue= AlpProjInquire(m_AlpId,ALP_PROJ_QUEUE_MODE,&projInquireSequenceMode);

   if ( projInquireSequenceMode==ALP_PROJ_SEQUENCE_QUEUE )
   {  cerr << "[ALP LOG] ALP sequence mode is ALP_PROJ_SEQUENCE_QUEUE" << endl;
   }
   if ( projInquireSequenceMode==ALP_PROJ_LEGACY )
   {  cerr << "[ALP LOG] ALP sequence mode is ALP_PROJ_LEGACY" << endl;
   }

   switch (projControlQueueModeReturn)
   {  case ALP_NOT_AVAILABLE:
      {  throw std::runtime_error("[ALP ERROR]");
         break;
      }
      case ALP_NOT_READY:
      {  throw std::runtime_error("[ALP ERROR]");
         break;
      }
      case ALP_PARM_INVALID:
      {  throw std::runtime_error("[ALP ERROR]");
         break;
      }
      case ALP_NOT_IDLE:
      {  throw std::runtime_error("[ALP ERROR]");
         break;
      }
   }
}

/**
 * @brief ALPProjector::setPicturesTimeus
 * @param pictureTime in microseconds
 */
void ALPProjector::setPicturesTimeus(long int pictureTime)
{  this->m_Timing_PictureTime = pictureTime;
   cerr << "[ALP LOG] Timing to " << m_Timing_PictureTime << " [us]" << endl;
   cerr << "[ALP LOG] Pictures are all displayed in " << m_Timing_PictureTime*m_PicNum << " [us]" << endl;
}


/**
 * @brief ALPProjector::initLED
 * @param value
 */
void ALPProjector::initLED()
{  if (m_bAlpLEDInit)
   {  AlpLedFree(m_AlpId, m_AlpLED_id);
      m_bAlpLEDInit=false;
   }
   cerr << "[ALP LOG] Allocating LEDs " << endl;
   long alpLEDAllocResult = AlpLedAlloc( this->m_AlpId, ALP_HLD_CBT140_WHITE, NULL, &(this->m_AlpLED_id) );
   switch (alpLEDAllocResult)
   {  case ALP_PARM_INVALID:
         throw std::runtime_error("[ALP ERROR] AlpLedAlloc invalid LedType");
         break;
      case ALP_ERROR_INIT:
         throw std::runtime_error("[ALP ERROR] AlpLedAlloc invalid AllocParams, or error initializing the LED driver");
         break;
      case ALP_NOT_ONLINE:
         throw std::runtime_error("[ALP ERROR] AlpLedAlloc none of the known addresses works");
         break;
      case ALP_NOT_READY:
         throw std::runtime_error("[ALP ERROR] AlpLedAlloc one of the requested I2C addresses has already been allocated");
         break;
   }

   m_bAlpLEDInit=true;
}

/**
* @brief setLED
**/
void ALPProjector::setLED(long int ledCurrentMilliAmperes, long int ledBrightnessPercent)
{  // Change LED current using AlpLedControl
   // here we don't want to exceed led brightness, so we put a maximum 10000 milliamperes
   long alpLEDcontrolResult = AlpLedControl( this->m_AlpId, this->m_AlpLED_id, ALP_LED_SET_CURRENT, ledCurrentMilliAmperes%18000);
   switch (alpLEDcontrolResult)
   {  case ALP_NOT_AVAILABLE:
      {  throw std::runtime_error("[ALP ERROR] AlpLedControl invalid DeviceId");
         break;
      }
      case ALP_PARM_INVALID:
      {  throw std::runtime_error("[ALP ERROR] AlpLedControl Value out of range (current*brightness/100% exceeds the capabilities of the LED type), invalid ControlType, or invalid LedId");
         break;
      }
      case ALP_ERROR_COMM:
      {  throw std::runtime_error("[ALP ERROR] AlpLedControl USB communication error or I2C bus error");
         break;
      }
   }

   // Change LED brightness using AlpLedControl
   // here we don't want to exceed led brightness, so we put a maximum 100%
   long int effectiveBrightness=ledBrightnessPercent > 100 ? 100 : ledBrightnessPercent;
   alpLEDcontrolResult = AlpLedControl( this->m_AlpId, this->m_AlpLED_id, ALP_LED_BRIGHTNESS, effectiveBrightness);
   switch (alpLEDcontrolResult)
   {  case ALP_NOT_AVAILABLE:
         throw std::runtime_error("[ALP ERROR] AlpLedControl invalid DeviceId");
         break;
      case ALP_PARM_INVALID:
         throw std::runtime_error("[ALP ERROR] AlpLedControl Value out of range (current*brightness/100% exceeds the capabilities of the LED type), invalid ControlType, or invalid LedId");
         break;
      case ALP_ERROR_COMM:
         throw std::runtime_error("[ALP ERROR] AlpLedControl USB communication error or I2C bus error");
         break;
   }
}

/**
 * @brief ALPProjector::loadSequence
 * @param frequency
 * @param imageSequence
 */
long int ALPProjector::loadSequence(unsigned int nPictures, unsigned char *imageSequence)
{  this->m_AlpSeqId.push_back(sequenceID++);
   this->m_PicNum = nPictures;
   if (!m_bAlpInit)	// check for initialization
   {  throw std::runtime_error("[ALP ERROR] "+std::string(ALP_ERROR_ALP_NOT_INIT) );
   }

   // allocate ALP sequence memory
   if ( ( m_AlpSeqId.size() == SEQU_MAX) || (AlpSeqAlloc(m_AlpId, m_BitNum, m_PicNum, &(m_AlpSeqId.back()) ) != ALP_OK) )
   {  throw std::runtime_error("[ALP ERROR] "+std::string(ALP_ERROR_SEQ_ALLOC) );
   }

   // set sequence timing
   AlpSeqTiming(m_AlpId,  m_AlpSeqId.back(), m_Timing_IlluminateTime, m_Timing_PictureTime, m_Timing_SynchDelay, m_Timing_SynchPulseWidth, m_Timing_TriggerInDelay);

   // init sequence data - only once
   // if the pointer is NULL then we send a wheel rotating. Just for demo...
   if ( imageSequence==NULL )
   {  if (!m_pData)
      {  m_pData = new char unsigned [m_nSizeX*m_nSizeY*m_PicNum];
         this->initWheel(m_pData, m_PicNum);
      }
      else
      {  delete m_pData;
         m_pData=NULL;
         m_pData = new char unsigned [m_nSizeX*m_nSizeY*m_PicNum];
         this->initWheel(m_pData, m_PicNum);
      }
   }
   else
      m_pData = imageSequence;

   // load sequence data into ALP memory
   if (AlpSeqPut(m_AlpId,  m_AlpSeqId.back(), 0, m_PicNum, m_pData) != ALP_OK)
   {  throw std::runtime_error("[ALP ERROR] " + std::string("Data transfer of sequence ") +std::to_string( (_ULonglong)(m_AlpSeqId.size())) + std::string(" failed"));
   }
   m_AlpSeqDisp = m_AlpSeqId.back();
   cerr << "[ALP LOG] Sequence " << m_AlpSeqDisp << " successfully loaded" << endl;
   return m_AlpSeqDisp;
}

/**
* @brief removeSequence
* @param sequenceID
**/
void ALPProjector::removeSequence(long int sequenceID)
{  if ( m_AlpSeqId.empty() )
      throw std::runtime_error("[ALP ERROR] No sequences allocated");

   std::vector<ALP_ID>::iterator iter = std::find(m_AlpSeqId.begin(),m_AlpSeqId.end(),sequenceID);
   if ( iter == m_AlpSeqId.end() )
   {  throw std::runtime_error("[ALP ERROR] Removing sequence, no suitable sequence ID found in current sequence list");
   }

   long int alpSeqFreeResult = AlpSeqFree(this->m_AlpId, sequenceID);
   switch (alpSeqFreeResult)
   {  case ALP_NOT_AVAILABLE:
         throw std::runtime_error("[ALP ERROR] Removing sequence, the specified ALP identifier is not valid");
         break;
      case ALP_NOT_READY:
         throw std::runtime_error("[ALP ERROR] Removing sequence, the specified ALP is in use by another function");
         break;
      case ALP_NOT_IDLE:
         throw std::runtime_error("[ALP ERROR] Removing sequence, the ALP is not in the idle wait state");
         break;
      case ALP_SEQ_IN_USE:
         throw std::runtime_error("[ALP ERROR] Removing sequence, the sequence specified is currently in use");
         break;
      case ALP_PARM_INVALID:
         throw std::runtime_error("[ALP ERROR] Removing sequence, one of the parameters is invalid");
         break;
   }
   m_AlpSeqId.erase(iter);
   if (!m_AlpSeqId.empty())
      m_AlpSeqDisp=m_AlpSeqId.back();
   else
      m_AlpSeqDisp=-1;
}

/**
* @brief cleanAllSequences Free all the memory occupated from the sequences
**/
void ALPProjector::cleanAllSequences()
{  for (unsigned int i=0; i<m_AlpSeqId.size(); i++)
   {  long int alpSeqFreeResult = AlpSeqFree(this->m_AlpId, m_AlpSeqId.at(i));
      switch (alpSeqFreeResult)
      {  case ALP_NOT_AVAILABLE:
            throw std::runtime_error("[ALP ERROR] Removing sequence, the specified ALP identifier is not valid");
            break;
         case ALP_NOT_READY:
            throw std::runtime_error("[ALP ERROR] Removing sequence, the specified ALP is in use by another function");
            break;
         case ALP_NOT_IDLE:
            throw std::runtime_error("[ALP ERROR] Removing sequence, the ALP is not in the idle wait state");
            break;
         case ALP_SEQ_IN_USE:
            throw std::runtime_error("[ALP ERROR] Removing sequence, the sequence specified is currently in use");
            break;
         case ALP_PARM_INVALID:
            throw std::runtime_error("[ALP ERROR] Removing sequence, one of the parameters is invalid");
            break;
      }
   }
   m_AlpSeqDisp=-1;
   m_AlpSeqId.clear();
}

/**
 * @brief ALPProjector::changeSequence
**/
void ALPProjector::changeSequence(long int sequenceID)
{  if ( m_AlpSeqId.empty() )
      throw std::runtime_error("[ALP ERROR] No sequences allocated");

   if ( std::find(m_AlpSeqId.begin(),m_AlpSeqId.end(),sequenceID ) == m_AlpSeqId.end() )
   {  throw std::runtime_error("[ALP ERROR] No suitable sequence ID found in current sequence list");
   }
   this->m_AlpSeqDisp=sequenceID;//this->m_AlpSeqId.at(sequenceID);
}

/**
 * @brief ALPProjector::start
 */
void ALPProjector::start()
{  if (!m_bAlpInit)	// check for initialization
   {  throw std::runtime_error("[ALP ERROR] "+ ALP_ERROR_ALP_NOT_INIT);
   }

   if (m_AlpSeqDisp==-1)
   {  throw std::runtime_error("[ALP ERROR] Invalid sequence to display, call ALPProjector::changeSequence first");
   }
   cerr << "[ALP LOG] Starting projection on sequence " <<  m_AlpSeqDisp << endl;
   AlpProjStartCont(m_AlpId, m_AlpSeqDisp);
   m_bDisp = true;
}

/**
 * @brief loopAllSequences
**/
void ALPProjector::loopAllSequences()
{  unsigned int nSequences = m_AlpSeqId.size();
   for (unsigned int i=0; i<nSequences; ++i)
   {  //AlpSeqControl(this->m_AlpId,m_AlpSeqId.at(i),ALP_SEQ_REPEAT,1);
      AlpProjStart(this->m_AlpId,this->m_AlpSeqId.at(i));
   }
}

/**
 * @brief ALPProjector::stop
 */
void ALPProjector::stop()
{  if (!m_bAlpInit)	// check for initialization
   {  throw std::runtime_error("[ALP ERROR] "+std::string(ALP_ERROR_ALP_NOT_INIT));
   }

   AlpProjHalt(m_AlpId);
   m_bDisp = false;
}

/**
 * @brief ALPProjector::dispose
 */
void ALPProjector::cleanup()
{  if (m_AlpLED_id!=0)
   {  AlpLedFree(m_AlpId, m_AlpLED_id);
      m_bAlpLEDInit=false;
   }

   if (AlpDevFree(m_AlpId) == ALP_OK)
   {  m_bAlpInit = false;
      m_AlpSeqDisp = -1;
      m_AlpSeqId.clear();
   }
}

/**
 * @brief ALPProjector::~ALPProjector
 */
ALPProjector::~ALPProjector()
{  if (m_bDisp) // the projector is still active
      this->stop();
   // Turns off ALP
   this->cleanup();
}

/**
 * @brief ALPProjector::initWheel
 * @param buf
 * @param num
 */

void ALPProjector::initWheel(unsigned char *buf, int num)
{  long const n = 3;						// half number of spokes >= 2 !
   long const d = min(m_nSizeY, m_nSizeX);	// wheel diameter
   double const pi = 3.14159;

   double	a	=	10.0;		// half width of the spokes
   double	b	=	5.0;			// tyre width
   double	c	=	0.5;			// hub radius (hub ratio)

   double alpha	=	pi/n;				// spoke angle
   double delta	=	alpha/num;			// frame angle
   double ra		=	a/asin(alpha/2);	// hub outer radius
   double ri		=	ra*c;				// hub inner radius
   double ru		=	d/2-b;				// rim radius
   double z		=	pi*ru/(2*n*num);

   if (a > z) z = a;

   int i, j, k, x, y;
   double r;
   double gamma_s[n];
   double gamma_c[n];

   // check buffer pointer
   //ASSERT(!IsBadWritePtr(buf,m_nSizeX*m_nSizeY*num));

   memset(buf, 0, m_nSizeX*m_nSizeY*num);
   for (i=0; i<num; i++)
   {  for (k=0; k<n; k++)
      {  gamma_s[k] = sin(pi/2 - k*alpha + i*delta);
         gamma_c[k] = cos(pi/2 - k*alpha + i*delta);
      }

      for (j=0; j<m_nSizeY; j++)
      {  if (j < m_nSizeY/2)
            y = m_nSizeY/2 - j;
         else
            y = m_nSizeY/2 - 1 - j;

         int i1 = m_nSizeX*m_nSizeY*i + m_nSizeX*j + m_nSizeX/2;
         int i2 = m_nSizeX*m_nSizeY*(i+1) - m_nSizeX*j - m_nSizeX/2 - 1;

         for (x=0; x<m_nSizeX/2; x++)
         {  // radius test
            r = sqrt((double)(x*x + y*y));
            if (r > ri && r < d/2)
            {  // hub or tyre
               if (r < ra || r > ru)
               {  buf[i1 + x] = 255;		// 1. & 4. quadrant
                  buf[i2 - x] = 255;		// 3. & 2. quadrant
               }
               else
                  // spoke test
                  for (k=0; k<n; k++)
                  {  double t = x*gamma_s[k] - y*gamma_c[k];
                     if (t >= 0)
                     {  if (t <= z)
                        {  buf[i1 + x] = 255;		// 1. & 4. quadrant
                           buf[i2 - x] = 255;		// 3. & 2. quadrant
                           break;					// no further tests
                        }
                     }
                     else if (t > -z)
                     {  buf[i1 + x] = 255;		// 1. & 4. quadrant
                        buf[i2 - x] = 255;		// 3. & 2. quadrant
                        break;					// no further tests
                     }
                  }
            }
         }
      }
   }
}
