import subprocess
import time
import os
import signal

PID_FILE = "tests/daemon_test.pid"
SERVER_BIN = "./httpd"

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
    
    # Start
    subprocess.check_call([
        SERVER_BIN,
        "--daemon", "start",
        "--pid-file", PID_FILE,
        "--port", "8081"
    ])
    
    time.sleep(1)
    assert os.path.exists(PID_FILE)
    
    # Stop
    subprocess.check_call([
        SERVER_BIN,
        "--daemon", "stop",
        "--pid-file", PID_FILE
    ])
    
    time.sleep(1)
    assert not os.path.exists(PID_FILE)
