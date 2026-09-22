# Cryptography Lab - Big Integer Arithmetic & Extended Euclidean Algorithm

[![Verification](https://img.shields.io/badge/Verification-72%2F72%20PASS-2ea44f?style=for-the-badge)](https://github.com/UjJwAl092003/calci)

Complete implementation of fundamental number-theoretic and cryptographic operations supporting **at least 512-bit integers** (~155 decimal digits), with side-by-side automated verification between a manual **Scratch implementation** and an authoritative **Predefined Boost Reference**.

---

## 1. Project Architecture

The project consists of simple, portable, student-friendly files:

```text
calci/
├── crypto.cpp        # Scratch C++ implementation + Verification & Timing Engine + CLI & Menu
├── predefined.hpp    # Reference implementation using boost::multiprecision::cpp_int
├── index.html        # Standalone client-side calculator UI with test runner (no server needed)
├── README.md         # Full documentation & viva guide
└── .gitignore        # Ignores build artifacts and compiled executables
```

---

## 2. Implemented Operations (12 Operations)

| # | Operation | Description & Mathematical Convention |
| :---: | :--- | :--- |
| **1** | **Addition (`+`)** | Column-by-column addition with carry; full support for negative operands. |
| **2** | **Subtraction (`-`)** | Column-by-column subtraction with borrow; full support for negative operands. |
| **3** | **Multiplication (`*`)** | Grade-school cross-product multiplication with sign handling. |
| **4** | **Division (`/`)** | Truncated integer division: $a = b \cdot q + r$, with quotient sign $(negA \oplus negB)$. |
| **5** | **Modulo (`%`)** | Truncated remainder matching the sign of dividend $a$. |
| **6** | **Euclidean GCD** | Non-negative greatest common divisor: $\gcd(a, b) = \gcd(\|a\|, \|b\|) \ge 0$. |
| **7** | **Extended Euclidean GCD** | Computes $\gcd(a, b)$ and signed $x, y \in \mathbb{Z}$ satisfying $a \cdot x + b \cdot y = \gcd(a, b)$. |
| **8** | **Modular Addition** | $((a_{\text{norm}} + b_{\text{norm}}) \pmod M)$, with $M = \|m\|$ and result in $[0, M-1]$. |
| **9** | **Modular Multiplication** | $((a_{\text{norm}} \times b_{\text{norm}}) \pmod M)$, with $M = \|m\|$ and result in $[0, M-1]$. |
| **10** | **Modular Inverse** | Finds $a^{-1} \pmod m$ using Extended GCD. Returns failure if $\gcd(a, m) \neq 1$. |
| **11** | **Naive Modular Power** | Computes $a^{\text{exp}} \pmod M$ via repeated multiplication (for complexity comparison). |
| **12** | **Square-and-Multiply** | Binary repeated squaring $O(\log \text{exp})$ handling arbitrary 512-bit exponents. |

---

## 3. Mathematical Conventions (Viva Preparation)

### Negative Number Handling
* **GCD**: Follows the universal standard $\gcd(a, b) = \gcd(\|a\|, \|b\|) \ge 0$.
* **Extended GCD**: Signed inputs $a$ and $b$ are adjusted so that the Bézout identity $a \cdot x + b \cdot y = \gcd(a, b)$ strictly holds true.

### Negative Modulus Normalization
* For all modular operations, the modulus is normalized to $M = \|m\|$.
* Any remainder is normalized into the standard range:
  $$r = a \pmod M, \quad \text{if } r < 0 \implies r = r + M \implies 0 \le r < M$$
* Examples:
  * $\text{modAdd}(-17, 3, -5) \implies M = 5, (-17 \equiv 3 \pmod 5) \implies (3 + 3) \pmod 5 = 1$.
  * $\text{modInverse}(-3, -11) \implies M = 11, (-3 \equiv 8 \pmod{11}) \implies 8^{-1} \equiv 7 \pmod{11}$ (since $8 \times 7 = 56 = 5(11) + 1$).

### Modular Exponentiation
* **Square-and-Multiply**: The exponent is processed bit-by-bit from LSB to MSB using custom string division by 2.
* Intermediate multiplications are reduced modulo $M$ at each step, preventing values from growing beyond $M^2$.

---

## 4. Compilation & Execution (Linux / Unix / macOS / Windows)

### Prerequisites
* Standard C++ compiler supporting C++14 or C++17 (`g++ -std=c++14` or `-std=c++17`).
* `boost/multiprecision/cpp_int.hpp` (for `predefined.hpp` reference verification).

### Compilation:
```bash
g++ -std=c++14 -O2 crypto.cpp -o crypto
```

### Running the Interactive Menu:
```bash
./crypto
```

### Running the Automated 72-Test Verification Suite:
```bash
./crypto test
```

### Running Direct CLI Commands:
```bash
./crypto add 12 5
./crypto sub 100 37
./crypto mul 123456789 987654321
./crypto div 100 7
./crypto gcd 48 -18
./crypto extgcd -30 12
./crypto modadd -17 3 -5
./crypto modmul -12 -5 7
./crypto modinv 3 11
./crypto modpow 2 10 1000
./crypto rand512
```

---

## 5. Automated Verification & Timing Summary

Running `./crypto test` executes **6 meaningful test cases across all 12 operations (72 total tests)** comparing the Scratch engine vs. Boost reference:

```text
====================================================
                 VERIFICATION SUMMARY
====================================================

Addition                    6/6 PASS
Subtraction                 6/6 PASS
Multiplication              6/6 PASS
Division                    6/6 PASS
Modulo                      6/6 PASS
GCD                         6/6 PASS
Extended GCD                6/6 PASS
Modular Addition            6/6 PASS
Modular Multiplication      6/6 PASS
Modular Inverse             6/6 PASS
Naive Modular Power         6/6 PASS
Square-and-Multiply         6/6 PASS

----------------------------------------------------
Overall: PASS (72/72 tests passed)
====================================================
```

* **Timing**: Measured separately using `std::chrono::high_resolution_clock` directly around the operation calls.
* **Extended GCD Verification**: Verifies both $\gcd$ and Bézout's identity $a \cdot x + b \cdot y == \gcd(a, b)$.
* **Modular Inverse Verification**: Verifies $(a \cdot \text{inv}) \pmod M == 1$ and handles non-coprime cases.
