#!/bin/bash
# Script to build the emulator using the modern GMP library from Homebrew

echo "Building emulator with modern GMP library from Homebrew..."

# Clean previous build
echo "Cleaning previous build..."
make clean

# Set environment variables to use Homebrew's GMP
export CFLAGS="-I/opt/homebrew/include -w -Wno-error -std=gnu89 -O2"
export LDFLAGS="-L/opt/homebrew/lib -lgmp -lm"

echo "Using Homebrew GMP include path: /opt/homebrew/include"
echo "Using Homebrew GMP library path: /opt/homebrew/lib"

# Build the emulator with the modern GMP
echo "Building the emulator with modern GMP..."
make

echo "Build completed."
echo "To enable GMP debugging, run: export GMP_DEBUG=1"
echo "Then use the debug_agents.sh script to capture debug output."
