cmake_minimum_required (VERSION 3.14)
project(mos)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

# Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
  GIT_TAG "v2.13.6"
)
FetchContent_MakeAvailable(Catch2)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.3.4"
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# OpenAL soft
FetchContent_Declare(
  openal-soft
  GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
  GIT_TAG "1.21.1"
)

FetchContent_GetProperties(openal-soft)
if(NOT openal-soft_POPULATED)
    FetchContent_Populate(openal-soft)

    set(ALSOFT_DLOPEN OFF CACHE BOOL "" FORCE)
    set(ALSOFT_WERROR OFF CACHE BOOL "" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(${openal-soft_SOURCE_DIR} ${openal-soft_BINARY_DIR})
endif()

# glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG "0.9.9.8"
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)

    set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

# gli
FetchContent_Declare(
  gli
  GIT_REPOSITORY "https://github.com/g-truc/gli.git"
)
FetchContent_MakeAvailable(gli)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.8.5"
)
FetchContent_MakeAvailable(spdlog)

# unicode
FetchContent_Declare(
  utfcpp
  GIT_REPOSITORY "https://github.com/nemtrif/utfcpp.git"
  GIT_TAG "v3.2.1"
)

FetchContent_GetProperties(utfcpp)
if(NOT utfcpp_POPULATED)
    FetchContent_Populate(utfcpp)

    set(UTF8_TESTS OFF CACHE BOOL "" FORCE)
    set(UTF8_SAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${utfcpp_SOURCE_DIR} ${utfcpp_BINARY_DIR})
endif()

# nlohmann-json
FetchContent_Declare(
  nlohmann-json
  GIT_REPOSITORY "https://github.com/azadkuh/nlohmann_json_release.git"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_MakeAvailable(nlohmann-json)

add_library(nlohmann-json INTERFACE)
target_include_directories(nlohmann-json INTERFACE ${nlohmann-json_SOURCE_DIR})

# glad
add_subdirectory(externals/glad)

# stb
add_subdirectory(externals/stb)

# Testing
enable_testing()
add_subdirectory(tests)

# mos
file(GLOB ROOT_SOURCE src/mos/*.cpp)
file(GLOB GFX_SOURCE src/mos/gfx/*.cpp src/mos/gfx/gl/*.cpp src/mos/gfx/gpu/*.cpp)
file(GLOB AUD_SOURCE src/mos/aud/*.cpp)
file(GLOB SIM_SOURCE src/mos/sim/*.cpp)
file(GLOB IO_SOURCE src/mos/io/*.cpp)
file(GLOB CORE_SOURCE src/mos/core/*.cpp)

file(GLOB ROOT_HEADER include/mos/*.hpp)
file(GLOB GFX_HEADER include/mos/gfx/*.hpp include/mos/gfx/gpu/*.hpp)
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
  externals/glad/include
  externals/stb
  ${utfcpp_SOURCE_DIR}/source
  ${openal-soft_SOURCE_DIR}/include
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
  nlohmann-json
  gli
  spdlog
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
