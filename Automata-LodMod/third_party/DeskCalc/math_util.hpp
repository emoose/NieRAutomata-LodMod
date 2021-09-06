#pragma once

#include "types.hpp"

constexpr bool is_zero(const Complex& num)
{
    return !num.real() && !num.imag();
}

Complex safe_div(const Complex& left, const Complex& right);
Complex safe_floordiv(Complex left, const Complex& right);
Complex safe_mod(const Complex& left, const Complex& right);

template<class T>
T safe_mod(T&& left, T&& right)
{
    if (right == 0)
        throw std::runtime_error{ "Divide by Zero" };
    return left % right;
}

double factorial(int n);
Complex factorial(const Complex& num);
Complex impedance_parallel(const Complex& R1, const Complex& R2);

std::size_t len(const List& list) noexcept;
Complex sum(const List& list);
Complex sqr_sum(const List& list);
Complex avg(const List& list);
Complex standard_deviation(const List& list);
Complex standard_uncertainty(const List& list);

Complex sqr(const Complex& num);
Complex pretty_pow(const Complex& base, const Complex& exp);

namespace temp {
    constexpr double absoluteZero{ -273.1499999999999773 };

    constexpr double CtoK(double celsius) noexcept { return celsius - absoluteZero; }
    constexpr double KtoC(double kelvin) noexcept { return kelvin + absoluteZero; }
    constexpr double CtoF(double celsius) noexcept { return 1.8 * celsius + 32; }
    constexpr double FtoC(double fahrenheit) noexcept { return (fahrenheit - 32) / 1.8; }
    constexpr double FtoK(double fahrenheit) noexcept { return CtoK(FtoC(fahrenheit)); }
    constexpr double KtoF(double kelvin) noexcept { return CtoF(KtoC(kelvin)); }
}

constexpr double pi{ 3.1415926535897932385 };

constexpr double deg(double rad) noexcept
{
    return 180 / pi * rad;
}

constexpr double rad(double deg) noexcept
{
    return pi / 180 * deg;
}
