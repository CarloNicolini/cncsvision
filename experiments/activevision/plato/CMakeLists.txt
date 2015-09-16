if(WIN32)
add_executable(platoTest platoTest.cpp)
target_link_libraries(platoTest Core Communication)
endif(WIN32)
