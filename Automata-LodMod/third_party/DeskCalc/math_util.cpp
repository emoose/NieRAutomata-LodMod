#include "math_util.hpp"

#include <cmath>
#include <numeric>
#include <stdexcept>

using std::runtime_error;

Complex sqr(const Complex& num)
{
    return num * num;
}

Complex safe_div(const Complex& left, const Complex& right)
{
    if (is_zero(right))
        throw runtime_error{ "Divide by zero" };
    return left / right;
}

Complex safe_floordiv(Complex left, const Complex& right)
{
    if (is_zero(right))
        throw runtime_error{ "Divide by zero" };
    left /= right;
    return { std::floor(left.real()), std::floor(left.imag()) };
}

Complex safe_mod(const Complex& left, const Complex& right)
{
    if (left.imag() || right.imag()) 
        throw runtime_error{ "Modulo not defined for complex numbers (yet)" };

    auto ltrunc = std::trunc(left.real());
    auto rtrunc = std::trunc(right.real());

    if (left.real() != ltrunc || right.real() != rtrunc)
        throw runtime_error{ "Modulo not defined for floating point numbers" };

    return safe_mod(static_cast<long>(ltrunc), static_cast<long>(rtrunc));
}

double factorial(int n)
{
    if (n < 0) throw runtime_error{ "Factorial not defined for negative numbers" };
    double ret{ 1 };
    for (int i = 2; i <= n; ++i)
        ret *= i;
    return ret;
}

Complex factorial(const Complex& num)
{
    auto trunc = std::trunc(num.real());
    if (num.imag() || trunc < 0 || trunc != num.real())
        throw runtime_error{ "Factorial only defined for natural numbers (and zero)" };
    return factorial(static_cast<int>(trunc));
}

Complex impedance_parallel(const Complex& R1, const Complex& R2)
{
    if (is_zero(R1) && is_zero(R2))
        throw runtime_error{ "Resistors must be greater than 0" };
    return R1 * R2 / (R1 + R2);
}

Complex pretty_pow(const Complex& base, const Complex& exp)
{   // I like i^3 to show -i and not -1.83697e-16-i or
    // (-3)^3 to show -27 and not -27+9.91964e-15i
    if (!exp.imag() && exp.real() > 1) {
        Complex res{ 1 };
        auto e = static_cast<unsigned>(exp.real());
        if (e == exp.real()) {
            for (unsigned i = 0; i < e; ++i)
                res *= base;
            return res;
        }
    }
    return std::pow(base, exp);
}

std::size_t len(const List& list) noexcept
{
    return list.size();
}

Complex sum(const List& list)
{
    return std::accumulate(cbegin(list), cend(list), Complex{});
}

Complex sqr_sum(const List& list)
{
    const auto acc_squared = [](auto sum, auto next)
    { return sum + sqr(next); };
    return std::accumulate(cbegin(list), cend(list), Complex{}, acc_squared);
}

Complex avg(const List& list)
{
    return sum(list) / static_cast<double>(len(list));
}

Complex standard_deviation(const List& list)
{
    const auto acc_distance_squared = [a = avg(list)](auto sum, auto next) 
    { return sum + sqr(next - a); };

    const auto s_sqr = std::accumulate(cbegin(list), cend(list), Complex{}, acc_distance_squared)
                       / static_cast<double>(len(list) - 1);

    return std::sqrt(s_sqr);
}

Complex standard_uncertainty(const List& list)
{
    return standard_deviation(list) / std::sqrt(len(list));
}
