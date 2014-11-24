cmake_minimum_required (VERSION 2.8)
project (mo)

add_definitions(-DGLM_FORCE_RADIANS)

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

# GLEW
add_subdirectory(externals/glew-cmake)
include_directories(externals/glew-cmake/include)

#openal soft
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

# tiny_obj_loader
add_subdirectory(externals/tinyobjloader)
include_directories(externals/tinyobjloader)

# ogli
include_directories(externals/ogli)

target_link_libraries(Isometric libglew_static ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(Isometric lodepng)
target_link_libraries(Isometric tinyobjloader)
target_link_libraries(Isometric stb)
