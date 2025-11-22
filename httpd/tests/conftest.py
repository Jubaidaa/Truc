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

def kill_process_on_port(port):
    try:
        cmd = f"lsof -t -i:{port}"
        pids = subprocess.check_output(cmd, shell=True).decode().split()
        for pid in pids:
            if pid:
                os.kill(int(pid), signal.SIGKILL)
                time.sleep(0.5) # Laisser le temps au socket de se libérer
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
    # 1. Nettoyage 
    kill_process_on_port(SERVER_PORT)
    setup_test_files()
    
    # 2. Démarrage du serveur
    args = [
        SERVER_BIN,
        "--port", str(SERVER_PORT),
        "--ip", SERVER_IP,
        "--root-dir", TEST_ROOT_DIR,
        "--default-file", "index.html",
        "--log", "false"
    ]
    
    proc = subprocess.Popen(
        args,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    
    # 3. Vérification crash
    time.sleep(0.2)
    if proc.poll() is not None:
        raise RuntimeError(f"Le serveur a crashé (Return code: {proc.returncode}).")

    # 4. Attente port
    if not wait_for_port(SERVER_PORT):
        proc.terminate()
        raise RuntimeError("Le serveur ne répond pas sur le port 8080.")

    yield f"http://{SERVER_IP}:{SERVER_PORT}"

    # 5. Clean
    proc.terminate()
    try:
        proc.wait(timeout=2)
    except subprocess.TimeoutExpired:
        proc.kill()
        
    shutil.rmtree(TEST_ROOT_DIR, ignore_errors=True)
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
