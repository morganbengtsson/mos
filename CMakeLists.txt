cmake_minimum_required (VERSION 2.8)
project(mos)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLEW_STATIC)
add_definitions(-DAL_LIBTYPE_STATIC)

# No binaries in source dir
if(CMAKE_CURRENT_SOURCE_DIR EQUAL CMAKE_CURRENT_BINARY_DIR)
	message(FATAL_ERROR "Source and binary directories must be different")
endif(CMAKE_CURRENT_SOURCE_DIR EQUAL CMAKE_CURRENT_BINARY_DIR)

# C++ 11
if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

#Copy HRT files for OpenAL
file(COPY externals/openal-soft/hrtf/default-48000.mhr DESTINATION ${CMAKE_BINARY_DIR})
file(COPY externals/openal-soft/hrtf/default-44100.mhr DESTINATION ${CMAKE_BINARY_DIR})

#Copy shader files to assets
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR}/assets)

# GLEW
add_subdirectory(externals/glew-cmake)
include_directories(externals/glew-cmake/include)

#openal soft
set(ALSOFT_EXAMPLES OFF CACHE STRING "" FORCE)
set(ALSOFT_UTILS OFF CACHE STRING "" FORCE)
add_subdirectory(externals/openal-soft)
include_directories(externals/openal-soft/include)

#glm
include_directories(externals/glm)
add_subdirectory(externals/glm)

# rapidxml
include_directories(externals/rapidxml)

# objload
include_directories(externals/objload)

# lodepng
add_subdirectory(externals/lodepng)
include_directories(externals/lodepng)

# stb
add_subdirectory(externals/stb)
include_directories(externals/stb)

# mo
file(GLOB_RECURSE ROOT_SOURCE src/mos/*.cpp)
file(GLOB_RECURSE ROOT_HEADER include/mos/*.hpp)
file(GLOB_RECURSE EXP_HEADER include/mos/exp/*.hpp)
file(GLOB VERTEX_SHADERS shaders/*.vert)
file(GLOB FRAGMENT_SHADERS shaders/*.frag)
file(GLOB GEOMETRY_SHADERS shaders/*.geom)
add_subdirectory(src/mos)
include_directories(include/mos)
add_library(mos STATIC ${ROOT_HEADER} ${EXP_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS})

IF(WIN32)
    target_link_libraries(mos OpenAL32)
ELSE()
    target_link_libraries(mos openal)
ENDIF()
target_link_libraries(mos libglew_static ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(mos lodepng)
target_link_libraries(mos stb)
