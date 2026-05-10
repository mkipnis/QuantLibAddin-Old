cmake_minimum_required(VERSION 3.16)
project(embed_python_example)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG v2.11.1
)

FetchContent_MakeAvailable(pybind11)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

add_executable(options options.cpp)

target_link_libraries(options PRIVATE pybind11::embed)

# ---- Copy qlo/ directory to build dir ----
add_custom_command(TARGET options POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/qlo
            $<TARGET_FILE_DIR:options>/qlo
)

message(STATUS "Scripts will be copied to: $<TARGET_FILE_DIR:options>/qlo")



add_executable(proxy_pricer proxy_pricer.cpp)
target_link_libraries(proxy_pricer PRIVATE pybind11::embed)

# ---- Copy pricers/ directory to build dir ----
add_custom_command(TARGET proxy_pricer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/pricers
            $<TARGET_FILE_DIR:proxy_pricer>/pricers
)

message(STATUS "Scripts will be copied to: $<TARGET_FILE_DIR:proxy_pricer>/pricers")

