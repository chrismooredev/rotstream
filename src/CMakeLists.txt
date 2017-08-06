include_directories(${PROJECT_SRC_DIR}/src/headers/)

#set(SOURCE
#	${SOURCE}
#	${CMAKE_CURRENT_SOURCE_DIR}/rotstream.c
#	${CMAKE_CURRENT_SOURCE_DIR}/librotstream.c
#	${CMAKE_CURRENT_SOURCE_DIR}/liblogging.c
#
#	PARENT_SCOPE
#)

add_executable(RotStream rotstream.c librotstream.c liblogging.c)

