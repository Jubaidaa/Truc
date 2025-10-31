# 1. Create a simple test file
echo "int main() { return 0; }" > hello.c

# 2. Create a test Makefile
cat > test.mk << 'EOF'
CC=
gcc

hello:
hello.c
gcc hello.c -o hello

.PHONY:
clean

clean:
rm -f hello
EOF

# 3. Build once
./minimake -f test.mk hello
# Output: gcc hello.c -o hello

# 4. Build again immediately
./minimake -f test.mk hello
# Output: minimake: 'hello' is up to date.  ✅

# 5. Touch the source to make it newer
touch hello.c

# 6. Build again (should recompile)
./minimake -f test.mk hello
# Output: gcc hello.c -o hello

# 7. Build again
./minimake -f test.mk hello
# Output: minimake: 'hello' is up to date.  ✅
