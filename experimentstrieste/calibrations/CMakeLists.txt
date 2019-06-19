include_directories(${CMAKE_SOURCE_DIR}/src/eigen)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/experiment)
include_directories(${CMAKE_SOURCE_DIR}/src/statistic)
include_directories(${CMAKE_SOURCE_DIR}/src/communications)
include_directories(${CMAKE_SOURCE_DIR}/src/glviz)
include_directories(${CMAKE_SOURCE_DIR}/src/optotrak)

if (COMPILE_OPTOTRAK)
add_executable(fullCalibratorTS fullCalibratorTS.cpp)
target_link_libraries(fullCalibratorTS Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(screenMirrorCalibratorTS screenMirrorCalibratorTS.cpp)
target_link_libraries(screenMirrorCalibratorTS Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)

add_executable(pureCalibrationTS pureCalibrationTS.cpp)
target_link_libraries(pureCalibrationTS Core GLViz Experiment ${GL_LIBS} ${Boost_LIBRARIES})
