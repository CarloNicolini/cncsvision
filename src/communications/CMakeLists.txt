set(COMMUNICATION_SRCS
AsyncSerial.cpp
BufferedAsyncSerial.cpp
RoveretoMotorFunctions.cpp
SerialMotor.cpp
SerialStream.cpp
SocketTransmission.cpp
BrownMotorFunctions.cpp
TriesteMotorFunctions.cpp
VmxLinearMotor.cpp
VmxRotationMotor.cpp
)

set(COMMUNICATION_HDRS
AsyncSerial.h
BrownMotorFunctions.h
BufferedAsyncSerial.h
RoveretoMotorFunctions.h
SerialMotor.h
SerialStream.h
SocketTransmission.h
TriesteMotorFunctions.h
VmxLinearMotor.h
VmxRotationMotor.h
)

if (WIN32)
    set(COMMUNICATION_SRCS ${COMMUNICATION_SRCS} PlatoGlasses.cpp)
    set(COMMUNICATION_HDRS ${COMMUNICATION_HDRS} PlatoGlasses.h)
endif(WIN32)

if (WIN32 AND ALP_SUPPORT)
    link_libraries(ALPProjector ${ALP_LIBRARY})
    add_library(ALPProjector ALPProjector.cpp ALPProjector.h)
    target_link_libraries(ALPProjector ${ALP_LIBRARY} Core)
    message(STATUS "ALP libraries linked ${ALP_LIBRARY}")
endif(WIN32 AND ALP_SUPPORT)

if (WIN32 AND SMI_SUPPORT)
    #link_libraries(SMI ${SMI_LIBRARY})
    add_library(SMI SmartMotor.cpp SmartMotor.h)
    #target_link_libraries(SMI ${SMI_LIBRARY} Core)
    message(STATUS "SmartMotor libraries linked ${SMI_LIBRARY}")
endif(WIN32 AND SMI_SUPPORT)

# To include EgWin.h under Windows
if (WIN32 AND EYEGAZE_SUPPORT)
    include_directories(${CMAKE_SOURCE_DIR}/deps/drivers/Eyegaze/)
    set(COMMUNICATION_SRCS ${COMMUNICATION_SRCS}  EyeFollower.cpp)
    set(COMMUNICATION_HDRS ${COMMUNICATION_HDRS}  EyeFollower.h)
endif(WIN32 AND EYEGAZE_SUPPORT)

add_library(Communication ${COMMUNICATION_SRCS} ${COMMUNICATION_HDRS})
link_libraries(Communication ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

if(COMPILE_TESTS)
add_executable(serialCommunication serialCommunication.cpp)
add_executable(testAsyncSerial			testAsyncSerial.cpp)
add_executable(testCommunication01		testCommunication01.cpp)
add_executable(testRoveretoMotorFunctions	testRoveretoMotorFunctions.cpp)
add_executable(testTriesteMotorFunctions	testTriesteMotorFunctions.cpp)
add_executable(testSocketTransmissionServer	testSocketTransmissionServer.cpp)
add_executable(testSocketTransmissionClient testSocketTransmissionClient.cpp)

target_link_libraries(testAsyncSerial			Core Communication ${Boost_LIBRARIES})
target_link_libraries(testCommunication01		Core Communication ${Boost_LIBRARIES})
target_link_libraries(testRoveretoMotorFunctions	Core Communication ${Boost_LIBRARIES})
target_link_libraries(testTriesteMotorFunctions		Core Communication ${Boost_LIBRARIES})
target_link_libraries(testSocketTransmissionServer	Core Communication ${Boost_LIBRARIES})
target_link_libraries(testSocketTransmissionClient	Core Communication ${Boost_LIBRARIES})

if (WIN32 AND ALP_SUPPORT)
    # Include the base directories so that every module has it included
    # This is the ScrollingSample from ALP API
    add_executable(testALPBasic1 testALPBasic1.cpp)
    target_link_libraries(testALPBasic1 Core Experiment Communication GLViz ${GL_LIBS} ${Boost_LIBRARIES} ${ALP_LIBRARY})
    # This is the  from ALP API
    add_executable(testALPProjector testALPProjector.cpp)
    target_link_libraries(testALPProjector ALPProjector)
endif (WIN32 AND ALP_SUPPORT)

if (WIN32 AND SMI_SUPPORT)
    # Include the base directories so that every module has it included
    add_executable(testSMI testSMI.cpp)
    target_link_libraries(testSMI SMI)
endif (WIN32 AND SMI_SUPPORT)

endif(COMPILE_TESTS)
