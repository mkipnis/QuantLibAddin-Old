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

# ============================================================
# Copy shared Python requirements
# ============================================================

add_custom_target(copy_requirements ALL

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/requirements.in
            ${CMAKE_BINARY_DIR}/requirements.in

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/requirements.txt
            ${CMAKE_BINARY_DIR}/requirements.txt

    COMMENT "Copying Python requirements files"
)

# ============================================================
# options executable
# ============================================================

add_executable(options options.cpp)

target_link_libraries(options PRIVATE pybind11::embed)

add_custom_command(TARGET options POST_BUILD

    # Copy qlo/ directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/qlo
            $<TARGET_FILE_DIR:options>/qlo
)

# ============================================================
# proxy_pricer executable
# ============================================================

add_executable(proxy_pricer proxy_pricer.cpp)

target_link_libraries(proxy_pricer PRIVATE pybind11::embed)

add_custom_command(TARGET proxy_pricer POST_BUILD

    # Copy pricers/ directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/pricers
            $<TARGET_FILE_DIR:proxy_pricer>/pricers
)

message(STATUS "Python resources will be copied to build output directories")
