add_executable(expCampagnoliShapeConstancyGrasp expCampagnoliShapeConstancyGrasp.cpp)
target_link_libraries(expCampagnoliShapeConstancyGrasp Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})


add_executable(expCampagnoliShapeConstancyAdjustment expCampagnoliShapeConstancyAdjustment.cpp)
target_link_libraries(expCampagnoliShapeConstancyAdjustment Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(spring13-staircasePointing2Rovereto spring13-staircasePointing2.cpp)
target_link_libraries(spring13-staircasePointing2Rovereto Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(testsummer16-multipleMaps testsummer16-multipleMaps.cpp)
target_link_libraries(testsummer16-multipleMaps Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(testsummer16-multipleMaps_test testsummer16-multipleMaps_test.cpp)
target_link_libraries(testsummer16-multipleMaps_test Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})
