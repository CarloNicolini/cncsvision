if(COMPILE_OPTOTRAK)
add_executable(activeStereopsysTest activeStereopsysTest.cpp)
target_link_libraries(activeStereopsysTest Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)

add_executable(passiveStereopsysTest passiveStereopsysTest.cpp)
target_link_libraries(passiveStereopsysTest  Core Experiment GLViz Communication ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(passiveStereopsysTestFactors passiveStereopsysTestFactors.cpp)
target_link_libraries(passiveStereopsysTestFactors  Core Experiment GLViz Communication ${GL_LIBS} ${Boost_LIBRARIES})
