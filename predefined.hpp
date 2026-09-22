#ifndef PREDEFINED_HPP
#define PREDEFINED_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <boost/multiprecision/cpp_int.hpp>

// ============================================================================
// Cryptography Lab - Predefined / Reference Implementation
// Uses Boost Multiprecision (cpp_int) as the authoritative reference
// ============================================================================

namespace predefined {

using boost::multiprecision::cpp_int;

inline cpp_int from_str(const std::string& s) {
    if (s.empty()) return 0;
    return cpp_int(s);
}

inline std::string to_str(const cpp_int& val) {
    return val.str();
}

// 1. Basic Arithmetic
inline std::string ref_add(const std::string& a, const std::string& b) {
    return to_str(from_str(a) + from_str(b));
}

inline std::string ref_sub(const std::string& a, const std::string& b) {
    return to_str(from_str(a) - from_str(b));
}

inline std::string ref_mul(const std::string& a, const std::string& b) {
    return to_str(from_str(a) * from_str(b));
}

inline std::string ref_div(const std::string& a, const std::string& b) {
    cpp_int div = from_str(b);
    if (div == 0) return "DIVISION BY ZERO";
    return to_str(from_str(a) / div);
}

inline std::string ref_mod(const std::string& a, const std::string& b) {
    cpp_int div = from_str(b);
    if (div == 0) return "MODULO BY ZERO";
    return to_str(from_str(a) % div);
}

// 2. Number Theory: Euclidean GCD (Non-negative convention)
inline std::string ref_gcd(const std::string& a, const std::string& b) {
    cpp_int x = boost::multiprecision::abs(from_str(a));
    cpp_int y = boost::multiprecision::abs(from_str(b));
    return to_str(boost::multiprecision::gcd(x, y));
}

// 3. Extended Euclidean GCD: returns gcd, and signed x, y such that a*x + b*y = gcd
inline std::string ref_extgcd(const std::string& a_str, const std::string& b_str, std::string& out_x, std::string& out_y) {
    cpp_int a = from_str(a_str);
    cpp_int b = from_str(b_str);

    cpp_int abs_a = boost::multiprecision::abs(a);
    cpp_int abs_b = boost::multiprecision::abs(b);

    cpp_int old_r = abs_a, r = abs_b;
    cpp_int old_s = 1, s = 0;
    cpp_int old_t = 0, t = 1;

    while (r != 0) {
        cpp_int q = old_r / r;
        cpp_int rem = old_r % r;

        old_r = r;
        r = rem;

        cpp_int next_s = old_s - q * s;
        old_s = s;
        s = next_s;

        cpp_int next_t = old_t - q * t;
        old_t = t;
        t = next_t;
    }

    // Adjust coefficients for original negative inputs so a*x + b*y = gcd(a, b)
    cpp_int x_res = (a < 0) ? -old_s : old_s;
    cpp_int y_res = (b < 0) ? -old_t : old_t;

    out_x = to_str(x_res);
    out_y = to_str(y_res);
    return to_str(old_r);
}

// 4. Modulo Normalization: M = |m|, result in [0, M-1]
inline cpp_int ref_normalize_mod(const cpp_int& a, const cpp_int& m) {
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return 0;
    cpp_int r = a % M;
    if (r < 0) r += M;
    return r;
}

// 5. Modular Addition
inline std::string ref_modAdd(const std::string& a_str, const std::string& b_str, const std::string& m_str) {
    cpp_int m = from_str(m_str);
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return "MODULUS CANNOT BE ZERO";

    cpp_int a = ref_normalize_mod(from_str(a_str), M);
    cpp_int b = ref_normalize_mod(from_str(b_str), M);
    return to_str((a + b) % M);
}

// 6. Modular Multiplication
inline std::string ref_modMul(const std::string& a_str, const std::string& b_str, const std::string& m_str) {
    cpp_int m = from_str(m_str);
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return "MODULUS CANNOT BE ZERO";

    cpp_int a = ref_normalize_mod(from_str(a_str), M);
    cpp_int b = ref_normalize_mod(from_str(b_str), M);
    return to_str((a * b) % M);
}

// 7. Modular Inverse: a^(-1) mod m
inline std::string ref_modInverse(const std::string& a_str, const std::string& m_str) {
    cpp_int m = from_str(m_str);
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return "MODULUS CANNOT BE ZERO";
    if (M == 1) return "INVERSE DOES NOT EXIST (gcd != 1)";

    cpp_int a = ref_normalize_mod(from_str(a_str), M);
    cpp_int g = boost::multiprecision::gcd(a, M);
    if (g != 1) {
        return "INVERSE DOES NOT EXIST (gcd != 1)";
    }

    std::string x_str, y_str;
    ref_extgcd(to_str(a), to_str(M), x_str, y_str);
    cpp_int inv = ref_normalize_mod(from_str(x_str), M);
    return to_str(inv);
}

// 8. Modular Exponentiation: base^exp mod m
inline std::string ref_modPow(const std::string& base_str, const std::string& exp_str, const std::string& m_str) {
    cpp_int m = from_str(m_str);
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return "MODULUS CANNOT BE ZERO";
    if (M == 1) return "0";

    cpp_int exp = from_str(exp_str);
    if (exp < 0) return "NEGATIVE EXPONENT NOT SUPPORTED";
    if (exp == 0) return "1";

    cpp_int base = ref_normalize_mod(from_str(base_str), M);
    return to_str(boost::multiprecision::powm(base, exp, M));
}

// 9. Naive Modular Exponentiation: repeated multiplication
inline std::string ref_modPowNaive(const std::string& base_str, const std::string& exp_str, const std::string& m_str) {
    cpp_int m = from_str(m_str);
    cpp_int M = boost::multiprecision::abs(m);
    if (M == 0) return "MODULUS CANNOT BE ZERO";
    if (M == 1) return "0";

    cpp_int exp = from_str(exp_str);
    if (exp < 0) return "NEGATIVE EXPONENT NOT SUPPORTED";
    if (exp == 0) return "1";

    cpp_int base = ref_normalize_mod(from_str(base_str), M);
    cpp_int res = 1;
    for (cpp_int i = 0; i < exp; ++i) {
        res = (res * base) % M;
    }
    return to_str(res);
}

} // namespace predefined

#endif // PREDEFINED_HPP
