# Elliptic cylinder
add_executable(expTexturingCylinderAdjustment expTexturingCylinderAdjustment.cpp)
add_executable(expTexturingCylinderDiscrimination expTexturingCylinderDiscrimination.cpp)
add_executable(expTexturingCylinderDiscriminationRandomDots expTexturingCylinderDiscriminationRandomDots.cpp)

# Paraboloid
add_executable(expTexturingParaboloidAdjustment expTexturingParaboloidAdjustment.cpp)
add_executable(expTexturingParaboloidDiscrimination expTexturingParaboloidDiscrimination.cpp)

# Parabolic cylinder
add_executable(expTexturingParabolicCylinderAdjustment expTexturingParabolicCylinderAdjustment.cpp)
add_executable(expTexturingParabolicCylinderDiscrimination expTexturingParabolicCylinderDiscrimination.cpp)

# Elliptic cylinder
target_link_libraries(expTexturingCylinderAdjustment  Core Experiment Communication GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(expTexturingCylinderDiscrimination  Core Experiment Communication GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(expTexturingCylinderDiscriminationRandomDots Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS} )

# Paraboloid
target_link_libraries(expTexturingParaboloidAdjustment  Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS}  ${GLEW_LIBRARY} )
target_link_libraries(expTexturingParaboloidDiscrimination Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )

# Parabolic cylinder
target_link_libraries(expTexturingParabolicCylinderAdjustment  Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS}  ${GLEW_LIBRARY} )
target_link_libraries(expTexturingParabolicCylinderDiscrimination Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )

# Synopter
add_executable(expSynopter expSynopter.cpp)
target_link_libraries(expSynopter glfw ${GLFW_LIBRARIES} Core Experiment Communication GLViz ${Boost_LIBRARIES} ${GL_LIBS}  ${GLEW_LIBRARY} )

# Synopter
add_executable(expSynopterOptotrak expSynopterOptotrak.cpp)
target_link_libraries(expSynopterOptotrak glfw ${GLFW_LIBRARIES} Core Experiment Communication GLViz Optotrak ${Boost_LIBRARIES} ${GL_LIBS}  ${GLEW_LIBRARY} )
