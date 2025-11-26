import pytest
import subprocess
import time
import socket
import os
import shutil
import signal

SERVER_BIN = "./httpd"
SERVER_PORT = 8080
SERVER_IP = "127.0.0.1"
TEST_ROOT_DIR = "tests/www"
LOG_FILE = "tests/test.log"
PID_FILE = "tests/server.pid"

def kill_process_on_port(port):
    try:
        cmd = f"lsof -t -i:{port}"
        pids = subprocess.check_output(cmd, shell=True).decode().split()
        for pid in pids:
            if pid:
                os.kill(int(pid), signal.SIGKILL)
                time.sleep(0.5)
    except subprocess.CalledProcessError:
        pass 

def setup_test_files():
    if os.path.exists(TEST_ROOT_DIR):
        shutil.rmtree(TEST_ROOT_DIR)
    os.makedirs(TEST_ROOT_DIR)
    
    with open(os.path.join(TEST_ROOT_DIR, "index.html"), "w") as f:
        f.write("<h1>Test OK</h1>")

def wait_for_port(port, timeout=5):
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            with socket.create_connection((SERVER_IP, port), timeout=0.5):
                return True
        except (OSError, ConnectionRefusedError):
            time.sleep(0.1)
    return False

@pytest.fixture(scope="session")
def server():
    kill_process_on_port(SERVER_PORT)
    setup_test_files()
    
    # Suppression du fichier PID s'il existe deja
    if os.path.exists(PID_FILE):
        os.remove(PID_FILE)

    args = [
        SERVER_BIN,
        "--port", str(SERVER_PORT),
        "--ip", SERVER_IP,
        "--root-dir", TEST_ROOT_DIR,
        "--default-file", "index.html",
        "--log", "false",
        "--server-name", "localhost",
        "--pid-file", PID_FILE
    ]
    
    proc = subprocess.Popen(
        args,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    
    time.sleep(0.2)
    if proc.poll() is not None:
        raise RuntimeError(f"Le serveur a crashé (Return code: {proc.returncode}). Verifiez les arguments obligatoires.")

    if not wait_for_port(SERVER_PORT):
        proc.terminate()
        raise RuntimeError("Le serveur ne répond pas sur le port 8080.")

    yield f"http://{SERVER_IP}:{SERVER_PORT}"

    proc.terminate()
    try:
        proc.wait(timeout=2)
    except subprocess.TimeoutExpired:
        proc.kill()
        
    shutil.rmtree(TEST_ROOT_DIR, ignore_errors=True)
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
    if os.path.exists(PID_FILE):
        os.remove(PID_FILE)
        