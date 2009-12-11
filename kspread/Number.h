/* This file is part of the KDE project
   Copyright 2007 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_NUMBER_H
#define KSPREAD_NUMBER_H

// #define KSPREAD_HIGH_PRECISION_SUPPORT

#ifndef KSPREAD_HIGH_PRECISION_SUPPORT

#include <math.h>

typedef long double Number;

inline long double numToDouble(Number n)
{
    return n;
}

namespace KSpread
{

inline Number log(const Number &n, Number base)
{
    return ::log10(n) / ::log10(base);
}
inline Number ln(const Number &n)
{
    return ::log(n);
}
inline Number tg(const Number &n)
{
    return ::tan(n);
}
inline Number atg(const Number &n)
{
    return ::atan(n);
}
inline Number tgh(const Number &n)
{
    return ::tanh(n);
}
inline Number atgh(const Number &n)
{
    return ::atanh(n);
}

} // namespace KSpread

#else // KSPREAD_HIGH_PRECISION_SUPPORT

#include <QSharedDataPointer>

#include "kspread_export.h"

#include <complex>

using namespace std;

namespace KSpread
{

/**
The Number class holds a single floating-point number. At the moment, it's just a wrapper for long double, but it's going to support GnuMP or something eventually.

The class is made so that if high precision is not desired, a "typedef long double Number" will revert us back to doubles.

The class will be able to format itself into a string, using provided locale settings. (TODO: how to handle this so that parsing/formatting works even if we typedef this class out?)

Out-of-class methods for computations are provided
*/

class KSPREAD_EXPORT Number
{
public:
    enum Type {
        Float  // GnuMP will be here as well, eventually
    };

    // constructors
    Number();
    Number(int num);
    Number(long double num);

    Number(const Number& n);

    ~Number();

    long double asFloat() const;

    // set/get
    Number& operator= (const Number &n);

    // basic operations
    Number operator+ (const Number &n) const;
    Number operator- (const Number &n) const;
    Number operator*(const Number &n) const;
    Number operator/ (const Number &n) const;

    void operator+= (const Number &n);
    void operator-= (const Number &n);
    void operator*= (const Number &n);
    void operator/= (const Number &n);

    void operator++ () {
        return operator+= (1);
    }
    void operator-- () {
        return operator-= (1);
    }

    // unary -
    Number operator- () const;

    Number mod(const Number &n) const;

    // comparison
    bool operator<= (const Number &n) const;
    bool operator< (const Number &n) const;
    bool operator== (const Number &n) const;
    bool operator!= (const Number &n) const {
        return (!operator== (n));
    }
    bool operator>= (const Number &n) const {
        return (!operator< (n));
    }
    bool operator> (const Number &n) const {
        return (!operator<= (n));
    }

    // absolute value
    Number abs() const;
    // negative value
    Number neg() const;
    // power
    Number pow(const Number &exp) const;
    // logarithms
    Number log(Number base) const;
    Number ln() const;
    Number exp() const;

    // goniometric functions
    Number sin() const;
    Number cos() const;
    Number tg() const;
    Number cotg() const;
    Number asin() const;
    Number acos() const;
    Number atg() const;
    static Number atan2(const Number &y, const Number &x);

    // hyperbolic functions
    Number sinh() const;
    Number cosh() const;
    Number tgh() const;
    Number asinh() const;
    Number acosh() const;
    Number atgh() const;

    // TODO: add more functions, as needed

    // TODO: functions to output the number to a string

private:
    class Private;
    QSharedDataPointer<Private> d;

};  // class Number

// conversion to double ... when we add the option to #define the Number class as double, this routine should be kept in place, and it should simply return its parameter
// usage of this function should eventually be removed, because places that use it are not ready for high precision support
KSPREAD_EXPORT long double numToDouble(Number n);

// external operators, so that we can do things like 4+a without having to create temporary objects
// not provided for complex numbers, as we won't be using them often like that
Number operator+ (long double n1, const Number &n2);
Number operator- (long double n1, const Number &n2);
Number operator*(long double n1, const Number &n2);
Number operator/ (long double n1, const Number &n2);
bool operator<= (long double n1, const Number &n2);
bool operator< (long double n1, const Number &n2);
bool operator== (long double n1, const Number &n2);
bool operator!= (long double n1, const Number &n2);
bool operator>= (long double n1, const Number &n2);
bool operator> (long double n1, const Number &n2);

// external versions of the functions
Number fmod(const Number &n1, const Number &n2);
Number fabs(const Number &n);
Number abs(const Number &n);
Number neg(const Number &n);
Number pow(const Number &n, const Number &exp);
Number sqrt(const Number &n);
Number log(const Number &n, Number base);
Number ln(const Number &n);
Number log(const Number &n);
Number log10(const Number &n);
Number exp(const Number &n);
Number sin(const Number &n);
Number cos(const Number &n);
Number tg(const Number &n);
Number cotg(const Number &n);
Number asin(const Number &n);
Number acos(const Number &n);
Number atg(const Number &n);
Number atan2(const Number &y, const Number &x);
Number sinh(const Number &n);
Number cosh(const Number &n);
Number tgh(const Number &n);
Number asinh(const Number &n);
Number acosh(const Number &n);
Number atgh(const Number &n);

} // namespace KSpread

#endif // KSPREAD_HIGH_PRECISION_SUPPORT

#endif // KSPREAD_NUMBER_H
