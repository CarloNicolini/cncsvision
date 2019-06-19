if (COMPILE_OPTOTRAK AND WIN32)
add_executable(mirroredFinger_v1 mirroredFinger_v1.cpp)
target_link_libraries(mirroredFinger_v1 Core Experiment GLViz Optotrak Communication ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expFingerFollowingPath expFingerFollowingPath.cpp)
target_link_libraries(expFingerFollowingPath Core Experiment GLViz Optotrak Communication ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK AND WIN32)
