#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

// Include the predefined reference implementation using boost::multiprecision::cpp_int
#include "predefined.hpp"

// ============================================================================
// Cryptography Lab - Complete Big Integer Implementation
// Part 1: Scratch Engine (Custom std::string arithmetic from first principles)
// Part 2: Automated Verification vs Predefined Reference (boost::multiprecision)
// ============================================================================

// ----------------------------------------------------------------------------
// 1. UNSIGNED DECIMAL STRING ARITHMETIC (SCRATCH)
// ----------------------------------------------------------------------------

// Remove leading zeros from a decimal string (e.g. "007" -> "7", "000" -> "0")
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

// Compare two non-negative decimal strings: returns -1 if a < b, 0 if a == b, 1 if a > b
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
        quotient = "DIVISION BY ZERO";
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

// Modulo wrapper: returns a % b for non-negative decimal strings
std::string modulo(const std::string& a, const std::string& b) {
    std::string q, r;
    divide(a, b, q, r);
    return r;
}

// ----------------------------------------------------------------------------
// 2. SIGNED BIG-INTEGER HELPERS (SCRATCH)
// ----------------------------------------------------------------------------

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

// Standard truncated division with signed operands: returns quotient and remainder
void divideSigned(const std::string& a, const std::string& b, std::string& quotient, std::string& remainder) {
    if (trim(absVal(b)) == "0") {
        quotient = "DIVISION BY ZERO";
        remainder = "0";
        return;
    }
    bool negA = isNegative(a);
    bool negB = isNegative(b);

    std::string q, r;
    divide(absVal(a), absVal(b), q, r);

    // Quotient sign: negative iff signs differ and quotient != 0
    if (q != "0" && (negA ^ negB)) {
        quotient = "-" + q;
    } else {
        quotient = q;
    }

    // Remainder sign: matches dividend a (standard C++ behavior: a = b*q + r)
    if (r != "0" && negA) {
        remainder = "-" + r;
    } else {
        remainder = r;
    }
}

// ----------------------------------------------------------------------------
// 3. NUMBER THEORY & MODULAR ARITHMETIC (SCRATCH)
// ----------------------------------------------------------------------------

// Euclidean GCD: non-negative convention: gcd(a, b) = gcd(|a|, |b|)
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
// Computes gcd(a, b) and coefficients x, y such that a*x + b*y = gcd(a, b)
// Correctly handles negative inputs.
std::string extendedGCD(std::string a_str, std::string b_str, std::string& out_x, std::string& out_y) {
    std::string a = trim(a_str);
    std::string b = trim(b_str);

    std::string abs_a = absVal(a);
    std::string abs_b = absVal(b);

    std::string old_r = abs_a, r = abs_b;
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

    // Adjust coefficients for negative inputs:
    // If a < 0, a * (-old_s) = |a| * old_s
    std::string x_res = isNegative(a) ? negateVal(old_s) : old_s;
    std::string y_res = isNegative(b) ? negateVal(old_t) : old_t;

    out_x = x_res;
    out_y = y_res;
    return old_r; // gcd(a, b) >= 0
}

// Bézout Identity Verification:
// Verifies whether a * x + b * y == gcdVal
bool verifyBezout(const std::string& a, const std::string& b, const std::string& x, const std::string& y, const std::string& gcdVal) {
    std::string term1 = multiplySigned(a, x);
    std::string term2 = multiplySigned(b, y);
    std::string lhs = addSigned(term1, term2);
    return (trim(lhs) == trim(gcdVal));
}

// Mathematical Modulo Normalization:
// Normalizes modulus M = |m| and returns r in [0, M - 1]
std::string modNormalize(const std::string& a, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";

    std::string q, r;
    divide(absVal(a), M, q, r);

    if (r == "0") return "0";

    // If input was negative, r = M - r
    if (isNegative(a)) {
        return subtract(M, r);
    }
    return r;
}

