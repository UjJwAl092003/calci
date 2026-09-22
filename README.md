# Cryptography Calculator

An arbitrary-precision decimal integer arithmetic and cryptographic calculator developed in C++ with an interactive web demonstration. The project supports arbitrary-precision integers exceeding 512 bits (~155 decimal digits) and features a comprehensive automated verification engine comparing a manual scratch implementation against Boost multiprecision reference arithmetic.

---

## Live Demo

Try the interactive web calculator online:

👉 [https://ujjwal092003.github.io/calci/](https://ujjwal092003.github.io/calci/)

---

## Features

The calculator implements 12 core arithmetic and cryptographic operations:

1. **Addition (`+`)**: Arbitrary-precision column addition with carry and signed operand decomposition.
2. **Subtraction (`-`)**: Digit-by-digit magnitude borrow subtraction supporting signed numbers.
3. **Multiplication (`*`)**: Grade-school cross-product multiplication with carry propagation.
4. **Division (`/`)**: Truncated long division using a shift-and-subtract quotient estimation approach.
5. **Modulo (`%`)**: Canonical integer remainder reduction.
6. **Euclidean GCD**: Greatest common divisor computed using the iterative Euclidean algorithm ($\gcd(a, b) = \gcd(|a|, |b|) \ge 0$).
7. **Extended Euclidean GCD**: Computes $\gcd(a, b)$ and integer Bézout coefficients $(x, y)$ such that $a \cdot x + b \cdot y = \gcd(a, b)$ for all signed inputs.
8. **Modular Addition**: Canonical addition $((a \bmod M) + (b \bmod M)) \bmod M$ with modulus normalization.
9. **Modular Multiplication**: Canonical multiplication $((a \bmod M) \cdot (b \bmod M)) \bmod M$.
10. **Modular Multiplicative Inverse**: Computes $a^{-1} \pmod m$ using the Extended Euclidean algorithm. Detects non-coprime cases when $\gcd(a, m) \neq 1$.
11. **Naive Modular Exponentiation**: Direct repeated multiplication $a^{\text{exp}} \pmod M$ (intended for small exponents and complexity comparison).
12. **Square-and-Multiply Exponentiation**: Binary repeated squaring $O(\log \text{exp})$ handling arbitrary 512-bit exponents.

---

## Implementation

The project maintains two distinct implementations:

* **Manual Scratch Implementation (`crypto.cpp`)**:
  - Implements all arithmetic operations from scratch using `std::string` for dynamic base-10 digit storage.
  - Zero external big-integer library dependencies.
  - No fixed 64-bit or 512-bit integer length limitations.
* **Predefined Reference Implementation (`predefined.hpp`)**:
  - Uses `boost::multiprecision::cpp_int` from the Boost C++ Libraries.
  - Acts as an authoritative ground-truth reference for mathematical verification and performance comparisons.

---

## Verification

The automated verification suite focuses on core number-theoretic and cryptographic operations, executing **28 automated test cases** (7 operations $\times$ 4 test categories):

* **Operations Evaluated**:
  1. Euclidean GCD
  2. Extended Euclidean GCD (with Bézout identity verification)
  3. Modular Addition
  4. Modular Multiplication
  5. Modular Multiplicative Inverse
  6. Naive Modular Exponentiation
  7. Square-and-Multiply Modular Exponentiation (512-bit)

* **Categories Evaluated (4 Cases per Operation)**:
  1. Normal / small values
  2. Different-sized / large values
  3. Negative values and negative moduli
  4. Large / ~512-bit values & edge cases

### Correctness Comparison
Every test case compares the output of the **Manual Implementation** against the **Library Implementation** (`boost::multiprecision::cpp_int` reference):
* Modular operations and GCD must produce identical results.
* Extended GCD verifies $\gcd(a, b)$ and independently asserts the Bézout identity: $a \cdot x + b \cdot y == \gcd(a, b)$.
* Modular Inverse confirms $(a \cdot x) \bmod M == 1$ when an inverse exists, and correctly reports `"INVERSE DOES NOT EXIST (gcd != 1)"` when $\gcd(a, m) \neq 1$.

### Performance Measurement
For every test case, execution times are measured independently using `std::chrono::high_resolution_clock`:
* **Manual Implementation Time** is measured separately around the custom algorithm call.
* **Library Implementation Time** is measured separately around the Boost reference call.
* Timers isolate function execution and strictly exclude I/O, parsing, and console printing.

---

## Negative Number and Modulus Handling

* **Negative Operands**: Inputs are parsed into a sign flag and an absolute magnitude string. All basic operations decompose into magnitude arithmetic with proper sign assignment.
* **Bézout Identity for Extended GCD**: When input $a < 0$, the coefficient $x$ is negated ($x = -x_{abs}$), and similarly for $y$ when $b < 0$. This ensures $a \cdot x + b \cdot y = \gcd(a, b)$ holds strictly for all signed inputs.
* **Negative Modulus Normalization**: Modular arithmetic normalizes any modulus $m$ to its absolute value $M = |m|$. Remainder results are always mapped into the standard canonical non-negative range $r \in [0, M - 1]$ by applying $r = (r \bmod M + M) \bmod M$.

---

## How to Compile

A standard C++ compiler supporting C++14 or C++17 (such as `g++`) is required along with Boost headers.

```bash
g++ -std=c++14 -O2 crypto.cpp -o crypto
```

---

## How to Run

### Interactive Terminal Mode
```bash
./crypto
```

### Automated 72-Test Verification Suite
```bash
./crypto test
```

### Direct CLI Commands
```bash
# Basic Arithmetic
./crypto add 12345678901234567890 98765432109876543210
./crypto sub 100000000000 1
./crypto mul 123456789 987654321
./crypto div 100 7

# Number Theory
./crypto gcd 48 -18
./crypto extgcd -30 12

# Modular Arithmetic & Exponentiation
./crypto modadd -17 3 -5
./crypto modmul -12 -5 7
./crypto modinv 3 11
./crypto modpow 2 1000 1000000007

# 512-bit Random Number Generator
./crypto rand512
```

---

## Project Structure

```text
calci/
├── crypto.cpp        # Manual scratch implementation, test runner, CLI, and interactive terminal menu
├── predefined.hpp    # Reference implementation wrapping boost::multiprecision::cpp_int
├── index.html        # Standalone web calculator UI and verification runner
└── README.md         # Project documentation and usage guide
```

---

## License / Notes

Academic project developed for Cryptography Laboratory (CS800). Distributed for educational and evaluation purposes.
