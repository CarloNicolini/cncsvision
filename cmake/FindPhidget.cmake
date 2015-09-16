# Find the Phidgets includes and library
#
# This module defines
# PHIDGETS_INCLUDE_DIR
# PHIDGETS_LIBRARIES
# PHIDGETS_FOUND
# Checking OS

### FIND PHIDGET ON  LINUX
# Download phidget from website, then unzip
# ./configure; make; sudo make install
# and it should go into /usr/lib
if(UNIX)
    find_path(PHIDGETS_INCLUDE_DIR
    NAMES 
      phidget21.h
    PATHS
        /usr/local/include
        /usr/include)
    find_library(PHIDGETS_LIBRARIES
    NAMES
        phidget21
    PATHS
        /usr/local/lib
        /usr/lib)
endif(UNIX)

### FIND PHIDGET ON  WINDOWS
if(WIN32)
message(STATUS "Looking for phidget on Windows")
    find_path(PHIDGETS_INCLUDE_DIR
    NAMES 
      phidget21.h
    PATHS
        "C:/Program Files/Phidgets/"
    DOC "Full path of library file 'phidget21.so' or 'phidget21.lib'" )
      
    find_library(PHIDGETS_LIBRARIES
    NAMES
        phidget21
    PATHS
        "C:/Program Files/Phidgets/"
        )
endif(WIN32)

### FIND PHIDGET ON  OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(PHIDGETS_LIBRARY Phidget21)
    if (PHIDGETS_INCLUDE_DIR1)
        set(PHIDGETS_INCLUDE_DIR ${PHIDGETS_INCLUDE_DIR1})
    endif (PHIDGETS_INCLUDE_DIR1)
    if (PHIDGETS_LIBRARY)
        set(PHIDGETS_LIBRARIES ${PHIDGETS_LIBRARY})
    endif (PHIDGETS_LIBRARY)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


#if(PHIDGETS_LIBRARIES)
    message(STATUS "Found Phidget:")
    message(STATUS " - Includes: ${PHIDGETS_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${PHIDGETS_LIBRARIES}")
    set(PHIDGETS_FOUND True)
#endif(PHIDGETS_LIBRARIES)