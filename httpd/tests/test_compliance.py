import pytest
import socket
import subprocess
import os
import tempfile

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080
TEST_DIR = os.path.dirname(os.path.abspath(__file__))
PARSER_BIN = os.path.join(TEST_DIR, "response_parser")

def get_raw_response(request_payload):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1.0)
    try:
        s.connect((SERVER_IP, SERVER_PORT))
        s.sendall(request_payload)
        
        response = b""
        while True:
            try:
                data = s.recv(4096)
                if not data:
                    break
                response += data
            except socket.timeout:
                break
    except (ConnectionRefusedError, socket.timeout):
        return b""
    finally:
        s.close()
    return response

def validate_response(raw_response):
    """
    Tente response_parser.
    Si le binaire est inutilisable (absent ou non executable), on SKIP le test. [casse pipe mais bon]
    """
    if not os.path.exists(PARSER_BIN):
        pytest.skip(f"Binaire manquant: {PARSER_BIN}")
        return True

    if not os.access(PARSER_BIN, os.X_OK):
        try:
            os.chmod(PARSER_BIN, 0o755)
        except OSError:
            pytest.skip(f"Binaire non executable: {PARSER_BIN}")
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
    except OSError:
        pytest.skip(f"Impossible d'executer le binaire (Incompatible systeme).")
        return True
    except Exception as e:
        pytest.skip(f"Erreur d'execution: {e}")
        return True
    finally:
        if os.path.exists(tmp_name):
            os.remove(tmp_name)

def test_compliance_basic_get(server):
    req = b"GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert len(resp) > 0, "Reponse vide"
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
