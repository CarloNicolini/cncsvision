set(EXPERIMENTS_SRCS
BalanceFactor.cpp
BoxNoiseStimulus.cpp
CubePointsStimulus.cpp
CylinderPointsStimulus.cpp
EllipsoidPointsStimulus.cpp
ExtrudedTrapezePointsStimulus.cpp
ParaboloidPointsStimulus.cpp
ParabolicCylinderPointsStimulus.cpp
ParametersLoader.cpp
ParStaircase.cpp
PointwiseStimulus.cpp
RadialPointStimulus.cpp
SpherePointsStimulus.cpp
Staircase.cpp
TrialGenerator.cpp
WedgePointsStimulus.cpp
)

set(EXPERIMENT_HDRS
BalanceFactor.h
BoxNoiseStimulus.h
CubePointsStimulus.h
CylinderPointsStimulus.h
EllipsoidPointsStimulus.h
ExtrudedTrapezePointsStimulus.h
ParaboloidPointsStimulus.h
ParabolicCylinderPointsStimulus.cpp
ParametersLoader.h
ParStaircase.h
PointwiseStimulus.h
RadialPointStimulus.h
SpherePointsStimulus.h
Staircase.h
Stimulus.h
TrialGenerator.h
Triangle3.h
WedgePointsStimulus.h
)

# These classes are deprecated because are old or never used and maintained
set(DEPRECATED_EXPERIMENT_HDRS
deprecated/BoxShadedStimulus.cpp
deprecated/CylinderShadedStimulus.cpp
deprecated/ShadedStimulus.cpp
deprecated/SphereShadedStimulus.cpp
deprecated/BalanceFactor2.cpp
)

set(DEPRECATED_EXPERIMENT_SRCS
deprecated/BoxShadedStimulus.h
deprecated/CylinderShadedStimulus.h
deprecated/ShadedStimulus.h
deprecated/SphereShadedStimulus.h
deprecated/BalanceFactor2.h
)

add_library(Experiment ${EXPERIMENTS_SRCS} ${EXPERIMENT_HDRS})
target_link_libraries(Experiment Core)

if(COMPILE_TESTS)
add_executable(testBalanceFactor testBalanceFactor.cpp)
add_executable(testExperiment01 testExperiment01.cpp)
add_executable(testExperiment02 testExperiment02.cpp)
add_executable(testHVDiscrimination testHVDiscrimination.cpp)
add_executable(testOneTwoPointsDiscrimination testOneTwoPointsDiscrimination.cpp)
add_executable(testParametersLoader testParametersLoader.cpp)
add_executable(testShuffleDeque testShuffleDeque.cpp)
add_executable(testStaircase2 testStaircase2.cpp)
add_executable(testStaircase3 testStaircase3.cpp)
add_executable(testStaircase4 testStaircase4.cpp)
add_executable(testStaircase testStaircase.cpp)
add_executable(testStimulus testStimulus.cpp)
add_executable(testTrialGenerator2 testTrialGenerator2.cpp)
add_executable(testTrialGenerator testTrialGenerator.cpp)
add_executable(testEulerCircuit testEulerCircuit.cpp)
add_executable(testBalanceFactorReinsertion testBalanceFactorReinsertion.cpp)

target_link_libraries(testBalanceFactor Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testExperiment01 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testExperiment02 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testHVDiscrimination Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testOneTwoPointsDiscrimination Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testParametersLoader Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testShuffleDeque Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testStaircase2 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testStaircase3 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testStaircase4 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testStaircase Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testStimulus Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testTrialGenerator2 Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testTrialGenerator Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testEulerCircuit Core Experiment ${Boost_LIBRARIES})
target_link_libraries(testBalanceFactorReinsertion Core Experiment ${Boost_LIBRARIES})
endif(COMPILE_TESTS)
