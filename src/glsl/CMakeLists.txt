set(GLVIZ_SRCS 
GLSL.cpp
)

set(GLVIZ_HDRS
GLSL.h
)

add_library(GLSL ${GLVIZ_SRCS} ${GLVIZ_HDRS})
target_link_libraries(GLSL Core Experiment)

if(COMPILE_TESTS)
add_executable(testGLSL		testGLSL.cpp)
add_executable(testGLSL2		testGLSL2.cpp)
add_executable(testGLSL3		testGLSL3.cpp)
add_executable(testGLSL4		testGLSL4.cpp)
add_executable(testGLSLcubemapSkyBox testGLSLcubemapSkyBox.cpp)
add_executable(testGLSLgeometryshader testGLSLgeometryshader.cpp)
add_executable(testGLSLRayTracingShader testGLSLRayTracingShader.cpp)
add_executable(testGLSLTexture3D testGLSLTexture3D.cpp)

target_link_libraries(testGLSL	Core GLViz GLSL Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL2	Core GLViz GLSL Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL3	Core GLViz GLSL Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL4	Core GLViz GLSL Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLcubemapSkyBox Core GLViz GLSL  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLgeometryshader Core GLViz GLSL  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLRayTracingShader Core GLViz GLSL  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLTexture3D  Core GLViz GLSL  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY})
endif(COMPILE_TESTS)