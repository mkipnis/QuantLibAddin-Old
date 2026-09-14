cmake_minimum_required(VERSION 3.20)

project(ObjectHandler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(WIN32)

    set(BUILD_SHARED_LIBS OFF)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_MULTITHREADED ON)

endif()

if(MSVC)
    add_compile_options(/W4 /EHsc /MP)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Boost REQUIRED COMPONENTS serialization filesystem)
find_package(gensrc REQUIRED)

set(OH_GENSRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gensrc")
set(GENSRC_TIMESTAMP "${CMAKE_CURRENT_BINARY_DIR}/gensrc.timestamp")

set(GENSRC_METADATA
    ${OH_GENSRC_DIR}/metadata/addins/doxygen.xml
    ${OH_GENSRC_DIR}/metadata/addins/excel.xml
    ${OH_GENSRC_DIR}/metadata/addins/loop.xml
    ${OH_GENSRC_DIR}/metadata/addins/valueobjects.xml
    ${OH_GENSRC_DIR}/metadata/functions/enumerations.xml
    ${OH_GENSRC_DIR}/metadata/functions/garbagecollection.xml
    ${OH_GENSRC_DIR}/metadata/functions/logging.xml
    ${OH_GENSRC_DIR}/metadata/functions/ohutils.xml
    ${OH_GENSRC_DIR}/metadata/functions/valueobjects.xml
    ${OH_GENSRC_DIR}/metadata/rules/doxygen.xml
    ${OH_GENSRC_DIR}/metadata/rules/excel.xml
    ${OH_GENSRC_DIR}/metadata/rules/loop.xml
    ${OH_GENSRC_DIR}/metadata/rules/valueobjects.xml
    ${OH_GENSRC_DIR}/metadata/types/types.xml
)

set(GENSRC_STUBS
    ${OH_GENSRC_DIR}/stubs/stub.copyright
    ${OH_GENSRC_DIR}/stubs/stub.excel.exports
    ${OH_GENSRC_DIR}/stubs/stub.excel.includes
)

set(GENSRC_INPUTS
    ${GENSRC_METADATA}
    ${GENSRC_STUBS}
    ${OH_GENSRC_DIR}/code/codedict.py
    ${OH_GENSRC_DIR}/config/config.xml
)

set(GENSRC_PYTHONPATH
    "${OH_GENSRC_DIR};${gensrc_CODE_DIR}"
)

add_custom_command(
    OUTPUT
        ${GENSRC_TIMESTAMP}

    COMMAND
        ${CMAKE_COMMAND} -E echo "Running gensrc.py"

    COMMAND
        ${CMAKE_COMMAND} -E env
            "PYTHONPATH=${GENSRC_PYTHONPATH};$ENV{PYTHONPATH}"
            ${Python3_EXECUTABLE}
            ${gensrc_SCRIPT}
            -xdvl
            --oh_dir=${CMAKE_CURRENT_SOURCE_DIR}

    COMMAND
        ${CMAKE_COMMAND} -E touch
            ${GENSRC_TIMESTAMP}

    DEPENDS
        ${gensrc_SCRIPT}
        ${GENSRC_INPUTS}

    WORKING_DIRECTORY
        ${OH_GENSRC_DIR}

    VERBATIM

    COMMENT
        "Running gensrc.py"
)

add_custom_target(gensrc_generate ALL DEPENDS ${GENSRC_TIMESTAMP})

file(GLOB_RECURSE OH_SOURCES CONFIGURE_DEPENDS oh/*.cpp)
file(GLOB_RECURSE OH_HEADERS CONFIGURE_DEPENDS oh/*.hpp oh/*.h)

add_library(oh STATIC ${OH_SOURCES} ${OH_HEADERS})

add_dependencies(oh gensrc_generate)

target_include_directories(
    oh
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
    oh
    PUBLIC
        Boost::serialization
        Boost::filesystem
)

set_target_properties(
    oh
    PROPERTIES
        OUTPUT_NAME "ObjectHandler"
)

install(
    TARGETS oh
    EXPORT ObjectHandlerTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY oh/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ObjectHandler/oh
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

install(
    EXPORT ObjectHandlerTargets
    FILE ObjectHandlerTargets.cmake
    NAMESPACE ObjectHandler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ObjectHandler
)

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

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ObjectHandlerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ObjectHandler
)

message(STATUS "Boost Include Dir = ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost Libraries Dir = ${Boost_LIBRARIES}")

message(STATUS "gensrc root     = ${gensrc_ROOT}")
message(STATUS "gensrc scripts  = ${gensrc_SCRIPT}")
message(STATUS "gensrc code dir = ${gensrc_CODE_DIR}")

message(STATUS "Python          = ${Python3_EXECUTABLE}")
message(STATUS "OH gensrc dir   = ${OH_GENSRC_DIR}")

message(STATUS "Functions XML:")
foreach(FILE ${GENSRC_METADATA})
    message(STATUS " ${FILE}")
endforeach()