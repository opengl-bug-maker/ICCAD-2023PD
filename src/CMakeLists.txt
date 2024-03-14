set(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "-O3")

if (APPLE) 
       include_directories("/opt/homebrew/Cellar/glpk/5.0/include")
endif ()

add_subdirectory(bounding)
add_subdirectory(output_data)
add_subdirectory(plugin)
add_subdirectory(polygon)
add_subdirectory(process)
add_subdirectory(quadratic)
add_subdirectory(static_data)
add_subdirectory(utilities)

add_executable(iccad2023PD main.cpp)

#target_compile_definitions(output_data PUBLIC DEBUG_DEF)
#target_compile_definitions(plugin PUBLIC DEBUG_DEF)
#target_compile_definitions(polygon PUBLIC DEBUG_DEF)
#target_compile_definitions(process PUBLIC DEBUG_DEF)
#target_compile_definitions(quadratic PUBLIC DEBUG_DEF)
#target_compile_definitions(static_data PUBLIC DEBUG_DEF)
#target_compile_definitions(utilities PUBLIC DEBUG_DEF)

target_link_libraries(iccad2023PD
        bounding
        output_data
        process
        quadratic
        polygon
        plugin
        static_data
        mcnc
        utilities
        )

