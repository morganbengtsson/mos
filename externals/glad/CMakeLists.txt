cmake_minimum_required (VERSION 2.8)

project(glad)

include_directories(include)

set(HEADERS
include/glad/glad.h
include/KHR/khrplatform.h)

set(SOURCES
src/glad.c)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()

