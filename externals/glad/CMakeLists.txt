cmake_minimum_required (VERSION 2.8)

project(glad)

include_directories(include)

set(HEADERS
include/glad/glad.h
include/KHR/khrplatform.h)

set(SOURCES
src/glad.c)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
