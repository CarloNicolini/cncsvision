cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(Optotrak)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ndlib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ndlib/lib")
set(CMAKE_CXX_FLAGS "-O0 -g3 -Wall -Wno-format -Wno-format-security -Wno-unused-value -DPLATFORM_LINUX")

add_library(ot_aux ot_aux.c)
add_library(certus_aux certus_aux.c sleep.c)
add_definitions("-DPLATFORM_LINUX")
add_executable(align align.c)
target_link_libraries(align oapi ot_aux certus_aux dl)

add_executable(certus_sample11 certus_sample11.c)
target_link_libraries(certus_sample11 oapi ot_aux certus_aux dl)

add_executable(certus_sample12 certus_sample12.c)
target_link_libraries(certus_sample12 oapi ot_aux certus_aux dl)

add_executable(certus_sample13 certus_sample13.c)
target_link_libraries(certus_sample13 oapi ot_aux certus_aux dl)

add_executable(certus_sample14 certus_sample14.c)
target_link_libraries(certus_sample14 oapi ot_aux certus_aux dl)

add_executable(certus_sample15 certus_sample15.c)
target_link_libraries(certus_sample15 oapi ot_aux certus_aux dl)

add_executable(certus_sample16 certus_sample16.c)
target_link_libraries(certus_sample16 oapi ot_aux certus_aux dl)

add_executable(certus_sample17 certus_sample17.c)
target_link_libraries(certus_sample17 oapi ot_aux certus_aux dl)

add_executable(certus_sample18 certus_sample18.c)
target_link_libraries(certus_sample18 oapi ot_aux certus_aux dl)

add_executable(certus_sample19 certus_sample19.c)
target_link_libraries(certus_sample19 oapi ot_aux certus_aux dl)

add_executable(certus_sample1 certus_sample1.c)
target_link_libraries(certus_sample1 oapi ot_aux certus_aux dl)

add_executable(certus_sample20 certus_sample20.c)
target_link_libraries(certus_sample20 oapi ot_aux certus_aux dl)

add_executable(certus_sample2 certus_sample2.c)
target_link_libraries(certus_sample2 oapi ot_aux certus_aux dl)

add_executable(certus_sample3 certus_sample3.c)
target_link_libraries(certus_sample3 oapi ot_aux certus_aux dl)

add_executable(certus_sample4 certus_sample4.c)
target_link_libraries(certus_sample4 oapi ot_aux certus_aux dl)

add_executable(certus_sample certus_sample.c)
target_link_libraries(certus_sample oapi ot_aux certus_aux dl)

add_executable(download download.c)
target_link_libraries(download oapi ot_aux certus_aux dl)

add_executable(optsetup optsetup.c)
target_link_libraries(optsetup oapi ot_aux certus_aux dl)


add_executable(register register.c)
target_link_libraries(register oapi ot_aux certus_aux dl)

add_executable(sample10 sample10.c)
target_link_libraries(sample10 oapi ot_aux certus_aux dl)

add_executable(sample11 sample11.c)
target_link_libraries(sample11 oapi ot_aux certus_aux dl)

add_executable(sample12 sample12.c)
target_link_libraries(sample12 oapi ot_aux certus_aux dl)

add_executable(sample13 sample13.c)
target_link_libraries(sample13 oapi ot_aux certus_aux dl)

add_executable(sample14 sample14.c)
target_link_libraries(sample14 oapi ot_aux certus_aux dl)

add_executable(sample15 sample15.c)
target_link_libraries(sample15 oapi ot_aux certus_aux dl)

add_executable(sample16 sample16.c)
target_link_libraries(sample16 oapi ot_aux certus_aux dl)

add_executable(sample17 sample17.c)
target_link_libraries(sample17 oapi ot_aux certus_aux dl)

add_executable(sample18 sample18.c)
target_link_libraries(sample18 oapi ot_aux certus_aux dl)

add_executable(sample19 sample19.c)
target_link_libraries(sample19 oapi ot_aux certus_aux dl)

add_executable(sample1 sample1.c)
target_link_libraries(sample1 oapi ot_aux certus_aux dl)

add_executable(sample20 sample20.c)
target_link_libraries(sample20 oapi ot_aux certus_aux dl)

add_executable(sample21 sample21.c)
target_link_libraries(sample21 oapi ot_aux certus_aux dl)

add_executable(sample22 sample22.c)
target_link_libraries(sample22 oapi ot_aux certus_aux dl)

add_executable(sample2 sample2.c)
target_link_libraries(sample2 oapi ot_aux certus_aux dl)

add_executable(sample3 sample3.c)
target_link_libraries(sample3 oapi ot_aux certus_aux dl)

add_executable(sample4 sample4.c)
target_link_libraries(sample4 oapi ot_aux certus_aux dl)

add_executable(sample5 sample5.c)
target_link_libraries(sample5 oapi ot_aux certus_aux dl)

add_executable(sample6 sample6.c)
target_link_libraries(sample6 oapi ot_aux certus_aux dl)

add_executable(sample7 sample7.c)
target_link_libraries(sample7 oapi ot_aux certus_aux dl)

add_executable(sample8 sample8.c)
target_link_libraries(sample8 oapi ot_aux certus_aux dl)

add_executable(sample9 sample9.c)
target_link_libraries(sample9 oapi ot_aux certus_aux dl)
