import subprocess
import time
import os
import signal

PID_FILE = "tests/daemon_test.pid"
SERVER_BIN = "./httpd"
SERVER_IP = "127.0.0.1"
SERVER_PORT = "8081"
SERVER_NAME = "test_daemon"
ROOT_DIR = "tests"

def clean_pid_file():
    if os.path.exists(PID_FILE):
        try:
            with open(PID_FILE, 'r') as f:
                pid = int(f.read().strip())
            os.kill(pid, signal.SIGTERM)
        except (OSError, ValueError):
            pass
        if os.path.exists(PID_FILE):
            os.remove(PID_FILE)

def test_daemon_lifecycle():
    clean_pid_file()
    
    common_args = [
        "--pid-file", PID_FILE,
        "--port", SERVER_PORT,
        "--ip", SERVER_IP,
        "--server-name", SERVER_NAME,
        "--root-dir", ROOT_DIR
    ]

    # Start
    cmd_start = [SERVER_BIN, "--daemon", "start"] + common_args
    
    subprocess.check_call(cmd_start)
    
    time.sleep(1)
    assert os.path.exists(PID_FILE)
    
    # Stop
    cmd_stop = [SERVER_BIN, "--daemon", "stop"] + common_args
    
    subprocess.check_call(cmd_stop)
    
    time.sleep(1)
    assert not os.path.exists(PID_FILE)
