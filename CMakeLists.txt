cmake_minimum_required(VERSION 3.18)
project(iccad2023PD)

if (APPLE)
    add_compile_definitions(PROJ_HOME_DIR="../../")
else ()
    add_compile_definitions(PROJ_HOME_DIR="/home/jrchang/project/iccad-2023PD/")
endif()

add_subdirectory(src)
set(CMAKE_BUILD_TYPE Debug)