# TP1 - Anneaux euclidiens, Division et Anneaux quotients

## Objectifs pédagogiques

Ce TP a pour but de vous faire manipuler concrètement les notions introduites dans le cours. En particulier, l'objectif est de :

* Comprendre comment les opérations sur un anneau se traduisent en fonctions informatiques, notamment grâce à la surcharge d'opérateurs (*operator overloading*).
* Implémenter des classes Python représentant des anneaux particuliers : $\mathbb{Z}/n\mathbb{Z}$ et $K[X]$.
* Réaliser la division euclidienne dans un anneau euclidien générique.
* Implémenter l’algorithme d’Euclide étendu pour calculer les coefficients de Bézout et les inverses modulaires.

Chaque partie est indépendante et peut être testée isolément. Les difficultés augmentent progressivement.

## Méthodologie

Pour chaque exercice, vous avez un fichier Python modèle dans lequel vous devez implémenter les méthodes demandées.

Le fichier est fait de sorte que vous pouvez exécuter des tests unitaires en lançant le fichier directement.

---

## Préambule

Comme vous avez déjà compris, un anneau est une triple $(A, +, \cdot)$ où $+$ et $\cdot$ sont deux opérations binaires internes.
Cela se traduit en Python par une classe `A` avec une méthode `__add__` pour l'addition et une méthode `__mul__` pour la multiplication.

L'addition doit avoir un élément neutre $0$ et chaque élément doit avoir un opposé $-a$. Ceci se traduit par une méthode
de classe `zero()` qui retourne l'élément neutre, et par une méthode `__neg__` qui retourne l'opposé d'un élément.

La multiplication doit aussi avoir un élément neutre $1$, ce qui se traduit par une méthode de classe `one()`.

De plus, on souhaite pouvoir comparer deux éléments de l'anneau, ce qui se traduit par une méthode `__eq__` qui retourne `True` si deux éléments sont égaux, et `False` sinon.

On veut aussi pouvoir afficher les éléments de l'anneau de manière lisible, ce qui se traduit par une méthode `__repr__`.

En résumé, un anneau est une classe (une interface, en langage orienté objet) qui a cette forme:

```python
class RingElement:
    @classmethod
    def zero(cls):
        """
        Retourne l'élément neutre pour l'addition.
        >>> zero = RingElement.zero()  # doit retourner l'élément neutre
        >>> a: RingElement  # un élément quelconque de l'anneau
        >>> a + zero == a
        True
        >>> zero + a == a
        True
        """
        raise NotImplementedError('La méthode zero doit être implémentée')

    @classmethod
    def one(cls):
        """
        Retourne l'élément neutre pour la multiplication.
        >>> one = RingElement.one()  # doit retourner l'élément neutre
        >>> a: RingElement  # un élément quelconque de l'anneau
        >>> a * one == a
        True
        >>> one * a == a
        True
        """
        raise NotImplementedError('La méthode one doit être implémentée')

    def __add__(self, other):
        """
        Addition de deux éléments de l'anneau.
        >>> a: RingElement
        >>> b: RingElement
        >>> c = a + b  # doit retourner un élément de l'anneau
        >>> assert isinstance(c, RingElement)
        """
        raise NotImplementedError('La méthode __add__ doit être implémentée')

    def __neg__(self):
        """
        Opposé d'un élément de l'anneau.
        >>> a: RingElement
        >>> b = -a  # doit retourner un élément de l'anneau
        >>> assert isinstance(b, RingElement)
        >>> a + b == RingElement.zero()
        True
        """
        raise NotImplementedError('La méthode __neg__ doit être implémentée')

    def __mul__(self, other):
        """
        Multiplication de deux éléments de l'anneau.
        >>> a: RingElement
        >>> b: RingElement
        >>> c = a * b  # doit retourner un élément de l'anneau
        >>> assert isinstance(c, RingElement)
        """
        raise NotImplementedError('La méthode __mul__ doit être implémentée')

    def __eq__(self, other):
        """
        Comparaison de deux éléments de l'anneau.
        >>> a: RingElement
        >>> b: RingElement
        >>> result = (a == b)  # doit retourner un booléen
        >>> assert isinstance(result, bool)
        """
        raise NotImplementedError('La méthode __eq__ doit être implémentée')

    def __repr__(self):
        """
        Représentation lisible de l'élément de l'anneau.
        >>> a: RingElement
        >>> repr_a = repr(a)  # doit retourner une chaîne de caractères
        >>> assert isinstance(repr_a, str)
        """
        raise NotImplementedError('La méthode __repr__ doit être implémentée')
```

## Exercice 1 :

Dans cet exercice, vous allez implémenter l'anneau $\mathbb{Z}/n\mathbb{Z}$ des entiers modulo $n$ et ses opérations.

