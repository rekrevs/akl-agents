#!/bin/bash
# Build script for AKL modernization project

set -e  # Exit on error

# Default values
BUILD_TYPE="Debug"
BUILD_TESTS=ON
WITH_GMP=ON
USE_SYSTEM_GMP=OFF
ENABLE_ASAN=OFF
USE_MODERN_TAGGED_POINTERS=OFF
BUILD_DIR="build"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    --release)
      BUILD_TYPE="Release"
      shift
      ;;
    --debug)
      BUILD_TYPE="Debug"
      shift
      ;;
    --no-tests)
      BUILD_TESTS=OFF
      shift
      ;;
    --no-gmp)
      WITH_GMP=OFF
      shift
      ;;
    --system-gmp)
      USE_SYSTEM_GMP=ON
      shift
      ;;
    --asan)
      ENABLE_ASAN=ON
      shift
      ;;
    --modern-tagged-pointers)
      USE_MODERN_TAGGED_POINTERS=ON
      shift
      ;;
    --build-dir)
      BUILD_DIR="$2"
      shift 2
      ;;
    --clean)
      echo "Cleaning build directory..."
      rm -rf "$BUILD_DIR"
      shift
      ;;
    --help)
      echo "Usage: $0 [options]"
      echo "Options:"
      echo "  --release        Build in Release mode (default: Debug)"
      echo "  --debug          Build in Debug mode"
      echo "  --no-tests       Disable building tests"
      echo "  --no-gmp         Disable GMP support"
      echo "  --system-gmp     Use system GMP instead of bundled version"
      echo "  --asan           Enable AddressSanitizer"
      echo "  --modern-tagged-pointers  Use modernized tagged pointer implementation"
      echo "  --build-dir DIR  Set build directory (default: build)"
      echo "  --clean          Clean build directory before building"
      echo "  --help           Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Use --help for usage information"
      exit 1
      ;;
  esac
done

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DAKL_BUILD_TESTS="$BUILD_TESTS" \
  -DAKL_WITH_GMP="$WITH_GMP" \
  -DAKL_USE_MODERN_TAGGED_POINTERS="$USE_MODERN_TAGGED_POINTERS" \
  -DAKL_USE_SYSTEM_GMP="$USE_SYSTEM_GMP" \
  -DAKL_ENABLE_ASAN="$ENABLE_ASAN"

# Build
echo "Building..."
cmake --build . -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Run tests if enabled
if [ "$BUILD_TESTS" = "ON" ]; then
  echo "Running tests..."
  ctest --output-on-failure
fi

echo "Build completed successfully!"
