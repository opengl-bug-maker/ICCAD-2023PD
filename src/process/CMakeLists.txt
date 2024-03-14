file(GLOB_RECURSE SRC_FILE *.cpp *.c)
file(GLOB_RECURSE HEADER_FILE *.hpp *.h)

set(SRC_FILE ${SRC_FILE})
set(HEADER_FILE ${HEADER_FILE})

set(glpk_prefix ${CMAKE_SOURCE_DIR}/glpk-5.0)

include_directories(${glpk_prefix}/include)


add_library(process ${SRC_FILE} ${HEADER_FILE})

if (APPLE) 
    target_link_libraries(process ${glpk_prefix}/lib/libglpk_mac.a)
else ()
    target_link_libraries(process ${glpk_prefix}/lib/libglpk.a)
endif ()


target_include_directories(process PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)


target_compile_definitions(process INTERFACE PROCESS_DEBUG)