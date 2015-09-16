set(GLVIZ_SRCS 
Arcball.cpp
#BezierCurve.cpp
BoundChecker.cpp
#CalibrationHelper.cpp
#Camera.cpp
#FramerateDisplayer.cpp
GL2DPainter.cpp
#GLInfo.cpp
GLLight.cpp
GLMaterial.cpp
GLSL.cpp
GLText.cpp
GLTexture.cpp
GLUtils.cpp
Grid.cpp
Icosphere.cpp
Imageloader.cpp
#MinJerkCurve.cpp
ObjLoader2.cpp
ObjLoader.cpp
#OpticFlowEstimator.cpp
StimulusDrawer.cpp
TinyObjLoader.cpp
VolumetricMeshIntersection.cpp
VolumetricSurfaceIntersection.cpp
VRCamera.cpp
)

set(GLVIZ_HDRS
Arcball.h
#BezierCurve.h
BoundChecker.h
#CalibrationHelper.h
#Camera.h
#FramerateDisplayer.h
GL2DPainter.h
#GLee.h
#GLInfo.h
GLLight.h
GLMaterial.h
GLSL.h
GLText.h
GLTexture.h
GLUtils.h
Grid.h
Icosphere.h
Imageloader.h
IncludeGL.h
#MinJerkCurve.h
ObjLoader2.h
ObjLoader.h
#OpticFlowEstimator.h
StimulusDrawer.h
TinyObjLoader.h
VolumetricMeshIntersection.h
VolumetricSurfaceIntersection.h
VRCamera.h
)

add_library(GLViz ${GLVIZ_SRCS} ${GLVIZ_HDRS})
target_link_libraries(GLViz Core Experiment ${GLEW_LIBRARY})

if(COMPILE_TESTS)
add_executable(testGLSL2		testGLSL2.cpp)
add_executable(testGLSL3		testGLSL3.cpp)
add_executable(testGLSL4		testGLSL4.cpp)
add_executable(testGLSLcubemapSkyBox testGLSLcubemapSkyBox.cpp)
add_executable(testGLSLgeometryshader testGLSLgeometryshader.cpp)
add_executable(testGLSLRayTracingShader testGLSLRayTracingShader.cpp)
add_executable(testGLSLRayCastingShader testGLSLRayCastingShader.cpp)
add_executable(testGLSL		testGLSL.cpp)
add_executable(testGLSLTexture3D testGLSLTexture3D.cpp)
add_executable(testGLTexture    testGLTexture.cpp)
add_executable(testTinyObjLoader testTinyObjLoader.cpp)
add_executable(testVolumetricHelicoid testVolumetricHelicoid.cpp)
add_executable(testVolumetricHelicoidFiltered testVolumetricHelicoidFiltered.cpp)
add_executable(testVolumetricSurfaceIntersection testVolumetricSurfaceIntersection.cpp)

target_link_libraries(testGLSL2	Core GLViz Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL3	Core GLViz Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL4	Core GLViz Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSL	Core GLViz Experiment  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLcubemapSkyBox Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLgeometryshader Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLRayTracingShader Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLRayCastingShader Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLSLTexture3D  Core GLViz ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testGLTexture Core GLViz ${Boost_LIBRARIES} ${GL_LIBS} )
target_link_libraries(testTinyObjLoader Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testVolumetricHelicoid Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testVolumetricHelicoidFiltered Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
target_link_libraries(testVolumetricSurfaceIntersection Core GLViz  ${Boost_LIBRARIES} ${GL_LIBS} ${GLEW_LIBRARY} )
endif(COMPILE_TESTS)
