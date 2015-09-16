################## TRY TO FIND EYEGAZE PACKAGE ########################
# Find, include and link Eyegaze directory for using examples with EyeGaze 
set(EYEGAZE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/drivers/Eyegaze/")
message(STATUS "Eyegaze include directory= ${EYEGAZE_INCLUDE_DIR}" )
include_directories(${CMAKE_SOURCE_DIR}/deps/drivers/Eyegaze/)

find_path( EYEGAZE_INCLUDE_DIR
	NAMES EgWin.h EgErrorMessage.h EgServer.h
	PATHS deps/drivers/Eyegaze/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)

find_library( EYEGAZE_LIBRARY
	NAMES eyegaze EgClient 
	PATHS deps/drivers/Eyegaze/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)
######################### END EYEGAZE PACKAGE SEARCH #################