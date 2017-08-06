cmake_minimum_required(VERSION 3.5.1)
project (RotStream C)

set(USE_CMAKE_VERSION 1)
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(SOURCE src/)

configure_file(
	"${PROJECT_SOURCE_DIR}/src/headers/libversion.h"
	"${PROJECT_BINARY_DIR}/src/headers/libversion.h.out"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(RotStream PROPERTIES OUTPUT_NAME ${CMAKE_SOURCE_DIR}/rotlin)
#include_directories(${PROJECT_SRC_DIR}/headers/)

#add_executable(RotStream rotstream.c librotstream.c liblogging.c)
include_directories(${PROJECT_SRC_DIR}/src/headers/)

add_executable(RotStream src/rotstream.c src/librotstream.c src/liblogging.c)
#add_subdirectory(src)
#install (TARGETS ${PROJECT_SOURCE_DIR}/src/rotstream DESTINATION bin)

#add_executable(rotstream rotstream.c librotstream.c liblogging.c)