cmake_minimum_required (VERSION 3.1.0)
project(mos)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLEW_STATIC)

set(CMAKE_CXX_STANDARD 17)

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

#glad
add_subdirectory(externals/glad)
include_directories(externals/glad/include)

# Filesystem
add_subdirectory(externals/filesystem)
include_directories(externals/filesystem)

# glm
include_directories(externals/glm)
add_subdirectory(externals/glm)

# stb
add_subdirectory(externals/stb)

# Json
include_directories(externals/nlohmann_json_release)

# mos
file(GLOB_RECURSE ROOT_SOURCE src/mos/*.cpp)
file(GLOB_RECURSE ROOT_HEADER include/mos/*.hpp)
file(GLOB VERTEX_SHADERS assets/shaders/*.vert)
file(GLOB FRAGMENT_SHADERS assets/shaders/*.frag)
file(GLOB GEOMETRY_SHADERS assets/shaders/*.geom)
file(GLOB BRDF_LUT assets/brdfLUT.png)

include_directories(include)
add_library(${PROJECT_NAME} STATIC ${ROOT_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

target_link_libraries(${PROJECT_NAME} OpenAL)
target_link_libraries(${PROJECT_NAME} ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} stb)
target_link_libraries(${PROJECT_NAME} glad)
target_include_directories(${PROJECT_NAME} PUBLIC
externals/glad/include
externals/glfw/include
externals/openal-soft/include
externals/stb
externals/glm
externals/filesystem
externals/nlohmann_json_release)

add_custom_target(copy_resources DEPENDS ${FRAGMENT_SHADERS} ${VERTEX_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

#Copy HRT files for OpenAL
add_custom_command(TARGET copy_resources POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/externals/openal-soft/hrtf ${CMAKE_BINARY_DIR})

#Copy shader files to assets and brdfLUT to assets
add_custom_command(TARGET copy_resources POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

# Copy shaders on each build
add_dependencies(${PROJECT_NAME} copy_resources)


file(GLOB ASSETS assets/*)

add_custom_target(copy_assets DEPENDS ${ASSETS})

#Copy assets
add_custom_command(TARGET copy_assets POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

# Copy shaders on each build
add_dependencies(${PROJECT_NAME} copy_assets)