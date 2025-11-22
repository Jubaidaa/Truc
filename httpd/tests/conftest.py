import pytest
import subprocess
import time
import socket
import os

SERVER_BIN = "./httpd"
SERVER_PORT = 8080
SERVER_IP = "127.0.0.1"

def wait_for_port(port, timeout=5):
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            with socket.create_connection((SERVER_IP, port), timeout=1):
                return True
        except (OSError, ConnectionRefusedError):
            time.sleep(0.1)
    return False

@pytest.fixture(scope="session")
def server():
    # Start server
    proc = subprocess.Popen(
        [SERVER_BIN, "--port", str(SERVER_PORT), "--ip", SERVER_IP],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    
    if not wait_for_port(SERVER_PORT):
        proc.terminate()
        raise RuntimeError("Server failed to start")

    yield f"http://{SERVER_IP}:{SERVER_PORT}"

    # Cleanup
    proc.terminate()
    proc.wait()
