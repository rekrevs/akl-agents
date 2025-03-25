#!/bin/bash
# Debug script for running the emulator with a simple command
# Run this with lldb to debug the Trace/BPT trap issue

# Run the emulator with a simple command
echo "compilef('aggregate')." | ./emulator/oagents -b ./environment/boot.pam -b ./oldcompiler/comp.pam -b ./version.pam
