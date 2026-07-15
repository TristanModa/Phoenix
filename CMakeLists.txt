cmake_minimum_required(VERSION 4.2)
project(Phoenix C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_SHARED_LIBRARY_PREFIX  "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(engine)
add_subdirectory(phoenix)
add_subdirectory(vendor)
