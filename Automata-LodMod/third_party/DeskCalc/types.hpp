#pragma once

#include <complex>
#include <functional>
#include <iostream>
#include <vector>

using Complex = std::complex<double>;
using List = std::vector<Complex>;
using Func = Complex(*)(const List&); // using Func = std::function<Complex(const List&)>;

template<class T>
void print_complex(std::ostream& os, const std::complex<T>& n)
{   // The standard already defines a different operator<< for std::complex
    if (n.imag()) {
        if (n.real()) {
            os << n.real();
            if (n.imag() > 0)
                os << '+';
        }
        if (std::abs(n.imag()) != 1)
            os << n.imag();
        os << (n.imag() == -1 ? "-" : "") << 'i';
    }
    else
        os << n.real();
}

template<class T>
void print_list(std::ostream& os, const std::vector<T>& v)
{
    os << '[';
    std::string sep;
    for (const auto& item : v) {
        os << sep;
        print_complex(os, item);
        sep = ", ";
    }
    os << ']';
}
