if(COMPILE_OPTOTRAK)
    add_executable(expMicroHeadMovementsActive_TS expMicroHeadMovementsActive_TS.cpp)
    target_link_libraries(expMicroHeadMovementsActive_TS Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
    add_executable(expMicroHeadMovementsPassiveStandard_TS expMicroHeadMovementsPassiveStandard_TS.cpp)
    target_link_libraries(expMicroHeadMovementsPassiveStandard_TS Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
endif(COMPILE_OPTOTRAK)
