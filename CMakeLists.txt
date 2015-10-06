project(cncsvision)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
################################ CMAKE OPTIONS ###############################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/" CACHE PATH "Single directory for all libraries")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/" CACHE PATH "Single directory for all executables")

#add_definitions("-DCNCSVISION_BASE_DIRECTORY")
####################### COMPILE OPTIONS  ##################################
option(BUILD_SHARED_LIBS OFF)
option(COMPILE_TESTS "Compile the tests run to assess functionalities of CNCSVision" OFF)
option(COMPILE_OPTOTRAK "Compile the examples that use the Optotrak" ON)
option(COMPILE_STATISTIC "Compile the statistic module, needed for Bayes network analysis" OFF)
option(COMPILE_GLFW "Compile the GLFW library for OpenGL context creation" OFF)
option(QT_SUPPORT "Compile and link the Qt4 related examples and libraries" OFF)
option(COMPILE_LAB_TRIESTE_EXPERIMENTS "Compile Trieste Lab experiments" OFF)
option(COMPILE_LAB_BROWN_EXPERIMENTS "Compile Brown Lab experiments" OFF)
option(COMPILE_LAB_BROWN_SIMULATION "Use simulation mode on brown experiment" OFF)
option(COMPILE_LAB_ROVERETO_EXPERIMENTS "Compile Rovereto Lab experiments" OFF)
option(COMPILE_LIB_NOISE "Compile libnoise from source" OFF)
option(PHIDGET_SUPPORT "Use the Phidget libraries, (must install the drivers)" OFF)
option(PYTHON_SUPPORT "Compile Cython extension to make CNCSVision available as Python library" OFF)
option(QT5_SUPPORT "Compile and link the Qt5 related examples and libraries" OFF)

if (COMPILE_LAB_BROWN_SIMULATION)
    add_definitions("-DSIMULATION")
endif (COMPILE_LAB_BROWN_SIMULATION)

####################### FIND PACKAGES AND LIBRARIES ####################
find_package(StandardMathLibrary)
if(WIN32)
    option(EYEGAZE_SUPPORT "Include and link EyeGaze libraries (only for Windows)" OFF)
    option(ALP_SUPPORT "Include and link ALP projector libraries (Win32 only)" OFF)
    option(SMI_SUPPORT "Include and link Smart Motor Interface" OFF)

    if(EYEGAZE_SUPPORT)
    find_package(Eyegaze)
    endif(EYEGAZE_SUPPORT)

    if(ALP_SUPPORT)
    find_package(ALP)
    endif(ALP_SUPPORT)

    if(SMI_SUPPORT)
    find_package(SmartMotor)
    endif(SMI_SUPPORT)

    if(COMPILE_OPTOTRAK)
    find_package(Optotrak)
    endif(COMPILE_OPTOTRAK)
endif(WIN32)

if(COMPILE_OPTOTRAK)
find_package(Optotrak)
endif(COMPILE_OPTOTRAK)

# Support for GLW, please keep it up-to-date running a git pull on "deps/glfw3"
if(COMPILE_GLFW)
    # See http://www.glfw.org/docs/latest/build.html
    add_subdirectory("${PROJECT_SOURCE_DIR}/deps/glfw-3.1.1")
    set(GLFW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/deps/glfw-3.1.1/include")
    # "if The Xf86VidMode library and headers were not found"
    # On Ubuntu 12.04 LTS need to install
    # sudo apt-get install xorg-dev  libglu1-mesa-dev
    include_directories(${GLFW_INCLUDE_DIR})
endif(COMPILE_GLFW)

######################## QT4 ########################
if(QT_SUPPORT)
    set(QT_MIN_VERSION "4.8.0")
    set(QT_USE_QTMAIN TRUE)
    set(QT_USE_OPENGL TRUE)
    find_package(Qt4 ${QT_MIN_VERSION} COMPONENTS QtGui QtCore QtOpenGL REQUIRED )
    include(${QT_USE_FILE})
    include_directories(${QT_INCLUDE_PATH} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})
