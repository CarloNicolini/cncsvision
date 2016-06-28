# Find the SDL includes and library
#
# This module defines
# IL_INCLUDE_DIR
# IL_LIBRARIES
# IL_FOUND

### FIND IL ON  LINUX
# Download SDL from https://www.SDL.org, then unzip
# ./configure; make; sudo make install

if(UNIX)
    find_path(IL_INCLUDE_DIR
    NAMES 
      il.h
    PATHS
        /usr/local/include/IL
    )
    find_library(IL_LIBRARIES
    NAMES
        libIL.a
    PATHS
        /usr/local/lib)
endif(UNIX)

if(WIN32)
message(STATUS "Looking for SDL on Windows")
    find_path(IL_INCLUDE_DIR
    NAMES 
      il.h
    PATHS
        "C:/Program Files/DevIL-SDK-x86-1.7.8/include/IL/")
      
    find_library(IL_LIBRARIES
    NAMES
        DevIL
    PATHS
        "C:/Program Files/DevIL-SDK-x86-1.7.8/lib/")
endif(WIN32)

if(IL_LIBRARIES)
    message(STATUS "Found IL:")
    message(STATUS " - Includes: ${IL_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${IL_LIBRARIES}")
    set(IL_FOUND True)
endif(IL_LIBRARIES)