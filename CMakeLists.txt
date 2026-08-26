cmake_minimum_required(VERSION 3.25)

project(FireDemo VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
include(cmake/FireTargets.cmake)

add_subdirectory(src/sim)
add_subdirectory(src/render)
add_subdirectory(src/metrics)
add_subdirectory(src/engine)
add_subdirectory(src/app)
