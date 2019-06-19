if(COMPILE_OPTOTRAK)
add_executable(expGiovanniRotationYaw expGiovanniRotationYaw.cpp)
add_executable(expGiovanniRotationYawOPTIMIZED2 expGiovanniRotationYawOPTIMIZED2.cpp)
add_executable(expGiovanniRotationYawPassive expGiovanniRotationYawPassive.cpp)

target_link_libraries(expGiovanniRotationYaw Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
target_link_libraries(expGiovanniRotationYawOPTIMIZED2 Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
target_link_libraries(expGiovanniRotationYawPassive Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)
