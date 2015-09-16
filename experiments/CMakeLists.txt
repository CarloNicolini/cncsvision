include_directories(../src/eigen)
include_directories(../src/core)
include_directories(../src/experiment)
include_directories(../src/statistic)
include_directories(../src/communications)
include_directories(../src/glviz)
include_directories(../src/optotrak)

if(GLEW_FOUND)
include_directories(../src/glsl/)
endif(GLEW_FOUND)

# This directory contains the LatestCalibration.h file
include_directories(calibrations)

add_subdirectory(activevision)
add_subdirectory(calibrations)
add_subdirectory(haptic)
add_subdirectory(demos)
add_subdirectory(other)
add_subdirectory(texturing)

if ( WIN32 AND EYEGAZE_SUPPORT )
add_subdirectory(eyegaze)
endif ( WIN32 AND EYEGAZE_SUPPORT )

if (COMPILE_GLFW)
add_subdirectory(glfw_prototypes)
endif (COMPILE_GLFW)
