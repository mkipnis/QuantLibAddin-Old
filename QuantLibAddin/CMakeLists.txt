cmake_minimum_required(VERSION 3.20)
project(QuantLibAddin VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/W4 /EHsc /MP)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Boost REQUIRED COMPONENTS serialization filesystem)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(QuantLib CONFIG REQUIRED)


message(STATUS "Boost include dir : ${Boost_INCLUDE_DIRS}")
message(STATUS "QuantLib found    : ${QuantLib_FOUND}")
message(STATUS "gensrc script     : ${gensrc_SCRIPT}")
message(STATUS "gensrc code dir   : ${gensrc_CODE_DIR}")
message(STATUS "OH gensrc dir     : ${OH_GENSRC_DIR}")
message(STATUS "Python3           : ${Python3_EXECUTABLE}")

# ------------------------------------------------------------------------------
# gensrc
# ------------------------------------------------------------------------------

set(ADDIN_GENSRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gensrc")
set(GENSRC_TIMESTAMP "${CMAKE_CURRENT_BINARY_DIR}/gensrc.timestamp")

file(GLOB_RECURSE GENSRC_INPUTS CONFIGURE_DEPENDS
    "${ADDIN_GENSRC_DIR}/metadata/*.xml"
    "${ADDIN_GENSRC_DIR}/stubs/stub.*"
)

list(APPEND GENSRC_INPUTS
    "${ADDIN_GENSRC_DIR}/code/codedict.py"
    "${ADDIN_GENSRC_DIR}/config/config.xml"
)

add_custom_command(
    OUTPUT ${GENSRC_TIMESTAMP}
    COMMAND ${CMAKE_COMMAND} -E echo "Running gensrc.py"
    COMMAND ${CMAKE_COMMAND} -E env
        "PYTHONPATH=${ADDIN_GENSRC_DIR};${gensrc_CODE_DIR};$ENV{PYTHONPATH}"
        ${Python3_EXECUTABLE} ${gensrc_SCRIPT} -vsledxop
        --oh_dir=${OBJECTHANDLER_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${GENSRC_TIMESTAMP}
    DEPENDS ${gensrc_SCRIPT} ${GENSRC_INPUTS}
    WORKING_DIRECTORY ${ADDIN_GENSRC_DIR}
    VERBATIM
)

add_custom_target(qla_gensrc_generate ALL DEPENDS ${GENSRC_TIMESTAMP})
# ------------------------------------------------------------------------------
# Common includes
# ------------------------------------------------------------------------------

set(QLA_COMMON_INCLUDES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

function(add_qla_library TARGET)
    cmake_parse_arguments(ARGS "" "" "SOURCES;INCLUDES;LINKS" ${ARGN})

    add_library(${TARGET} STATIC ${ARGS_SOURCES})

    target_compile_features(${TARGET} PUBLIC cxx_std_17)

    target_include_directories(${TARGET}
        PUBLIC
            ${QLA_COMMON_INCLUDES}
            ${ARGS_INCLUDES}
    )

    target_link_libraries(${TARGET}
        PUBLIC
            ${ARGS_LINKS}
    )
endfunction()

# ------------------------------------------------------------------------------
# QuantLibAddin
# ------------------------------------------------------------------------------

file(GLOB_RECURSE QUANTLIBADDIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/qlo/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qlo/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qlo/*.h"
)

add_qla_library(QuantLibAddin
    SOURCES ${QUANTLIBADDIN_SOURCES}
    INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/conventions>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/enumerations>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/loop>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/serialization>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qlo/valueobjects>
    LINKS
        QuantLib::QuantLib
        ObjectHandler::oh
)

add_dependencies(QuantLibAddin qla_gensrc_generate)

# ------------------------------------------------------------------------------
# QuantLibAddinCpp
# ------------------------------------------------------------------------------

file(GLOB_RECURSE QUANTLIBADDIN_CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Addins/Cpp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Addins/Cpp/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Addins/Cpp/*.h"
)

add_qla_library(QuantLibAddinCpp
    SOURCES ${QUANTLIBADDIN_CPP_SOURCES}
    INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Addins/Cpp>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Addins/Cpp>
    LINKS
        QuantLibAddin
        QuantLib::QuantLib
        ObjectHandler::oh
)

add_dependencies(QuantLibAddinCpp qla_gensrc_generate)

# ------------------------------------------------------------------------------
# Install libraries
# ------------------------------------------------------------------------------

install(TARGETS
    QuantLibAddin
    QuantLibAddinCpp
    EXPORT QuantLibAddinTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ------------------------------------------------------------------------------
# Install headers directly under include/
# ------------------------------------------------------------------------------

install(DIRECTORY qlo/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qlo
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

install(DIRECTORY Addins/Cpp/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Addins/Cpp
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# ------------------------------------------------------------------------------
# Install CMake targets
# ------------------------------------------------------------------------------

install(EXPORT QuantLibAddinTargets
    FILE QuantLibAddinTargets.cmake
    NAMESPACE QuantLibAddin::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QuantLibAddin
)

# ------------------------------------------------------------------------------
# Package configuration
# ------------------------------------------------------------------------------

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

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/QuantLibAddinConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QuantLibAddin
)

add_subdirectory(Clients/Cpp)
