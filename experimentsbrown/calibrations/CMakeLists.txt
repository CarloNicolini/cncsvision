include_directories(${CMAKE_SOURCE_DIR}/src/eigen)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/experiment)
include_directories(${CMAKE_SOURCE_DIR}/src/statistic)
include_directories(${CMAKE_SOURCE_DIR}/src/communications)
include_directories(${CMAKE_SOURCE_DIR}/src/glviz)
include_directories(${CMAKE_SOURCE_DIR}/src/optotrak)

if (COMPILE_OPTOTRAK)
add_executable(fullCalibratorBrown fullCalibratorBrown.cpp)
target_link_libraries(fullCalibratorBrown Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(screenMirrorCalibratorBrown screenMirrorCalibratorBrown.cpp)
target_link_libraries(screenMirrorCalibratorBrown Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)

add_executable(testRandomization testRandomization.cpp)
target_link_libraries(testRandomization Core Experiment ${Boost_LIBRARIES})

add_executable(pureCalibrationBrown pureCalibrationBrown.cpp)
target_link_libraries(pureCalibrationBrown Core GLViz Experiment ${GL_LIBS} ${Boost_LIBRARIES})
