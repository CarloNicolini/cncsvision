cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(cncsvision)

#add_library(FAGSO SHARED ${FAGSO_SRCS} ${FAGSO_HDRS})
set_source_files_properties(pybalancefactor.pyx PROPERTIES CYTHON_IS_CXX 1)
# put cython wrapper and the implementation of the classes used (NOT THE CXX!)
cython_add_module(pybalancefactor pybalancefactor.pyx)
target_link_libraries(pybalancefactor Experiment Core)
