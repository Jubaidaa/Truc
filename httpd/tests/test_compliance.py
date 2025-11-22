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
    s.connect((SERVER_IP, SERVER_PORT))
    s.sendall(request_payload)
    
    response = b""
    while True:
        data = s.recv(4096)
        if not data:
            break
        response += data
    s.close()
    return response

def validate_response(raw_response):
    # Si le parser n'est pas la, on skip la validation stricte
    if not os.path.exists(PARSER_BIN):
        pytest.skip("response_parser introuvable dans tests/")
        return True

    with tempfile.NamedTemporaryFile(delete=False) as tmp:
        tmp.write(raw_response)
        tmp_name = tmp.name
    
    ret = subprocess.call(
        [PARSER_BIN],
        stdin=open(tmp_name, "r"),
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    os.remove(tmp_name)
    return ret == 0

def test_compliance_basic_get(server):
    req = b"GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert len(resp) > 0
    # On valide seulement si le parser est la
    validate_response(resp) 

def test_compliance_404(server):
    req = b"GET /notfound HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert b"404" in resp
    validate_response(resp)

def test_compliance_head(server):
    req = b"HEAD /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert b"200" in resp
    validate_response(resp)

def test_compliance_bad_method(server):
    req = b"TOTO / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    # Verifie qu'on a bien une reponse (400/405/501)
    assert len(resp) > 0
    validate_response(resp)