### Instructions
Téléchargez le fichier `zmodn.py` et implémentez les méthodes
- `__add__`
- `__neg__`
- `__mul__`
- `__eq__`
- `__repr__`
de la classe `Z_nZ` qui hérite de `RingElement`.

### Indications
- Vous pouvez utiliser l'opérateur modulo `%` pour obtenir le reste de la division euclidienne.
- Ne changez pas la valeur de `self.value` après l'initialisation. Vous pouvez ajouter des attributs supplémentaires si nécessaire.

## Exercice 2 : Anneau des polynômes $A[X]$

Dans cet exercice, vous allez implémenter un anneau de polynômes à coefficients dans un anneau $A$.

Il est très important que votre implémentation soit générique, c’est-à-dire qu’elle puisse fonctionner avec n’importe classe qui respecte l'interface donnée par `RingElement`.

Votre classe devra être capable de manipuler les polynômes formels, c’est-à-dire des expressions de la forme :

$$ a_0 + a_1 X + a_2 X^2 + \dots + a_n X^n $$

avec $a_i \in A$.

### Instructions
Téléchargez le fichier `poly_ring.py` et implémentez les méthodes
- `__add__`
- `__neg__`
- `__sub__`
- `__mul__`
- `__eq__`
- `__repr__`
de la classe `Poly` qui hérite de `RingElement`.

### Spécifications fonctionnelles
Le constructeur de la classe `Poly` a la signature suivante :

```python
class Poly(RingElement):
    def __init__(self, *coeffs: RingElement):
        """
        Initialise un polynôme avec une tuple de coefficients.
        >>> P = Poly(3, 0, 2)  # représente le polynôme 3 + X^2 où les coefficients sont des entiers
        >>> Q = Poly(Z5(1), Z5(1), Z5(0))  # représente le polynôme [1]_5 + X où les coefficients sont dans Z/5Z
        """
        self.coeffs = coeffs
```

Un polynôme est donc représenté par une liste de coefficients, où l’indice dans la liste correspond au degré du terme.

### Conseils

- Vous pouvez implémenter une fonction `normalize()` appelée à la fin de chaque opération pour supprimer les zéros en fin de liste.

### Extension (facultative)

Pour les plus curieux, essayez d’ajouter :

- une méthode `degree()` pour retourner le degré du polynôme ;
- une méthode `__call__(self, x)` pour évaluer le polynôme en une valeur donnée ;

---

## Exercice 3.1 : Division euclidienne dans $\mathbb{Z}[i]$

Dans cet exercice, vous allez implémenter la **division euclidienne dans l’anneau des entiers de Gauss**~:
$$
\mathbb{Z}[i] = { a + bi \mid a,b \in \mathbb{Z} }
$$

C’est un **anneau euclidien** pour le stathme :
$$
v(a+bi) = a^2 + b^2.
$$

Pour deux éléments $z_1, z_2 \in \mathbb{Z}[i]$, il existe toujours $q, r \in \mathbb{Z}[i]$ tels que :
$$
z_1 = qz_2 + r \quad \text{avec} \quad v(r) < v(z_2).
$$

### Instructions

Téléchargez le fichier `gauss_division.py` et implémentez la classe `Z_i`, qui hérite de `EuclideanRingElement`, avec les méthodes suivantes :

* `__add__`
* `__neg__`
* `__sub__`
* `__mul__`
* `__eq__`
* `__repr__`
* `stathm` : retourne la norme $a^2 + b^2$
* `__floordiv__` et `__mod__` : réalisent la division euclidienne

## Exercice 3.2 : Division euclidienne dans $K[X]$

Dans cet exercice, vous allez implémenter la **division euclidienne** de deux polynômes dans $K[X]$, où $K$ est un corps.

Un corps est un anneau dans lequel tout élément non nul possède un inverse multiplicatif. L'interface correspondante est :

```python
class FieldElement(RingElement):
    def inverse(self):
        """
        Retourne l'inverse multiplicatif de l'élément.
        >>> a: FieldElement  # un élément non nul du corps
        >>> b = a.inverse()  # doit retourner un élément de l'anneau
        >>> assert isinstance(b, FieldElement)
        >>> a * b == FieldElement.one()
        True
        """
        if self == self.zero():
            raise ZeroDivisionError('L\'élément nul n\'a pas d\'inverse')
        raise NotImplementedError('La méthode inverse doit être implémentée')
```

Un anneau de polynômes à coefficients dans un corps est un anneau euclidien, ce qui signifie que l’on peut y effectuer la division euclidienne.
Pour cela, nous avons besoin de la notion de stathme et de la réalisation d'une division euclidienne.

Dans ce contexte, nous allons définir une méthode `stathm()` qui retourne un entier et les méthodes `__floordiv__` et `__mod__` qui correspondent aux opérateurs `//` et `%` en Python.

L'interface d'un anneau euclidien est donc :

