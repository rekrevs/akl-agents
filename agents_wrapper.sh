#!/bin/bash
# Wrapper script for agents to handle SIGTRAP on Apple Silicon M1

# Check if we're on Apple Silicon
if [[ $(uname -s) == "Darwin" && $(uname -m) == "arm64" ]]; then
    echo "Running on Apple Silicon M1, trapping SIGTRAP signal"
    # Use trap to catch SIGTRAP and ignore it
    trap '' TRAP
fi

# Run the original agents script with all arguments
/Users/sverker/gitrepos/akl-agents/agents "$@"
