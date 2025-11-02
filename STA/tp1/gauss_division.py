from generic import EuclideanRingElement


class Z_i(EuclideanRingElement):
    """Élément de l'anneau euclidien Z[i]."""

    def __init__(self, a: int, b: int = 0):
        self.a = a
        self.b = b

    @classmethod
    def zero(cls):
        return Z_i(0, 0)
        # raise NotImplementedError("À compléter !")

    @classmethod
    def one(cls):
        return Z_i(1, 0)
        # raise NotImplementedError("À compléter !")

    def __add__(self, other):
        return Z_i(self.a + other.a, self.b + other.b)
        # raise NotImplementedError("À compléter !")

    def __neg__(self):
        return Z_i(-self.a, -self.b)
        # raise NotImplementedError("À compléter !")

    def __mul__(self, other):
        # (a + bi)(c + di) = (ac - bd) + (ad + bc)i
        real = self.a * other.a - self.b * other.b
        imag = self.a * other.b + self.b * other.a
        return Z_i(real, imag)
        # raise NotImplementedError("À compléter !")

    def __eq__(self, other):
        return self.a == other.a and self.b == other.b
        # raise NotImplementedError("À compléter !")

    def __repr__(self):
        if self.b == 0:
            return str(self.a)
        if self.a == 0:
            if self.b == 1:
                return "i"
            elif self.b == -1:
                return "-i"
            else:
                return f"{self.b}i"
        
        if self.b > 0:
            return f"{self.a}+{self.b}i"
        else:
            return f"{self.a}{self.b}i"
        # raise NotImplementedError("À compléter !")

    def stathm(self):
        return self.a * self.a + self.b * self.b
        # raise NotImplementedError("À compléter !")

    def conjugate(self):
        """Retourne le conjugué de z = a + bi, soit a - bi."""
        return Z_i(self.a, -self.b)

    def __floordiv__(self, other):
        """Division euclidienne (quotient) dans Z[i]."""
        if other == Z_i.zero():
            raise ZeroDivisionError("Division par zéro.")
        
        # z1 / z2 = z1 * conj(z2) / |z2|²
        numerator = self * other.conjugate()
        denominator = other.stathm()
        q_real = round(numerator.a / denominator)
        q_imag = round(numerator.b / denominator)
        
        return Z_i(q_real, q_imag)
        # raise NotImplementedError("À compléter !")

    def __mod__(self, other):
        """Reste de la division euclidienne."""
        q = self // other
        return self - q * other
        # raise NotImplementedError("À compléter !")


if __name__ == "__main__":
    _a = Z_i(7, 3)
    _b = Z_i(2, 1)
    _c = Z_i(4, -5)
    _d = Z_i(-3, 1)
    zero = Z_i.zero()
    one = Z_i.one()

    # Représentation __repr__
    assert repr(_a) == "7+3i"
    assert repr(_b) == "2+1i"
    assert repr(_c) == "4-5i"
    assert repr(_d) == "-3+1i"
    assert repr(zero) == "0"
    assert repr(one) == "1"

    # Égalité __eq__
    assert _a == Z_i(7, 3)
    assert _b != _c
    assert zero != one
    assert -_a != _a

    # Addition __add__
    assert _a + _b == Z_i(9, 4)
    assert _b + _c == Z_i(6, -4)
    assert _c + _d == Z_i(1, -4)
    assert _a + zero == _a
    assert _b + one == Z_i(3, 1)

    # Opposé __neg__ et soustraction __sub__
    assert -_a == Z_i(-7, -3)
    assert -_b == Z_i(-2, -1)
    assert -zero == zero
    assert -one == Z_i(-1, 0)
    assert _a - _b == Z_i(5, 2)
    assert _b - _a == Z_i(-5, -2)
    assert _a - zero == _a
    assert zero - _a == -_a

    # Multiplication __mul__
    assert _a * _b == Z_i(11, 13)
    assert _b * _c == Z_i(13, -6)
    assert _c * _d == Z_i(-7, 19)
    assert _a * one == _a
    assert _b * zero == zero

    # Norme (stathm)
    assert _a.stathm() == 58
    assert _b.stathm() == 5
    assert _c.stathm() == 41
    assert _d.stathm() == 10
    assert zero.stathm() == 0
    assert one.stathm() == 1

    # Conjugué
    assert _a.conjugate() == Z_i(7, -3)
    assert _c.conjugate() == Z_i(4, 5)

    # Division euclidienne __floordiv__ et __mod__
    _q = _a // _b
    _r = _a % _b
    assert _q == Z_i(3, 0)
    assert _r == Z_i(1, 0)
    assert _a == _q * _b + _r

    _q = _c // _d
    _r = _c % _d
    assert _c == _q * _d + _r
    assert _r.stathm() < _d.stathm()

    q3 = Z_i(1, 1) // Z_i(1, 2)
    r3 = Z_i(1, 1) % Z_i(1, 2)
    assert Z_i(1, 1) == q3 * Z_i(1, 2) + r3
    assert r3.stathm() < Z_i(1, 2).stathm()

    # Division par l'unité
    assert _a // one == _a
    assert _a % one == zero

    # Division par zéro
    try:
        _ = _a // zero
        assert False, "Division par zéro n'a pas levé d'exception"
    except ZeroDivisionError:
        pass

    print("Tous les tests ont réussi ✔️")