class RingElement:
    """Interface pour les éléments d’un anneau."""
    @classmethod
    def zero(cls):
        """
        Retourne l'élément neutre pour l'addition.
        """
        raise NotImplementedError('La méthode zero doit être implémentée')

    @classmethod
    def one(cls):
        """
        Retourne l'élément neutre pour la multiplication.
        """
        raise NotImplementedError('La méthode one doit être implémentée')

    def __add__(self, other):
        """
        Addition de deux éléments de l'anneau.
        """
        raise NotImplementedError('La méthode __add__ doit être implémentée')

    def __neg__(self):
        """
        Opposé d'un élément de l'anneau.
        """
        raise NotImplementedError('La méthode __neg__ doit être implémentée')

    def __mul__(self, other):
        """
        Multiplication de deux éléments de l'anneau.
        """
        raise NotImplementedError('La méthode __mul__ doit être implémentée')

    def __eq__(self, other):
        """
        Comparaison de deux éléments de l'anneau.
        """
        raise NotImplementedError('La méthode __eq__ doit être implémentée')

    def __repr__(self):
        """
        Représentation lisible de l'élément de l'anneau.
        """
        raise NotImplementedError('La méthode __repr__ doit être implémentée')

    def __sub__(self, other):
        """
        Soustraction de deux éléments de l'anneau.
        """
        return self + (-other)


class FieldElement(RingElement):
    """Interface pour les éléments d’un corps."""
    def inverse(self):
        """
        Retourne l’inverse multiplicatif de l’élément.
        """
        if self == self.zero():
            raise ZeroDivisionError("L'élément zéro n'a pas d'inverse multiplicatif.")
        raise NotImplementedError("La méthode inverse doit être implémentée.")

    def __truediv__(self, other):
        """
        Division de self par other.
        """
        return self * other.inverse()


class EuclideanRingElement(RingElement):
    def stathm(self) -> int:
        """
        Retourne le stathme de l'élément.
        """
        raise NotImplementedError('La méthode stathm doit être implémentée')

    def __floordiv__(self, other):
        """
        Division euclidienne (quotient) de self par other.
        """
        raise NotImplementedError('La méthode __floordiv__ doit être implémentée')

    def __mod__(self, other):
        """
        Division euclidienne (reste) de self par other.
        """
