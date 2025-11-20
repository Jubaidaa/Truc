#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
SERVER_BIN="./httpd"
SERVER_PORT=8080
SERVER_IP="127.0.0.1"
TEST_DIR="tests/www"
LOG_FILE="tests/server_test.log"
PID_FILE="tests/httpd.pid"
SERVER_URL="http://${SERVER_IP}:${SERVER_PORT}"

# Counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

# Functions
print_header() {
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
}

print_test() {
    echo -e "${YELLOW}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

print_failure() {
    echo -e "${RED}[✗]${NC} $1"
    TESTS_FAILED=$((TESTS_FAILED + 1))
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

# Setup test environment
setup_test_env() {
    print_header "SETUP TEST ENVIRONMENT"
    
    # Create test directory
    mkdir -p "$TEST_DIR"
    
    # Create test files
    echo "<h1>Hello World!</h1>" > "$TEST_DIR/index.html"
    echo "<html><body><h1>Test Page</h1></body></html>" > "$TEST_DIR/test.html"
    echo "This is a text file" > "$TEST_DIR/test.txt"
    
    # Create a simple CSS file
    echo "body { background: #fff; }" > "$TEST_DIR/style.css"
    
    # Create a simple JS file
    echo "console.log('test');" > "$TEST_DIR/script.js"
    
    print_success "Test directory created: $TEST_DIR"
    print_success "Test files created"
}

# Start server
start_server() {
    print_header "STARTING SERVER"
    
    # Kill any existing server on this port
    lsof -ti:$SERVER_PORT | xargs kill -9 2>/dev/null
    
    # Start server in background
    $SERVER_BIN --port $SERVER_PORT --ip $SERVER_IP --root-dir "$TEST_DIR" \
                --log-file "$LOG_FILE" --log true > /dev/null 2>&1 &
    
    SERVER_PID=$!
    echo $SERVER_PID > "$PID_FILE"
    
    # Wait for server to start
    sleep 1
    
    # Check if server is running
    if kill -0 $SERVER_PID 2>/dev/null; then
        print_success "Server started (PID: $SERVER_PID)"
        return 0
    else
        print_failure "Failed to start server"
        return 1
    fi
}

# Stop server
stop_server() {
    print_header "STOPPING SERVER"
    
    if [ -f "$PID_FILE" ]; then
        SERVER_PID=$(cat "$PID_FILE")
        if kill -0 $SERVER_PID 2>/dev/null; then
            kill $SERVER_PID
            wait $SERVER_PID 2>/dev/null
            print_success "Server stopped (PID: $SERVER_PID)"
        fi
        rm -f "$PID_FILE"
    fi
    
    # Make sure port is free
    lsof -ti:$SERVER_PORT | xargs kill -9 2>/dev/null
}

# Test functions
test_get_index() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "GET / returns 200 OK"
    
    HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/)
    
    if [ "$HTTP_CODE" = "200" ]; then
        print_success "GET / returned 200 OK"
    else
        print_failure "GET / returned $HTTP_CODE (expected 200)"
    fi
}

test_get_file() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "GET /test.html returns 200 OK"
    
    HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/test.html)
    
    if [ "$HTTP_CODE" = "200" ]; then
        print_success "GET /test.html returned 200 OK"
    else
        print_failure "GET /test.html returned $HTTP_CODE (expected 200)"
    fi
}

test_head_request() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "HEAD / returns 200 OK without body"
    
    RESPONSE=$(curl -I -s $SERVER_URL/)
    HTTP_CODE=$(echo "$RESPONSE" | head -1 | awk '{print $2}')
    
    if [ "$HTTP_CODE" = "200" ]; then
        print_success "HEAD / returned 200 OK"
    else
        print_failure "HEAD / returned $HTTP_CODE (expected 200)"
    fi
}

test_404_not_found() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "GET /notfound.html returns 404"
    
    HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL/notfound.html)
    
    if [ "$HTTP_CODE" = "404" ]; then
        print_success "GET /notfound.html returned 404 Not Found"
    else
        print_failure "GET /notfound.html returned $HTTP_CODE (expected 404)"
    fi
}

test_405_method_not_allowed() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "POST / returns 405 Method Not Allowed"
    
    HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" -X POST $SERVER_URL/)
    
    if [ "$HTTP_CODE" = "405" ]; then
        print_success "POST / returned 405 Method Not Allowed"
    else
        print_failure "POST / returned $HTTP_CODE (expected 405)"
    fi
}

test_connection_close() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "Response contains 'Connection: close'"
    
    RESPONSE=$(curl -i -s $SERVER_URL/)
    
    if echo "$RESPONSE" | grep -qi "Connection: close"; then
        print_success "Connection: close header present"
    else
        print_failure "Connection: close header missing"
    fi
}

test_date_header() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "Response contains Date header"
    
    RESPONSE=$(curl -i -s $SERVER_URL/)
    
    if echo "$RESPONSE" | grep -qi "Date:"; then
        print_success "Date header present"
    else
        print_failure "Date header missing"
    fi
}

