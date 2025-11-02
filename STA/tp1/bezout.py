from generic import EuclideanRingElement

def extended_euclidean_algorithm(a: EuclideanRingElement, b: EuclideanRingElement):
    """Algorithme d'Euclide étendu.
    Retourne (g, u, v) tels que g = u·a + v·b.
    """

    old_r, r = a, b
    old_u, u = type(a).one(), type(a).zero()
    old_v, v = type(a).zero(), type(a).one()
    
    while r != type(a).zero():
        quotient = old_r // r
        old_r, r = r, old_r - quotient * r
        old_u, u = u, old_u - quotient * u
        old_v, v = v, old_v - quotient * v
    
    return old_r, old_u, old_v
    # raise NotImplementedError("À compléter !")

if __name__ == "__main__":
    from common_rings import Z

    _a, _b = Z(56), Z(15)
    _g, _u, _v = extended_euclidean_algorithm(_a, _b)
    assert _g == Z(1)
    assert (_u * _a + _v * _b) == _g

    print("Tous les tests ont réussi ✔️")
