// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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
#include "Timer.h"
#include <windows.h>
#include <EgWin.h>
#include <EgErrorMessage.h>
#include "EyeFollower.h"

using namespace std;

int main(int argc, char *argv[])
{

	SetCurrentDirectory("C:\\Eyegaze\\");
	EyeFollower eyeFollower;
	
	eyeFollower.init(true,1024,768,"SOCKET","192.168.231.179");
	// BLOCKING CALLS METHOD
	//eyeFollower.startBackgroundCollection();
	
	while ( true )
	{
		eyeFollower.querySystem();
		cout << eyeFollower.getGazePos().transpose() << " " <<eyeFollower.isVisible() << endl;
		Sleep(100);
	}

	return 0;
}
