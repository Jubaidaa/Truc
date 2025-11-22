import pytest
import socket
import subprocess
import os
import tempfile

# Configuration
SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080
# Chemin absolu pour eviter les erreurs de dossier courant
TEST_DIR = os.path.dirname(os.path.abspath(__file__))
PARSER_BIN = os.path.join(TEST_DIR, "response_parser")

def get_raw_response(request_payload):
    """Envoie une requete brute et recupere la reponse brute."""
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
    except ConnectionRefusedError:
        return b""
    finally:
        s.close()
    return response

def validate_response(raw_response):
    """
    Passe la reponse brute au binaire response_parser fourni par l'ecole.
    Si le binaire est absent, on SKIP le test proprement.
    """
    # 1. Verification stricte de l'existence du binaire
    if not os.path.exists(PARSER_BIN):
        pytest.skip(f"Binaire manquant: {PARSER_BIN}. Test ignore.")
        return True
        
    # 2. Verification des permissions d'execution
    if not os.access(PARSER_BIN, os.X_OK):
        # Tente de rendre executable si possible (best effort)
        try:
            os.chmod(PARSER_BIN, 0o755)
        except OSError:
            pytest.skip(f"Binaire non executable: {PARSER_BIN}. Test ignore.")
            return True

    # 3. Execution du test
    with tempfile.NamedTemporaryFile(delete=False) as tmp:
        tmp.write(raw_response)
        tmp_name = tmp.name
    
    try:
        # Redirection vers /dev/null pour ne pas polluer la sortie
        ret = subprocess.call(
            [PARSER_BIN],
            stdin=open(tmp_name, "r"),
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        )
        return ret == 0
    except Exception as e:
        pytest.fail(f"Erreur lors de l'execution du parser: {e}")
        return False
    finally:
        if os.path.exists(tmp_name):
            os.remove(tmp_name)

def test_compliance_basic_get(server):
    req = b"GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    assert len(resp) > 0, "Le serveur n'a rien renvoye"
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
    # HEAD ne doit pas avoir de corps
    assert validate_response(resp)

def test_compliance_bad_method(server):
    req = b"TOTO / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = get_raw_response(req)
    # Le serveur doit repondre quelque chose (400, 405 ou 501)
    assert len(resp) > 0
    assert validate_response(resp)
