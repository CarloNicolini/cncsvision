add_executable(expPointsStrip_base expPointsStrip_base.cpp)
target_link_libraries(expPointsStrip_base Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPointsStrip_adaptation expPointsStrip_adaptation.cpp)
target_link_libraries(expPointsStrip_adaptation Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPointsStripActive expPointsStripActive.cpp)
target_link_libraries(expPointsStripActive Core Experiment Optotrak GLViz ${GL_LIBS} ${Boost_LIBRARIES})
