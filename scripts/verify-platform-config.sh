#!/bin/bash
# Verify platform configuration is correct for x86-64
# Part of ATTEMPT-01, Phase 2, Step 2.2

echo "=== Platform Configuration Verification ==="
echo ""

ERRORS=0

# Check for x86-64 definition
echo "[1/4] Checking x86-64 platform detection..."
if grep -q "x86_64\|amd64" emulator/sysdeps.h; then
    echo "    ✓ x86-64 detection present"
else
    echo "    ✗ x86-64 detection missing"
    ERRORS=$((ERRORS + 1))
fi

# Check TADBITS
echo "[2/4] Checking TADBITS setting..."
if grep -A 3 "x86_64\|amd64" emulator/sysdeps.h | grep -q "TADBITS.*64"; then
    echo "    ✓ TADBITS = 64"
else
    echo "    ✗ TADBITS not set correctly"
    ERRORS=$((ERRORS + 1))
fi

# Check PTR_ORG (should use default 0, not override)
echo "[3/4] Checking PTR_ORG setting..."
if grep -A 5 "x86_64\|amd64" emulator/sysdeps.h | grep -q "PTR_ORG"; then
    echo "    ⚠ PTR_ORG explicitly set (should use default)"
    echo "    Verify it's set to 0"
else
    echo "    ✓ PTR_ORG not overridden (uses default 0)"
fi

# Check WORDALIGNMENT (should use default derived from TADBITS, not override)
echo "[4/4] Checking WORDALIGNMENT setting..."
if grep -A 5 "x86_64\|amd64" emulator/sysdeps.h | grep -q "WORDALIGNMENT"; then
    echo "    ⚠ WORDALIGNMENT explicitly set (should be derived)"
    echo "    Verify it's set to 8"
else
    echo "    ✓ WORDALIGNMENT not overridden (derived from TADBITS/8)"
fi

echo ""
if [ $ERRORS -eq 0 ]; then
    echo "=== All checks passed ✓ ==="
    exit 0
else
    echo "=== $ERRORS check(s) failed ✗ ==="
    exit 1
fi