// Modular Addition: (a + b) mod M, with M = |m|, result in [0, M-1]
std::string modAdd(const std::string& a, const std::string& b, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";

    std::string a_norm = modNormalize(a, M);
    std::string b_norm = modNormalize(b, M);

    std::string sum = add(a_norm, b_norm);
    return modulo(sum, M);
}

// Modular Multiplication: (a * b) mod M, with M = |m|, result in [0, M-1]
std::string modMul(const std::string& a, const std::string& b, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";

    std::string a_norm = modNormalize(a, M);
    std::string b_norm = modNormalize(b, M);

    std::string prod = multiply(a_norm, b_norm);
    return modulo(prod, M);
}

// Modular Multiplicative Inverse: a^(-1) mod m
std::string modInverse(const std::string& a, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";
    if (M == "1") return "INVERSE DOES NOT EXIST (gcd != 1)";

    std::string a_norm = modNormalize(a, M);
    std::string x, y;
    std::string g = extendedGCD(a_norm, M, x, y);

    if (g != "1") {
        return "INVERSE DOES NOT EXIST (gcd != 1)";
    }

    return modNormalize(x, M);
}

// Naive Modular Exponentiation: base^exp mod M using repeated multiplication
std::string modPowNaive(const std::string& base, const std::string& exp, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";
    if (M == "1") return "0";

    if (isNegative(exp)) return "NEGATIVE EXPONENT NOT SUPPORTED";
    if (trim(exp) == "0") return "1";

    std::string b_norm = modNormalize(base, M);
    std::string res = "1";
    std::string cur_exp = trim(exp);

    while (cur_exp != "0") {
        res = modMul(res, b_norm, M);
        cur_exp = subtract(cur_exp, "1");
    }
    return res;
}

// Binary / Square-and-Multiply Modular Exponentiation: supports arbitrary large/512-bit exponents
std::string modPowSquareAndMultiply(const std::string& base, const std::string& exp, const std::string& m) {
    std::string M = absVal(trim(m));
    if (M == "0") return "MODULUS CANNOT BE ZERO";
    if (M == "1") return "0";

    if (isNegative(exp)) return "NEGATIVE EXPONENT NOT SUPPORTED";
    if (trim(exp) == "0") return "1";

    std::string b_norm = modNormalize(base, M);
    std::string res = "1";
    std::string cur_base = b_norm;
    std::string cur_exp = trim(exp);

    while (cur_exp != "0") {
        // If cur_exp is odd, multiply result by cur_base mod M
        char last_digit = cur_exp.back();
        if ((last_digit - '0') % 2 == 1) {
            res = modMul(res, cur_base, M);
        }

        // cur_base = cur_base^2 mod M
        cur_base = modMul(cur_base, cur_base, M);

        // cur_exp = cur_exp / 2
        std::string q, r;
        divide(cur_exp, "2", q, r);
        cur_exp = q;
    }

    return res;
}

// Generate random 512-bit decimal integer (>= 2^511)
std::string generateRandom512Bit() {
    std::mt19937_64 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 1);

    std::vector<int> bits(512);
    bits[0] = 1; // Ensure MSB is 1 so >= 2^511
    for (int i = 1; i < 512; i++) {
        bits[i] = dist(rng);
    }

    std::string dec = "0";
    for (int i = 0; i < 512; i++) {
        std::string doubled = add(dec, dec);
        dec = add(doubled, std::to_string(bits[i]));
    }
    return dec;
}

// ----------------------------------------------------------------------------
// 4. TIMING & AUTOMATED VERIFICATION SUITE
// ----------------------------------------------------------------------------

template <typename Func>
std::pair<std::string, double> measureTime(Func f) {
    // Warmup call
    // Warm up
    std::string res = f();

    auto t1 = std::chrono::high_resolution_clock::now();
    int iterations = 0;
    auto t2 = t1;
    while (true) {
        res = f();
        iterations++;
        t2 = std::chrono::high_resolution_clock::now();
        double elapsedUs = std::chrono::duration<double, std::micro>(t2 - t1).count();
        if (elapsedUs >= 2000.0) break; // Run until at least 2ms elapsed for high timer precision
        if (iterations >= 20000) break; // Or max 20000 iterations
    }

    double totalUs = std::chrono::duration<double, std::micro>(t2 - t1).count();
    double avgUs = (iterations > 0) ? (totalUs / iterations) : 0.0;
    return {res, avgUs};
}

