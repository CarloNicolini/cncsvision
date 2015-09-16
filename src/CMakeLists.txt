# Include the base directories so that every module has it included
include_directories(eigen)
include_directories(core)
include_directories(experiment)
include_directories(communications)
include_directories(glviz)

add_subdirectory(core)
add_subdirectory(experiment)
add_subdirectory(communications)
add_subdirectory(glviz)
add_subdirectory(glsandbox)

if(COMPILE_STATISTIC)
include_directories(statistic)
endif()

if(COMPILE_OPTOTRAK)
include_directories(optotrak)
endif()

# Add the subdirectories to the project
if(COMPILE_STATISTIC)
add_subdirectory(statistic)
endif()

if(COMPILE_OPTOTRAK)
add_subdirectory(optotrak)
endif()

if(QT_SUPPORT)
add_subdirectory(QT)
endif(QT_SUPPORT)

if(PYTHON_SUPPORT)
add_subdirectory(pycncsvision)
endif(PYTHON_SUPPORT)