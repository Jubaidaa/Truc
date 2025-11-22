import socket
import pytest

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080

def send_raw_payload(payload):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((SERVER_IP, SERVER_PORT))
    s.sendall(payload)
    response = s.recv(4096)
    s.close()
    return response

def test_null_byte_header(server):
    payload = (
        b"GET / HTTP/1.1\r\n"
        b"Host: local\0host\r\n"
        b"\r\n"
    )
    response = send_raw_payload(payload)
    assert len(response) > 0

def test_http_version_not_supported(server):
    payload = b"GET / HTTP/2.0\r\nHost: localhost\r\n\r\n"
    response = send_raw_payload(payload)
    assert b"505" in response

def test_malformed_method(server):
    payload = b"NOTAMETHOD / HTTP/1.1\r\n\r\n"
    response = send_raw_payload(payload)
    # 400 Bad Request ou 405 ou 501
    assert b"HTTP/1.1" in response
