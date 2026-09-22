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

The automated verification suite executes **72 automated test cases** (12 operations $\times$ 6 test categories):

* **Categories Evaluated**:
  1. Small numbers
  2. Numbers of different lengths
  3. Large numbers (~40 digits / 128-bit)
  4. 512-bit integers (~155 digits)
  5. Negative numbers and negative moduli
  6. Edge cases (zeros, identical values, non-coprime inputs)

### Correctness Comparison
Every test case compares the output of the **Scratch implementation** against the **Boost `cpp_int` reference implementation**:
* Arithmetic and modular operations must produce identical string representations.
* Extended GCD is verified by checking the GCD match and independently testing the Bézout identity: $a \cdot x + b \cdot y == \gcd(a, b)$.
* Modular Inverse is verified by confirming $(a \cdot x) \bmod M == 1$ when an inverse exists, or verifying error detection when $\gcd(a, m) \neq 1$.

### Performance Measurement
For every test case, execution times are measured independently using `std::chrono::high_resolution_clock`:
* **Scratch execution time** is measured separately around the manual function call.
* **Predefined execution time** is measured separately around the Boost reference call.
* Timers isolate the calculation logic and exclude I/O, parsing, and console printing.

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
