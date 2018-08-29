cmake_minimum_required (VERSION 3.1.0)
project(mos)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DMOS_EFX)

set(CMAKE_CXX_STANDARD 17)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_USE_DWM_SWAP_INTERVAL ON CACHE BOOL "")
add_subdirectory(externals/glfw)

#openal soft
set(ALSOFT_DLOPEN OFF CACHE BOOL "")
set(ALSOFT_WERROR OFF CACHE BOOL "")
set(ALSOFT_UTILS OFF CACHE BOOL "")
set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "")
set(ALSOFT_TESTS OFF CACHE BOOL "")
set(ALSOFT_CONFIG OFF CACHE BOOL "") 
set(ALSOFT_HRTF_DEFS OFF CACHE BOOL "")
set(ALSOFT_AMBDEC_PRESETS OFF CACHE BOOL "")
set(ALSOFT_INSTALL OFF CACHE BOOL "")
add_subdirectory(externals/openal-soft)

#glad
add_subdirectory(externals/glad)

# Filesystem
add_subdirectory(externals/filesystem)

# stb
add_subdirectory(externals/stb)

# mos
file(GLOB_RECURSE ROOT_SOURCE src/mos/*.cpp)
file(GLOB_RECURSE ROOT_HEADER include/mos/*.hpp)
file(GLOB VERTEX_SHADERS assets/shaders/*.vert)
file(GLOB FRAGMENT_SHADERS assets/shaders/*.frag)
file(GLOB GEOMETRY_SHADERS assets/shaders/*.geom)
file(GLOB BRDF_LUT assets/brdfLUT.png)

add_library(${PROJECT_NAME} STATIC ${ROOT_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

target_link_libraries(${PROJECT_NAME} PUBLIC OpenAL)
target_link_libraries(${PROJECT_NAME} PUBLIC ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC stb)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_include_directories(${PROJECT_NAME}
PUBLIC
    include
    externals/glfw/include
    externals/openal-soft/include
    externals/glad/include
    externals/glm
    externals/nlohmann_json_release
    externals/stb
    externals/filesystem
)

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