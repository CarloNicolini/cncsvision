if(COMPILE_OPTOTRAK AND WIN32)
add_executable(demoWarrenPassiveOnlyTranslation demoWarrenPassiveOnlyTranslation.cpp)
target_link_libraries(demoWarrenPassiveOnlyTranslation Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(demoWarrenPassiveOnlyTranslationOccluded demoWarrenPassiveOnlyTranslationOccluded.cpp)
target_link_libraries(demoWarrenPassiveOnlyTranslationOccluded Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(demoWarrenPassiveRotation demoWarrenPassiveRotation.cpp)
target_link_libraries(demoWarrenPassiveRotation Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK AND WIN32)
