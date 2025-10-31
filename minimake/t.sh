#!/bin/bash

echo "========================================="
echo "Testing 'Already Up to Date' Functionality"
echo "========================================="
echo ""

# Create source file
echo "Step 1: Creating hello.c..."
cat > hello.c << 'CFILE'
#include <stdio.h>
int main() {
    printf("Hello World!\n");
    return 0;
}
CFILE

# Create Makefile
echo "Step 2: Creating test Makefile..."
cat > test_uptodate.mk << 'MKFILE'
.PHONY: clean

hello: hello.c
	gcc hello.c -o hello
	@echo "✓ Compiled hello"

clean:
	rm -f hello
MKFILE

echo ""
echo "========================================="
echo "Test 1: First Build (should compile)"
echo "========================================="
./minimake -f test_uptodate.mk hello

echo ""
echo "========================================="
echo "Test 2: Immediate Rebuild (should be up to date)"
echo "========================================="
./minimake -f test_uptodate.mk hello

echo ""
echo "========================================="
echo "Test 3: Touch source file"
echo "========================================="
sleep 1
touch hello.c
echo "✓ Touched hello.c (made it newer)"

echo ""
echo "========================================="
echo "Test 4: Rebuild (should recompile)"
echo "========================================="
./minimake -f test_uptodate.mk hello

echo ""
echo "========================================="
echo "Test 5: Immediate Rebuild Again (should be up to date)"
echo "========================================="
./minimake -f test_uptodate.mk hello

echo ""
echo "========================================="
echo "Cleaning up..."
echo "========================================="
./minimake -f test_uptodate.mk clean
rm -f hello.c test_uptodate.mk

echo ""
echo "✅ Test complete!"