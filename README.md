CNCSVision
===

-------------------------------------------------

A library for experiments in visual perception, psychophysics and visualization.
-------------------------------------------------


Requirements
------------

This is the minimal list of what you need to compile *cncsvision* on Windows, Linux and OSX.

- cmake www.cmake.org
- C/C++ compiler (`gcc`, `clang`, Microsoft Visual Studio `msvc`)
- boost C++ libraries http://www.boost.org (latest tested `1.59`)

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

If you dont'have the necessary libraries installed this is a list of what you need:

- build-essential (constains gcc and other facilities as `gnu-make`)
- libboost-all-dev (all the boost libraries)
- freeglut3-dev (freeglut is a replacement of glut )
- libglew-dev (for support of advanced OpenGL commands and glsl shaders)
- libglu1-mesa-dev
- libgl1-mesa-glx
- mesa-common-dev
- libxi-dev
- libxmu-dev
- qt4-dev-tools

You can issue the following command on the command line to install everything:

    $> sudo apt-get install build-essential libboost-all-dev freeglut3-dev libglew-dev  libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev mesa-common-dev libxi-dev libxmu-dev libxmuu-dev qt4-dev-tools

## CMake compile-time options
You can compile all the experiments by specifying `cmake` flags when creating the project. You do this in the following way:

    $ cd cncsvision/build
    $ cmake -D{XXX}=True

where ``{XXX}`` is one of the following:

- **COMPILE_TESTS**

  Set it to `True` to compile all the test executables for all CNCSVision classes.

- **COMPILE_OPTOTRAK**

  Set it to `True` if you want CNCSVision support and compile `Optotrak` related classes.

- **COMPILE_GLFW**

  Set it to `True` to compile `GLFW3` from source (included in CNCSVision).

- **COMPILE_STATISTIC**

  Set it to `True` if you want CNCSVision to compile the statistic module, needed for Bayes network analysis of optic flow models (DEPRECATED)
- COMPILE_GLFW
  Set it to `True` if you want CNCSVision compile from scratch the *GLFW3* library needed for `OpenGL` context creation.

- **QT_SUPPORT**

  Set it to `True` if you want CNCSVision to compile the executables, examples and tests that need Qt4 (programs which uses GUI).

- {**COMPILE_LAB_BROWN_EXPERIMENTS**, **COMPILE_LAB_TRIESTE_EXPERIMENTS**,  **COMPILE_LAB_ROVERETO_EXPERIMENTS**}

  Set it to `True` to compile experiments for the different laboratories where CNCSVision is used.

- **COMPILE_LIB_NOISE**

  Set it to `True` to compile `libnoise` from scratch, instead of relying on system-wise installed version.

- **PHIDGET_SUPPORT**

  Set it to `True` to allow support of Phidgets www.phidgets.com in some experiments. Needed only when using `COMPILE_LAB_BROWN_EXPERIMENTS` flag.

- **PYTHON_SUPPORT**

  Set it to `True` to compile some `cython` extensions of the classes of CNCSVision. This option makes use of `Cython` so you must have latest cython installed. You can install cython with `sudo pip install cython`. Not tested under Windows.
  This is only partially supported and not all functions are available.

- **QT5_SUPPORT**

  Definitely forget about it...not tested nor working actually.

### Options supported only under Windows

- **EYEGAZE_SUPPORT**
  Support for the Eyegaze eye-follower device.  Dependencies and libraries are in the `deps/drivers/Eyegaze` folder.

- **ALP_SUPPORT**
  Support for the ultra-high-speed ALP projector. Dependencies and libraries are in the `deps/drivers/ALP` folder.

- **SMI_SUPPORT**
  Support for the SmartMotor Interfaces. Dependencies and libraries are in the `deps/drivers/SMI` folder.


FAQ
---

(to be filled)


Compiling Boost on Windows
-----------
These steps are written and tested for compilation of Boost >= 1.59, Windows 7 (no newer versions tested), VisualStudio 2010 and newer.

1. Download Boost from http://www.boost.org/users/download/
2. Unzip the folder in your `C:\boosttmp` directory
3. Open VisualStudio Command MS Window as *Administrator*
4. Change your folder position so that you are in the unzipped folder `C:\boosttmp`
5. Run `bootstrap.bat` (you find it in the root folder)
6. Run `b2 --build-type=complete --build-dir=C:\Boost\bin toolset=msvc`
7. Wait until it finish to compile (this process takes some time, if it takes few seconds something went wrong!)
8. At the end, if you've got no errors, the prompt will show you the path you need to add to the System Path.
9. If you're not getting errors and the prompt doesn't give you any path check the following path:


    C:\boosttmp\stage\lib\ --> here you should find the .dll and .lib files
    C:\boosttmp\


- These are the paths you should add to system `PATH`.
- If you have no stage folder, check in the `C:\` folder if it has been created the Boost folder. This folder must contain the folder `bin.v2`, which contains all the builds for all the libraries.
- If you don't have these folders, you should try again to build the libraries (from step 5).


References
----------

[1] Nicolini, C., Fantoni, C., Mancuso, G., Volcic, R., & Domini, F. (2014). A framework for the study of vision in active observers.' In: Human Vision and Electronic Imaging XIX, Rogowitz, B.E., Pappas, T.N. and de Ridder, H. (eds.), Proceedings of SPIE, 9014.
