if(COMPILE_OPTOTRAK)
##### ATTIVO #####
add_executable(expTriesteRotatingCylinder expTriesteRotatingCylinder.cpp)
target_link_libraries(expTriesteRotatingCylinder Core Communication Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

endif(COMPILE_OPTOTRAK)
