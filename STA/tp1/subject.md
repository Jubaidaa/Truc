# TP1 – Euclidean Rings, Division, and Quotient Rings

## Learning Objectives

This lab aims to give you hands-on experience with the notions introduced in the course. In particular, the goal is to:

* Understand how operations on a ring are translated into computer functions, notably through *operator overloading*.
* Implement Python classes representing specific rings: $\mathbb{Z}/n\mathbb{Z}$ and $K[X]$.
* Perform Euclidean division in a generic Euclidean ring.
* Implement the extended Euclidean algorithm to compute Bézout coefficients and modular inverses.

Each part is independent and can be tested separately. The difficulty increases progressively.

## Methodology

For each exercise, you are provided with a Python template file in which you must implement the required methods.

The file is designed so that you can run unit tests by executing it directly.

---

## Preamble

As you have already understood, a ring is a triple $(A, +, \cdot)$ where $+$ and $\cdot$ are two internal binary operations.
In Python, this translates to a class `A` with a method `__add__` for addition and a method `__mul__` for multiplication.

Addition must have a neutral element $0$, and each element must have an opposite $-a$.
This corresponds to a class method `zero()` that returns the neutral element and a method `__neg__` that returns the opposite of an element.

Multiplication must also have a neutral element $1$, represented by a class method `one()`.

Moreover, we want to be able to compare two elements of the ring, which translates to a method `__eq__` returning `True` if two elements are equal, and `False` otherwise.

We also want to display ring elements in a readable way, which is done via a `__repr__` method.

In summary, a ring is a class (an interface, in object-oriented terms) of the following form:

```python
class RingElement:
    @classmethod
    def zero(cls):
        """
        Returns the neutral element for addition.
        >>> zero = RingElement.zero()  # must return the neutral element
        >>> a: RingElement  # any element of the ring
        >>> a + zero == a
        True
        >>> zero + a == a
        True
        """
        raise NotImplementedError('The zero method must be implemented')

    @classmethod
    def one(cls):
        """
        Returns the neutral element for multiplication.
        >>> one = RingElement.one()  # must return the neutral element
        >>> a: RingElement  # any element of the ring
        >>> a * one == a
        True
        >>> one * a == a
        True
        """
        raise NotImplementedError('The one method must be implemented')

    def __add__(self, other):
        """
        Addition of two ring elements.
        >>> a: RingElement
        >>> b: RingElement
        >>> c = a + b  # must return a ring element
        >>> assert isinstance(c, RingElement)
        """
        raise NotImplementedError('The __add__ method must be implemented')

    def __neg__(self):
        """
        Opposite of a ring element.
        >>> a: RingElement
        >>> b = -a  # must return a ring element
        >>> assert isinstance(b, RingElement)
        >>> a + b == RingElement.zero()
        True
        """
        raise NotImplementedError('The __neg__ method must be implemented')

    def __mul__(self, other):
        """
        Multiplication of two ring elements.
        >>> a: RingElement
        >>> b: RingElement
        >>> c = a * b  # must return a ring element
        >>> assert isinstance(c, RingElement)
        """
        raise NotImplementedError('The __mul__ method must be implemented')

    def __eq__(self, other):
        """
        Comparison of two ring elements.
        >>> a: RingElement
        >>> b: RingElement
        >>> result = (a == b)  # must return a boolean
        >>> assert isinstance(result, bool)
        """
        raise NotImplementedError('The __eq__ method must be implemented')

    def __repr__(self):
        """
        Readable representation of a ring element.
        >>> a: RingElement
        >>> repr_a = repr(a)  # must return a string
        >>> assert isinstance(repr_a, str)
        """
        raise NotImplementedError('The __repr__ method must be implemented')
```

## Exercise 1

In this exercise, you will implement the ring $\mathbb{Z}/n\mathbb{Z}$ of integers modulo $n$ and its operations.

### Instructions

Download the file `zmodn.py` and implement the methods:

* `__add__`
* `__neg__`
* `__mul__`
* `__eq__`
* `__repr__`
  of the class `Z_nZ`, which inherits from `RingElement`.

### Hints

