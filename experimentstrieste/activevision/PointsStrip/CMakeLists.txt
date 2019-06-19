add_executable(expPointsStrip_adaptationTS expPointsStrip_adaptationTS.cpp)
target_link_libraries(expPointsStrip_adaptationTS Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})
