cmake_minimum_required(VERSION 3.20)

project(QuantLibAddin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(MSVC)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_options(/W4 /EHsc /MP)

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)

endif()

find_package(Boost REQUIRED COMPONENTS serialization filesystem)
find_package(QuantLib REQUIRED)
find_package(ObjectHandler REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(gensrc REQUIRED)

message(STATUS "Boost include dir    : ${Boost_INCLUDE_DIRS}")
message(STATUS "QuantLib found       : ${QuantLib_FOUND}")
message(STATUS "gensrc script        : ${gensrc_SCRIPT}")
message(STATUS "gensrc code dir      : ${gensrc_CODE_DIR}")
message(STATUS "OH gensrc dir        : ${OH_GENSRC_DIR}")
message(STATUS "Python3              : ${Python3_EXECUTABLE}")

set(ADDIN_GENSRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gensrc")

set(GENSRC_TIMESTAMP
    "${CMAKE_CURRENT_BINARY_DIR}/gensrc.timestamp"
)

file(
    GLOB_RECURSE
    GENSRC_METADATA
    CONFIGURE_DEPENDS
    "${ADDIN_GENSRC_DIR}/metadata/*.xml"
)

file(
    GLOB_RECURSE
    GENSRC_STUBS
    CONFIGURE_DEPENDS
    "${ADDIN_GENSRC_DIR}/stubs/stub.*"
)

set(
    GENSRC_INPUTS
    ${GENSRC_METADATA}
    ${GENSRC_STUBS}
    "${ADDIN_GENSRC_DIR}/code/codedict.py"
    "${ADDIN_GENSRC_DIR}/config/config.xml"
)

set(
    GENSRC_PYTHONPATH
    "${ADDIN_GENSRC_DIR};${gensrc_CODE_DIR}"
)

add_custom_command(
    OUTPUT
        ${GENSRC_TIMESTAMP}

    COMMAND
        ${CMAKE_COMMAND} -E echo "Running gensrc.py"

    COMMAND
        ${CMAKE_COMMAND} -E env
        "PYTHONPATH=${GENSRC_PYTHONPATH}"
        ${Python3_EXECUTABLE}
        ${gensrc_SCRIPT}
        -esvp
        --oh_dir=${CMAKE_CURRENT_SOURCE_DIR}

    COMMAND
        ${CMAKE_COMMAND} -E touch
        ${GENSRC_TIMESTAMP}

    DEPENDS
        ${gensrc_SCRIPT}
        ${GENSRC_INPUTS}

    WORKING_DIRECTORY
        ${ADDIN_GENSRC_DIR}

    VERBATIM
)

add_custom_target(
    gensrc_generate
    ALL
    DEPENDS
        ${GENSRC_TIMESTAMP}
)

file(
    GLOB_RECURSE
    QUANTLIBADDIN_SOURCES
    CONFIGURE_DEPENDS
    qlo/*.cpp
)

file(
    GLOB_RECURSE
    QUANTLIBADDIN_HEADERS
    CONFIGURE_DEPENDS
    qlo/*.hpp
    qlo/*.h
)

add_library(
    QuantLibAddin
    STATIC
    ${QUANTLIBADDIN_SOURCES}
    ${QUANTLIBADDIN_HEADERS}
)

add_dependencies(
    QuantLibAddin
    gensrc_generate
)

target_include_directories(
    QuantLibAddin
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/conventions>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/enumerations>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/loop>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/serialization>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/valueobjects>

        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${ObjectHandler_INCLUDE_DIRS}>

        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    QuantLibAddin
    PUBLIC
        QuantLib::QuantLib
        ObjectHandler::oh
        Boost::serialization
        Boost::filesystem
)

install(
    TARGETS QuantLibAddin
    EXPORT QuantLibAddinTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY qlo/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QuantLibAddin/qlo
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

install(
    EXPORT QuantLibAddinTargets
    FILE QuantLibAddinTargets.cmake
    NAMESPACE QuantLibAddin::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QuantLibAddin
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QuantLibAddinConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfig.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/QuantLibAddin
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/QuantLibAddin
)

message(STATUS "Source files : ${QUANTLIBADDIN_SOURCES}")
message(STATUS "Header files : ${QUANTLIBADDIN_HEADERS}")
