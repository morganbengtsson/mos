cmake_minimum_required (VERSION 3.1.0)
project(mos)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLEW_STATIC)

set(CMAKE_CXX_STANDARD 14)

#Copy HRT files for OpenAL
file(COPY externals/openal-soft/hrtf/default-48000.mhr DESTINATION ${CMAKE_BINARY_DIR})
file(COPY externals/openal-soft/hrtf/default-44100.mhr DESTINATION ${CMAKE_BINARY_DIR})

#Copy shader files to assets
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR}/assets)

#Copy brdf LUT etc
file(COPY assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets)

# GLEW
set(glew-cmake_BUILD_SHARED OFF)
add_subdirectory(externals/glew-cmake)
include_directories(externals/glew_cmake/include)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE STRING "" FORCE)
set(GLFW_USE_DWM_SWAP_INTERVAL ON)
add_subdirectory(externals/glfw)
include_directories(externals/glfw/include)

#openal soft
set(ALSOFT_EXAMPLES OFF CACHE STRING "" FORCE)
set(ALSOFT_UTILS OFF CACHE STRING "" FORCE)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_HRTF_DEFS OFF)
add_subdirectory(externals/openal-soft)
include_directories(externals/openal-soft/include)

# Filesystem
add_subdirectory(externals/filesystem)
include_directories(externals/filesystem)

# glm
include_directories(externals/glm)
add_subdirectory(externals/glm)

# stb
add_subdirectory(externals/stb)

# Json
include_directories(externals/json/src)
add_subdirectory(externals/json)

# mos
file(GLOB_RECURSE ROOT_SOURCE src/mos/*.cpp)
file(GLOB_RECURSE ROOT_HEADER include/mos/*.hpp)
file(GLOB_RECURSE EXP_HEADER include/mos/exp/*.hpp)
file(GLOB VERTEX_SHADERS shaders/*.vert)
file(GLOB FRAGMENT_SHADERS shaders/*.frag)
file(GLOB GEOMETRY_SHADERS shaders/*.geom)

include_directories(include)
add_library(mos STATIC ${ROOT_HEADER} ${EXP_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS})

target_link_libraries(mos OpenAL)
target_link_libraries(mos libglew_static ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(mos glfw ${GLFW_LIBRARIES})
target_link_libraries(mos stb)
target_include_directories(mos PUBLIC
externals/glew-cmake/include
externals/glfw/include
externals/openal-soft/include
externals/stb
externals/objload
externals/glm
externals/filesystem
externals/json/src)
