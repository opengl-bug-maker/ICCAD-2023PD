file(GLOB SRC_FILE *.cpp *.c)
file(GLOB HEADER_FILE *.hpp *.h)

set(SRC_FILE ${SRC_FILE})
set(HEADER_FILE ${HEADER_FILE} sequence_pair_t.h sequence_pair_t.cpp ILP_solver_t.cpp ILP_solver_t.h edge_t.cpp edge_t.h)


set(glpk_prefix ${CMAKE_SOURCE_DIR}/glpk-5.0)

include_directories(${glpk_prefix}/include)

add_library(process ${SRC_FILE} ${HEADER_FILE})

target_link_libraries(process ${glpk_prefix}/lib/libglpk.a)
target_include_directories(process PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)


target_compile_definitions(process INTERFACE PROCESS_DEBUG)