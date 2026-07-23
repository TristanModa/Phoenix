cmake_minimum_required(VERSION 4.2)
project(Phoenix C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (${LINUX})
	add_definitions(-DLINUX)
elseif (${WINDOWS})
	add_definitions(-DWINDOWS)
endif ()

add_subdirectory(engine)
add_subdirectory(phoenix)
add_subdirectory(vendor)
add_subdirectory(tests)