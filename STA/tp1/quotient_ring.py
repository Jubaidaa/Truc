from generic import EuclideanRingElement
from bezout import extended_euclidean_algorithm
from typing import TypeVar, Type

A_Type = TypeVar("A_Type", bound=EuclideanRingElement)

def create_quotient_ring(A: Type[A_Type], p: A_Type):
    """Fabrique la classe représentant l'anneau quotient A/(p)."""

    class A_mod_p(A):
        """Élément de l'anneau quotient A/(p)."""

        def __init__(self, value):
            # Reduce value modulo p
            self.value = value % p
            # raise NotImplementedError("À compléter !")

        @classmethod
        def zero(cls):
            return A_mod_p(A.zero())
            # raise NotImplementedError("À compléter !")

        @classmethod
        def one(cls):
            return A_mod_p(A.one())
            # raise NotImplementedError("À compléter !")

        def __repr__(self):
            return f"[{repr(self.value)}] mod ({repr(p)})"
            # raise NotImplementedError("À compléter !")

        def __eq__(self, other):
            diff = self.value - other.value
            return diff % p == A.zero()
            # raise NotImplementedError("À compléter !")

        def __add__(self, other):
            return A_mod_p(self.value + other.value)
            # raise NotImplementedError("À compléter !")

        def __neg__(self):
            return A_mod_p(-self.value)
            # raise NotImplementedError("À compléter !")

        def __mul__(self, other):
            return A_mod_p(self.value * other.value)
            # raise NotImplementedError("À compléter !")

        def inverse(self):
            """Calculate multiplicative inverse using extended Euclidean algorithm."""
            g, u, _ = extended_euclidean_algorithm(self.value, p) # '_' ==> varaible non utilisée
            if g == A.one():
                return A_mod_p(u)
            else:
                raise ValueError("Element is not invertible in this quotient ring")
            # raise NotImplementedError("À compléter !")

    return A_mod_p

if __name__ == "__main__":
    from common_rings import Z

    Z5 = create_quotient_ring(Z, Z(5))
    assert Z5(Z(1)).inverse() == Z5(Z(1))
    assert Z5(Z(2)).inverse() == Z5(Z(3))
    assert Z5(Z(3)).inverse() == Z5(Z(2))
    assert Z5(Z(4)).inverse() == Z5(Z(4))

    from poly_division import create_euclidean_poly
    PolyZ5 = create_euclidean_poly(Z5)
    _p = PolyZ5(Z5(Z(1)), Z5(Z(1)), Z5(Z(1)))

    PolyZ5_mod_p = create_quotient_ring(PolyZ5, _p)

    _a = PolyZ5(Z5(Z(1)), Z5(Z(2)), Z5(Z(0)), Z5(Z(4)))
    _a_i = PolyZ5_mod_p(_a).inverse()
    assert (_a_i * PolyZ5_mod_p(_a)) == PolyZ5_mod_p.one()

    print("Tous les tests ont réussi ✔️")