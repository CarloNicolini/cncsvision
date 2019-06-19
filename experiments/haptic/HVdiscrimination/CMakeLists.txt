#add_executable(HVDiscrimination HVDiscrimination.cpp)
add_executable(OneTwoPointDiscrimination OneTwoPointDiscrimination.cpp)

#target_link_libraries(HVDiscrimination  Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
target_link_libraries(OneTwoPointDiscrimination  Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})