endif(QT_SUPPORT)

if(QT5_SUPPORT)
find_package(Qt5Widgets REQUIRED)
    if(NOT MSVC)
          # Enable the C++11 standard
          set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
    endif(NOT MSVC)
endif(QT5_SUPPORT)

####################### BOOST #######################
find_package(Threads REQUIRED)
message(STATUS "Using ${CMAKE_THREAD_LIBS_INIT} threading library")
set(BOOST_LIBS thread date_time system filesystem program_options regex serialization iostreams)
set(Boost_USE_STATIC_LIBS        ON)#cambiato era ON SU WINDOWS
set(Boost_USE_MULTITHREADED      ON) #cambiato era ON SU WINDOWS
set(Boost_USE_STATIC_RUNTIME    OFF)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
# Use Boost 1.4.7 with Win7_64 + Qt4.8 because of bugs in Qt and Boost
# Download Boost 1_47_0 from http://sourceforge.net/projects/boost/files/boost/ and compile it


##########  CROSS-PLATFORM OPENGL + GLUT + GLEW  ########################
############################ FIND OPENGL EXTENSIONS GLEW ################
find_package(GLEW)
message(WARNING "Please update the glew source dir with latest glew")
if(NOT GLEW_FOUND)
  message (FATAL_ERROR "GLEW not found! CNCSVision needs GLEW libraries in order to support advanced rendering options.")
endif()