* You can use the modulo operator `%` to obtain the remainder of Euclidean division.
* Do not modify `self.value` after initialization. You may add extra attributes if necessary.

## Exercise 2: Polynomial Ring $A[X]$

In this exercise, you will implement a ring of polynomials with coefficients in a ring $A$.

It is very important that your implementation be generic, meaning it should work with any class that follows the `RingElement` interface.

Your class must be able to handle formal polynomials, that is, expressions of the form:

$$ a_0 + a_1 X + a_2 X^2 + \dots + a_n X^n $$

with $a_i \in A$.

### Instructions

Download the file `poly_ring.py` and implement the methods:

* `__add__`
* `__neg__`
* `__sub__`
* `__mul__`
* `__eq__`
* `__repr__`
  of the class `Poly`, which inherits from `RingElement`.

### Functional Specifications

The class constructor has the following signature:

```python
class Poly(RingElement):
    def __init__(self, *coeffs: RingElement):
        """
        Initializes a polynomial with a tuple of coefficients.
        >>> P = Poly(3, 0, 2)  # represents the polynomial 3 + X^2 where coefficients are integers
        >>> Q = Poly(Z5(1), Z5(1), Z5(0))  # represents the polynomial [1]_5 + X with coefficients in Z/5Z
        """
        self.coeffs = coeffs
```

A polynomial is thus represented by a list of coefficients, where the index in the list corresponds to the degree of the term.

### Tips

* You can implement a function `normalize()` called at the end of each operation to remove trailing zeros.

### Extension (optional)

For the curious:

* Add a method `degree()` to return the degree of the polynomial;
* Add a method `__call__(self, x)` to evaluate the polynomial at a given value.

---

## Exercise 3.1: Euclidean Division in $\mathbb{Z}[i]$

In this exercise, you will implement **Euclidean division in the ring of Gaussian integers**:
$$
\mathbb{Z}[i] = { a + bi \mid a,b \in \mathbb{Z} }
$$

This is a **Euclidean ring** for the stathme:
$$
v(a+bi) = a^2 + b^2.
$$

For any two elements $z_1, z_2 \in \mathbb{Z}[i]$, there always exist $q, r \in \mathbb{Z}[i]$ such that:
$$
z_1 = qz_2 + r \quad \text{with} \quad v(r) < v(z_2).
$$

### Instructions

Download the file `gauss_division.py` and implement the class `Z_i`, inheriting from `EuclideanRingElement`, with the following methods:

* `__add__`
* `__neg__`
* `__sub__`
* `__mul__`
* `__eq__`
* `__repr__`
* `stathm`: returns the norm $a^2 + b^2$
* `__floordiv__` and `__mod__`: perform Euclidean division

## Exercise 3.2: Euclidean Division in $K[X]$

In this exercise, you will implement **Euclidean division** of two polynomials in $K[X]$, where $K$ is a field.

A field is a ring in which every nonzero element has a multiplicative inverse.
The corresponding interface is:

```python
class FieldElement(RingElement):
    def inverse(self):
        """
        Returns the multiplicative inverse of the element.
        >>> a: FieldElement  # a nonzero element of the field
        >>> b = a.inverse()  # must return a ring element
        >>> assert isinstance(b, FieldElement)
        >>> a * b == FieldElement.one()
        True
        """
        if self == self.zero():
            raise ZeroDivisionError('The zero element has no inverse')
        raise NotImplementedError('The inverse method must be implemented')
```

A polynomial ring over a field is a Euclidean ring, meaning that Euclidean division can be performed in it.
To do so, we need a notion of stathme and an implementation of Euclidean division.

We will therefore define a method `stathm()` returning an integer, and methods `__floordiv__` and `__mod__` corresponding to the Python operators `//` and `%`.

The interface of a Euclidean ring is thus:

