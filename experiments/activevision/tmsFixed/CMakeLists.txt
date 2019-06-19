if(WIN32)
add_executable(tmsFixed tmsFixed.cpp)
add_executable(tmsFixedStaircase tmsFixedStaircase.cpp)

target_link_libraries(tmsFixed Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
target_link_libraries(tmsFixedStaircase Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(WIN32)
