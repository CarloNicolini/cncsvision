################## TRY TO FIND ALP PACKAGE ########################
# Find, include and link ALP directory for using examples with ALP
set(ALP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/drivers/ALP/")
message(STATUS "ALP include directory= ${ALP_INCLUDE_DIR}" )
include_directories(${ALP_INCLUDE_DIR})

find_path( ALP_INCLUDE_DIR
	NAMES alp.h
	PATHS deps/drivers/ALP/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)

find_library( ALP_LIBRARY
	NAMES alpV42
	PATHS deps/drivers/ALP/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)
######################### END ALP PACKAGE SEARCH #################