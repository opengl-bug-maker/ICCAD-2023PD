cmake_minimum_required(VERSION 3.23)
project(2023PD)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE process "src/process/*.cpp")
file(GLOB_RECURSE static_data "src/static_data/*.cpp")
file(GLOB_RECURSE output_data "src/output_data/*.cpp")
file(GLOB_RECURSE quadratic "src/quadratic/*.cpp")
file(GLOB_RECURSE polygon "src/polygon/*.cpp")
file(GLOB_RECURSE plugin "src/plugin/*.cpp")
file(GLOB_RECURSE utilities "src/utilities/*.cpp")

include_directories("src")


add_executable(2023PD main.cpp ${process} ${static_data} ${output_data} ${quadratic} ${polygon} ${plugin} ${utilities})
