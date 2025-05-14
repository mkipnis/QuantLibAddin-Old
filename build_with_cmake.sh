#!/bin/bash

set -euo pipefail

set -x

# Define directories
QL_REST_SOURCE_DIR="$(pwd)"
INSTALL_DIR="${1:-$HOME/}"
LD_LIBRARY_PATH="LD_LIBRARY_PATH"

mkdir -p "$INSTALL_DIR"
mkdir -p build
cd build

cmake .. -DQLA_INSTALL_PREFIX=${INSTALL_DIR}

cmake --build . --target install
