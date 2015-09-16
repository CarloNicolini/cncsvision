include_directories(${CMAKE_SOURCE_DIR}/src/eigen)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/experiment)
include_directories(${CMAKE_SOURCE_DIR}/src/statistic)
include_directories(${CMAKE_SOURCE_DIR}/src/communications)
include_directories(${CMAKE_SOURCE_DIR}/src/glviz)
include_directories(${CMAKE_SOURCE_DIR}/src/optotrak)

add_executable(pureCalibration pureCalibration.cpp)
target_link_libraries(pureCalibration Core GLViz Experiment ${GL_LIBS} ${Boost_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT})

if(COMPILE_OPTOTRAK)
add_executable(screenMirrorCalibrator screenMirrorCalibrator.cpp)
target_link_libraries(screenMirrorCalibrator Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(fullCalibratorMotors fullCalibratorMotors.cpp)
target_link_libraries(fullCalibratorMotors Core GLViz Experiment Optotrak ${GL_LIBS} ${Boost_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT})
endif(COMPILE_OPTOTRAK)
