#!/bin/bash
# Debug script to run agents under LLDB and capture a stack trace

# Enable GMP debugging
export GMP_DEBUG=1

# Create a log file for the debug output
DEBUG_LOG="gmp_debug_$(date +%Y%m%d_%H%M%S).log"
echo "Debug log will be saved to $DEBUG_LOG"

# Create a temporary file with LLDB commands
cat > /tmp/lldb_commands.txt << EOF
# Set up LLDB to stop on the trap
process handle SIGTRAP --stop true --pass false --notify true

# Set breakpoints on key GMP-related functions and initialization
breakpoint set --name make_bignum
breakpoint set --name new_bignum
breakpoint set --name bignum_gc
breakpoint set --name initialize_bignum
breakpoint set --name initialize_finite

# Set breakpoints on critical macros and functions
breakpoint set --file fd_akl.c --line 986
breakpoint set --file config.h --line 77

# Set breakpoints on GMP functions
breakpoint set --name mpz_init
breakpoint set --name mpz_init_set_si
breakpoint set --name mpz_clear
breakpoint set --name mpz_set_si

# Set breakpoint on SIGTRAP to catch the exact point of failure
process handle SIGTRAP --stop true --pass false --notify true

# Set breakpoints for GMP library functions
breakpoint set --name mpz_init
breakpoint set --name mpz_clear
breakpoint set --name mpz_set_si
breakpoint set --name mpz_get_si

# Set breakpoints for error handling
breakpoint set --name error_exit

# Run the program with arguments
run -b /Users/sverker/gitrepos/akl-agents/environment/boot.pam -b /Users/sverker/gitrepos/akl-agents/oldcompiler/comp.pam -b /Users/sverker/gitrepos/akl-agents/version.pam

# When it stops (likely on SIGTRAP), print detailed diagnostic information
bt all
thread backtrace all
frame variable --show-all-children
register read --all
image list

# Print information about the current frame
frame info

# Print the source code context
list

# Try to identify what GMP function might be causing the issue
image lookup --address $pc

# Try to continue and see if it crashes again
continue

# If it continues running, stop after a while
process interrupt
bt all
quit
EOF

# Run the emulator under LLDB with the command file
echo "Running agents under LLDB to capture stack trace..."
echo "compilef('assoc')." | lldb -s /tmp/lldb_commands.txt -- /Users/sverker/gitrepos/akl-agents/emulator/oagents > debug_output.txt 2>&1

# Extract and save GMP debug output to our log file
grep "GMP_DEBUG" debug_output.txt > "$DEBUG_LOG"

echo "Debug output saved to debug_output.txt"
echo "GMP debug output saved to $DEBUG_LOG"
echo "Showing last 30 lines of debug output:"
tail -n 30 debug_output.txt

# Extract the backtrace information for easier analysis
echo "\nExtracted backtrace information:"
grep -A 20 "\* thread" debug_output.txt | head -n 21

# Keep the debug output for analysis
echo "Full debug output is available in debug_output.txt"
