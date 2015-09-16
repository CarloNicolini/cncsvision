add_executable(expChiaraGraspingThumbFeedback expChiaraGraspingThumbFeedback.cpp)
target_link_libraries(expChiaraGraspingThumbFeedback Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraGraspingThumbFeedback2 expChiaraGraspingThumbFeedback2.cpp)
target_link_libraries(expChiaraGraspingThumbFeedback2 Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraGraspingThumbFeedback3 expChiaraGraspingThumbFeedback3.cpp)
target_link_libraries(expChiaraGraspingThumbFeedback3 Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraNoObjectGrasp expChiaraNoObjectGrasp.cpp)
target_link_libraries(expChiaraNoObjectGrasp Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraVirtualReal expChiaraVirtualReal.cpp)
target_link_libraries(expChiaraVirtualReal Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraVirtual_Pilot expChiaraVirtual_Pilot.cpp)
target_link_libraries(expChiaraVirtual_Pilot Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(expChiaraTrapezoid expChiaraTrapezoid.cpp)
target_link_libraries(expChiaraTrapezoid Core Experiment Communication GLViz Optotrak ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(trapezoidCalibration trapezoidCalibration.cpp)
target_link_libraries(trapezoidCalibration Core GLViz Experiment ${Boost_LIBRARIES} ${GL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
