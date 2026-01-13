#!/bin/bash

set -e

# Build mode (debug or release)
MODE="${MODE:-debug}"
if [ "$MODE" != "debug" ] && [ "$MODE" != "release" ]; then
    echo "Invalid MODE: $MODE (use 'debug' or 'release')"
    exit 1
fi

# Build test executable (1 or 0)
TEST="${TEST:-0}"

# Detect OS
OS="${OS:-$(uname -s)}"
if [ "$OS" = "Darwin" ]; then
    PLATFORM="${PLATFORM:-macos}"
    SHARED_EXT="dylib"
elif [ "$OS" = "Linux" ]; then
    PLATFORM="${PLATFORM:-linux}"
    SHARED_EXT="so"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

# Detect architecture
ARCH="${ARCH:-$(uname -m)}"
if [ "$ARCH" = "aarch64" ]; then
    ARCH="arm64"
fi

BUILD_DIR="build/${MODE}"
TARGET="${ARCH}-${PLATFORM}"
LIB_NAME="libexhelper_${PLATFORM}_${ARCH}.${SHARED_EXT}"
TEST_NAME="exhelper_test_${PLATFORM}_${ARCH}"

# Compiler settings
CC="${CC:-clang}"
CXX="${CXX:-clang++}"

TARGET_FLAG="-target ${ARCH}-${PLATFORM}"
LDFLAGS=""

# Mode-specific flags
if [ "$MODE" = "debug" ]; then
    OPT_FLAGS="-O0 -g"
else
    OPT_FLAGS="-O2 -DNDEBUG"
    if [ "$OS" = "Darwin" ]; then
        # -dead_strip removes unused code, -x strips local symbols
        STRIP_FLAGS="-Wl,-dead_strip,-x"
    else
        # -x strips local symbols, --gc-sections removes unused sections
        STRIP_FLAGS="-Wl,--gc-sections,-x"
    fi
fi

# Platform-specific flags
if [ "$OS" = "Darwin" ]; then
    SHARED_FLAGS="-dynamiclib -install_name @rpath/${LIB_NAME}"
    RPATH_FLAG="-Wl,-rpath,@loader_path"
else
    SHARED_FLAGS="-shared"
    RPATH_FLAG="-Wl,-rpath,\$ORIGIN"
fi

# Create output directories
mkdir -p ${BUILD_DIR}/obj
mkdir -p ${BUILD_DIR}/bin

echo "Building for $TARGET ($MODE)..."

# Build shared library
echo "Compiling library..."
$CC $OPT_FLAGS $TARGET_FLAG -fPIC -fno-omit-frame-pointer -funwind-tables -Isrc/include -c src/main.c -o ${BUILD_DIR}/obj/main.c.o
$CC $OPT_FLAGS $TARGET_FLAG -fPIC -fno-omit-frame-pointer -funwind-tables -Isrc/include -c src/main.S -o ${BUILD_DIR}/obj/main.S.o
$CC $OPT_FLAGS $TARGET_FLAG -fPIC -fno-omit-frame-pointer -funwind-tables $SHARED_FLAGS ${STRIP_FLAGS:-} -o ${BUILD_DIR}/bin/$LIB_NAME ${BUILD_DIR}/obj/main.c.o ${BUILD_DIR}/obj/main.S.o $LDFLAGS

echo "Shared library built: ${BUILD_DIR}/bin/$LIB_NAME"

# Build test executable
if [ "$TEST" = "1" ]; then
    echo "Compiling test..."
    $CXX $OPT_FLAGS $TARGET_FLAG -fno-omit-frame-pointer -funwind-tables -fexceptions -std=c++17 -Isrc/include -c src/test.cpp -o ${BUILD_DIR}/obj/test.cpp.o
    $CC  $OPT_FLAGS $TARGET_FLAG -fno-omit-frame-pointer -funwind-tables -Isrc/include -c src/test.S -o ${BUILD_DIR}/obj/test.S.o
    $CXX $OPT_FLAGS $TARGET_FLAG -fno-omit-frame-pointer -funwind-tables ${STRIP_FLAGS:-} -o ${BUILD_DIR}/bin/$TEST_NAME ${BUILD_DIR}/obj/test.cpp.o ${BUILD_DIR}/obj/test.S.o -L${BUILD_DIR}/bin -lexhelper_${PLATFORM}_${ARCH} $RPATH_FLAG $LDFLAGS

    echo "Test binary built: ${BUILD_DIR}/bin/$TEST_NAME"

    # Run tests
    echo ""
    echo "Running tests..."
    set +e
    ./${BUILD_DIR}/bin/$TEST_NAME
    EXIT_CODE=$?
    set -e
    if [ $EXIT_CODE -eq 0 ]; then
        echo "✅ Tests passed"
    else
        echo "❌ Tests failed (exit code: $EXIT_CODE)"
        exit 1
    fi
fi
