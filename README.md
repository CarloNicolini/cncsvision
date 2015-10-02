CNCSVision
=====

-------------------------------------------------
A library for experiments in visual perception, psychophysics and visualization.
-------------------------------------------------


Requirements
------------

This is the minimal list of what you need to compile *cncsvision* on Windows, Linux and OSX.

- cmake Cmake_
- C/C++ compiler (`gcc`, `clang`, Microsoft Visual Studio `msvc`)
- boost_ C++ libraries

.. _CMake: http://www.cmake.org
.. _boost: http://www.boost.org

Informations on how to compile Boost on Windows are given in the following sections.

Compilation on Linux (tested on Ubuntu 12.04 and newer)
-----------

This is easy, assuming that you have all the necessary libraries needed by CNCSVision. From the command line:

    $ git clone https://github.com/CarloNicolini/cncsvision.git
    $ cd cncsvision
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Compilation on Windows (tested on Windows 7, VisualStudio 2010 and newer)
-----------

1) Download Boost from http://www.boost.org/users/download/
2) Unzip the folder in your C:\ directory
3) Open VS Command MS Window as Administrator
4) Change your folder position so that you are in the unzipped folder
5) Run bootstrap.bat (you found it in the root folder)
6) Run b2 --build-type=complete --build-dir=C:\Boost\bin toolset=msvc 
7) Wait until it finish to compile (this process takes some time, if it takes few seconds something went wrong!)
8)At the end, if you've got no errors, the prompt will show you the path you need to add to the System Path.
  If you're not getting errors and the prompt doesn't give you any path check the following path:
  C:\yourunzippedfolder\stage\lib\ --> here you should find the .dll and .lib files
  C:\yourunzippedfolder\
  These are the paths you should add. If you have no stage folder, check in the C:\ folder if it has been created the Boost folder. 
  This folder must contain the folder bin.v2, which contains all the builds for all the libraries.
  If you don't have these folders, you should try again to build the libraries (from step 5).


Usage
-----


References
----------

.. [1] Nicolini, C., Fantoni, C., Mancuso, G., Volcic, R., & Domini, F. (2014). 'A framework for the study of vision in active observers.' In: Human Vision and Electronic Imaging XIX, Rogowitz, B.E., Pappas, T.N. and de Ridder, H. (eds.), Proceedings of SPIE, 9014.

