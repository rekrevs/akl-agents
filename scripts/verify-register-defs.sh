#!/bin/bash
# Verify register definitions for x86-64
# Part of ATTEMPT-01, Phase 2, Step 2.2

echo "=== Register Definitions Verification ==="
echo ""

REGFILE="emulator/regdefs.h"
ERRORS=0

# Check for x86-64 section
echo "[1/7] Checking x86-64 HARDREGS section..."
if grep -q "x86_64\|amd64" "$REGFILE"; then
    echo "    ✓ x86-64 HARDREGS section present"
else
    echo "    ✗ x86-64 HARDREGS section missing"
    ERRORS=$((ERRORS + 1))
fi

# Check for critical registers used by engine (REGISTER1-6)
echo "[2/7] Checking critical register definitions..."
CRITICAL_REGS=(r15 r14 r13 r12 rbx rbp)
REG_NUM=1
for reg in "${CRITICAL_REGS[@]}"; do
    if grep -A 20 "x86_64\|amd64" "$REGFILE" | grep -q "\"$reg\""; then
        echo "    ✓ REGISTER$REG_NUM → $reg defined"
    else
        echo "    ✗ REGISTER$REG_NUM → $reg missing or not using correct syntax"
        ERRORS=$((ERRORS + 1))
    fi
    REG_NUM=$((REG_NUM + 1))
done

# Test if GCC accepts the register constraints
echo "[3/7] Testing GCC register constraint syntax..."
TEST_FILE="/tmp/test_register_$$.c"
TEST_OBJ="/tmp/test_register_$$.o"

cat > "$TEST_FILE" << 'EOF'
register int test_var asm("r15");
int main() { return 0; }
EOF

if gcc -c "$TEST_FILE" -o "$TEST_OBJ" 2>/dev/null; then
    echo "    ✓ GCC accepts register constraint syntax"
    rm -f "$TEST_FILE" "$TEST_OBJ"
else
    echo "    ⚠ GCC rejects register constraint (may need % prefix)"
    echo "    Try: asm(\"%r15\") instead of asm(\"r15\")"
    rm -f "$TEST_FILE"
fi

# Check HARDREGS wrapper
echo "[4/7] Checking HARDREGS conditional..."
if grep -B 5 "x86_64\|amd64" "$REGFILE" | grep -q "HARDREGS"; then
    echo "    ✓ x86-64 section wrapped in HARDREGS check"
else
    echo "    ⚠ x86-64 section may not be inside HARDREGS conditional"
    echo "    Verify it's wrapped like Alpha section"
fi

# Verify register count
echo "[5/7] Counting defined registers..."
REG_COUNT=$(grep -A 15 "x86_64\|amd64" "$REGFILE" | grep -c "REGISTER[0-9]")
if [ $REG_COUNT -ge 6 ]; then
    echo "    ✓ At least 6 registers defined (found $REG_COUNT)"
else
    echo "    ✗ Only $REG_COUNT registers defined (need at least 6)"
    ERRORS=$((ERRORS + 1))
fi

# Check for Alpha comparison
echo "[6/7] Comparing with Alpha configuration..."
ALPHA_COUNT=$(grep -A 15 "^#ifdef alpha" "$REGFILE" | grep -c "REGISTER[0-9]")
X86_COUNT=$(grep -A 15 "x86_64\|amd64" "$REGFILE" 2>/dev/null | grep -c "REGISTER[0-9]")
if [ $X86_COUNT -eq $ALPHA_COUNT ]; then
    echo "    ✓ Same register count as Alpha ($ALPHA_COUNT)"
elif [ $X86_COUNT -gt 0 ]; then
    echo "    ⚠ Different register count: x86-64=$X86_COUNT, Alpha=$ALPHA_COUNT"
else
    echo "    (x86-64 section not yet added - this is normal before Phase 3)"
fi

# Summary
echo "[7/7] Verification summary..."
echo ""
if [ $ERRORS -eq 0 ]; then
    echo "=== All checks passed ✓ ==="
    exit 0
else
    echo "=== $ERRORS check(s) failed ✗ ==="
    exit 1
fi
