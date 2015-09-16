####################### OPTOTRAK #######################
if(WIN32)
include_directories(${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Win32/ndlib/include)
find_library( NDI_OAPI_LIBRARIES
                  NAMES "oapi"
                  PATHS ${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Win32/ndlib/msvc/ )
message(STATUS "OPTOTRAK libraries found ${NDI_OAPI_LIBRARIES}")
endif(WIN32)

# This is for UNIX operating systems which must link a different oapi library
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-unused-value -DPLATFORM_LINUX")
    add_definitions("-DPLATFORM_LINUX")
    include_directories(${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Linux_x86_64/ndlib/include/)
    include_directories(${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Linux_x86_64/ndlib/lib/)
    link_directories(${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Linux_x86_64/ndlib/lib/)
    find_library( NDI_OAPI_LIBRARIES
                  NAMES "oapi"
                  PATHS ${CMAKE_SOURCE_DIR}/deps/drivers/NDIoapi/Linux_x86_64/ndlib/lib/ )
    message(STATUS "OPTOTRAK libraries found ${NDI_OAPI_LIBRARIES}")  
endif(UNIX)
