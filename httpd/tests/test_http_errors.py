import socket
import pytest
import time

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080

def send_raw_payload(payload):
    """
    Envoie un payload brut au serveur via un socket TCP et retourne la réponse.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2.0)
    try:
        s.connect((SERVER_IP, SERVER_PORT))
        s.sendall(payload)
        
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

def parse_status_code(response):
    """
    Extrait le code de statut HTTP de la première ligne de la réponse.
    """
    try:
        first_line = response.split(b"\r\n")[0]
        parts = first_line.split(b" ")
        if len(parts) >= 2:
            return int(parts[1])
    except (ValueError, IndexError):
        pass
    return 0

# --- Tests Bad Host Header ---

def test_wrong_host_value(server):
    # Host header ne correspond ni à server_name ni à IP:Port
    payload = (
        b"GET /index.html HTTP/1.1\r\n"
        b"Host: wrong_server_name\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400
    assert b"Bad Request" in resp

def test_missing_host_header(server):
    # Pas de header Host du tout
    payload = (
        b"GET /index.html HTTP/1.1\r\n"
        b"User-Agent: test-suite\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400
    assert b"Bad Request" in resp

def test_empty_host_header(server):
    # Header Host vide
    payload = (
        b"GET /index.html HTTP/1.1\r\n"
        b"Host: \r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

# --- Tests Malformed Request Line ---

def test_missing_leading_slash(server):
    # Target sans slash initial (ex: "index.html" au lieu de "/index.html")
    payload = (
        b"GET index.html HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

def test_wrong_http_version_1_0(server):
    # Version HTTP non supportée (1.0) -> Devrait être 505
    payload = (
        b"GET / HTTP/1.0\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    status = parse_status_code(resp)
    # RFC autorise 505 ou traitement comme 1.1, mais le sujet demande 505 strict
    assert status == 505

def test_malformed_http_version_string(server):
    # Chaîne de version invalide
    payload = (
        b"GET / HTTP=1.1\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

# --- Tests Null Bytes (Security) ---

def test_null_byte_in_header_value(server):
    # Null byte au milieu d'un header
    payload = (
        b"GET / HTTP/1.1\r\n"
        b"Host: local\0host\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

def test_null_byte_in_request_line(server):
    # Null byte dans la request line
    payload = (
        b"GET /idx\0.html HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

def test_null_byte_trailing_header(server):
    # Null byte à la fin d'un header
    payload = (
        b"GET / HTTP/1.1\r\n"
        b"Host: localhost\0\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 400

# --- Tests Basic Errors ---

def test_404_not_found(server):
    # Fichier inexistant
    payload = (
        b"GET /imaginary_file.txt HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 404
    assert b"Not Found" in resp

def test_405_method_not_allowed(server):
    # Méthode inconnue/non autorisée
    payload = (
        b"DELETE /index.html HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"\r\n"
    )
    resp = send_raw_payload(payload)
    assert parse_status_code(resp) == 405
    assert b"Method Not Allowed" in resp
    