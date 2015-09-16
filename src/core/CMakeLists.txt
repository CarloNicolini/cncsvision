set(CORE_SRCS 
BinVoxReader.cpp
CoordinatesExtractor2.cpp
CoordinatesExtractor.cpp
DaisyFilter.cpp
EulerExtractor.cpp
Homography.cpp
Mathcommon.cpp
Point3D.cpp
RigidBody.cpp

Screen.cpp
Timer.cpp
Util.cpp
)

set(CORE_HDRS
../../experiments/calibrations/LatestCalibration.h
BinVoxReader.h
Circle.h
Convolution.h
CoordinatesExtractor2.h
CoordinatesExtractor.h
DaisyFilter.h
EulerExtractor.h
Homography.h
Marker.h
Mathcommon.h
MatrixStream.h
MersenneTwister.h
Point3D.h
RigidBody.h
RigidTransformation.h
Screen.h
Timer.h
Util.h
)

set(CORE_DEPRECATED_HDRS
ScreenCoordinatesExtractor.h
)

set(CORE_DEPRECATED_SRCS
ScreenCoordinatesExtractor.cpp
)

if(UNIX AND NOT APPLE)
    set(CORE_SRCS ${CORE_SRCS} beep.h)
endif(UNIX AND NOT APPLE)

add_library(Core ${CORE_SRCS} ${CORE_HDRS})
if (UNIX)
link_libraries(Core pthread ${Boost_LIBRARIES})
else (UNIX) #for Win32 we don't need pthread support linking
link_libraries(Core ${Boost_LIBRARIES})
endif(UNIX)

if(COMPILE_TESTS)
add_executable(testBinVoxReader testBinVoxReader.cpp)
add_executable(testBackAndForthStream testBackAndForthStream.cpp)
add_executable(testComplexConvolution testComplexConvolution.cpp)
add_executable(testConvolution testConvolution.cpp)
add_executable(testEulerAngles testEulerAngles.cpp)
add_executable(testGeometry01 testGeometry01.cpp)
add_executable(testHomography testHomography.cpp)
add_executable(testLexicalCast testLexicalCast.cpp)
add_executable(testLinearSystem testLinearSystem.cpp)
add_executable(testMatrixStream testMatrixStream.cpp)
add_executable(testMeshFunction testMeshFunction.cpp)
add_executable(testProjectiveTransform testProjectiveTransform.cpp)
add_executable(testRandomness testRandomness.cpp)
add_executable(testRandomShuffle testRandomShuffle.cpp)
add_executable(testRayCylinderIntersection testRayCylinderIntersection.cpp)
add_executable(testReflection testReflection.cpp)
add_executable(testRigidTransformation testRigidTransformation.cpp)
add_executable(testRotation testRotation.cpp)
add_executable(testStringToDoubleConversion testStringToDoubleConversion.cpp)
add_executable(testThread2 testThread2.cpp)
add_executable(testThread3 testThread3.cpp)
add_executable(testThread testThread.cpp)
add_executable(testTimer testTimer.cpp)
add_executable(testDeBrujin testDeBrujin.cpp)

target_link_libraries(testBinVoxReader Core ${Boost_LIBRARIES})
target_link_libraries(testBackAndForthStream Core ${Boost_LIBRARIES})
target_link_libraries(testComplexConvolution Core ${Boost_LIBRARIES})
target_link_libraries(testConvolution Core ${Boost_LIBRARIES})
target_link_libraries(testEulerAngles Core ${Boost_LIBRARIES})
target_link_libraries(testGeometry01 Core ${Boost_LIBRARIES})
target_link_libraries(testHomography Core  ${Boost_LIBRARIES})
target_link_libraries(testLexicalCast Core ${Boost_LIBRARIES})
target_link_libraries(testLinearSystem Core ${Boost_LIBRARIES})
target_link_libraries(testMatrixStream Core ${Boost_LIBRARIES})
target_link_libraries(testMeshFunction Core)
target_link_libraries(testProjectiveTransform Core ${Boost_LIBRARIES})
target_link_libraries(testRandomness Core ${Boost_LIBRARIES})
target_link_libraries(testRandomShuffle Core ${Boost_LIBRARIES})
target_link_libraries(testRayCylinderIntersection Core  ${Boost_LIBRARIES})
target_link_libraries(testReflection Core ${Boost_LIBRARIES})
target_link_libraries(testRigidTransformation Core ${Boost_LIBRARIES})
target_link_libraries(testRotation Core  ${Boost_LIBRARIES} )
target_link_libraries(testStringToDoubleConversion Core ${Boost_LIBRARIES})
target_link_libraries(testThread2 Core  ${Boost_LIBRARIES})
target_link_libraries(testThread3 Core  ${Boost_LIBRARIES})
target_link_libraries(testThread Core  ${Boost_LIBRARIES})
target_link_libraries(testTimer Core  ${Boost_LIBRARIES})
target_link_libraries(testDeBrujin Core ${Boost_LIBRARIES})

if(UNIX AND NOT APPLE)
add_executable(testBeepLinux testBeepLinux.cpp)
target_link_libraries(testBeepLinux Core)
endif(UNIX AND NOT APPLE)
endif(COMPILE_TESTS)
