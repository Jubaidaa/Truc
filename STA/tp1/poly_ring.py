from generic import RingElement
from typing import TypeVar

A_Type = TypeVar("A_Type", bound=RingElement)

def create_poly_A(A: A_Type) -> type[A_Type]:
    """Fabrique une classe représentant l'anneau de polynômes A[X]."""

    class Poly(RingElement):
        """Élément de l'anneau A[X]."""

        def __init__(self, *coeffs: A):
            self.coeffs = coeffs

        @classmethod
        def zero(cls):
            """Polynôme nul."""
            return Poly(A.zero())
            # raise NotImplementedError("À compléter !")

        @classmethod
        def one(cls):
            """Polynôme unité."""
            return Poly(A.one())
            # raise NotImplementedError("À compléter !")

        def _normalize(self):
            """Retire les zéros de fin."""
            coeffs = list(self.coeffs)
            while len(coeffs) > 1 and coeffs[-1] == A.zero():
                coeffs.pop()
            return coeffs

        def __repr__(self):
            """Affichage lisible du polynôme."""
            coeffs = self._normalize()
            
            if len(coeffs) == 0 or (len(coeffs) == 1 and coeffs[0] == A.zero()):
                return repr(A.zero())
            
            terms = []
            for i, c in enumerate(coeffs):
                if c == A.zero():
                    continue
                    
                if i == 0:
                    terms.append(repr(c))
                elif i == 1:
                    if c == A.one():
                        terms.append("X")
                    else:
                        terms.append(f"{repr(c)} X")
                else:
                    if c == A.one():
                        terms.append(f"X^{i}")
                    else:
                        terms.append(f"{repr(c)} X^{i}")
            
            if not terms:
                return repr(A.zero())
            return " + ".join(terms)
            # raise NotImplementedError("À compléter !")

        def __eq__(self, other):
            """Égalité de polynômes."""
            coeffs1 = self._normalize()
            coeffs2 = other._normalize()
            if len(coeffs1) != len(coeffs2):
                return False
            for c1, c2 in zip(coeffs1, coeffs2):
                if c1 != c2:
                    return False
            return True
            # raise NotImplementedError("À compléter !")

        def __add__(self, other):
            """Addition de deux polynômes."""
            max_len = max(len(self.coeffs), len(other.coeffs))
            new_coeffs = []
            for i in range(max_len):
                c1 = self.coeffs[i] if i < len(self.coeffs) else A.zero()
                c2 = other.coeffs[i] if i < len(other.coeffs) else A.zero()
                new_coeffs.append(c1 + c2)
            return Poly(*new_coeffs)
            # raise NotImplementedError("À compléter !")

        def __neg__(self):
            """Opposé d'un polynôme."""
            return Poly(*[-c for c in self.coeffs])
            # raise NotImplementedError("À compléter !")

        def __mul__(self, other):
            """Multiplication de deux polynômes."""
            if len(self.coeffs) == 0 or len(other.coeffs) == 0:
                return Poly(A.zero())
            
            result_coeffs = [A.zero()] * (len(self.coeffs) + len(other.coeffs) - 1)
            for i, c1 in enumerate(self.coeffs):
                for j, c2 in enumerate(other.coeffs):
                    result_coeffs[i + j] = result_coeffs[i + j] + (c1 * c2)
            return Poly(*result_coeffs)
            # raise NotImplementedError("À compléter !")

        def degree(self):
            """Retourne le degré du polynôme."""
            coeffs = self._normalize()
            if len(coeffs) == 0 or (len(coeffs) == 1 and coeffs[0] == A.zero()):
                return float('-inf')
            return len(coeffs) - 1

    return Poly


if __name__ == "__main__":
    from zmodn import create_Z_nZ

    Z5 = create_Z_nZ(5)
    PolyZ5 = create_poly_A(Z5)

    P = PolyZ5(Z5(3), Z5(0), Z5(2))
    Q = PolyZ5(Z5(1), Z5(1))
    Z = PolyZ5(Z5(0), Z5(0), Z5(0))
    U = PolyZ5(Z5(1))
    C = PolyZ5(Z5(4))
    zero = PolyZ5.zero()
    one = PolyZ5.one()

    # Représentation __repr__
    assert repr(P) == "[3]_5 + [2]_5 X^2"
    assert repr(Q) == "[1]_5 + X"
    assert repr(Z) == "[0]_5"
    assert repr(U) == "[1]_5"
    assert repr(C) == "[4]_5"
    assert repr(zero) == "[0]_5"
    assert repr(one) == "[1]_5"

    # Égalité __eq__
    assert P == PolyZ5(Z5(3), Z5(0), Z5(2))
    assert P == PolyZ5(Z5(3), Z5(0), Z5(2), Z5(0), Z5(0))
    assert P != Q
    assert zero == Z
    assert one != zero

    # Addition __add__
    assert P + Q == PolyZ5(Z5(4), Z5(1), Z5(2))
    assert Q + P == P + Q
    assert P + zero == P
    assert zero + Q == Q
    assert PolyZ5(Z5(1)) + PolyZ5(Z5(4)) == zero

    # Opposé __neg__
    negP = -P
    assert -P == PolyZ5(Z5(2), Z5(0), Z5(3))
    assert P + (-P) == zero
    assert -zero == Z
    assert -one == PolyZ5(Z5(4))

    # Multiplication __mul__
    assert P * Q == PolyZ5(Z5(3), Z5(3), Z5(2), Z5(2))
    assert P * zero == zero
    assert P * one == P
    assert one * Q == Q
    assert Q * Q == PolyZ5(Z5(1), Z5(2), Z5(1))

    # --- Test degree ---
    assert P.degree() == 2
    assert Q.degree() == 1
    assert zero.degree() == float('-inf')
    assert one.degree() == 0

    print("T(ous les tests ont réussi ✔️")
