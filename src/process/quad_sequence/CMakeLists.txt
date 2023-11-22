file(GLOB SRC_FILE *.cpp *.c)
file(GLOB HEADER_FILE *.hpp *.h)

set(SRC_FILE ${SRC_FILE})
set(HEADER_FILE ${HEADER_FILE})

add_library(quad_sequence ${SRC_FILE} ${HEADER_FILE})

target_include_directories(quad_sequence PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)


target_compile_definitions(quad_sequence INTERFACE)
