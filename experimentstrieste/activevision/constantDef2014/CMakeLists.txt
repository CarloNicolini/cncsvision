if(COMPILE_OPTOTRAK)
add_executable(expPassiveCdefNoProbe4dirs expPassiveCdefNoProbe4dirs.cpp)
target_link_libraries(expPassiveCdefNoProbe4dirs Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveCdefNoProbe4dirsKeyboard expWarrenPassiveCdefNoProbe4dirsKeyboard.cpp)
target_link_libraries(expPassiveCdefNoProbe4dirsKeyboard Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)
