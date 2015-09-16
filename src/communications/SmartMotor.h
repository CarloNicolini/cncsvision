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


#ifndef _SMART_MOTOR_H_
#define _SMART_MOTOR_H_

//#if !defined(AFX_USEENGINE_H__7FD37733_0C30_4AD7_ACA1_13FA5962DC2F__INCLUDED_)
//#define AFX_USEENGINE_H__7FD37733_0C30_4AD7_ACA1_13FA5962DC2F__INCLUDED_

/*
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
*/
#include "stdafx.h"
#import "IntegMotorInterface.tlb"
#include "SMIDefs.h"
#include <comdef.h>

#ifndef BSTR2STR
#define BSTR2STR(X) std::string(const_cast<char *>(_com_util::ConvertBSTRToString( X )))
#endif
#ifndef STR2BSTR
#define STR2BSTR(X) _com_util::ConvertStringToBSTR(const_cast<const char*>(X.c_str()))
#endif
using namespace INTEGMOTORINTERFACELib;

class SmartMotor
{
public:
    INTEGMOTORINTERFACELib::ISMICommPtr		CommInterface;
    //INTEGMOTORINTERFACELib::ISMICMotionPtr	CMotionInterface;
    //INTEGMOTORINTERFACELib::ISMIPathPtr		PathInterface;

    SmartMotor();
	~SmartMotor();
	void init();

	void openPort();
	void addressChain();
    void detect232();
	void closePort();

	void rotateAngle(double angle);
    void startRotation(int speed=1075000);
};

#endif
