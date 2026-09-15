# Cryptography Lab - Big Integer Arithmetic & Extended Euclidean Algorithm

This repository contains the C++ implementation for the **Cryptography Lab (Lab 1)** focusing on arbitrary-precision arithmetic, Euclidean GCD, and the Extended Euclidean Algorithm.

---

## Overview

In cryptographic algorithms (such as RSA and Diffie-Hellman), numbers often exceed the capacity of standard 32-bit and 64-bit primitive integer types. This project implements arbitrary-precision decimal arithmetic from first principles using standard C++ (`std::string`), without relying on external libraries such as Boost or GMP.

The code supports integers of **at least 512 bits** (~155 decimal digits).

---

## Implemented Operations

1. **Addition (`+`)**: Column-by-column addition with carry propagation.
2. **Subtraction (`-`)**: Column-by-column subtraction with borrow handling ($a \ge b$ for unsigned, full support for signed results).
3. **Multiplication (`*`)**: Grade-school multiplication with positional cross-products.
4. **Division (`/`) & Modulo (`%`)**: Long division producing both quotient and remainder.
5. **Euclidean GCD**: Classical Euclidean algorithm computing $\gcd(a, b) = \gcd(b, a \pmod b)$ until remainder is 0.
6. **Extended Euclidean GCD**: Iterative Extended Euclidean Algorithm computing $\gcd(a, b)$ and Bézout coefficients $x, y \in \mathbb{Z}$ such that:
   $$a \cdot x + b \cdot y = \gcd(a, b)$$
   Supports negative values for $x$ and $y$.
7. **Bézout Identity Verification**: Computes $a \cdot x + b \cdot y$ and automatically verifies equality with $\gcd(a, b)$ (`PASS` / `FAIL`).
8. **512-bit Number Generation**: Generates random numbers $\ge 2^{511}$ (~155 decimal digits) using bitwise double-and-add.

---

## Repository Structure

```text
calci/
├── src/
│   └── main.cpp      # Complete portable C++ source code
├── README.md         # Project documentation and lab guide
└── .gitignore        # Standard Git ignore rules
```

---

## Compilation & Execution (Linux / Unix / macOS / Windows)

### Prerequisites
- A standard C++ compiler (`g++` supporting C++17 or C++11).
- No external libraries required (only standard C++ library headers).

### 1. Compilation
From the project root directory:

```bash
g++ -std=c++17 src/main.cpp -o crypto_lab
```

### 2. Running Interactive Menu
Run the compiled binary without arguments to access the interactive calculator menu:

```bash
./crypto_lab
```

#### Menu Preview:
```text
=========================================
      CRYPTOGRAPHY LAB - CALCULATOR
=========================================
 1. Addition (+)
 2. Subtraction (-)
 3. Multiplication (*)
 4. Division (/)
 5. Euclidean GCD
 6. Extended Euclidean GCD
 7. Generate 512-bit Number
 8. Exit
=========================================
Enter your choice (1-8):
```

### 3. Running via Command-Line (CLI Mode)
You can also run specific operations directly via terminal arguments:

```bash
# Addition
./crypto_lab add 45 55

# Subtraction
./crypto_lab sub 100 37

# Multiplication
./crypto_lab mul 123456789 987654321

# Division (prints quotient and remainder)
./crypto_lab div 1000 7

# Euclidean GCD
./crypto_lab gcd 48 18

# Extended Euclidean GCD (prints gcd, x, y, and Bézout verification)
./crypto_lab extgcd 30 12

# Generate a random 512-bit decimal number
./crypto_lab rand512
```

---

## Example Output (Extended Euclidean Algorithm)

```text
Enter First Number:  30
Enter Second Number: 12

-----------------------------------------
GCD: 6
x:   1
y:   -2
Bézout Identity: a*x + b*y = gcd(a,b)
Verification:    PASS
-----------------------------------------
```

Check: $30(1) + 12(-2) = 30 - 24 = 6 = \gcd(30, 12)$.
