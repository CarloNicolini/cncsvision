set(OPTOTRAK_SRCS
Optotrak.cpp
Optotrak2.cpp)

set(OPTOTRAK_HDRS
Optotrak.h
Optotrak2.h)

if( COMPILE_OPTOTRAK )
    add_library(Optotrak STATIC ${OPTOTRAK_SRCS} ${OPTOTRAK_HDRS})
    target_link_libraries(Optotrak Core ${NDI_OAPI_LIBRARIES} ${CMAKE_DL_LIBS})
    if(COMPILE_TESTS)
        add_executable(testOptotrak01 testOptotrak01.cpp)
        target_link_libraries(testOptotrak01 Core Optotrak)

        add_executable(testOptotrak02 testOptotrak02.cpp)
        target_link_libraries(testOptotrak02 Core Optotrak)
        if (UNIX)
            add_executable(testOptotrak_linux testOptotrak_linux.cpp)
            target_link_libraries(testOptotrak_linux Core Optotrak)
        endif(UNIX)
    endif(COMPILE_TESTS)
endif(COMPILE_OPTOTRAK)

