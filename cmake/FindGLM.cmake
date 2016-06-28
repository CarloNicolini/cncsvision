# Find the GLM includes and library
#
# This module defines
# GLM_INCLUDE_DIR
# GLM_LIBRARIES
# GLM_FOUND

### FIND GLM ON  LINUX
# Download GLM from http://devernay.free.fr/hacks/glm/, then unzip
# ./configure; make; sudo make install

if(UNIX)
    find_path(GLM_INCLUDE_DIR
    NAMES 
      glm.h
    PATHS
        /usr/local/include
        /usr/include)
    find_library(GLM_LIBRARIES
    NAMES
        libglm
    PATHS
        /usr/local/lib)

endif(UNIX)

if(WIN32)
message(STATUS "Looking for GLM on Windows")
    find_path(GLM_INCLUDE_DIR
    NAMES 
      glm.h
	  PATHS
	  "C:/cygwin/home/visionlab/glm-0.3.2/glm")
      
    find_library(GLM_LIBRARIES
    NAMES
    libglm.a
    PATHS
      "C:/cygwin/usr/local/lib/"
	  #"C:/cygwin/home/visionlab/workspace/cncsvision/glm-0.3.2/glm/.libs/"
	  )
endif(WIN32)

#if(GLM_LIBRARIES)
    message(STATUS "Found GLM:")
    message(STATUS " - Includes: ${GLM_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${GLM_LIBRARIES}")
    set(GLM_FOUND True)
#endif(GLM_LIBRARIES)