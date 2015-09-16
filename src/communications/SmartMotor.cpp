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

#include <string>
#include <exception>
#include <iostream>
#include "Util.h"
#include "SmartMotor.h"

using std::cerr;
using std::endl;
using std::string;

/**
 * @brief SmartMotor::SmartMotor
 */
SmartMotor::SmartMotor()
{
}

/**
 * @brief SmartMotor::~SmartMotor
 */
SmartMotor::~SmartMotor()
{  CommInterface->BaudRate = 9600;
   this->closePort();
}

/**
 * @brief SmartMotor::init
 */
void SmartMotor::init()
{  HRESULT hr = CoInitialize(NULL); // Important to initialize COM
   if(FAILED(hr))
   {  throw std::runtime_error("[Smart Motor] Failure to initialize COM interface");
   }

   cerr << "[Smart Motor] COM successfully initializated" << endl;

   if(!AfxOleInit())
   {  AfxMessageBox("OLE initialization failed.  Make sure that the OLE libraries are the correct version.");
      throw std::runtime_error("OLE initialization failed.  Make sure that the OLE libraries are the correct version.");
   }
   AfxEnableControlContainer();
   cerr << "[Smart Motor] AFX successfully initializated" << endl;
   try
   {  // Create SMIHost object and interfaces
      HRESULT hr = CommInterface.CreateInstance(__uuidof(SMIHost));
      if(FAILED(hr))
      {  AfxMessageBox("Cannot create an instance of \"SMIHost\" class!");
         return;
      }
      /*
        hr = CommInterface.QueryInterface(__uuidof(ISMICMotion),&CMotionInterface);
        if(FAILED(hr))
        {
            AfxMessageBox("The interface \"ISMICMotion\" not found!");
            return;
        }
        hr = CommInterface.QueryInterface(__uuidof(ISMIPath),&PathInterface);
        if(FAILED(hr))
        {
            AfxMessageBox("The interface \"ISMIPath\" not found!");
            return;
        }
      */
      cerr << "[Smart Motor] Interface successfully created" << endl;
      CString Str = "N/A";
      long Version = CommInterface->EngineVersion;
      if (Version)
      {  // Windows way to convert strings from long int...bleah!
         Str.Format("IntegMotorInterface Version: %d.%d%d%d",
                    HIBYTE(HIWORD(Version)),
                    LOBYTE(HIWORD(Version)),
                    HIBYTE(LOWORD(Version)),
                    LOBYTE(LOWORD(Version)));
         AfxMessageBox(Str);
      }
      // Set baud rate
      CommInterface->BaudRate = 38400;
   }
   catch (_com_error e)
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
}

/**
 * @brief SmartMotor::openPort
 */
void SmartMotor::openPort()
{  cerr << "[Smart Motor] Opening port COM4...";
   try
   {  CommInterface->OpenPort("Com4");
   }
   catch (_com_error e)
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
   cerr << "DONE!" << endl;
}

/**
 * @brief SmartMotor::closePort
 */
void SmartMotor::closePort()
{  cerr << "[Smart Motor] Closing port COM4...";
   try
   {  CommInterface->ClosePort();
   }
   catch (_com_error e)
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
   cerr << "DONE!" << endl;
}

/**
 * @brief SmartMotor::addressChain
 */
void SmartMotor::addressChain()
{  cerr << "[Smart Motor] Addressing motor chain...";
   long MaxAddress=10;
   long flags=0;
   try
   {  // Establish chain
      CommInterface->AddressMotorChain();
      CommInterface->DefaultMotor = 1;
   }
   catch (_com_error e )
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
   cerr << "DONE" << endl;
}

/**
 * @brief SmartMotor::detect232
 */
void SmartMotor::detect232()
{  cerr << "[Smart Motor] Detecting RS232...";
   try
   {  long MaxAddress=10;
      long flags=0;
      switch(CommInterface->DetectRS232(MaxAddress,flags) )
      {  case CER_SOMEADDRESSED:
         {  cerr << "Some Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
            break;
         }
         case CER_BIGADDRESS:
         {  cerr << "Some Motors have big addresses!" <<" IntegMotorInterface Error!" << endl;
            break;
         }
         case CER_DUPLICATEADDR:
         {  cerr << "Some Motors have duplicate addresses!"<<" IntegMotorInterface Error!"<< endl;
            break;
         }
         case CER_NOTADDRESSED:
         {  cerr << "Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
            break;
         }
         case CER_COMMERROR:
         {  cerr << "Communication Error!"<<" IntegMotorInterface Error!"<< endl;
            break;
         }
         case CER_NOMOTOR:
         {  cerr << "No Motor Found!"<<" IntegMotorInterface Error!" << endl;
            break;
         }
         default:
         {  cerr << CommInterface->NoOfMotors << endl;
            break;
         }
      }
   }
   catch(_com_error e)
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
   cerr << "DONE" << endl;
}

/**
 * @brief SmartMotor::rotateAngle
 * @param angle
 */
void SmartMotor::rotateAngle(double angle)
{  try
   {  int stepsAngle = int(angle/(360.0)*8000.0);
      std::string anglestring("PRT=");
      anglestring += util::stringify<int>(stepsAngle);

      CommInterface->ClearBuffer();
      CommInterface->WriteCommand("ZS");
      CommInterface->WriteCommand("EIGN(W,0)");
      CommInterface->WriteCommand("O=0");
      CommInterface->WriteCommand("ZS");
      CommInterface->WriteCommand("EL=-1");

      CommInterface->WriteCommand("MP");
      CommInterface->WriteCommand("ADT=1");
      CommInterface->WriteCommand("VT=100000");
      CommInterface->WriteCommand(STR2BSTR(anglestring));
      CommInterface->WriteCommand("G");
   }
   catch (_com_error e )
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
}

/**
 * @brief SmartMotor::startRotation
 * @param stepSpeed
 */
void SmartMotor::startRotation(int speed)
{  try
   {  string speedstring("VT=");
      speedstring+=util::stringify<int>(speed);
      CommInterface->ClearBuffer();
      CommInterface->WriteCommand("EIGN(W,0)");
      CommInterface->WriteCommand("O=0");
      CommInterface->WriteCommand("ZS");
      CommInterface->WriteCommand("EL=-1");
      // Set the PID settings
      CommInterface->WriteCommand("KP=560");
      CommInterface->WriteCommand("KI=5");
      CommInterface->WriteCommand("KD=4500");

      CommInterface->WriteCommand("KL=32767");
      CommInterface->WriteCommand("KS=3");
      CommInterface->WriteCommand("KV=1000");
      CommInterface->WriteCommand("KA=0");
      CommInterface->WriteCommand("KG=0");

      // Activate the buffered PID settings
      CommInterface->WriteCommand("F");
      CommInterface->WriteCommand("MV");
      CommInterface->WriteCommand("ADT=1");
      CommInterface->WriteCommand(STR2BSTR(speedstring));
      CommInterface->WriteCommand("G");
   }
   catch (_com_error e )
   {  AfxMessageBox(e.Description());
      throw std::runtime_error(BSTR2STR(e.Description()));
   }
}
