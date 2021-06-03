cmake_minimum_required (VERSION 3.1.0)
project(mos)

set(CMAKE_CXX_STANDARD 20)

# Testing
enable_testing()
add_subdirectory(externals/Catch2)
add_subdirectory(tests)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_USE_DWM_SWAP_INTERVAL ON CACHE BOOL "" FORCE)
add_subdirectory(externals/glfw)

# OpenAL soft
set(ALSOFT_DLOPEN OFF CACHE BOOL "" FORCE)
set(ALSOFT_WERROR OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_CONFIG OFF CACHE BOOL "" FORCE) 
set(ALSOFT_HRTF_DEFS OFF CACHE BOOL "" FORCE)
set(ALSOFT_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(externals/openal-soft)

# glad
add_subdirectory(externals/glad)

# glm
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(externals/glm)

# gli
add_subdirectory(externals/gli)

# stb
add_subdirectory(externals/stb)

# spdlog
add_subdirectory(externals/spdlog)

# unicode
set(UTF8_TESTS OFF CACHE BOOL "" FORCE)
set(UTF8_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(externals/utfcpp)

# mos
file(GLOB ROOT_SOURCE src/mos/*.cpp)
file(GLOB GFX_SOURCE src/mos/gfx/*.cpp src/mos/gfx/gl/*.cpp)
file(GLOB AUD_SOURCE src/mos/aud/*.cpp)
file(GLOB SIM_SOURCE src/mos/sim/*.cpp)
file(GLOB IO_SOURCE src/mos/io/*.cpp)
file(GLOB CORE_SOURCE src/mos/core/*.cpp)

file(GLOB ROOT_HEADER include/mos/*.hpp)
file(GLOB GFX_HEADER include/mos/gfx/*.hpp)
file(GLOB AUD_HEADER include/mos/aud/*.hpp)
file(GLOB SIM_HEADER include/mos/sim/*.hpp)
file(GLOB IO_HEADER include/mos/io/*.hpp)
file(GLOB CORE_HEADER include/mos/core/*.hpp)

file(GLOB VERTEX_SHADERS assets/shaders/*.vert)
file(GLOB FRAGMENT_SHADERS assets/shaders/*.frag)
file(GLOB GEOMETRY_SHADERS assets/shaders/*.geom)
file(GLOB BRDF_LUT assets/brdfLUT.png)

add_library(${PROJECT_NAME} STATIC
  ${ROOT_HEADER} ${GFX_HEADER} ${AUD_HEADER} ${SIM_HEADER} ${IO_HEADER} ${CORE_HEADER}
  ${ROOT_SOURCE} ${GFX_SOURCE} ${AUD_SOURCE} ${SIM_SOURCE} ${IO_SOURCE} ${CORE_SOURCE}
  ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

#Available effects are : MOS_EFFECT_VIGNETTE, MOS_EFFECT_VHS and MOS_EFFECT_DITHER
# NMINMAX fixes windows build
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  MOS_EFX
  AL_ALEXT_PROTOTYPES
  PUBLIC
  GLM_ENABLE_EXPERIMENTAL
  NOMINMAX
  STB_IMAGE_IMPLEMENTATION)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  include
  externals/glfw/include
  externals/openal-soft/include
  externals/glad/include
  externals/glm
  externals/nlohmann_json_release
  externals/stb
  externals/utfcpp/source
  externals/gli
  externals/spdlog/include
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  OpenAL
  ${GL_LIBRARY}
  ${PLATFORM_SPECIFIC_LIBRARIES}
  glfw
  ${GLFW_LIBRARIES}
  stb
  glad
  )

add_custom_target(copy_resources DEPENDS ${FRAGMENT_SHADERS} ${VERTEX_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

#Copy shader files to assets and brdfLUT to assets
add_custom_command(TARGET copy_resources POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)

# Copy OpenAL dll on Windows
if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:OpenAL>" "${CMAKE_BINARY_DIR}/")
endif()

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
