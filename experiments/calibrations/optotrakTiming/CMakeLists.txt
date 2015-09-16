if(COMPILE_OPTOTRAK AND WIN32)
add_executable(fullCalibratorMotors fullCalibratorMotors.cpp)
target_link_libraries(fullCalibratorMotors Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(optotrakFrequencyTestVelocity optotrakFrequencyTestVelocity.cpp)
target_link_libraries(optotrakFrequencyTestVelocity Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK AND WIN32)
