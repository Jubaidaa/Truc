from generic import RingElement


class Z(RingElement):
    """Classe représentant les entiers relatifs."""
    def __init__(self, value: int):
        self.value = value

    @classmethod
    def zero(cls):
        return cls(0)

    @classmethod
    def one(cls):
        return cls(1)

    def __add__(self, other):
        return self.__class__(self.value + other.value)

    def __neg__(self):
        return self.__class__(-self.value)

    def __mul__(self, other):
        return self.__class__(self.value * other.value)

    def __eq__(self, other):
        return self.value == other.value

    def __repr__(self):
        return f"{self.value}"

    def __floordiv__(self, other):
        if other.value == 0:
            raise ZeroDivisionError("Division par zéro.")
        return self.__class__(self.value // other.value)

    def __mod__(self, other):
        if other.value == 0:
            raise ZeroDivisionError("Division par zéro.")
        return self.__class__(self.value % other.value)

    def stathm(self) -> int:
        return abs(self.value)


class Q(RingElement):
    """Classe représentant les nombres rationnels."""
    def __init__(self, numerator: int, denominator: int = 1):
        if denominator == 0:
            raise ValueError("Le dénominateur ne peut pas être zéro.")
        self.numerator = numerator
        self.denominator = denominator

        pgcd = 1
        for d in range(2, min(abs(numerator), abs(denominator)) + 1):
            if numerator % d == 0 and denominator % d == 0:
                pgcd = d

        self.cannonical_numerator = numerator // pgcd
        self.cannonical_denominator = denominator // pgcd

    @classmethod
    def zero(cls):
        return cls(0, 1)

    @classmethod
    def one(cls):
        return cls(1, 1)

    def __add__(self, other):
        new_numerator = (self.numerator * other.denominator +
                         other.numerator * self.denominator)
        new_denominator = self.denominator * other.denominator
        return Q(new_numerator, new_denominator)

    def __neg__(self):
        return Q(-self.numerator, self.denominator)

    def __mul__(self, other):
        return Q(self.numerator * other.numerator,
                 self.denominator * other.denominator)

    def __eq__(self, other):
        return (self.numerator * other.denominator ==
                other.numerator * self.denominator)

    def __repr__(self):
        if self.denominator == 1:
            return f"{self.cannonical_numerator}"
        return f"{self.cannonical_numerator}/{self.cannonical_denominator}"

    def inverse(self):
        if self.numerator == 0:
            raise ZeroDivisionError("L'élément zéro n'a pas d'inverse multiplicatif.")
        return Q(self.denominator, self.numerator)
