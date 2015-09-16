set(STATISTIC_HDRS 
BayesNetwork.h
DBayesianModel.h
Density.h
DExample.h
Distribution.h
DMultivariateGaussian.h
Inferencer.h
Sampler.h
)

set(STATISTIC_SRCS
BayesNetwork.cpp
DBayesianModel.cpp
Density.cpp
DExample.cpp
DMultivariateGaussian.cpp
Inferencer.cpp
Sampler.cpp
)

add_library(Statistic ${STATISTIC_HDRS} ${STATISTIC_SRCS} )
target_link_libraries(Statistic Experiment)

if (COMPILE_TESTS)
add_executable(testStatistic01 testStatistic01.cpp)
add_executable(testStatistic02 testStatistic02.cpp)

target_link_libraries(testStatistic01 Core Experiment Statistic)
target_link_libraries(testStatistic02 Core Experiment Statistic)
endif(COMPILE_TESTS)