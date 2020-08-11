cmake_minimum_required (VERSION 3.1.0)
project(tests)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC mos Catch2::Catch2)

add_test(
  NAME ${PROJECT_NAME}
  COMMAND ${PROJECT_NAME})
