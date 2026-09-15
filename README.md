# Cryptography Lab - Big Integer Arithmetic & Extended Euclidean Algorithm

This project implements fundamental number-theoretic operations for **Cryptography Lab (Lab 1)**, handling large integers of **at least 512 bits** (~155 decimal digits) without external libraries.

---

## Project Structure

The project consists of two clean, self-contained files:

```text
calci/
├── crypto.cpp    # Complete standalone C++ program (algorithms + interactive menu)
└── index.html    # Standalone calculator UI (runs directly in any browser)
```

---

## 1. C++ Implementation (`crypto.cpp`)

Contains the complete C++ implementation using standard `std::string` arithmetic:

1. **Addition (`+`)**: Column-by-column addition with carry propagation.
2. **Subtraction (`-`)**: Column-by-column subtraction with borrow handling.
3. **Multiplication (`*`)**: Grade-school multiplication with cross-products.
4. **Division (`/`) & Modulo (`%`)**: Long division producing quotient and remainder.
5. **Euclidean GCD**: Computes $\gcd(a, b) = \gcd(b, a \pmod b)$ until remainder is 0.
6. **Extended Euclidean GCD**: Computes $\gcd(a, b)$ and Bézout coefficients $x, y \in \mathbb{Z}$ such that:
   $$a \cdot x + b \cdot y = \gcd(a, b)$$
   Supports negative values for $x$ and $y$.
7. **Bézout Identity Verification**: Evaluates $a \cdot x + b \cdot y == \gcd(a, b)$ and reports `PASS` or `FAIL`.
8. **512-bit Random Number Generator**: Generates random numbers $\ge 2^{511}$ (~155 digits) in milliseconds using bitwise double-and-add.

### How to Compile & Run on Linux / Unix / macOS / Windows

```bash
# 1. Compile
g++ -std=c++17 crypto.cpp -o crypto

# 2. Run Interactive Menu
./crypto

# 3. (Optional) Run Direct CLI Command
./crypto extgcd 30 12
./crypto rand512
```

---

## 2. Web Calculator (`index.html`)

A single, self-contained web file containing HTML, CSS, and JavaScript in one file.
- **No server required**: Double-click `index.html` to open it in any web browser.
- **Identical logic**: Implements the same string-based big integer arithmetic directly in client-side JavaScript.
- **Live Demo**: [https://ujjwal092003.github.io/calci/](https://ujjwal092003.github.io/calci/)
