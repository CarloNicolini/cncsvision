################## TRY TO FIND SMI PACKAGE ########################
# Find, include and link SmartMotor for using examples with SmartMotor
set(SMI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/drivers/SMI/")
message(STATUS "SMI include directory= ${SMI_INCLUDE_DIR}" )

find_path( SMI_INCLUDE_DIR
	NAMES IntegMotorInterface.h
	PATHS deps/drivers/SMI/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)

find_library( SMI_LIBRARY
	NAMES IntegMotorInterface
	PATHS deps/drivers/SMI/
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH
	NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH
	NO_CMAKE_SYSTEM_PATH
)

message(STATUS "SMI_INCLUDE_DIR ${SMI_INCLUDE_DIR}")
include_directories(${SMI_INCLUDE_DIR})

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) 

######################### END SMI PACKAGE SEARCH #################