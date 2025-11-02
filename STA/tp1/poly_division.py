from generic import FieldElement, EuclideanRingElement
from poly_ring import create_poly_A
from typing import TypeVar

K_Type = TypeVar("K_Type", bound=FieldElement)
A_Type = TypeVar("A_Type", bound=EuclideanRingElement)


def create_euclidean_poly(K: type[K_Type]) -> type[A_Type]:
    """Fabrique une classe représentant l’anneau euclidien A[X]."""
    Poly = create_poly_A(K)

    class EuclideanPoly(Poly):
        """Élément de l’anneau euclidien A[X]."""

        def stathm(self):
            raise NotImplementedError("À compléter !")

        def __floordiv__(self, other):
            """Quotient de la division euclidienne dans A[X]."""
            raise NotImplementedError("À compléter !")

        def __mod__(self, other):
            """Reste de la division euclidienne dans A[X]."""
            raise NotImplementedError("À compléter !")

    return EuclideanPoly


if __name__ == "__main__":
    from common_rings import Q

    PolyQ = create_euclidean_poly(Q)

    A = PolyQ(Q(2), Q(0), Q(7, 2), Q(1), Q(3))  # 2 + 7/2 X^2 + X^3 + 3 X^4
    B = PolyQ(Q(1), Q(0), Q(2, 1))  # 1 + 2 X

    q = A // B
    r = A % B
    assert q == PolyQ(Q(1), Q(1, 2), Q(3, 2))
    assert r == PolyQ(Q(1), Q(-1, 2))

    assert (B * q + r) == A
    assert A // PolyQ.one() == A
    assert A % PolyQ.one() == PolyQ.zero()

    try:
        _ = A // PolyQ.zero()
        assert False, "Division par zéro n'a pas levé d'exception"
    except ZeroDivisionError:
        pass

    print("Tous les tests ont réussi ✔️")
