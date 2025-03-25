#!/bin/bash
# Script to build the emulator with debugging enabled

echo "Building emulator with standard flags..."

# Clean and rebuild
echo "Cleaning previous build..."
make clean

echo "Building the emulator..."
make

echo "Build completed."
echo "To enable GMP debugging, run: export GMP_DEBUG=1"
echo "Then use the debug_agents.sh script to capture debug output."
