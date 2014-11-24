cmake_minimum_required (VERSION 2.8)

file(GLOB ROOT_SOURCE *.cpp)
file(GLOB ROOT_HEADER *.h)

add_library(lodepng STATIC ${ROOT_HEADER} ${ROOT_SOURCE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