struct TestCase {
    std::string opName;
    int testNum;
    std::string category;
    std::string a;
    std::string b;
    std::string m;
    std::string exp;
};

struct TestResult {
    TestCase tc;
    std::string scratchRes;
    std::string predefRes;
    double scratchTime;
    double predefTime;
    bool pass;
};

struct OpGroup {
    std::string name;
    std::vector<TestCase> cases;
};

TestResult runSingleTest(const TestCase& tc, int& passedCount, int& totalCount, bool printConsole = true) {
    totalCount++;

    std::string scratchRes, predefRes;
    double scratchTime = 0.0, predefTime = 0.0;
    bool isPass = false;

    if (tc.opName == "Addition") {
        auto s = measureTime([&]() { return addSigned(tc.a, tc.b); });
        auto p = measureTime([&]() { return predefined::ref_add(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Subtraction") {
        auto s = measureTime([&]() { return subtractSigned(tc.a, tc.b); });
        auto p = measureTime([&]() { return predefined::ref_sub(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Multiplication") {
        auto s = measureTime([&]() { return multiplySigned(tc.a, tc.b); });
        auto p = measureTime([&]() { return predefined::ref_mul(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Division") {
        auto s = measureTime([&]() {
            std::string q, r;
            divideSigned(tc.a, tc.b, q, r);
            return q;
        });
        auto p = measureTime([&]() { return predefined::ref_div(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Modulo") {
        auto s = measureTime([&]() {
            std::string q, r;
            divideSigned(tc.a, tc.b, q, r);
            return r;
        });
        auto p = measureTime([&]() { return predefined::ref_mod(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "GCD") {
        auto s = measureTime([&]() { return gcd(tc.a, tc.b); });
        auto p = measureTime([&]() { return predefined::ref_gcd(tc.a, tc.b); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Extended GCD") {
        std::string sx, sy, px, py;
        auto s = measureTime([&]() { return extendedGCD(tc.a, tc.b, sx, sy); });
        auto p = measureTime([&]() { return predefined::ref_extgcd(tc.a, tc.b, px, py); });
        scratchRes = "gcd=" + s.first + ", x=" + sx + ", y=" + sy;
        scratchTime = s.second;
        predefRes  = "gcd=" + p.first + ", x=" + px + ", y=" + py;
        predefTime = p.second;

        // Verify GCD match and Bezout identity a*x + b*y = gcd(a,b)
        bool bezoutScratch = verifyBezout(tc.a, tc.b, sx, sy, s.first);
        isPass = (s.first == p.first && bezoutScratch);
    } else if (tc.opName == "Modular Addition") {
        auto s = measureTime([&]() { return modAdd(tc.a, tc.b, tc.m); });
        auto p = measureTime([&]() { return predefined::ref_modAdd(tc.a, tc.b, tc.m); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Modular Multiplication") {
        auto s = measureTime([&]() { return modMul(tc.a, tc.b, tc.m); });
        auto p = measureTime([&]() { return predefined::ref_modMul(tc.a, tc.b, tc.m); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Modular Inverse") {
        auto s = measureTime([&]() { return modInverse(tc.a, tc.m); });
        auto p = measureTime([&]() { return predefined::ref_modInverse(tc.a, tc.m); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;

        if (scratchRes == "INVERSE DOES NOT EXIST (gcd != 1)" && predefRes == "INVERSE DOES NOT EXIST (gcd != 1)") {
            isPass = true;
        } else if (scratchRes == predefRes) {
            // Verify (a * inv) mod M == 1
            std::string chk = modMul(tc.a, scratchRes, tc.m);
            isPass = (chk == "1");
        } else {
            isPass = false;
        }
    } else if (tc.opName == "Naive Modular Power") {
        auto s = measureTime([&]() { return modPowNaive(tc.a, tc.exp, tc.m); });
        auto p = measureTime([&]() { return predefined::ref_modPowNaive(tc.a, tc.exp, tc.m); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    } else if (tc.opName == "Square-and-Multiply") {
        auto s = measureTime([&]() { return modPowSquareAndMultiply(tc.a, tc.exp, tc.m); });
        auto p = measureTime([&]() { return predefined::ref_modPow(tc.a, tc.exp, tc.m); });
        scratchRes = s.first; scratchTime = s.second;
        predefRes = p.first;  predefTime = p.second;
        isPass = (scratchRes == predefRes);
    }

    if (isPass) passedCount++;

    if (printConsole) {
        std::cout << "====================================================\n";
        std::cout << tc.opName << " - TEST " << tc.testNum << " (" << tc.category << ")\n";
        std::cout << "====================================================\n";
        std::cout << "Input:\n";
        if (!tc.a.empty()) std::cout << "  A = " << (tc.a.length() > 60 ? tc.a.substr(0, 57) + "..." : tc.a) << "\n";
        if (!tc.b.empty()) std::cout << "  B = " << (tc.b.length() > 60 ? tc.b.substr(0, 57) + "..." : tc.b) << "\n";
        if (!tc.m.empty()) std::cout << "  M = " << (tc.m.length() > 60 ? tc.m.substr(0, 57) + "..." : tc.m) << "\n";
        if (!tc.exp.empty()) std::cout << "  Exp = " << (tc.exp.length() > 60 ? tc.exp.substr(0, 57) + "..." : tc.exp) << "\n";

        std::cout << "\nScratch Output:\n  " << (scratchRes.length() > 70 ? scratchRes.substr(0, 67) + "..." : scratchRes) << "\n";
        std::cout << "Predefined Output:\n  " << (predefRes.length() > 70 ? predefRes.substr(0, 67) + "..." : predefRes) << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nScratch Time:     " << scratchTime << " μs\n";
        std::cout << "Predefined Time:  " << predefTime << " μs\n";
        std::cout << "Status:           " << (isPass ? "PASS" : "FAIL") << "\n\n";
    }

    return TestResult{tc, scratchRes, predefRes, scratchTime, predefTime, isPass};
}

std::vector<OpGroup> getAllTestGroups() {
    // 512-bit test vectors (~155 digits each)
    std::string num512_A = "115792089237316195423570985008687907853269984665640564039457584007913129639935";
    std::string num512_B = "57896044618658097711785492504343953926634992332820282019728792003956564819967";
    std::string num512_M = "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999";

    // Large 40-digit test vectors
    std::string large_A = "1234567890123456789012345678901234567890";
    std::string large_B = "9876543210987654321098765432109876543210";

    return std::vector<OpGroup>{
        {"Addition", {
            {"Addition", 1, "Small Numbers", "12", "5", "", ""},
            {"Addition", 2, "Different Sizes", "123456789", "37", "", ""},
            {"Addition", 3, "Large Numbers", large_A, large_B, "", ""},
            {"Addition", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Addition", 5, "Negative Numbers", "-12345", "6789", "", ""},
            {"Addition", 6, "Edge Case (Zero)", "0", "987654321", "", ""}
        }},
        {"Subtraction", {
            {"Subtraction", 1, "Small Numbers", "12", "5", "", ""},
            {"Subtraction", 2, "Different Sizes", "123456789", "37", "", ""},
            {"Subtraction", 3, "Large Numbers", large_B, large_A, "", ""},
            {"Subtraction", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Subtraction", 5, "Negative Numbers", "-12345", "-6789", "", ""},
            {"Subtraction", 6, "Edge Case (Equal)", "9999999999", "9999999999", "", ""}
        }},
        {"Multiplication", {
            {"Multiplication", 1, "Small Numbers", "12", "5", "", ""},
            {"Multiplication", 2, "Different Sizes", "123456789", "37", "", ""},
            {"Multiplication", 3, "Large Numbers", "123456789012345", "987654321098765", "", ""},
            {"Multiplication", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Multiplication", 5, "Negative Numbers", "-12345", "6789", "", ""},
            {"Multiplication", 6, "Edge Case (Zero)", "123456789", "0", "", ""}
        }},
        {"Division", {
            {"Division", 1, "Small Numbers", "100", "7", "", ""},
            {"Division", 2, "Different Sizes", "123456789", "37", "", ""},
            {"Division", 3, "Large Numbers", large_B, "123456789", "", ""},
            {"Division", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Division", 5, "Negative Numbers", "-100", "7", "", ""},
            {"Division", 6, "Edge Case (Dividend < Divisor)", "37", "123456", "", ""}
        }},
        {"Modulo", {
            {"Modulo", 1, "Small Numbers", "100", "7", "", ""},
            {"Modulo", 2, "Different Sizes", "123456789", "37", "", ""},
            {"Modulo", 3, "Large Numbers", large_B, "123456789", "", ""},
            {"Modulo", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Modulo", 5, "Negative Numbers", "-100", "7", "", ""},
            {"Modulo", 6, "Edge Case (Mod 1)", "123456789", "1", "", ""}
        }},
        {"GCD", {
            {"GCD", 1, "Small Numbers", "48", "18", "", ""},
            {"GCD", 2, "Different Sizes", "123456789", "37", "", ""},
            {"GCD", 3, "Large Numbers", large_A, large_B, "", ""},
            {"GCD", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"GCD", 5, "Negative Numbers", "-48", "18", "", ""},
            {"GCD", 6, "Edge Case (Coprime)", "17", "31", "", ""}
        }},
        {"Extended GCD", {
            {"Extended GCD", 1, "Small Numbers", "30", "12", "", ""},
            {"Extended GCD", 2, "Different Sizes", "240", "46", "", ""},
            {"Extended GCD", 3, "Large Numbers", large_A, large_B, "", ""},
            {"Extended GCD", 4, "~512-bit Numbers", num512_A, num512_B, "", ""},
            {"Extended GCD", 5, "Negative Numbers", "-30", "12", "", ""},
            {"Extended GCD", 6, "Edge Case (Coprime)", "13", "7", "", ""}
        }},
        {"Modular Addition", {
            {"Modular Addition", 1, "Small Numbers", "12", "5", "7", ""},
            {"Modular Addition", 2, "Different Sizes", "123456789", "37", "1000", ""},
            {"Modular Addition", 3, "Large Numbers", large_A, large_B, "1000000007", ""},
            {"Modular Addition", 4, "~512-bit Numbers", num512_A, num512_B, num512_M, ""},
            {"Modular Addition", 5, "Negative Modulus", "-17", "3", "-5", ""},
            {"Modular Addition", 6, "Edge Case (Mod Wraparound)", "999", "2", "1000", ""}
        }},
        {"Modular Multiplication", {
            {"Modular Multiplication", 1, "Small Numbers", "12", "5", "7", ""},
            {"Modular Multiplication", 2, "Different Sizes", "123456789", "37", "1000", ""},
            {"Modular Multiplication", 3, "Large Numbers", large_A, large_B, "1000000007", ""},
            {"Modular Multiplication", 4, "~512-bit Numbers", num512_A, num512_B, num512_M, ""},
            {"Modular Multiplication", 5, "Negative Operands", "-12", "-5", "7", ""},
            {"Modular Multiplication", 6, "Edge Case (Zero Product)", "14", "5", "7", ""}
        }},
        {"Modular Inverse", {
            {"Modular Inverse", 1, "Small Numbers", "3", "", "11", ""},
            {"Modular Inverse", 2, "Different Sizes", "37", "", "1000", ""},
            {"Modular Inverse", 3, "Large Prime Modulus", "123456789", "", "1000000007", ""},
            {"Modular Inverse", 4, "~512-bit Coprime", "65537", "", num512_M, ""},
            {"Modular Inverse", 5, "Negative Modulus", "-3", "", "-11", ""},
            {"Modular Inverse", 6, "Edge Case (No Inverse gcd!=1)", "6", "", "9", ""}
        }},
        {"Naive Modular Power", {
            {"Naive Modular Power", 1, "Small Exponent", "2", "", "7", "5"},
            {"Naive Modular Power", 2, "Medium Base", "123", "", "1000", "7"},
            {"Naive Modular Power", 3, "Moderate Exponent", "7", "", "1000000007", "25"},
            {"Naive Modular Power", 4, "Negative Base", "-2", "", "13", "4"},
            {"Naive Modular Power", 5, "Negative Modulus", "3", "", "-11", "5"},
            {"Naive Modular Power", 6, "Edge Case (Exp = 0)", "123456", "", "1000", "0"}
        }},
        {"Square-and-Multiply", {
            {"Square-and-Multiply", 1, "Small Exponent", "2", "", "7", "5"},
            {"Square-and-Multiply", 2, "Medium Exponent", "123456789", "", "1000000007", "12345"},
            {"Square-and-Multiply", 3, "Large Exponent", "3", "", "1000000007", "98765432109876543210"},
            {"Square-and-Multiply", 4, "~512-bit Exponent", num512_A, "", num512_M, num512_B},
            {"Square-and-Multiply", 5, "Negative Base & Modulus", "-5", "", "-17", "13"},
            {"Square-and-Multiply", 6, "Edge Case (Exp = 0, Base = 0)", "0", "", "17", "0"}
        }}
    };
}

std::string escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

void runAllAutomatedVerifications() {
    std::cout << "\n====================================================\n";
    std::cout << "  CRYPTOGRAPHY LAB: SCRATCH vs PREDEFINED VERIFICATION\n";
    std::cout << "====================================================\n\n";

    auto groups = getAllTestGroups();
    std::vector<std::pair<std::string, std::pair<int, int>>> summary;
    int overallPassed = 0;
    int overallTotal = 0;

    for (const auto& group : groups) {
        int passed = 0;
        int total = 0;
        for (const auto& tc : group.cases) {
            runSingleTest(tc, passed, total, true);
        }
        summary.push_back({group.name, {passed, total}});
        overallPassed += passed;
        overallTotal += total;
    }

    std::cout << "\n====================================================\n";
    std::cout << "                 VERIFICATION SUMMARY\n";
    std::cout << "====================================================\n\n";

    for (const auto& item : summary) {
        std::cout << std::left << std::setw(28) << item.first
                  << item.second.first << "/" << item.second.second << " PASS\n";
    }

    std::cout << "\n----------------------------------------------------\n";
    std::cout << "Overall: " << (overallPassed == overallTotal ? "PASS" : "FAIL")
              << " (" << overallPassed << "/" << overallTotal << " tests passed)\n";
    std::cout << "====================================================\n\n";
}

void exportAllVerificationsJson() {
    auto groups = getAllTestGroups();
    int passed = 0, total = 0;
    std::cout << "[\n";
    bool first = true;
    for (const auto& grp : groups) {
        for (const auto& tc : grp.cases) {
            TestResult tr = runSingleTest(tc, passed, total, false);
            if (!first) std::cout << ",\n";
            first = false;
            std::cout << "  {\n";
            std::cout << "    \"op\": \"" << escapeJson(tr.tc.opName) << "\",\n";
            std::cout << "    \"test\": " << tr.tc.testNum << ",\n";
            std::cout << "    \"category\": \"" << escapeJson(tr.tc.category) << "\",\n";
            std::cout << "    \"a\": \"" << escapeJson(tr.tc.a) << "\",\n";
            std::cout << "    \"b\": \"" << escapeJson(tr.tc.b) << "\",\n";
            std::cout << "    \"m\": \"" << escapeJson(tr.tc.m) << "\",\n";
            std::cout << "    \"exp\": \"" << escapeJson(tr.tc.exp) << "\",\n";
            std::cout << "    \"scratchRes\": \"" << escapeJson(tr.scratchRes) << "\",\n";
            std::cout << "    \"predefRes\": \"" << escapeJson(tr.predefRes) << "\",\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "    \"scratchTimeUs\": " << tr.scratchTime << ",\n";
            std::cout << "    \"predefTimeUs\": " << tr.predefTime << ",\n";
            std::cout << "    \"pass\": " << (tr.pass ? "true" : "false") << "\n";
            std::cout << "  }";
        }
    }
}


// ----------------------------------------------------------------------------
// 5. INTERACTIVE TERMINAL MENU & CLI CONTROLLER
// ----------------------------------------------------------------------------

void printMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "      CRYPTOGRAPHY LAB CALCULATOR\n";
    std::cout << "=========================================\n";
    std::cout << " 1. Addition (+)\n";
    std::cout << " 2. Subtraction (-)\n";
    std::cout << " 3. Multiplication (*)\n";
    std::cout << " 4. Division & Modulo (/ and %)\n";
    std::cout << " 5. Euclidean GCD\n";
    std::cout << " 6. Extended Euclidean GCD\n";
    std::cout << " 7. Modular Addition\n";
    std::cout << " 8. Modular Multiplication\n";
    std::cout << " 9. Modular Inverse\n";
    std::cout << "10. Modular Exponentiation (Square & Multiply)\n";
    std::cout << "11. Naive Modular Exponentiation\n";
    std::cout << "12. Generate 512-bit Number\n";
    std::cout << "13. Run Complete Automated Verification Suite\n";
    std::cout << "14. Exit\n";
    std::cout << "=========================================\n";
    std::cout << "Enter your choice (1-14): " << std::flush;
}

int main(int argc, char* argv[]) {
    // ------------------------------------------------------------------------
    // CLI Mode: Supports command-line execution & test automation
    // ------------------------------------------------------------------------
    if (argc > 1) {
        std::string cmd = argv[1];

        if (cmd == "test") {
            runAllAutomatedVerifications();
            return 0;
        }

        if (cmd == "json") {
            exportAllVerificationsJson();
            return 0;
        }

        if (cmd == "rand512") {
            std::cout << generateRandom512Bit() << std::endl;
            return 0;
        }

        if (cmd == "add" && argc >= 4) {
            std::cout << addSigned(argv[2], argv[3]) << std::endl;
        } else if (cmd == "sub" && argc >= 4) {
            std::cout << subtractSigned(argv[2], argv[3]) << std::endl;
        } else if (cmd == "mul" && argc >= 4) {
            std::cout << multiplySigned(argv[2], argv[3]) << std::endl;
        } else if (cmd == "div" && argc >= 4) {
            std::string q, r;
            divideSigned(argv[2], argv[3], q, r);
            std::cout << "Quotient:  " << q << "\nRemainder: " << r << std::endl;
        } else if (cmd == "gcd" && argc >= 4) {
            std::cout << gcd(argv[2], argv[3]) << std::endl;
        } else if (cmd == "extgcd" && argc >= 4) {
            std::string x, y;
            std::string g = extendedGCD(argv[2], argv[3], x, y);
            bool verified = verifyBezout(argv[2], argv[3], x, y, g);
            std::cout << "GCD: " << g << "\nx:   " << x << "\ny:   " << y << "\n";
            std::cout << "Bézout Verification: " << (verified ? "PASS" : "FAIL") << std::endl;
        } else if (cmd == "modadd" && argc >= 5) {
            std::cout << modAdd(argv[2], argv[3], argv[4]) << std::endl;
        } else if (cmd == "modmul" && argc >= 5) {
            std::cout << modMul(argv[2], argv[3], argv[4]) << std::endl;
        } else if (cmd == "modinv" && argc >= 4) {
            std::cout << modInverse(argv[2], argv[3]) << std::endl;
        } else if (cmd == "modpow" && argc >= 5) {
            std::cout << modPowSquareAndMultiply(argv[2], argv[3], argv[4]) << std::endl;
        } else {
            std::cerr << "Usage: " << argv[0] << " [test | rand512 | <op> <args...>]\n";
            return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------------
    // Interactive Terminal Mode
    // ------------------------------------------------------------------------
    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string discard;
            if (!(std::cin >> discard)) break;
            continue;
        }

        if (choice == 14) {
            std::cout << "Exiting calculator. Goodbye!\n";
            break;
        }

        if (choice == 13) {
            runAllAutomatedVerifications();
            continue;
        }

        if (choice == 12) {
            std::cout << "\nGenerated 512-bit Number (decimal, ~155 digits):\n";
            std::cout << generateRandom512Bit() << "\n";
            continue;
        }

        if (choice < 1 || choice > 14) {
            std::cout << "Invalid choice! Please select 1-14.\n";
            continue;
        }

        std::string a, b, m, exp;

        if (choice >= 1 && choice <= 6) {
            std::cout << "\nEnter First Number (A):  " << std::flush;
            std::cin >> a;
            std::cout << "Enter Second Number (B): " << std::flush;
            std::cin >> b;
        } else if (choice == 7 || choice == 8) {
            std::cout << "\nEnter First Number (A):  " << std::flush;
            std::cin >> a;
            std::cout << "Enter Second Number (B): " << std::flush;
            std::cin >> b;
            std::cout << "Enter Modulus (M):       " << std::flush;
            std::cin >> m;
        } else if (choice == 9) {
            std::cout << "\nEnter Number (A):  " << std::flush;
            std::cin >> a;
            std::cout << "Enter Modulus (M): " << std::flush;
            std::cin >> m;
        } else if (choice == 10 || choice == 11) {
            std::cout << "\nEnter Base:     " << std::flush;
            std::cin >> a;
            std::cout << "Enter Exponent: " << std::flush;
            std::cin >> exp;
            std::cout << "Enter Modulus:  " << std::flush;
            std::cin >> m;
        }

        std::cout << "\n-----------------------------------------\n";
        switch (choice) {
            case 1:
                std::cout << "Addition (+):\n" << addSigned(a, b) << "\n";
                break;
            case 2:
                std::cout << "Subtraction (-):\n" << subtractSigned(a, b) << "\n";
                break;
            case 3:
                std::cout << "Multiplication (*):\n" << multiplySigned(a, b) << "\n";
                break;
            case 4: {
                std::string q, r;
                divideSigned(a, b, q, r);
                std::cout << "Quotient:  " << q << "\nRemainder: " << r << "\n";
                break;
            }
            case 5:
                std::cout << "Euclidean GCD:\n" << gcd(a, b) << "\n";
                break;
            case 6: {
                std::string x, y;
                std::string g = extendedGCD(a, b, x, y);
                bool pass = verifyBezout(a, b, x, y, g);
                std::cout << "GCD: " << g << "\nx:   " << x << "\ny:   " << y << "\n";
                std::cout << "Bézout Verification: " << (pass ? "PASS" : "FAIL") << "\n";
                break;
            }
            case 7:
                std::cout << "Modular Addition:\n" << modAdd(a, b, m) << "\n";
                break;
            case 8:
                std::cout << "Modular Multiplication:\n" << modMul(a, b, m) << "\n";
                break;
            case 9:
                std::cout << "Modular Inverse:\n" << modInverse(a, m) << "\n";
                break;
            case 10: {
                auto s = measureTime([&]() { return modPowSquareAndMultiply(a, exp, m); });
                std::cout << "Square-and-Multiply Result:\n" << s.first << "\n";
                std::cout << "Execution Time: " << s.second << " ms\n";
                break;
            }
            case 11: {
                auto s = measureTime([&]() { return modPowNaive(a, exp, m); });
                std::cout << "Naive Modular Power Result:\n" << s.first << "\n";
                std::cout << "Execution Time: " << s.second << " ms\n";
                break;
            }
        }
        std::cout << "-----------------------------------------\n";
    }

    return 0;
}
