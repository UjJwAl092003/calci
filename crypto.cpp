#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>

// ============================================================================
// Cryptography Lab - Large Integer Arithmetic, Euclidean GCD & Extended GCD
//
// All operations are implemented directly on std::string (decimal digits).
// Arbitrary-precision (>= 512 bits) without external libraries (No Boost, No GMP).
// ============================================================================

// Remove leading zeros from a decimal string (e.g., "007" -> "7", "000" -> "0")
std::string trim(std::string s) {
    bool neg = false;
    if (!s.empty() && s[0] == '-') {
        neg = true;
        s = s.substr(1);
    }
    size_t first = s.find_first_not_of('0');
    if (first == std::string::npos) {
        return "0";
    }
    std::string res = s.substr(first);
    return neg ? ("-" + res) : res;
}

// Compare two non-negative decimal strings:
// Returns: -1 if a < b, 0 if a == b, 1 if a > b
int compare(const std::string& a, const std::string& b) {
    std::string s1 = trim(a);
    std::string s2 = trim(b);
    if (s1.length() < s2.length()) return -1;
    if (s1.length() > s2.length()) return 1;
    if (s1 < s2) return -1;
    if (s1 > s2) return 1;
    return 0;
}

// Column-by-column addition of two non-negative decimal strings
std::string add(const std::string& a, const std::string& b) {
    std::string s1 = trim(a);
    std::string s2 = trim(b);
    std::string result = "";
    int i = (int)s1.length() - 1;
    int j = (int)s2.length() - 1;
    int carry = 0;

    while (i >= 0 || j >= 0 || carry > 0) {
        int d1 = (i >= 0) ? (s1[i] - '0') : 0;
        int d2 = (j >= 0) ? (s2[j] - '0') : 0;
        int sum = d1 + d2 + carry;
        result.push_back((sum % 10) + '0');
        carry = sum / 10;
        i--;
        j--;
    }
    std::reverse(result.begin(), result.end());
    return trim(result);
}

