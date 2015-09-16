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


#include "StdAfx.h"
#include <iostream>
#include "SmartMotor.h"
#include <windows.h>
#import "IntegMotorInterface.tlb"
#include "SMIDefs.h"
#define IDM_ABOUTBOX                    0x0010
#define IDD_ABOUTBOX                    100
#define IDS_ABOUTBOX                    101
#define IDD_USEENGINE_DIALOG            102
#define IDR_MAINFRAME                   128
#define IDC_NOOFMOTORS                  1000
#define IDC_ADDRESSCHAIN                1001
#define IDC_INITENGINE                  1002
#define IDC_DETECT232                   1002
#define IDC_COMMAND                     1003
#define IDC_RESPONSE                    1004
#define IDC_SEND                        1005
#define IDC_GET                         1006
#define IDC_COMCOMBO                    1007
#define IDC_OPEN                        1008
#define IDC_BAUDRATECOMBO               1009
#define IDC_CURRENTMOTOR                1010
#define IDC_SENDANDGET                  1011
#define IDC_HOSTMODE                    1012
#define IDC_PATHGEN                     1013
#define IDC_INITRS485                   1013
#define IDC_LINE                        1014
#define IDC_CIRCLE                      1015
#define IDC_BUTTON1                     1016
#define IDC_DETECTRS485                 1016
#define IDC_FILENAME                    1017
#define IDC_SCAN                        1018
#define IDC_MAKESMX                     1019
#define IDC_DOWNLOAD                    1020
#define IDC_NEXTERROR                   1021
#define IDC_ERRORTEXT                   1022
#define IDC_NOOFERRORS                  1023
#define IDC_NOOFWARNINGS                1024
#define IDC_ERRORPOS                    1025
#define IDC_ERRORTYPE                   1026
#define IDC_FIRSTERROR                  1027
#define IDC_PERCENT                     1028
#define IDC_ADDRESSSERVOS               1029
#define IDC_MAXADDRESS                  1030
#define IDC_MOTORCHANNEL                1031
#define IDC_GETSN                       1032
#define IDC_GETMODELSTR                 1033
#define IDC_PROGRESS                    1034
#define IDC_STATUS                      1035
#define IDC_VERSION                     1036
#define IDC_UPLOAD                      1037
#define IDC_CLEARPROGRAM                1038
#define IDC_CLOSE                       1039
#define IDC_CHECK1                      1040
#define IDC_CUSTOM1                     1041

using namespace INTEGMOTORINTERFACELib;


using namespace std;

int main()
{  /*
   ISMICommPtr		CommInterface;
    ISMICMotionPtr	CMotionInterface;
    ISMIPathPtr		PathInterface;
   CoInitialize(NULL);
    if(!AfxOleInit())
    {
        throw std::runtime_error("OLE initialization failed.  Make sure that the OLE libraries are the correct version.");
    }
    AfxEnableControlContainer();
   try
   {
   	// Create SMIHost object and interfaces
   	HRESULT hr = CommInterface.CreateInstance(__uuidof(SMIHost));
   	if(FAILED(hr))
   	{
   		AfxMessageBox("Cannot create an instance of \"SMIHost\" class!");
   	}
   	hr = CommInterface.QueryInterface(__uuidof(ISMICMotion),&CMotionInterface);
   	if(FAILED(hr))
   	{
   		AfxMessageBox("The interface \"ISMICMotion\" not found!");
   	}
   	hr = CommInterface.QueryInterface(__uuidof(ISMIPath),&PathInterface);
   	if(FAILED(hr))
   	{
   		AfxMessageBox("The interface \"ISMIPath\" not found!");
   	}

   	CString Str = "N/A";
   	long Version = CommInterface->EngineVersion;
   	if(Version)
   	{
   		Str.Format("IntegMotorInterface Version: %d.%d%d%d",
   		HIBYTE(HIWORD(Version)),
   		LOBYTE(HIWORD(Version)),
   		HIBYTE(LOWORD(Version)),
   		LOBYTE(LOWORD(Version)));
   		AfxMessageBox(Str);
   	}

   	// Open PORT 4
   	try
   	{
   	CommInterface->BaudRate = 9600;
   	CommInterface->OpenPort("Com4");
   	}
   	catch (_com_error e)
   	{
   		throw std::exception("Error opening COM4 port");
   	}
   	// Detect RS232
   	UINT x= 10;//GetDlgItemInt(IDC_MAXADDRESS);
   	switch(CommInterface->DetectRS232(10,0))
   	{
   		case CER_SOMEADDRESSED:
   			cerr << "Some Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
   			break;

   		case CER_BIGADDRESS:
   			cerr << "Some Motors have big addresses!" <<" IntegMotorInterface Error!" << endl;
   			break;

   		case CER_DUPLICATEADDR:
   			cerr << "Some Motors have duplicate addresses!"<<" IntegMotorInterface Error!"<< endl;
   			break;

   		case CER_NOTADDRESSED:
   			cerr << "Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
   			break;

   		case CER_COMMERROR:
   			cerr << "Communication Error!"<<" IntegMotorInterface Error!"<< endl;
   			break;

   		case CER_NOMOTOR:
   			cerr << "No Motor Found!"<<" IntegMotorInterface Error!" << endl;
   			break;
   		default:
   			cerr << CommInterface->NoOfMotors << endl;
   			//cerr << CommInterface->DefaultMotor = long(1);//GetDlgItemInt(IDC_CURRENTMOTOR);
   	}
   	// Establish chain
   	//CommInterface->EstablishChain();
   	// Address Chain
   	long n = CommInterface->AddressMotorChain();
   	cerr << CommInterface->GetDefaultMotor() << endl;
   	CommInterface->DefaultMotor = 1;
   	cerr << CommInterface->GetDefaultMotor() << endl;
   	CommInterface->ClearBuffer();
   	CommInterface->WriteCommand("ZS");
   	CommInterface->WriteCommand("EIGN(W,0)");
   	CommInterface->WriteCommand("O=0");
   	CommInterface->WriteCommand("ZS");
   	CommInterface->WriteCommand("EL=-1");

   	CommInterface->WriteCommand("MP");
   	CommInterface->WriteCommand("ADT=1");
   	CommInterface->WriteCommand("VT=100000");
   	CommInterface->WriteCommand("PRT=8000");
   	CommInterface->WriteCommand("G");
   }
   catch(_com_error e)
   {
   	//MessageBox(e.Description(),"IntegMotorInterface Error!",MB_ICONWARNING);
   }
   */
   SmartMotor smart;
   smart.init();
   try
   {  smart.openPort();
      smart.detect232();
      smart.addressChain();
   }
   catch (_com_error e)
   {  throw std::runtime_error(BSTR2STR(e.Description()));
   }

   smart.startRotation(0);
}