if(APPLE)
        # Set the architectures to both i386 and x86_64 so the generated libraries have both and can interface with both of them
        # Comment it if the build is too slow
        #set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
        message(STATUS "Building on Apple for the following architectures: ${CMAKE_OSX_ARCHITECTURES}")
        include_directories ( /System/Library/Frameworks )
        find_package(OpenGL REQUIRED)
        find_package(GLUT REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        find_library(OpenGL_LIBRARY OpenGL )
        find_library(GLUT_LIBRARY GLUT )
        # Linkare le librerie opengl e glut e glew (obtained with macport)
        set(GL_LIBS "-framework GLUT -framework OpenGL -lglew" )
endif(APPLE)

if(UNIX AND NOT APPLE)
        find_package(OpenGL REQUIRED)
        include_directories(${OPENGL_INCLUDE_DIR})
        find_package(GLUT REQUIRED)
        set(GL_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif(UNIX AND NOT APPLE)

if(WIN32)
        # Since typical Windows installation have OpenGL in system path but not GLUT
        #  here we find the OpenGL package automatically but search for GLUT in the
        # self provided directory deps/glutwin32 instead of copying files around in Windows/System32
        find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
        # Find GLUT in the supplied code (substitution for FindGLUT.cmake)
        find_path( GLUT_INCLUDE_DIR
                   NAMES glut.h
                   PATHS ${CMAKE_SOURCE_DIR}/deps/glutwin32
                   NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
                   NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
                   NO_CMAKE_SYSTEM_PATH
                   )
                # Here we include our glut header file provided in deps/glutwin32
        include_directories(${GLUT_INCLUDE_DIR})
                find_library( GLUT_LIBRARIES
                      NAMES glut32
                      PATHS  ${CMAKE_SOURCE_DIR}/deps/glutwin32
                      NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
                      NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
                      NO_CMAKE_SYSTEM_PATH
                      )
        if( GLUT_INCLUDE_DIR AND GLUT_LIBRARIES )
          set( GLUT_FOUND 1 )
                else ( GLUT_INCLUDE_DIR AND GLUT_LIBRARIES )
                  message(FATAL_ERROR "GLUT is not found on this system")
        endif( GLUT_INCLUDE_DIR AND GLUT_LIBRARIES )

                message(STATUS "GLUT headers found in: ${GLUT_INCLUDE_DIR}")
                message(STATUS "GLUT libraries found in: ${GLUT_LIBRARIES}")
                set(GL_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif(WIN32)
################################## END OPENGL ##################################

##########################  if GNU COMPILER ####################################
if(CMAKE_COMPILER_IS_GNUCXX)
        if(COMPILE_OPTOTRAK)	# THESE FLAGS ARE NEEDED FOR COMPATIBILITY WITH NDI OAPI LIBRARIES
        if(NOT APPLE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -ldl")
        endif(NOT APPLE)
        endif(COMPILE_OPTOTRAK)
    set(CMAKE_CXX_FLAGS "-Wno-write-strings")
endif(CMAKE_COMPILER_IS_GNUCXX)


########################## endif GNU COMPILER ##################################

########################## if MICROSOFT COMPILER ###############################
# Push more optimization options with Microsoft Compiler
if(MSVC)
# This set the following defines when compiling on windows
add_definitions("-D_WIN32 -DNOMINMAX /EHsc /wd4127 /wd4129 /wd4505 /wd4714 /wd4800 /wd4181 /wd4244 /wd4996 /wd4250 -D_WIN32_WINNT=0x0501")

if( NOT SMI_SUPPORT)
    add_definitions("/MP") #disable multiprocessor compilation when including Smart Motor
endif(NOT SMI_SUPPORT)

if(ALP_SUPPORT)
    add_definitions("-DALP_SUPPORT")
    set(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    message(STATUS "MSVC: Enabled large address awareness")
endif(ALP_SUPPORT)


# This push some other optimizations in order: Full optimization (Ox), omit frame pointer (Oy), Enable FiberSafe optimizations(GT),
# Whole Program Optimization (/GL), OpenMP support (/openmp), NONdynamic runtime libraries (MT)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /arch:SSE2 /Ox /Oy /GT /GL")
# Specify to use multithreaded debug (non dynamic)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=2 /INCREMENTAL:NO")
endif(MSVC)
####################### END if MICROSOFT COMPILER #############################

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR} )

######### START PYTHON SUPPORT THROUGH CYTHON #################
if(PYTHON_SUPPORT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    find_package(PythonLibs REQUIRED)
        #----- Print some informations on python libraries linked and included
        message(STATUS "Python libraries are ${PYTHON_LIBRARIES}")
        message(STATUS "Python libraries are included in ${PYTHON_INCLUDE_DIRS}")
        message(STATUS "Python versions found is ${PYTHONLIBS_VERSION_STRING}")
        include_directories("${PYTHON_INCLUDE_DIR}")
    include(UseCython)
    add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake
      ${CMAKE_CURRENT_BINARY_DIR}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endif(PYTHON_SUPPORT)

add_subdirectory(src)

if(COMPILE_LAB_ROVERETO_EXPERIMENTS)
    add_subdirectory( experiments )
endif(COMPILE_LAB_ROVERETO_EXPERIMENTS)

if(COMPILE_LAB_TRIESTE_EXPERIMENTS)
    add_subdirectory( experimentstrieste )
endif(COMPILE_LAB_TRIESTE_EXPERIMENTS)

if(COMPILE_LAB_BROWN_EXPERIMENTS)
    if(PHIDGET_SUPPORT)
        find_package(Phidget)
        if(PHIDGETS_FOUND)
            add_definitions("-DPHIDGET_SUPPORT")
            include_directories(${PHIDGETS_INCLUDE_DIR})
        else(PHIDGETS_FOUND)
            message(ERROR "PHIDGET DRIVERS ARE NOT FOUND, SPECIFY THE PHIDGET INSTALLATION FOLDER")
        endif(PHIDGETS_FOUND)
    endif(PHIDGET_SUPPORT)

    if(COMPILE_LIB_NOISE)
        message(STATUS "Added support for compilation of libnoise 1.0 from source")
        add_definitions("-DLIBNOISE_SUPPORT")
        add_subdirectory("${PROJECT_SOURCE_DIR}/deps/libnoise")
        include_directories("${PROJECT_SOURCE_DIR}/deps/libnoise/src/")
    endif(COMPILE_LIB_NOISE)
    include_directories(experimentsbrown) # because some source files are there
    add_subdirectory(experimentsbrown)
endif(COMPILE_LAB_BROWN_EXPERIMENTS)
