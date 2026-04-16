#!/bin/bash

set -e

YELLOW='\033[33m'
GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

print_info() {
    echo -e "${YELLOW}[INFO] $1${NC}"
}

print_success() {
    echo -e "${GREEN}[SUCCESS] $1${NC}"
}

print_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

NUM_CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)

BUILD_TYPE="Release"
CLEAN_BUILD=false

for arg in "$@"
do
    case $arg in
        "debug")
            BUILD_TYPE="Debug"
            ;;
        "release")
            BUILD_TYPE="Release"
            ;;
        "clean")
            CLEAN_BUILD=true
            ;;
        *)
            print_error "Unknown argument: $arg. Use: debug, release, clean"
            exit 1
            ;;
    esac
done

build() {
    if [ "$CLEAN_BUILD" = true ]; then
        print_info "Cleaning build directory..."
        rm -rf out/build
    fi

    print_info "Configuring cmake ($BUILD_TYPE mode)..."
    cmake -B out/build -S . -DCMAKE_BUILD_TYPE=$BUILD_TYPE

    print_info "Compiling with make ($NUM_CORES cores)..."
    cmake --build out/build --parallel $NUM_CORES --config $BUILD_TYPE
    
    print_success "Build finished successfully!"
    print_success "Executable is at: ./out/bin/wfcrp"
}

build
