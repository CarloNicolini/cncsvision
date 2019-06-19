add_executable(summer14-PonzoGraspRovereto summer14-PonzoGraspRovereto.cpp)
target_link_libraries(summer14-PonzoGraspRovereto Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(summer14-platoGraspRovereto summer14-platoGraspRovereto.cpp)
target_link_libraries(summer14-platoGraspRovereto Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(summer14-platoBlockGraspRovereto summer14-platoBlockGraspRovereto.cpp)
target_link_libraries(summer14-platoBlockGraspRovereto Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

