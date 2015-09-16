## FALL 14
add_executable(testBrown1 testBrown1.cpp)
target_link_libraries(testBrown1  Core GLViz Experiment Optotrak Communication Communication ${GL_LIBS} ${Boost_LIBRARIES})
add_executable(testBrown2 testBrown2.cpp)
target_link_libraries(testBrown2  Core GLViz Experiment Optotrak Communication Communication ${GL_LIBS} ${Boost_LIBRARIES})
add_executable(testBrown3 testBrown3.cpp)
target_link_libraries(testBrown3  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})
add_executable(testBrown4 testBrown4.cpp)
target_link_libraries(testBrown4  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})
add_executable(testBrown5 testBrown5.cpp)
target_link_libraries(testBrown5  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})
add_executable(testBrown4a testBrown4a.cpp)
target_link_libraries(testBrown4a  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})
## SPRING 15
add_executable(testspring15-armlength-adj testspring15-armlength-adj.cpp)
target_link_libraries(testspring15-armlength-adj  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(testspring15-armlength-grasp testspring15-armlength-grasp.cpp)
target_link_libraries(testspring15-armlength-grasp  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(testspring15-armlength-graspWtraining testspring15-armlength-graspWtraining.cpp)
target_link_libraries(testspring15-armlength-graspWtraining  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets testspring15-phidgets.cpp)
target_link_libraries(testspring15-phidgets  Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets2 testspring15-phidgets2.cpp)
target_link_libraries(testspring15-phidgets2  Core GLViz Experiment Communication BrownSupportLibs  ${PHIDGET_LIBRARIES} ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-texture4 testspring15-texture4.cpp)
target_link_libraries(testspring15-texture4  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-SMT testspring15-SMT.cpp)
target_link_libraries(testspring15-SMT  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-texture5 testspring15-texture5.cpp)
target_link_libraries(testspring15-texture5  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets3 testspring15-phidgets3.cpp)
target_link_libraries(testspring15-phidgets3  Core GLViz Experiment Communication BrownSupportLibs  ${PHIDGET_LIBRARIES} ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets4 testspring15-phidgets4.cpp)
target_link_libraries(testspring15-phidgets4  Core GLViz Experiment Communication BrownSupportLibs ${PHIDGET_LIBRARIES} ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

## SUMMER 15
add_executable(testsummer15-threeRods testsummer15-threeRods.cpp)
target_link_libraries(testsummer15-threeRods  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-threeRodsDiag testsummer15-threeRodsDiag.cpp)
target_link_libraries(testsummer15-threeRodsDiag  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motion_3rods_perceptual testsummer15-disparity_motion_3rods_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motion_3rods_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_3rods_perceptual testsummer15-disparity_motionMono_3rods_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motionMono_3rods_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual testsummer15-disparity_motionMono_planes_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motion_planes_perceptual testsummer15-disparity_motion_planes_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motion_planes_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual2 testsummer15-disparity_motionMono_planes_perceptual2.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual2  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual3 testsummer15-disparity_motionMono_planes_perceptual3.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual3  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_ramp testsummer15-stereomotion_perc_ramp.cpp)
target_link_libraries(testsummer15-stereomotion_perc_ramp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_stair testsummer15-stereomotion_perc_stair.cpp)
target_link_libraries(testsummer15-stereomotion_perc_stair  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_planes testsummer15-stereomotion_perc_planes.cpp)
target_link_libraries(testsummer15-stereomotion_perc_planes  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-SMT-pyramid testsummer15-SMT-pyramid.cpp)
target_link_libraries(testsummer15-SMT-pyramid  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-SMT-rampwings testsummer15-SMT-rampwings.cpp)
target_link_libraries(testsummer15-SMT-rampwings  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid testsummer15-3Dcues-paraboloid.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-translation testsummer15-3Dcues-paraboloid_motion-translation.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-translation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation testsummer15-3Dcues-paraboloid_motion-rotation.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(test_nico test_nico.cpp)
target_link_libraries(test_nico  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_thetaStaircase testsummer15-3Dcues-paraboloid_thetaStaircase.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation-grasp testsummer15-3Dcues-paraboloid_motion-rotation-grasp.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation-grasp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testReinsertion testReinsertion.cpp)
target_link_libraries(testReinsertion Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-cylinder_thetaStaircase testsummer15-3Dcues-cylinder_thetaStaircase.cpp)
target_link_libraries(testsummer15-3Dcues-cylinder_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-cylinder_motion-rotation testsummer15-3Dcues-cylinder_motion-rotation.cpp)
target_link_libraries(testsummer15-3Dcues-cylinder_motion-rotation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_simultaneous testfall15-3Dcues-wedges_simultaneous.cpp)
target_link_libraries(testfall15-3Dcues-wedges_simultaneous  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_thetaStaircase testfall15-3Dcues-wedges_thetaStaircase.cpp)
target_link_libraries(testfall15-3Dcues-wedges_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${PHIDGET_LIBRARIES} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

# Add other experiments
add_subdirectory(ernst_banks_2002)
add_subdirectory(examples)

