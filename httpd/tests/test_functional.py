import requests
import pytest

def test_get_index(server):
    response = requests.get(f"{server}/")
    assert response.status_code == 200
    # On verifie un contenu basique si index.html existe
    # Sinon 404 est acceptable si pas de fichier, mais ici on attend 200

def test_head_request(server):
    response = requests.head(f"{server}/")
    assert response.status_code == 200
    assert response.text == ""

def test_404_not_found(server):
    response = requests.get(f"{server}/nonexistent_file.html")
    assert response.status_code == 404

def test_405_method_not_allowed(server):
    response = requests.post(f"{server}/")
    assert response.status_code == 405

def test_headers_presence(server):
    response = requests.get(f"{server}/")
    headers = response.headers
    assert "Date" in headers
    assert "Content-Length" in headers
    assert headers["Connection"] == "close"

def test_malformed_request(server):
    # Requests gere mal les requetes mal formees nativement
    # On utilise un test manuel via socket dans test_security.py
    pass
