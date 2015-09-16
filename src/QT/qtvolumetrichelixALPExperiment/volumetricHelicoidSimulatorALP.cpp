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

#include <QApplication>
#include <QDesktopWidget>
#include "MainWindow.h"
#ifdef WIN32
#include <windows.h>
void EnableCrashingOnCrashes()
{
        typedef BOOL (WINAPI *tGetPolicy)(LPDWORD lpFlags);
        typedef BOOL (WINAPI *tSetPolicy)(DWORD dwFlags);
        const DWORD EXCEPTION_SWALLOWING = 0x1;
        HMODULE kernel32 = LoadLibraryA("kernel32.dll");
        tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32,
                    "GetProcessUserModeExceptionPolicy");
        tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32,
                    "SetProcessUserModeExceptionPolicy");
        if (pGetPolicy && pSetPolicy)
        {
            DWORD dwFlags;
            if (pGetPolicy(&dwFlags))
            {
                // Turn off the filter
                pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
            }
        }
}
#endif


int main(int argc, char *argv[])
{
#ifdef WIN32
    AllocConsole();  // Create Console Window
    freopen("CONIN$","rb",stdin);   // reopen stdin handle as console window input
    freopen("CONOUT$","wb",stdout);  // reopen stout handle as console window output
    freopen("CONOUT$","wb",stderr); // reopen stderr handle as console window output
	EnableCrashingOnCrashes();
#endif
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(window.sizeHint());
    window.show();
    return app.exec();
}
