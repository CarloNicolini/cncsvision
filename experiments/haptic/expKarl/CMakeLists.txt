add_executable(expKarl expKarl.cpp)
target_link_libraries(expKarl Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
 