test_content_length() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "Response contains Content-Length header"
    
    RESPONSE=$(curl -i -s $SERVER_URL/)
    
    if echo "$RESPONSE" | grep -qi "Content-Length:"; then
        print_success "Content-Length header present"
    else
        print_failure "Content-Length header missing"
    fi
}

test_content_type_html() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "HTML file has correct Content-Type"
    
    RESPONSE=$(curl -i -s $SERVER_URL/test.html)
    
    if echo "$RESPONSE" | grep -qi "Content-Type: text/html"; then
        print_success "Content-Type: text/html for .html file"
    else
        print_failure "Wrong Content-Type for .html file"
    fi
}

test_content_type_txt() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "TXT file has correct Content-Type"
    
    RESPONSE=$(curl -i -s $SERVER_URL/test.txt)
    
    if echo "$RESPONSE" | grep -qi "Content-Type: text/plain"; then
        print_success "Content-Type: text/plain for .txt file"
    else
        print_failure "Wrong Content-Type for .txt file"
    fi
}

test_content_type_css() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "CSS file has correct Content-Type"
    
    RESPONSE=$(curl -i -s $SERVER_URL/style.css)
    
    if echo "$RESPONSE" | grep -qi "Content-Type: text/css"; then
        print_success "Content-Type: text/css for .css file"
    else
        print_failure "Wrong Content-Type for .css file"
    fi
}

test_content_type_js() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "JS file has correct Content-Type"
    
    RESPONSE=$(curl -i -s $SERVER_URL/script.js)
    
    if echo "$RESPONSE" | grep -qi "Content-Type: application/javascript"; then
        print_success "Content-Type: application/javascript for .js file"
    else
        print_failure "Wrong Content-Type for .js file"
    fi
}

test_malformed_request() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "Malformed request returns 400 Bad Request"
    
    HTTP_CODE=$(echo -ne "INVALID REQUEST\r\n\r\n" | nc -w 1 $SERVER_IP $SERVER_PORT | head -1 | awk '{print $2}')
    
    if [ "$HTTP_CODE" = "400" ]; then
        print_success "Malformed request returned 400 Bad Request"
    else
        print_failure "Malformed request returned $HTTP_CODE (expected 400)"
    fi
}

test_http_version_not_supported() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    print_test "HTTP/2.0 returns 505 Version Not Supported"
    
    HTTP_CODE=$(echo -ne "GET / HTTP/2.0\r\nHost: localhost\r\n\r\n" | nc -w 1 $SERVER_IP $SERVER_PORT | head -1 | awk '{print $2}')
    
    if [ "$HTTP_CODE" = "505" ]; then
        print_success "HTTP/2.0 returned 505 Version Not Supported"
    else
        print_failure "HTTP/2.0 returned $HTTP_CODE (expected 505)"
    fi
}

# Print summary
print_summary() {
    print_header "TEST SUMMARY"
    echo ""
    echo -e "Total tests:  ${BLUE}$TESTS_TOTAL${NC}"
    echo -e "Passed:       ${GREEN}$TESTS_PASSED${NC}"
    echo -e "Failed:       ${RED}$TESTS_FAILED${NC}"
    echo ""
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}═══════════════════════════════════════════════════════════════${NC}"
        echo -e "${GREEN}           ✓ ALL TESTS PASSED! ✓${NC}"
        echo -e "${GREEN}═══════════════════════════════════════════════════════════════${NC}"
        return 0
    else
        echo -e "${RED}═══════════════════════════════════════════════════════════════${NC}"
        echo -e "${RED}           ✗ SOME TESTS FAILED ✗${NC}"
        echo -e "${RED}═══════════════════════════════════════════════════════════════${NC}"
        return 1
    fi
}

# Cleanup
cleanup() {
    print_header "CLEANUP"
    stop_server
    print_info "Keeping test files in $TEST_DIR for inspection"
    print_info "Log file: $LOG_FILE"
}

# Main execution
main() {
    # Trap to ensure cleanup on exit
    trap cleanup EXIT
    
    echo ""
    print_header "HTTPD SERVER TEST SUITE"
    echo ""
    
    # Check if server binary exists
    if [ ! -f "$SERVER_BIN" ]; then
        print_failure "Server binary not found: $SERVER_BIN"
        print_info "Please run 'make' first"
        exit 1
    fi
    
    # Setup
    setup_test_env
    
    # Start server
    if ! start_server; then
        print_failure "Cannot start server, aborting tests"
        exit 1
    fi
    
    # Wait a bit for server to be ready
    sleep 1
    
    # Run tests
    print_header "RUNNING TESTS"
    echo ""
    
    test_get_index
    test_get_file
    test_head_request
    test_404_not_found
    test_405_method_not_allowed
    test_connection_close
    test_date_header
    test_content_length
    test_content_type_html
    test_content_type_txt
    test_content_type_css
    test_content_type_js
    test_malformed_request
    test_http_version_not_supported
    
    echo ""
    
    # Print summary
    print_summary
    EXIT_CODE=$?
    
    exit $EXIT_CODE
}

# Run main
main
