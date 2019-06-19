Introduction
------------

The OptotrakToolbox runs on Windows and on Linux and allows to
directly interact with the Optotrak 3020 from within Matlab. This is
achieved by a MEX-file which calls the original Optotrak API (=
"Application Programmer's Interface". This you must have bought from
Northern Digital. It is the C programming language interface to the
Optotrak system). 

Please note: 

 - As long as the sample C-programs you received with the Optotrak API
 do not work on your computer, you cannot expect the OptotrakToolbox
 to work! Unfortunately, on Windows you need to use the Microsoft
 Visual C++ compiler.

 - My very tight time-schedule does not permit me to provide any
 support for this program!!!

 - For Certus-users: We don't have a Certus, so I cannot check myself
 whether the OptotrakToolbox works for it. But, it seems to be working
 with the Certus (I performed some minor adaptations, according to
 some hints from OptotrakToolbox users; see the file ChangeLog for
 some further comments on this).

Quick start
-----------

 - Purchase the Optotrak API from Northern Digital, install it and get
   the sample C-programs to compile and run (independent of Matlab!)

 - In Matlab use "mex -setup" to select the same compiler you used in
   the previous step to compile the sample C-programs outside Matlab
   (preferably the Microsoft Visual C++ compiler). 

 - Edit make_all.m to reflect your operating system and the NDI paths.
 - Call make_all from within Matlab to compile the MEX file.

 - Determine system parameters with: determineOptotrakSystem.m

 - Work your way through the examples: sample1.m, sample2.m, etc.
   (the files sample1.c, sample2.c are the original examples for the
   Optotrak API and are there only for your reference). The most
   important example is sample5.m! See also: 
 
      http://www.allpsych.uni-giessen.de/vf/OptotrakToolbox/

Current notes on the C-Compiler and your Matlab system
------------------------------------------------------

With the advent of 64-bit machines and Windows 7 things got a bit
complicated. Here are some tipps as of March 2011. Note that all this
stuff is dependent on your specific machine and setup and that I
cannot provide further help with this stuff:

- Compiling the sample C-programs (independent of Matlab!):
  - ALWAYS compile 32bit even if on a 64bit machine!

  - You can use the free Microsoft Visual C Compiler: Visual Studio
    2010 Express (VS2010) as, for example, described here:
    http://www.mathworks.com/support/solutions/en/data/1-DLG5NF/index.html?solution=1-DLG5NF

  - To set VS2010 to compile 32 bit do something like this: 
    - open command shell:  
      StartMenu->Programs->SDK->CommandPrompt
    - enter in the shell: 
      setenv /x86 

- Calling the compiler using mex from within Matlab:
  - ALWAYS use 32bit Matlab even if on a 64bit machine!

  - If you encounter problems to select VS2010 after calling 
    "mex -setup", see: 
    http://www.mathworks.com/support/compilers/R2010a/win64.html#matlab


Features of the OptotrakToolbox
-------------------------------

 - 1-to-1 mapping of the relevant functions of the Optotrak API (which
 is written in C) to Matlab functions. For example, to load the
 Transputer system in C, you would call the function:

    TransputerLoadSystem("system")

 ... and in Matlab you would call:

    optotrak('TransputerLoadSystem','system');

 The advantage of this approach is: (1) Most sample programs which
 were provided from Northern Digital for C are transformed to
 corresponding matlab samples. (2) You can use the original manuals of
 the Optotrak API for the OptotrakToolbox (3) C-programmers who know
 the Optotrak API should be very quick in using the OptotrakToolbox.

 - In addition to the original functions of the Optotrak API, the
 OptotrakToolbox provides some Matlab functions which give you
 additional functionality. These functions have their own examples
 (see: todo). They allow a convenient handling of Optotrak files, of
 rigid bodies, and of the frame-of-reference of the cameras.

 - The OptotrakToolbox is intended for use with the Psychophysics
 toolbox (see: todo), but is completely independent of the
 Psychophysics toolbox.

Limitations
-----------

 - The ODAU is not supported (yet?). The ODAU allows you to store
 external analog data in parallel to the Optotrak data. For most
 people this is not a limitation, because they don't have the ODAU
 hardware.

 - Very few functions are not available in Matlab yet. This is for two
 reasons: (1) Some functions are not needed in Matlab. This mainly
 applies to file-handling routines, because this is done much better
 in Matlab. The method of choice is: First, you collect data-files
 with the Optotrak and the OptotrakToolbox. This creates files in the
 special Optotrak/NorthernDigital format. Then you read these files to
 Matlab using the OptotrakToolbox, and save the data as standard
 Matlab files (e.g., using the "save" command). After this, your data
 are platform independent, you don't need to have the Optotrak API
 installed, nor the OptotrakToolbox in order to read and analyze your
 data!!! For an example, see: todo (2) For some functions I had no
 time yet... If you need them, let me know and we see what happens :-)

License (see also COPYING.txt)
------------------------------

OptotrakToolbox: Control your Optotrak from within Matlab
Copyright (C) 2004 Volker Franz, volker.franz@psychol.uni-giessen.de 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

