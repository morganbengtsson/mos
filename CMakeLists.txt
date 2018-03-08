cmake_minimum_required (VERSION 3.1.0)
project(mos)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLEW_STATIC)

set(CMAKE_CXX_STANDARD 14)

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
file(GLOB VERTEX_SHADERS assets/shaders/*.vert)
file(GLOB FRAGMENT_SHADERS assets/shaders/*.frag)
file(GLOB GEOMETRY_SHADERS assets/shaders/*.geom)

include_directories(include)
add_library(${PROJECT_NAME} STATIC ${ROOT_HEADER} ${EXP_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS})

target_link_libraries(${PROJECT_NAME} OpenAL)
target_link_libraries(${PROJECT_NAME} libglew_static ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} stb)
target_include_directories(mos PUBLIC
externals/glew-cmake/include
externals/glfw/include
externals/openal-soft/include
externals/stb
externals/glm
externals/filesystem
externals/json/src)

add_custom_target(copy_shaders DEPENDS ${FRAGMENT_SHADERS} ${VERTEX_SHADERS} ${GEOMETRY_SHADERS})

#Copy HRT files for OpenAL
add_custom_command(TARGET copy_shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/externals/openal-soft/hrtf ${CMAKE_BINARY_DIR})

#Copy shader files to assets and brdfLUT to assets
add_custom_command(TARGET copy_shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

# Copy shaders on each build
add_dependencies(${PROJECT_NAME} copy_shaders)