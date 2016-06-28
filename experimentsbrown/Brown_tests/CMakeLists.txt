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
target_link_libraries(testspring15-SMT  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-texture5 testspring15-texture5.cpp)
target_link_libraries(testspring15-texture5  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets3 testspring15-phidgets3.cpp)
target_link_libraries(testspring15-phidgets3  Core GLViz Experiment Communication BrownSupportLibs  ${PHIDGET_LIBRARIES} ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring15-phidgets4 testspring15-phidgets4.cpp)
target_link_libraries(testspring15-phidgets4  Core GLViz Experiment Communication BrownSupportLibs ${PHIDGET_LIBRARIES} ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

## SUMMER 15
add_executable(testsummer15-threeRods testsummer15-threeRods.cpp)
target_link_libraries(testsummer15-threeRods  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-threeRodsDiag testsummer15-threeRodsDiag.cpp)
target_link_libraries(testsummer15-threeRodsDiag  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motion_3rods_perceptual testsummer15-disparity_motion_3rods_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motion_3rods_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_3rods_perceptual testsummer15-disparity_motionMono_3rods_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motionMono_3rods_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual testsummer15-disparity_motionMono_planes_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motion_planes_perceptual testsummer15-disparity_motion_planes_perceptual.cpp)
target_link_libraries(testsummer15-disparity_motion_planes_perceptual  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual2 testsummer15-disparity_motionMono_planes_perceptual2.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual2  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-disparity_motionMono_planes_perceptual3 testsummer15-disparity_motionMono_planes_perceptual3.cpp)
target_link_libraries(testsummer15-disparity_motionMono_planes_perceptual3  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_ramp testsummer15-stereomotion_perc_ramp.cpp)
target_link_libraries(testsummer15-stereomotion_perc_ramp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_stair testsummer15-stereomotion_perc_stair.cpp)
target_link_libraries(testsummer15-stereomotion_perc_stair  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-stereomotion_perc_planes testsummer15-stereomotion_perc_planes.cpp)
target_link_libraries(testsummer15-stereomotion_perc_planes  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-SMT-pyramid testsummer15-SMT-pyramid.cpp)
target_link_libraries(testsummer15-SMT-pyramid  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-SMT-rampwings testsummer15-SMT-rampwings.cpp)
target_link_libraries(testsummer15-SMT-rampwings  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid testsummer15-3Dcues-paraboloid.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-translation testsummer15-3Dcues-paraboloid_motion-translation.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-translation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation testsummer15-3Dcues-paraboloid_motion-rotation.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(test_nico test_nico.cpp)
target_link_libraries(test_nico  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_thetaStaircase testsummer15-3Dcues-paraboloid_thetaStaircase.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation-grasp testsummer15-3Dcues-paraboloid_motion-rotation-grasp.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation-grasp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testReinsertion testReinsertion.cpp)
target_link_libraries(testReinsertion Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock.cpp)
target_link_libraries(testsummer15-3Dcues-paraboloid_motion-rotation-grasp-oneblock  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-cylinder_thetaStaircase testsummer15-3Dcues-cylinder_thetaStaircase.cpp)
target_link_libraries(testsummer15-3Dcues-cylinder_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testsummer15-3Dcues-cylinder_motion-rotation testsummer15-3Dcues-cylinder_motion-rotation.cpp)
target_link_libraries(testsummer15-3Dcues-cylinder_motion-rotation  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

## FALL 15
add_executable(testfall15-3Dcues-wedges_simultaneous testfall15-3Dcues-wedges_simultaneous.cpp)
target_link_libraries(testfall15-3Dcues-wedges_simultaneous  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_thetaStaircase testfall15-3Dcues-wedges_thetaStaircase.cpp)
target_link_libraries(testfall15-3Dcues-wedges_thetaStaircase  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_adjustment testfall15-3Dcues-wedges_adjustment.cpp)
target_link_libraries(testfall15-3Dcues-wedges_adjustment  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_adjustment-ss testfall15-3Dcues-wedges_adjustment-ss.cpp)
target_link_libraries(testfall15-3Dcues-wedges_adjustment-ss  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_grasp testfall15-3Dcues-wedges_grasp.cpp)
target_link_libraries(testfall15-3Dcues-wedges_grasp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-wedges_mse testfall15-3Dcues-wedges_mse.cpp)
target_link_libraries(testfall15-3Dcues-wedges_mse Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-paraboloid_adjustment testfall15-3Dcues-paraboloid_adjustment.cpp)
target_link_libraries(testfall15-3Dcues-paraboloid_adjustment  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-paraboloid_grasp testfall15-3Dcues-paraboloid_grasp.cpp)
target_link_libraries(testfall15-3Dcues-paraboloid_grasp  Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-paraboloid_mse testfall15-3Dcues-paraboloid_mse.cpp)
target_link_libraries(testfall15-3Dcues-paraboloid_mse Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_mse testfall15-3Dcues-parabCylinder_mse.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_mse Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_grasp testfall15-3Dcues-parabCylinder_grasp.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_grasp Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_mse-texture testfall15-3Dcues-parabCylinder_mse-texture.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_mse-texture Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_grasp-texture testfall15-3Dcues-parabCylinder_grasp-texture.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_grasp-texture Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_mse-size testfall15-3Dcues-parabCylinder_mse-size.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_mse-size Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-3Dcues-parabCylinder_grasp-size testfall15-3Dcues-parabCylinder_grasp-size.cpp)
target_link_libraries(testfall15-3Dcues-parabCylinder_grasp-size Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(texture_mapping_obj texture_mapping_obj.cpp)
target_link_libraries(texture_mapping_obj Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-realObj_wedges testfall15-realObj_wedges.cpp)
target_link_libraries(testfall15-realObj_wedges Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testfall15-realObj_wedges_adjustment testfall15-realObj_wedges_adjustment.cpp)
target_link_libraries(testfall15-realObj_wedges_adjustment Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

## SPRING 16
add_executable(testspring16-karl-muller2 testspring16-karl-muller2.cpp)
target_link_libraries(testspring16-karl-muller2 Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-perc_action_perc-grasp testspring16-perc_action_perc-grasp.cpp)
target_link_libraries(testspring16-perc_action_perc-grasp Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_2sizes_both testspring16-karl-muller_2sizes_both.cpp)
target_link_libraries(testspring16-karl-muller_2sizes_both Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_2sizes_inc testspring16-karl-muller_2sizes_inc.cpp)
target_link_libraries(testspring16-karl-muller_2sizes_inc Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_2sizes_dec testspring16-karl-muller_2sizes_dec.cpp)
target_link_libraries(testspring16-karl-muller_2sizes_dec Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_small_both testspring16-karl-muller_small_both.cpp)
target_link_libraries(testspring16-karl-muller_small_both Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_small_inc testspring16-karl-muller_small_inc.cpp)
target_link_libraries(testspring16-karl-muller_small_inc Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_small_dec testspring16-karl-muller_small_dec.cpp)
target_link_libraries(testspring16-karl-muller_small_dec Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_large_both testspring16-karl-muller_large_both.cpp)
target_link_libraries(testspring16-karl-muller_large_both Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_large_inc testspring16-karl-muller_large_inc.cpp)
target_link_libraries(testspring16-karl-muller_large_inc Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-karl-muller_large_dec testspring16-karl-muller_large_dec.cpp)
target_link_libraries(testspring16-karl-muller_large_dec Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-perc_action_perc-mse testspring16-perc_action_perc-mse.cpp)
target_link_libraries(testspring16-perc_action_perc-mse Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-room017A_optotrak_test testspring16-room017A_optotrak_test.cpp)
target_link_libraries(testspring16-room017A_optotrak_test Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-perc_action_perc-mse_fixdist testspring16-perc_action_perc-mse_fixdist.cpp)
target_link_libraries(testspring16-perc_action_perc-mse_fixdist Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-perc_action_perc-grasp_fixdist testspring16-perc_action_perc-grasp_fixdist.cpp)
target_link_libraries(testspring16-perc_action_perc-grasp_fixdist Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-room017B_optotrak_test testspring16-room017B_optotrak_test.cpp)
target_link_libraries(testspring16-room017B_optotrak_test Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-simpleMSE_017A testspring16-simpleMSE_017A.cpp)
target_link_libraries(testspring16-simpleMSE_017A Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-simpleMSE_017B testspring16-simpleMSE_017B.cpp)
target_link_libraries(testspring16-simpleMSE_017B Core GLViz Experiment Communication BrownSupportLibs  ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-realObj_wedges_MSEgrasp testspring16-realObj_wedges_MSEgrasp.cpp)
target_link_libraries(testspring16-realObj_wedges_MSEgrasp Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-phidgets_realObj_wedges_MSEgrasp testspring16-phidgets_realObj_wedges_MSEgrasp.cpp)
target_link_libraries(testspring16-phidgets_realObj_wedges_MSEgrasp Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-simplegrasp_017A testspring16-simplegrasp_017A.cpp)
target_link_libraries(testspring16-simplegrasp_017A Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-realObj_wedges testspring16-realObj_wedges.cpp)
target_link_libraries(testspring16-realObj_wedges Core GLViz Experiment Communication BrownSupportLibs BrownFtdi ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

add_executable(testspring16-simplegrasp_017B testspring16-simplegrasp_017B.cpp)
target_link_libraries(testspring16-simplegrasp_017B Core GLViz Experiment Communication BrownSupportLibs ${GL_LIBS} ${Boost_LIBRARIES} ${PHIDGETS_LIBRARIES})

# Add other experiments
add_subdirectory(ernst_banks_2002)
add_subdirectory(examples)

 