// Column-by-column subtraction: assumes a >= b >= 0
std::string subtract(const std::string& a, const std::string& b) {
    std::string s1 = trim(a);
    std::string s2 = trim(b);
    if (s1 == s2) return "0";

    std::string result = "";
    int i = (int)s1.length() - 1;
    int j = (int)s2.length() - 1;
    int borrow = 0;

    while (i >= 0) {
        int d1 = s1[i] - '0' - borrow;
        int d2 = (j >= 0) ? (s2[j] - '0') : 0;
        if (d1 < d2) {
            d1 += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.push_back((d1 - d2) + '0');
        i--;
        j--;
    }
    std::reverse(result.begin(), result.end());
    return trim(result);
}

// Grade-school multiplication of two non-negative decimal strings
std::string multiply(const std::string& a, const std::string& b) {
    std::string s1 = trim(a);
    std::string s2 = trim(b);
    if (s1 == "0" || s2 == "0") return "0";

    int n = (int)s1.length();
    int m = (int)s2.length();
    std::vector<int> res(n + m, 0);

    for (int i = n - 1; i >= 0; i--) {
        for (int j = m - 1; j >= 0; j--) {
            int mul = (s1[i] - '0') * (s2[j] - '0');
            int p1 = i + j;
            int p2 = i + j + 1;
            int sum = mul + res[p2];

            res[p2] = sum % 10;
            res[p1] += sum / 10;
        }
    }

    std::string s = "";
    for (int digit : res) {
        if (!(s.empty() && digit == 0)) {
            s.push_back(digit + '0');
        }
    }
    return s.empty() ? "0" : s;
}

// Long division of non-negative decimal strings: returns quotient and remainder
void divide(const std::string& a, const std::string& b, std::string& quotient, std::string& remainder) {
    std::string dividend = trim(a);
    std::string divisor = trim(b);

    if (divisor == "0") {
        quotient = "UNDEFINED (division by zero)";
        remainder = "0";
        return;
    }

    if (compare(dividend, divisor) < 0) {
        quotient = "0";
        remainder = dividend;
        return;
    }

    quotient = "";
    remainder = "";

    for (size_t i = 0; i < dividend.length(); i++) {
        remainder.push_back(dividend[i]);
        remainder = trim(remainder);

        int count = 0;
        while (compare(remainder, divisor) >= 0) {
            remainder = subtract(remainder, divisor);
            count++;
        }
        quotient.push_back(count + '0');
    }

    quotient = trim(quotient);
    remainder = trim(remainder);
}

// Modulo wrapper: returns a % b
std::string modulo(const std::string& a, const std::string& b) {
    std::string q, r;
    divide(a, b, q, r);
    return r;
}

// ============================================================================
// SIGNED BIG-INTEGER HELPERS (Needed because Bezout x and y can be negative)
// ============================================================================

bool isNegative(const std::string& s) {
    return !s.empty() && s[0] == '-';
}

std::string absVal(const std::string& s) {
    if (isNegative(s)) return s.substr(1);
    return s;
}

std::string negateVal(const std::string& s) {
    std::string t = trim(s);
    if (t == "0") return "0";
    if (isNegative(t)) return t.substr(1);
    return "-" + t;
}

std::string addSigned(const std::string& a, const std::string& b) {
    bool negA = isNegative(a);
    bool negB = isNegative(b);
    std::string absA = absVal(a);
    std::string absB = absVal(b);

    if (!negA && !negB) {
        return add(absA, absB);
    } else if (negA && negB) {
        std::string sum = add(absA, absB);
        return sum == "0" ? "0" : ("-" + sum);
    } else if (!negA && negB) {
        int cmp = compare(absA, absB);
        if (cmp >= 0) return subtract(absA, absB);
        else return "-" + subtract(absB, absA);
    } else { // negA && !negB
        int cmp = compare(absB, absA);
        if (cmp >= 0) return subtract(absB, absA);
        else return "-" + subtract(absA, absB);
    }
}

std::string subtractSigned(const std::string& a, const std::string& b) {
    return addSigned(a, negateVal(b));
}

std::string multiplySigned(const std::string& a, const std::string& b) {
    bool negA = isNegative(a);
    bool negB = isNegative(b);
    std::string prod = multiply(absVal(a), absVal(b));
    if (prod == "0") return "0";
    if (negA ^ negB) {
        return "-" + prod;
    }
    return prod;
}

// ============================================================================
// NUMBER THEORY: EUCLIDEAN & EXTENDED EUCLIDEAN ALGORITHM
// ============================================================================

// Euclidean GCD: repeatedly gcd(a, b) = gcd(b, a % b) until b == 0
std::string gcd(std::string a, std::string b) {
    a = absVal(trim(a));
    b = absVal(trim(b));

    while (b != "0") {
        std::string q, r;
        divide(a, b, q, r);
        a = b;
        b = r;
    }
    return a;
}

// Extended Euclidean Algorithm:
// Finds gcd(a, b) and integers x, y such that:
// a * x + b * y = gcd(a, b)
// Iterative state tracking:
// r_0 = a, r_1 = b
// s_0 = 1, s_1 = 0
// t_0 = 0, t_1 = 1
std::string extendedGCD(std::string a, std::string b, std::string& x, std::string& y) {
    a = absVal(trim(a));
    b = absVal(trim(b));

    std::string old_r = a, r = b;
    std::string old_s = "1", s = "0";
    std::string old_t = "0", t = "1";

    while (r != "0") {
        std::string q, rem;
        divide(old_r, r, q, rem);

        old_r = r;
        r = rem;

        // new_s = old_s - q * s
        std::string new_s = subtractSigned(old_s, multiplySigned(q, s));
        old_s = s;
        s = new_s;

        // new_t = old_t - q * t
        std::string new_t = subtractSigned(old_t, multiplySigned(q, t));
        old_t = t;
        t = new_t;
    }

    x = old_s;
    y = old_t;
    return old_r; // gcd(a, b)
}

// Bézout Identity Verification:
// Calculates a * x + b * y and verifies if it equals gcdVal
bool verifyBezout(const std::string& a, const std::string& b, const std::string& x, const std::string& y, const std::string& gcdVal) {
    std::string term1 = multiplySigned(a, x);
    std::string term2 = multiplySigned(b, y);
    std::string lhs = addSigned(term1, term2);
    return (trim(lhs) == trim(gcdVal));
}

// Generate a random 512-bit decimal number (>= 2^511)
// 512-bit numbers have ~154-155 decimal digits
std::string generateRandom512Bit() {
    // Generate 512 bits with MSB set to 1
    std::mt19937_64 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 1);

    std::vector<int> bits(512);
    bits[0] = 1; // Ensure MSB is 1 so it's strictly >= 2^511
    for (int i = 1; i < 512; i++) {
        bits[i] = dist(rng);
    }

    // Convert binary to decimal using double-and-add: dec = dec * 2 + bit
    std::string dec = "0";
    for (int i = 0; i < 512; i++) {
        std::string doubled = add(dec, dec);
        dec = add(doubled, std::to_string(bits[i]));
    }
    return dec;
}

