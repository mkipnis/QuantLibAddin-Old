cmake_minimum_required(VERSION 3.15)
project(QuantLibAddinBuild LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "QuantLib Addin Install Dir: ${QLA_INSTALL_PREFIX}")

include(ExternalProject)

# Define source directories
set(SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set(GENSRC_SOURCE_DIR ${SOURCE_ROOT}/gensrc)
set(OBJECTHANDLER_SOURCE_DIR ${SOURCE_ROOT}/ObjectHandler)
set(QUANTLIBADDIN_SOURCE_DIR ${SOURCE_ROOT}/QuantLibAddin)

# Define build directories
set(GENSRC_BUILD_DIR ${CMAKE_BINARY_DIR}/gensrc-build)
set(OBJECTHANDLER_BUILD_DIR ${CMAKE_BINARY_DIR}/ObjectHandler-build)
set(QUANTLIBADDIN_BUILD_DIR ${CMAKE_BINARY_DIR}/QuantLibAddin-build)

find_package(Boost REQUIRED COMPONENTS filesystem regex)

set(COMMON_CXXFLAGS "-I${Boost_INCLUDE_DIRS} -I${QLA_INSTALL_PREFIX}/include -std=c++17")
set(COMMON_LDFLAGS "-L${Boost_LIBRARY_DIRS} -L${QLA_INSTALL_PREFIX}/lib")

# gensrc
ExternalProject_Add(gensrc
    SOURCE_DIR ${GENSRC_SOURCE_DIR}
    CONFIGURE_COMMAND ${GENSRC_SOURCE_DIR}/autogen.sh &&
                      ${GENSRC_SOURCE_DIR}/configure --prefix=${QLA_INSTALL_PREFIX}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
)

# ObjectHandler
ExternalProject_Add(ObjectHandler
    SOURCE_DIR ${OBJECTHANDLER_SOURCE_DIR}
    DEPENDS gensrc
    CONFIGURE_COMMAND 
		${OBJECTHANDLER_SOURCE_DIR}/autogen.sh && export CXXFLAGS=${COMMON_CXXFLAGS} && export LDFLAGS=${COMMON_LDFLAGS} &&
                      ${OBJECTHANDLER_SOURCE_DIR}/configure
                        --prefix=${QLA_INSTALL_PREFIX}
                        --exec-prefix=${QLA_INSTALL_PREFIX}
                        --with-gensrc=${GENSRC_SOURCE_DIR}
                        --without-log4cxx
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
)

# Build QuantLib using its CMake configuration
ExternalProject_Add(QuantLib
    GIT_REPOSITORY https://github.com/lballabio/QuantLib.git
    GIT_TAG master
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${QLA_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=Release
    INSTALL_DIR ${INSTALL_DIR}
)

# QuantLibAddin
ExternalProject_Add(QuantLibAddin
    SOURCE_DIR ${QUANTLIBADDIN_SOURCE_DIR}
    DEPENDS gensrc ObjectHandler
    CONFIGURE_COMMAND ${QUANTLIBADDIN_SOURCE_DIR}/autogen.sh &&
                      ${QUANTLIBADDIN_SOURCE_DIR}/configure
                        --prefix=${QLA_INSTALL_PREFIX}
                        --exec-prefix=${QLA_INSTALL_PREFIX}
                        --with-gensrc=${GENSRC_SOURCE_DIR}
                        --enable-addin-cpp
                        --with-oh=${OBJECTHANDLER_SOURCE_DIR}
                        CXXFLAGS=${COMMON_CXXFLAGS}
                        LDFLAGS=${COMMON_LDFLAGS}
    BUILD_COMMAND make -j 4
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
)
