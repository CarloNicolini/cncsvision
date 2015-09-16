add_executable(expKatjaGraspingLatitudeMapping expKatjaGraspingLatitudeMapping.cpp)
target_link_libraries(expKatjaGraspingLatitudeMapping  Core Experiment GLViz  Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${GLEW_LIBRARY})

add_executable(expKatjaEggCubeMapMultiMarker expKatjaEggCubeMapMultiMarker.cpp)
target_link_libraries(expKatjaEggCubeMapMultiMarker Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${GLEW_LIBRARY})

add_executable(expKatjaGraspingCubeMapping_rotation expKatjaGraspingCubeMapping_rotation.cpp)
target_link_libraries(expKatjaGraspingCubeMapping_rotation  Core Experiment GLViz  Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${GLEW_LIBRARY})

add_executable(expKatjaGraspingCubeMapping expKatjaGraspingCubeMapping.cpp)
target_link_libraries(expKatjaGraspingCubeMapping  Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${GLEW_LIBRARY})

add_executable(expKatjaEggCubeMap expKatjaEggCubeMap.cpp)
target_link_libraries(expKatjaEggCubeMap  Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES} ${GLEW_LIBRARY})

