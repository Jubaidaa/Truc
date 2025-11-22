import pytest
import socket
import subprocess
import os
import tempfile

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080
PARSER_BIN = "./tests/response_parser"

def get_raw_response(request_payload):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((SERVER_IP, SERVER_PORT))
        s.sendall(request_payload)
        
        response = b""
        while True:
            data = s.recv(4096)
            if not data:
                break
            response += data
    finally:
        s.close()
    return response

def validate_response(raw_response):
    # Verification securisee de l'existence du binaire
    if not os.path.isfile(PARSER_BIN) or not os.access(PARSER_BIN, os.X_OK):
        # On skip silencieusement ou avec un warning, mais on ne crash pas
        pytest.skip(f"Binaire {PARSER_BIN} introuvable ou non executable. Copiez-le depuis l'archive fournie.")
        return True

    with tempfile.NamedTemporaryFile(delete=False) as tmp:
        tmp.write(raw_response)
        tmp_name = tmp.name
    
    try:
        ret = subprocess.call(
            [PARSER_BIN],
            stdin=open(tmp_name, "r"),
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        )
        return ret == 0
    finally:
        if os.path.exists(tmp_name):
            os.remove(tmp_name)

def test_compliance_basic_get(server):
    req = b"GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert len(resp) > 0
    assert validate_response(resp)

def test_compliance_404(server):
    req = b"GET /notfound HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert b"404" in resp
    assert validate_response(resp)

def test_compliance_head(server):
    req = b"HEAD /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert b"200" in resp
    assert validate_response(resp)

def test_compliance_bad_method(server):
    req = b"TOTO / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert len(resp) > 0
    assert validate_response(resp)
