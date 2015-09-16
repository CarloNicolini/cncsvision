if(COMPILE_OPTOTRAK AND WIN32)
add_executable(demoWarrenPassiveCdefNoProbe4dirs demoWarrenPassiveCdefNoProbe4dirs.cpp)
target_link_libraries(demoWarrenPassiveCdefNoProbe4dirs Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(demoWarrenPassiveCdefNoProbe4dirsKeyboard demoWarrenPassiveCdefNoProbe4dirsKeyboard.cpp)
target_link_libraries(demoWarrenPassiveCdefNoProbe4dirsKeyboard Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK AND WIN32)
