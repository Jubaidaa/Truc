#!/bin/bash

BINARY="./httpd"

test_fail() {
    test_name="$1"
    shift
    echo -n "Test $test_name : "
    $BINARY "$@" 2>/dev/null
    ret=$?
    
    if [ $ret -eq 2 ]; then
        echo -e "\033[0;32mPASS (Renvoie bien 2)\033[0m"
    else
        echo -e "\033[0;31mFAIL (Renvoie $ret au lieu de 2)\033[0m"
    fi
}

echo "--- Vérification des codes de retour ---"

# 1. Manque le Port (
test_fail "Missing PORT" --ip 127.0.0.1 --server-name test --root-dir . --pid-file test.pid

# 2. Manque l'IP
test_fail "Missing IP" --port 8080 --server-name test --root-dir . --pid-file test.pid

# 3. Manque le Server Name
test_fail "Missing SERVER_NAME" --port 8080 --ip 127.0.0.1 --root-dir . --pid-file test.pid

# 4. Manque le Root Dir
test_fail "Missing ROOT_DIR" --port 8080 --ip 127.0.0.1 --server-name test --pid-file test.pid

# 5. Manque le PID File
test_fail "Missing PID_FILE" --port 8080 --ip 127.0.0.1 --server-name test --root-dir .

# 6. Option inconnue (doit aussi renvoyer 2 sinon u ded)
test_fail "Unknown Option" --port 8080 --ip 127.0.0.1 --server-name test --root-dir . --pid-file test.pid --toto

