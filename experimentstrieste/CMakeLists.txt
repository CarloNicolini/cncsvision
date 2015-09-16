include_directories(../src/eigen)
include_directories(../src/core)
include_directories(../src/experiment)
include_directories(../src/statistic)
include_directories(../src/communications)
include_directories(../src/glviz)
include_directories(../src/optotrak)

# This directory contains the LatestCalibrationTrieste.h file that is needed for every experiment
include_directories(calibrations)

add_subdirectory(calibrations)
add_subdirectory(activevision)
add_subdirectory(demos)
