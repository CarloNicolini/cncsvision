add_executable(expDanrajImages expDanrajImages.cpp)
target_link_libraries(expDanrajImages Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})