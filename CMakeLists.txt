cmake_minimum_required(VERSION 3.20)

project(QuantLibAddinBuild LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(ExternalProject)

# ---------------------------------------------------------------------------
# Installation prefix
# ---------------------------------------------------------------------------

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_BINARY_DIR}/install"
        CACHE PATH "Install prefix"
        FORCE
    )
endif()

# ---------------------------------------------------------------------------
# gensrc
# ---------------------------------------------------------------------------

add_subdirectory(gensrc)

# gensrc is part of this source tree
set(gensrc_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/gensrc/gensrc.py"
)

set(gensrc_CODE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/gensrc"
)

# ---------------------------------------------------------------------------
# ObjectHandler
# ---------------------------------------------------------------------------

set(OBJECTHANDLER_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/ObjectHandler"
)

add_subdirectory(ObjectHandler)

# ---------------------------------------------------------------------------
# QuantLib
# ---------------------------------------------------------------------------

find_package(QuantLib CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# QuantLibAddin
# ---------------------------------------------------------------------------

add_subdirectory(QuantLibAddin)
