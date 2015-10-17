cmake_minimum_required (VERSION 2.8)

file(GLOB ROOT_SOURCE *.c)
file(GLOB ROOT_HEADER *.h)

add_library(stb STATIC ${ROOT_HEADER} ${ROOT_SOURCE})
