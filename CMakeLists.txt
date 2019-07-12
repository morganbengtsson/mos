cmake_minimum_required (VERSION 3.1.0)
project(mos)

set(CMAKE_CXX_STANDARD 17)

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

# Filesystem
add_subdirectory(externals/filesystem)

# stb
add_subdirectory(externals/stb)

# unicode
set(UTF8_TESTS OFF CACHE BOOL "" FORCE)
set(UTF8_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(externals/utfcpp)

# mos
file(GLOB_RECURSE ROOT_SOURCE src/mos/*.cpp)
file(GLOB_RECURSE ROOT_HEADER include/mos/*.hpp)
file(GLOB VERTEX_SHADERS assets/shaders/*.vert)
file(GLOB FRAGMENT_SHADERS assets/shaders/*.frag)
file(GLOB GEOMETRY_SHADERS assets/shaders/*.geom)
file(GLOB BRDF_LUT assets/brdfLUT.png)

add_library(${PROJECT_NAME} STATIC ${ROOT_HEADER} ${ROOT_SOURCE}
${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

target_compile_definitions(${PROJECT_NAME} PRIVATE MOS_EFX)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_ENABLE_EXPERIMENTAL)

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
        PRIVATE
        externals/utfcpp/source
        )

target_link_libraries(${PROJECT_NAME} PUBLIC OpenAL)
target_link_libraries(${PROJECT_NAME} PUBLIC ${GL_LIBRARY} ${PLATFORM_SPECIFIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC stb)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)

add_custom_target(copy_resources DEPENDS ${FRAGMENT_SHADERS} ${VERTEX_SHADERS} ${GEOMETRY_SHADERS} ${BRDF_LUT})

#Copy HRT files for OpenAL
add_custom_command(TARGET copy_resources POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/externals/openal-soft/hrtf ${CMAKE_BINARY_DIR})

#Copy shader files to assets and brdfLUT to assets
add_custom_command(TARGET copy_resources POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)
		
if(WIN32)	
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:OpenAL>" "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
		
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