```python
class EuclideanRingElement(RingElement):
    def stathm(self) -> int:
        """
        Returns the stathme of the element.
        >>> a: EuclideanRingElement
        >>> s = a.stathm()  # must return an integer
        >>> assert isinstance(s, int)
        """
        raise NotImplementedError('The stathm method must be implemented')

    def __floordiv__(self, other):
        """
        Euclidean division (quotient) of self by other.
        >>> a: EuclideanRingElement
        >>> b: EuclideanRingElement
        >>> q = a // b  # must return a ring element
        >>> assert isinstance(q, EuclideanRingElement)
        """
        raise NotImplementedError('The __floordiv__ method must be implemented')

    def __mod__(self, other):
        """
        Euclidean division (remainder) of self by other.
        >>> a: EuclideanRingElement
        >>> b: EuclideanRingElement
        >>> r = a % b  # must return a ring element
        >>> assert isinstance(r, EuclideanRingElement)
        """
```

### Instructions

Download the file `poly_division.py` and implement the methods:

* `stathm`
* `__floordiv__`
* `__mod__`

### Hints

* You can create an auxiliary function `poly_division(P, Q)` returning the pair `(S, R)` such that `P = Q*S + R` with `stathm(R) < stathm(Q)`.

---

## Exercise 4: Extended Euclidean Algorithm

In this exercise, you will implement **the extended Euclidean algorithm**, which computes Bézout coefficients for two elements of a Euclidean ring.

The goal is to find, for two elements $a$ and $b$ of a Euclidean ring, elements $u$ and $v$ such that:
$$
u \cdot a + v \cdot b = g
$$
where $g$ is the greatest common divisor (GCD) of $a$ and $b$.

---

### Instructions

Download the file `bezout.py` and implement the function:

```python
def extended_euclidean_algorithm(a: EuclideanRingElement, b: EuclideanRingElement):
    """
    Extended Euclidean algorithm.
    Returns (g, u, v) such that g = u*a + v*b.
    """
```

### Hints

* You must use a generic approach that works for any Euclidean ring.
  That means using the methods `__floordiv__` and `__mod__` for Euclidean division, and `zero()` and `one()` for neutral elements.

---

## Exercise 5: Quotient Ring of a Euclidean Ring

In this exercise, you will construct the **quotient ring** $A/(p)$ from a **Euclidean ring** $A$ and an element $p \in A$.

The goal is to dynamically create a class representing the elements of this quotient ring, with operations defined modulo the ideal $(p)$.

In other words, two elements $a, b \in A$ are identified if:
$$
a \equiv b \pmod{p} \quad \Longleftrightarrow \quad p \mid (a - b).
$$

---

### Instructions

Download the file `quotient_ring.py` and implement the function that creates the class:

```python
from typing import Type, TypeVar

T = TypeVar('T', bound='EuclideanRingElement')

def create_quotient_ring(A: Type[T], p: T):
    """
    Creates the class of the quotient ring A/(p).
    """
    class A_mod_p(RingElement):
        def inverse(self):
            """
            Returns the multiplicative inverse of the element in A/(p), if it exists.
            >>> a: A_mod_p  # a nonzero element of the quotient ring
            >>> try:
            >>>     b = a.inverse()  # must return an element of the quotient ring
            >>>     assert isinstance(b, A_mod_p)
            >>> except NotInvertibleError:
            >>>     pass  # the element is not invertible
            """
```

This function must return a class `A_mod_p` whose elements are representatives of equivalence classes modulo $(p)$.

### Hints

* Use the `__mod__` method of your Euclidean ring to reduce representatives.
* The inverse computation directly relies on the `extended_euclidean_algorithm` function implemented in the previous exercise.

---

## Grading

| Exercise                                  | File                    | Points  |
| ----------------------------------------- | ----------------------- | ------- |
| 1. Ring $\mathbb{Z}/n\mathbb{Z}$          | `zmodn.py`              | 10      |
| 2. Ring $A[X]$                            | `poly_ring.py`          | 15      |
| 3.1 Euclidean division in $\mathbb{Z}[i]$ | `gauss_division.py`     | 15      |
| 3.2 Euclidean division of polynomials     | `euclidean_division.py` | 20      |
| 4. Extended Euclidean algorithm           | `bezout.py`             | 20      |
| 5. Quotient ring                          | `quotient_ring.py`      | 20      |
|                                           | **Total**               | **100** |
