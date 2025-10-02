#pragma once

#include <iostream>

namespace cp
{
    /**
     * @brief Represents an integer in the ring Z/nZ (integers modulo n).
     *
     * This struct encapsulates an integer and performs all arithmetic operations
     * under the modulus `MOD`. The modulus is a compile-time constant, which
     * allows for significant compiler optimizations and type safety.
     *
     * @tparam MOD The modulus. Must be a positive integer.
     */
    template <int MOD>
    struct Zn
    {
        int value;

        /// @brief Default constructor, initializes value to 0.
        Zn(long long v = 0)
        {
            value = v % MOD;
            if (value < 0)
                value += MOD;
        }

        /**
         * @brief Calculates the modular multiplicative inverse.
         *
         * Uses Fermat's Little Theorem for the calculation.
         * @note This requires the modulus `MOD` to be a prime number.
         * @return The modular inverse as a `Zn` object.
         */
        Zn inverse() const
        {
            return power(MOD - 2);
        }

        /**
         * @brief Calculates modular exponentiation (this^exp).
         * @param exp The exponent.
         * @return The result of (this->value ^ exp) % MOD.
         */
        Zn power(long long exp) const
        {
            Zn res = 1, base = *this;
            while (exp > 0)
            {
                if (exp % 2 == 1)
                    res *= base;
                base *= base;
                exp /= 2;
            }
            return res;
        }

        /// @brief Adds another Zn value to this one.
        Zn &operator+=(const Zn &other)
        {
            value += other.value;
            if (value >= MOD)
                value -= MOD;
            return *this;
        }

        /// @brief Subtracts another Zn value from this one.
        Zn &operator-=(const Zn &other)
        {
            value -= other.value;
            if (value < 0)
                value += MOD;
            return *this;
        }

        /// @brief Multiplies this Zn value by another one.
        Zn &operator*=(const Zn &other)
        {
            value = static_cast<int>((static_cast<long long>(value) * other.value) % MOD);
            return *this;
        }

        /// @brief Divides this Zn value by another one.
        Zn &operator/=(const Zn &other)
        {
            return *this *= other.inverse();
        }

        /// @brief Unary negation operator.
        Zn operator-() const { return Zn(-value); }

        // Friend functions for binary and stream operators
        friend Zn operator+(Zn a, const Zn &b) { return a += b; }
        friend Zn operator-(Zn a, const Zn &b) { return a -= b; }
        friend Zn operator*(Zn a, const Zn &b) { return a *= b; }
        friend Zn operator/(Zn a, const Zn &b) { return a /= b; }

        friend bool operator==(const Zn &a, const Zn &b) { return a.value == b.value; }
        friend bool operator!=(const Zn &a, const Zn &b) { return a.value != b.value; }

        friend std::ostream &operator<<(std::ostream &os, const Zn &z) { return os << z.value; }
        friend std::istream &operator>>(std::istream &is, Zn &z)
        {
            long long v;
            is >> v;
            z = Zn(v);
            return is;
        }

        /// @brief Explicit conversion to the underlying integer type.
        explicit operator int() const { return value; }
    };
}

/*
int main()
{
    // Define an alias for Zn with our desired prime modulus
    using mint = cp::Zn<1000000007>;

    mint a = 5;
    mint b = 1000000000;

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;

    mint c = a + b; // 5 + 1,000,000,000 = 1,000,000,005
    std::cout << "a + b = " << c << std::endl;

    mint d = a - b; // 5 - 1,000,000,000 = -999,999,995 ≡ 12 (mod 1e9+7)
    std::cout << "a - b = " << d << std::endl;

    mint e = c * 2; // 1,000,000,005 * 2 = 2,000,000,010 ≡ 999,999,996 (mod 1e9+7)
    std::cout << "c * 2 = " << e << std::endl;

    mint f = e / 2; // Should get back to c
    std::cout << "e / 2 = " << f << std::endl;

    std::cout << "Equality check (f == c): " << std::boolalpha << (f == c) << std::endl;
}
/**/