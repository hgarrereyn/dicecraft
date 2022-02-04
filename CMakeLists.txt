
cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 11)

project(DiceCraft)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.3.6)

add_subdirectory(glm)

add_subdirectory(src)
