if(UNIX)
	set(EXPERIMENTBROWN_SRCS
	TrapezoidPointsStimulus.cpp
	)

	set(EXPERIMENTBROWN_HDRS
	TrapezoidPointsStimulus.h
	load_obj_defaults.h
	)
endif(UNIX)

if(COMPILE_LIB_NOISE)
    set(EXPERIMENTBROWN_SRCS ${EXPERIMENTBROWN_SRCS}
        noiseutils.cpp)
    set(EXPERIMENTBROWN_HDRS ${EXPERIMENTBROWN_HDRS}
        noiseutils.h)
endif(COMPILE_LIB_NOISE)

if(PHIDGET_SUPPORT)
    set(EXPERIMENTBROWN_SRCS ${EXPERIMENTBROWN_SRCS}
        BrownPhidgets.cpp)
    set(EXPERIMENTBROWN_HDRS ${EXPERIMENTBROWN_HDRS}
        BrownPhidgets.h)
endif(PHIDGET_SUPPORT)

add_library(BrownSupportLibs ${EXPERIMENTBROWN_SRCS} ${EXPERIMENTBROWN_HDRS})
target_link_libraries(BrownSupportLibs Experiment Core Optotrak)

if (COMPILE_LIB_NOISE)
    target_link_libraries(BrownSupportLibs noise)
endif (COMPILE_LIB_NOISE)

if(PHIDGET_SUPPORT)
	target_link_libraries(BrownSupportLibs ${PHIDGET_LIBRARIES})
endif(PHIDGET_SUPPORT)

if(UNIX)
	find_package(FTDI REQUIRED)
	if(FTDI_FOUND)
		message(STATUS " - FTDI found")
	endif()
endif(UNIX)

target_link_libraries(BrownSupportLibs ${SDL_LIBRARIES} glm ${IL_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

### FTDI
set(FTDI_SRCS
		BrownFtdi.cpp
	)

	set(FTDI_HDRS
		BrownFtdi.h
	)
add_library(BrownFtdi ${FTDI_SRCS} ${FTDI_HDRS})
target_link_libraries(BrownFtdi ftdi1)
###

include_directories(../src/eigen)
include_directories(../src/core)
include_directories(../src/experiment)
include_directories(../src/statistic)
include_directories(../src/communications)
include_directories(../src/glviz)
include_directories(../src/optotrak)
if(WIN32)
include_directories(${JPEG_INCLUDE_DIR})
endif()

# This directory contains the LatestCalibrationBrown.h file that is needed for every experiment
include_directories(calibrations)

add_subdirectory(calibrations)
add_subdirectory(ShapeConstancy)
add_subdirectory(Brown_tests)
