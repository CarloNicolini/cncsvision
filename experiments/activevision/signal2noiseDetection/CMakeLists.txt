add_executable(signal2noise_base signal2noise_base.cpp)
target_link_libraries(signal2noise_base Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

add_executable(signal2noise_ortho_base signal2noise_ortho_base.cpp)
target_link_libraries(signal2noise_ortho_base Core Experiment GLViz ${GL_LIBS} ${Boost_LIBRARIES})

if (COMPILE_OPTOTRAK AND WIN32)
add_executable(expDprimeTMSoverMT expDprimeTMSoverMT.cpp)
target_link_libraries(expDprimeTMSoverMT Core Experiment GLViz Optotrak Communication ${GL_LIBS} ${Boost_LIBRARIES})
endif (COMPILE_OPTOTRAK AND WIN32)