```python
class EuclideanRingElement(RingElement):
    def stathm(self) -> int:
        """
        Retourne le stathme de l'élément.
        >>> a: EuclideanRingElement
        >>> s = a.stathm()  # doit retourner un entier
        >>> assert isinstance(s, int)
        """
        raise NotImplementedError('La méthode stathm doit être implémentée')
    def __floordiv__(self, other):
        """
        Division euclidienne (quotient) de self par other.
        >>> a: EuclideanRingElement
        >>> b: EuclideanRingElement
        >>> q = a // b  # doit retourner un élément de l'anneau
        >>> assert isinstance(q, EuclideanRingElement)
        """
        raise NotImplementedError('La méthode __floordiv__ doit être implémentée')
    def __mod__(self, other):
        """
        Division euclidienne (reste) de self par other.
        >>> a: EuclideanRingElement
        >>> b: EuclideanRingElement
        >>> r = a % b  # doit retourner un élément de l'anneau
        >>> assert isinstance(r, EuclideanRingElement)
        """
```

### Instructions

Téléchargez le fichier `poly_division.py` et implémentez les méthodes 
- `stathm`
- `__floordiv__`
- `__mod__`

### Indications
- Vous pouvez créer une fonction auxiliaire `poly_division(P, Q)` qui retourne le couple `(S, R)` tel que `P = Q*S + R` avec `stathm(R) < stathm(Q)`.


## Exercice 4 : Algorithme d’Euclide étendu

Dans cet exercice, vous allez implémenter **l’algorithme d’Euclide étendu**, qui permet de calculer les coefficients de Bézout pour deux éléments d’un anneau euclidien.

L’objectif est de trouver, pour deux éléments $a$ et $b$ d’un anneau euclidien, des éléments $u$ et $v$ tels que :

$$ u · a + v · b = g $$

où $g$ est le plus grand diviseur commun (PGCD) de $a$ et $b$.

---

### Instructions

Téléchargez le fichier `bezout.py` et implémentez la fonction :

```python
def extended_euclidean_algorithm(a: EuclideanRingElement, b: EuclideanRingElement):
    """
    Algorithme d’Euclide étendu.
    Retourne (g, u, v) tels que g = u*a + v*b.
    """
```

### Indications
- Vous devez utiliser un approche générique qui fonctionne pour n’importe quel anneau euclidien. C'est-à-dire,
vous devez utiliser les méthodes `__floordiv__` et `__mod__` pour la division euclidienne, et `zero()` et `one()` pour les éléments neutres.

## Exercice 5 : Anneau quotient d’un anneau euclidien

Dans cet exercice, vous allez construire **l’anneau quotient** $A/(p)$ à partir d’un **anneau euclidien** $A$ et d’un élément $p \in A$.

L’objectif est de créer dynamiquement une classe représentant les éléments de cet anneau quotient, avec les opérations bien définies modulo l’idéal $(p)$.

Autrement dit, on identifie deux éléments $a, b \in A$ si :

$$ a \equiv b \pmod{p} \quad \Longleftrightarrow \quad p \mid (a - b). $$

---

### Instructions

Téléchargez le fichier `quotient_ring.py` et implémentez la fonction qui crée la classe :

```python
from typing import Type, TypeVar

T = TypeVar('T', bound='EuclideanRingElement')

def create_quotient_ring(A: Type[T], p: T):
    """
    Crée la classe de l'anneau quotient A/(p).
    """
    class A_mod_p(RingElement):
        def inverse(self):
            """
            Retourne l'inverse multiplicatif de l'élément dans A/(p) si il existe.
            >>> a: A_mod_p  # un élément non nul de l'anneau quotient
            >>> try:
            >>>     b = a.inverse()  # doit retourner un élément de l'anneau quotient
            >>>     assert isinstance(b, A_mod_p)
            >>> except NotInvertibleError:
            >>>     pass  # l'élément n'est pas inversible
            """
```

Cette fonction doit retourner une classe `A_mod_p` dont les éléments sont des représentants de classes d’équivalence modulo $(p)$.

### Indications

-Utilisez la méthode `__mod__` de votre anneau euclidien pour réduire les représentants.
- Le calcul d’inverse s’appuie directement sur la fonction `extended_euclidean_algorithm` implémentée à l’exercice précédent.

---

## Barème

| Exercice                                    | Fichier                 | Points  |
|---------------------------------------------|-------------------------|---------|
| 1. Anneau $\mathbb{Z}/n\mathbb{Z}$          | `zmodn.py`              | 10      |
| 2. Anneau $A[X]$                            | `poly_ring.py`          | 15      |
| 3.1 Division euclidienne en $\mathbb{Z}[i]$ | `gauss_division.py`     | 15      |
| 3.2 Division euclidienne de polynômes       | `euclidean_division.py` | 20      |
| 4. Algorithme d'Euclide étendu              | `bezout.py`             | 20      |
| 5. Anneau quotient                          | `quotient_ring.py`      | 20      |
|                                             | **Total**               | **100** |
