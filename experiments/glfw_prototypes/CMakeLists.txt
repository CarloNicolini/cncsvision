# GLFW framerate test without Optotrak
add_executable(glfw_framerate_test glfw_framerate_test.cpp)
target_link_libraries(glfw_framerate_test glfw ${GLFW_LIBRARIES} Core Experiment GLViz ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY})

# GLFW framerate test without Optotrak
if(COMPILE_OPTOTRAK)
add_executable(glfw_framerate_optotrak_test glfw_framerate_optotrak_test.cpp)
target_link_libraries(glfw_framerate_optotrak_test glfw ${GLFW_LIBRARIES} Core Experiment GLViz Optotrak ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY})
endif(COMPILE_OPTOTRAK)
