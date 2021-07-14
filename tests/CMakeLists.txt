cmake_minimum_required (VERSION 3.1.0)
project(tests)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp mesh_tests.cpp camera_tests.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC mos Catch2::Catch2)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
