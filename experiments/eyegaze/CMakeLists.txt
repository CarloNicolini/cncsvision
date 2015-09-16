add_executable(plotGaze plotGaze.cpp)
add_executable(testEyeFollower testEyeFollower.cpp)
add_executable(testEyeFollowerGL testEyeFollowerGL.cpp)

target_link_libraries(plotGaze Core Experiment Communication ${Boost_LIBRARIES} ${EYEGAZE_LIBRARY})
target_link_libraries(testEyeFollower Core Experiment Communication ${Boost_LIBRARIES} ${EYEGAZE_LIBRARY})
target_link_libraries(testEyeFollowerGL Core Experiment Communication GLViz ${GL_LIBS} ${Boost_LIBRARIES} ${EYEGAZE_LIBRARY})
