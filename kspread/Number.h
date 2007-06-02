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

#include <QSharedDataPointer>

#include "kspread_export.h"

#include <complex>

using namespace std;

namespace KSpread {

/**
The Number class holds a single floating-point number. At the moment, it's just a wrapper for double, but it's going to support GnuMP or something eventually.

The class is made so that if high precision is not desired, a "typedef Number double" will revert us back to doubles.

The class will be able to format itself into a string, using provided locale settings. (TODO: how to handle this so that parsing/formatting works even if we typedef this class out?)

Out-of-class methods for computations are provided
*/

class KSPREAD_EXPORT Number {
  public:
    enum Type {
      Float  // GnuMP will be here as well, eventually
    };

    // constructors
    Number (int num);
    Number (double num);

    Number (const Number& n);

    // set/get
    Number& operator= (const Number &n);

    double asFloat () const;

    // basic operations
    Number operator+ (const Number &n) const;
    Number operator- (const Number &n) const;
    Number operator* (const Number &n) const;
    Number operator/ (const Number &n) const;
    Number operator+ (double n) const;
    Number operator- (double n) const;
    Number operator* (double n) const;
    Number operator/ (double n) const;

    void operator+= (const Number &n);
    void operator-= (const Number &n);
    void operator*= (const Number &n);
    void operator/= (const Number &n);
    void operator+= (double n);
    void operator-= (double n);
    void operator*= (double n);
    void operator/= (double n);

    void operator++ () { return operator+= (1); };
    void operator-- () { return operator-= (1); };

    // comparison
    bool operator<= (const Number &n) const;
    bool operator< (const Number &n) const;
    bool operator== (const Number &n) const;
    bool operator!= (const Number &n) const { return (!operator== (n)); };
    bool operator>= (const Number &n) const { return (!operator< (n)); };
    bool operator> (const Number &n) const { return (!operator<= (n)); };
    bool operator<= (double n) const;
    bool operator< (double n) const;
    bool operator== (double n) const;
    bool operator!= (double n) const { return (!operator== (n)); };
    bool operator>= (double n) const { return (!operator< (n)); };
    bool operator> (double n) const { return (!operator<= (n)); };

    // absolute value
    Number abs () const;
    // negative value
    Number neg () const;
    // power
    Number pow (const Number &exp) const;
    Number pow (double exp) const;
    // logarithms
    Number log (double base) const;
    Number ln () const;

    // goniometric functions
    Number sin () const;
    Number cos () const;
    Number tg () const;
    Number cotg () const;
    Number asin () const;
    Number acos () const;
    Number atg () const;
    static Number atan2 (const Number &y, const Number &x);

    // hyperbolic functions
    Number sinh () const;
    Number cosh () const;
    Number tgh () const;
    Number asinh () const;
    Number acosh () const;
    Number atgh () const;

    // TODO: add more functions, as needed

    // TODO: functions to output the number to a string

  private:
    class Private;
    QSharedDataPointer<Private> d;

};  // class Number

// external operators, so that we can do things like 4+a without having to create temporary objects
// not provided for complex numbers, as we won't be using them often like that
Number operator+ (double n1, const Number &n2) { return n2 + n1; };
Number operator- (double n1, const Number &n2) { return (n2 - n1).neg(); };
Number operator* (double n1, const Number &n2) { return n2 * n1; };
Number operator/ (double n1, const Number &n2) { return Number(n1) / n2; /* TODO optimise perhaps */ };
bool operator<= (double n1, const Number &n2) { return (n2 >= n1); };
bool operator< (double n1, const Number &n2) { return (n2 > n1); };
bool operator== (double n1, const Number &n2) { return (n2 == n1); };
bool operator!= (double n1, const Number &n2) { return (n2 != n1); };
bool operator>= (double n1, const Number &n2) { return (n2 <= n1); };
bool operator> (double n1, const Number &n2) { return (n2 < n1); };

// external versions of the functions
Number abs (const Number &n) { return n.abs(); };
Number neg (const Number &n) { return n.neg(); };
Number pow (const Number &n, const Number &exp) { return n.pow (exp); };
Number pow (const Number &n, double exp) { return n.pow (exp); };
Number log (const Number &n, double base) { return n.log (base); };
Number ln (const Number &n) { return n.ln (); };
Number sin (const Number &n) { return n.sin(); };
Number cos (const Number &n) { return n.cos(); };
Number tg (const Number &n) { return n.tg(); };
Number cotg (const Number &n) { return n.cotg(); };
Number asin (const Number &n) { return n.asin(); };
Number acos (const Number &n) { return n.acos(); };
Number atg (const Number &n) { return n.atg(); };
Number atan2 (const Number &y, const Number &x) { return Number::atan2(y, x); };
Number sinh (const Number &n) { return n.sinh(); };
Number cosh (const Number &n) { return n.cosh(); };
Number tgh (const Number &n) { return n.tg(); };
Number asinh (const Number &n) { return n.asinh(); };
Number acosh (const Number &n) { return n.acosh(); };
Number atgh (const Number &n) { return n.atgh(); };

} // namespace KSpread

#endif // KSPREAD_NUMBER_H
