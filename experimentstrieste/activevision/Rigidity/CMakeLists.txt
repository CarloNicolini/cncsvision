if(COMPILE_OPTOTRAK)
##### ATTIVO #####
add_executable(expRigidityTS expRigidityTS.cpp)
target_link_libraries(expRigidityTS Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expRigidityOcclusionTS expRigidityOcclusionTS.cpp)
target_link_libraries(expRigidityOcclusionTS Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expRigidityOcclusionTS_ST expRigidityOcclusionTS_ST.cpp)
target_link_libraries(expRigidityOcclusionTS_ST Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expRigidityOcclusionTS_ST_RND expRigidityOcclusionTS_ST_RND.cpp)
target_link_libraries(expRigidityOcclusionTS_ST_RND Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expRigidityOcclusionTS_ST_RND_train expRigidityOcclusionTS_ST_RND_train.cpp)
target_link_libraries(expRigidityOcclusionTS_ST_RND_train Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

#### IL SUO PASSIVO ######
add_executable(expPassiveRigidityTS expPassiveRigidityTS.cpp)
target_link_libraries(expPassiveRigidityTS Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS expPassiveRigidityOcclusionTS.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS_ST expPassiveRigidityOcclusionTS_ST.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS_ST Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS_ST_RND expPassiveRigidityOcclusionTS_ST_RND.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS_ST_RND Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS_ST_RND_NT expPassiveRigidityOcclusionTS_ST_RND_NT.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS_ST_RND_NT Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS_ST_RND_train expPassiveRigidityOcclusionTS_ST_RND_train.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS_ST_RND_train Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expPassiveRigidityOcclusionTS_ST_RND_auto expPassiveRigidityOcclusionTS_ST_RND_auto.cpp)
target_link_libraries(expPassiveRigidityOcclusionTS_ST_RND_auto Core Experiment GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

endif(COMPILE_OPTOTRAK)
