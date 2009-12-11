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

#include "Number.h"

#ifdef KSPREAD_HIGH_PRECISION_SUPPORT

#include <math.h>

using namespace KSpread;

class Number::Private : public QSharedData
{
public:
    Private() {
        type = Number::Float;
        f = 0;
    }

    Private(const Private &o) : QSharedData(o), type(o.type) {
        switch (type) {
        case Number::Float:
            f = o.f;
            break;
        }
    }

    Number::Type type;

    union {
        long double f;
    };

    // static empty data to be shared
    static Private* null() {
        if (!s_null) s_null = new Private; return s_null;
    }

private:
    void operator=(const Number::Private& o);
    static Private *s_null;
};

Number::Private *Number::Private::s_null = 0;

// constructors

Number::Number()
        : d(Private::null())
{
    d->type = Number::Float;
    d->f = 0.0;
}

Number::Number(int num)
        : d(Private::null())
{
    d->type = Number::Float;
    d->f = (long double) num;
}

Number::Number(long double num)
        : d(Private::null())
{
    d->type = Number::Float;
    d->f = num;
}

Number::Number(const Number& n)
        : d(n.d)
{
}

// this destructor must exist here for the header file to compile properly,
// otherwise QSharedDataPointer destructor screams at us
Number::~Number()
{
}

// set/get
Number& Number::operator= (const Number & n)
{
    d = n.d;
    return *this;
}


long double Number::asFloat() const
{
    return d->f;
}

// basic operations
Number Number::operator+ (const Number &n) const
{
    return Number(d->f + n.d->f);
}

Number Number::operator- (const Number &n) const
{
    return Number(d->f - n.d->f);
}

Number Number::operator*(const Number &n) const
{
    return Number(d->f * n.d->f);
}

Number Number::operator/ (const Number &n) const
{
    return Number(d->f / n.d->f);
}

void Number::operator+= (const Number & n)
{
    d->f += n.d->f;
}

void Number::operator-= (const Number & n)
{
    d->f -= n.d->f;
}

void Number::operator*= (const Number & n)
{
    d->f *= n.d->f;
}

void Number::operator/= (const Number & n)
{
    d->f /= n.d->f;
}

Number Number::operator- () const
{
    return -(d->f);
}

// comparison
bool Number::operator<= (const Number &n) const
{
    return (d->f <= n.d->f);
}

bool Number::operator< (const Number &n) const
{
    return (d->f < n.d->f);
}

bool Number::operator== (const Number &n) const
{
    return (d->f == n.d->f);
}

Number Number::mod(const Number &n) const
{
    return Number(::fmod(d->f, n.d->f));
}

Number Number::abs() const
{
    return Number(::fabs(d->f));
}

Number Number::neg() const
{
    return Number(-1 * d->f);
}

Number Number::pow(const Number &exp) const
{
    return Number(::pow(d->f, exp.d->f));
}

Number Number::log(Number base) const
{
    long double logbase = ::log10(base.d->f);
    return Number(::log10(d->f) / logbase);
}

Number Number::ln() const
{
    return Number(::log(d->f));
}

Number Number::exp() const
{
    return Number(::exp(d->f));
}

// goniometric functions
Number Number::sin() const
{
    return Number(::sin(d->f));
}

Number Number::cos() const
{
    return Number(::cos(d->f));
}

Number Number::tg() const
{
    return Number(::tan(d->f));
}

Number Number::cotg() const
{
    return Number(1 / ::tan(d->f));
}

Number Number::asin() const
{
    return Number(::asin(d->f));
}

Number Number::acos() const
{
    return Number(::acos(d->f));
}

Number Number::atg() const
{
    return Number(::atan(d->f));
}

Number Number::atan2(const Number &y, const Number &x)
{
    return Number(::atan2(y.d->f, x.d->f));
}


// hyperbolic functions
Number Number::sinh() const
{
    return Number(::sinh(d->f));
}

Number Number::cosh() const
{
    return Number(::cosh(d->f));
}

Number Number::tgh() const
{
    return Number(::tanh(d->f));
}

Number Number::asinh() const
{
    return Number(::asinh(d->f));
}

Number Number::acosh() const
{
    return Number(::acosh(d->f));
}

Number Number::atgh() const
{
    return Number(::atanh(d->f));
}

// *** EXTERNAL FUNCTIONS ***

namespace KSpread
{

long double numToDouble(Number n)
{
    return n.asFloat();
}

// external operators, so that we can do things like 4+a without having to create temporary objects
// not provided for complex numbers, as we won't be using them often like that
Number operator+ (long double n1, const Number &n2)
{
    return n2 + n1;
}
Number operator- (long double n1, const Number &n2)
{
    return (n2 - n1).neg();
}
Number operator*(long double n1, const Number &n2)
{
    return n2 * n1;
}
Number operator/ (long double n1, const Number &n2)
{
    return Number(n1) / n2; /* TODO optimize perhaps */
}
bool operator<= (long double n1, const Number &n2)
{
    return (n2 >= n1);
}
bool operator< (long double n1, const Number &n2)
{
    return (n2 > n1);
}
bool operator== (long double n1, const Number &n2)
{
    return (n2 == n1);
}
bool operator!= (long double n1, const Number &n2)
{
    return (n2 != n1);
}
bool operator>= (long double n1, const Number &n2)
{
    return (n2 <= n1);
}
bool operator> (long double n1, const Number &n2)
{
    return (n2 < n1);
}

// external versions of the functions
Number fmod(const Number &n1, const Number &n2)
{
    return n1.mod(n2);
}
Number fabs(const Number &n)
{
    return n.abs();
}
Number abs(const Number &n)
{
    return n.abs();
}
Number neg(const Number &n)
{
    return n.neg();
}
Number pow(const Number &n, const Number &exp)
{
    return n.pow(exp);
}
Number sqrt(const Number &n)
{
    return n.pow(0.5);
}
Number log(const Number &n, Number base)
{
    return n.log(base);
}
Number ln(const Number &n)
{
    return n.ln();
}
Number log(const Number &n)
{
    return n.ln();
}
Number log10(const Number &n)
{
    return n.log(10);
}
Number exp(const Number &n)
{
    return n.exp();
}
Number sin(const Number &n)
{
    return n.sin();
}
Number cos(const Number &n)
{
    return n.cos();
}
Number tg(const Number &n)
{
    return n.tg();
}
Number cotg(const Number &n)
{
    return n.cotg();
}
Number asin(const Number &n)
{
    return n.asin();
}
Number acos(const Number &n)
{
    return n.acos();
}
Number atg(const Number &n)
{
    return n.atg();
}
Number atan2(const Number &y, const Number &x)
{
    return Number::atan2(y, x);
}
Number sinh(const Number &n)
{
    return n.sinh();
}
Number cosh(const Number &n)
{
    return n.cosh();
}
Number tgh(const Number &n)
{
    return n.tgh();
}
Number asinh(const Number &n)
{
    return n.asinh();
}
Number acosh(const Number &n)
{
    return n.acosh();
}
Number atgh(const Number &n)
{
    return n.atgh();
}

}  // nsmespace KSpread

#endif // KSPREAD_HIGH_PRECISION_SUPPORT