// ============================================================================
// TERMINAL / MENU CONTROLLER
// ============================================================================

void printMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "      CRYPTOGRAPHY LAB - CALCULATOR\n";
    std::cout << "=========================================\n";
    std::cout << " 1. Addition (+)\n";
    std::cout << " 2. Subtraction (-)\n";
    std::cout << " 3. Multiplication (*)\n";
    std::cout << " 4. Division (/)\n";
    std::cout << " 5. Euclidean GCD\n";
    std::cout << " 6. Extended Euclidean GCD\n";
    std::cout << " 7. Generate 512-bit Number\n";
    std::cout << " 8. Exit\n";
    std::cout << "=========================================\n";
    std::cout << "Enter your choice (1-8): " << std::flush;
}

int main(int argc, char* argv[]) {
    // ------------------------------------------------------------------------
    // CLI Mode: Direct evaluation via command-line arguments
    // ------------------------------------------------------------------------
    if (argc > 1) {
        std::string cmd = argv[1];

        if (cmd == "rand512") {
            std::cout << generateRandom512Bit() << std::endl;
            return 0;
        }

        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " <add|sub|mul|div|gcd|extgcd|rand512> <num1> <num2>" << std::endl;
            return 1;
        }

        std::string a = argv[2];
        std::string b = argv[3];

        if (cmd == "add") {
            std::cout << addSigned(a, b) << std::endl;
        } else if (cmd == "sub") {
            std::cout << subtractSigned(a, b) << std::endl;
        } else if (cmd == "mul") {
            std::cout << multiplySigned(a, b) << std::endl;
        } else if (cmd == "div") {
            std::string q, r;
            divide(a, b, q, r);
            std::cout << "Quotient:  " << q << "\nRemainder: " << r << std::endl;
        } else if (cmd == "gcd") {
            std::cout << gcd(a, b) << std::endl;
        } else if (cmd == "extgcd") {
            std::string x, y;
            std::string g = extendedGCD(a, b, x, y);
            bool verified = verifyBezout(a, b, x, y, g);
            std::cout << "GCD: " << g << "\nx:   " << x << "\ny:   " << y << "\n";
            std::cout << "Bézout Verification: " << (verified ? "PASS" : "FAIL") << std::endl;
        } else {
            std::cerr << "Unknown command: " << cmd << std::endl;
            return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------------
    // Interactive Terminal Mode: Standard menu for lab viva and evaluation
    // ------------------------------------------------------------------------
    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string discard;
            if (!(std::cin >> discard)) {
                break; // End of input (EOF)
            }
            continue;
        }

        if (choice == 8) {
            std::cout << "Exiting calculator. Goodbye!\n";
            break;
        }

        if (choice == 7) {
            std::cout << "\nGenerated 512-bit Number (decimal, ~155 digits):\n";
            std::cout << generateRandom512Bit() << "\n";
            continue;
        }

        if (choice < 1 || choice > 8) {
            std::cout << "Invalid choice! Please select 1-8.\n";
            continue;
        }

        std::string num1, num2;
        std::cout << "\nEnter First Number:  " << std::flush;
        std::cin >> num1;
        std::cout << "Enter Second Number: " << std::flush;
        std::cin >> num2;

        std::cout << "\n-----------------------------------------\n";
        switch (choice) {
            case 1:
                std::cout << "Result of Addition (+):\n" << addSigned(num1, num2) << "\n";
                break;
            case 2:
                std::cout << "Result of Subtraction (-):\n" << subtractSigned(num1, num2) << "\n";
                break;
            case 3:
                std::cout << "Result of Multiplication (*):\n" << multiplySigned(num1, num2) << "\n";
                break;
            case 4: {
                std::string q, r;
                divide(num1, num2, q, r);
                std::cout << "Quotient:  " << q << "\n";
                std::cout << "Remainder: " << r << "\n";
                break;
            }
            case 5:
                std::cout << "Euclidean GCD:\n" << gcd(num1, num2) << "\n";
                break;
            case 6: {
                std::string x, y;
                std::string g = extendedGCD(num1, num2, x, y);
                bool pass = verifyBezout(num1, num2, x, y, g);
                std::cout << "GCD: " << g << "\n";
                std::cout << "x:   " << x << "\n";
                std::cout << "y:   " << y << "\n";
                std::cout << "Bézout Identity: a*x + b*y = gcd(a,b)\n";
                std::cout << "Verification:    " << (pass ? "PASS" : "FAIL") << "\n";
                break;
            }
        }
        std::cout << "-----------------------------------------\n";
    }

    return 0;
}
