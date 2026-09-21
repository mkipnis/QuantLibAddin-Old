cmake_minimum_required(VERSION 3.20)

project(ObjectHandler VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_SHARED_LIBS "Build ObjectHandler as a shared library" OFF)

if(MSVC)
    add_compile_options(/W4 /EHsc /MP)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Boost REQUIRED COMPONENTS serialization filesystem)

set(OH_GENSRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gensrc")
set(GENSRC_TIMESTAMP "${CMAKE_CURRENT_BINARY_DIR}/gensrc.timestamp")

add_custom_command(
    OUTPUT ${GENSRC_TIMESTAMP}
    COMMAND ${CMAKE_COMMAND} -E echo "Generating ObjectHandler sources"
    COMMAND ${CMAKE_COMMAND} -E env
        "PYTHONPATH=${OH_GENSRC_DIR};${gensrc_CODE_DIR};$ENV{PYTHONPATH}"
        ${Python3_EXECUTABLE} ${gensrc_SCRIPT}
        -xdvl
        --oh_dir=${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${GENSRC_TIMESTAMP}
    WORKING_DIRECTORY ${OH_GENSRC_DIR}
    VERBATIM
)

add_custom_target(gensrc_generate ALL
    DEPENDS ${GENSRC_TIMESTAMP}
)

file(GLOB_RECURSE OH_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/oh/*.cpp"
)

file(GLOB_RECURSE OH_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/oh/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/oh/*.h"
)

add_library(oh ${OH_SOURCES} ${OH_HEADERS})
add_library(ObjectHandler::oh ALIAS oh)

add_dependencies(oh gensrc_generate)

target_compile_features(oh PUBLIC cxx_std_17)

target_include_directories(oh PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(oh PUBLIC
    Boost::serialization
    Boost::filesystem
)

if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OH_ARCH x64)
    else()
        set(OH_ARCH x86)
    endif()

    if(BUILD_SHARED_LIBS)
        set(OH_RUNTIME mt)
    else()
        set(OH_RUNTIME mt-s)
    endif()

    set(OH_OUTPUT_NAME "ObjectHandler-${OH_ARCH}-${OH_RUNTIME}")
else()
    set(OH_OUTPUT_NAME ObjectHandler)
endif()

set_target_properties(oh PROPERTIES
    OUTPUT_NAME "${OH_OUTPUT_NAME}"
)

# ------------------------------------------------------------------------------
# Install library
# ------------------------------------------------------------------------------

install(TARGETS oh
    EXPORT ObjectHandlerTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ------------------------------------------------------------------------------
# Install headers directly under include/
# ------------------------------------------------------------------------------

install(DIRECTORY oh/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oh
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# ------------------------------------------------------------------------------
# Install CMake targets
# ------------------------------------------------------------------------------

install(EXPORT ObjectHandlerTargets
    FILE ObjectHandlerTargets.cmake
    NAMESPACE ObjectHandler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ObjectHandler
)

# ------------------------------------------------------------------------------
# Package configuration
# ------------------------------------------------------------------------------

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ObjectHandlerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ObjectHandler
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ObjectHandler
)

message(STATUS "ObjectHandler ${PROJECT_VERSION}")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared library: ${BUILD_SHARED_LIBS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Boost:           ${Boost_VERSION}")
message(STATUS "  Python:          ${Python3_EXECUTABLE}")
message(STATUS "  gensrc:          ${gensrc_SCRIPT}")
