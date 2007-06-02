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

#include <math.h>

using namespace KSpread;

class Number::Private : public QSharedData {
 public:
  Private () {
    type = Number::Float;
    f = 0;
  }

  Private (const Private &o) : QSharedData (o), type (o.type)
  {
    switch (type)
    {
      case Number::Float:
        f = o.f;
        break;
    }
  }

  Number::Type type;

  union
  {
    double f;
  };

  private:
    void operator=( const Number::Private& o );
};

// constructors
Number::Number (int num)
{
  d->type = Number::Float;
  d->f = (double) num;
}

Number::Number (double num)
{
  d->type = Number::Float;
  d->f = num;
}

Number::Number (const Number& n)
  : d (n.d)
{
}

// set/get
Number& Number::operator= (const Number &n)
{
  d = n.d;
  return *this;
}


double Number::asFloat () const
{
  return d->f;
}

// basic operations
Number Number::operator+ (const Number &n) const
{
  return Number (d->f + n.d->f);
}

Number Number::operator- (const Number &n) const
{
  return Number (d->f - n.d->f);
}

Number Number::operator* (const Number &n) const
{
  return Number (d->f * n.d->f);
}

Number Number::operator/ (const Number &n) const
{
  return Number (d->f / n.d->f);
}

Number Number::operator+ (double n) const
{
  return Number (d->f + n);
}

Number Number::operator- (double n) const
{
  return Number (d->f - n);
}

Number Number::operator* (double n) const
{
  return Number (d->f * n);
}

Number Number::operator/ (double n) const
{
  return Number (d->f / n);
}

void Number::operator+= (const Number &n)
{
  d->f += n.d->f;
}

void Number::operator-= (const Number &n)
{
  d->f -= n.d->f;
}

void Number::operator*= (const Number &n)
{
  d->f *= n.d->f;
}

void Number::operator/= (const Number &n)
{
  d->f /= n.d->f;
}

void Number::operator+= (double n)
{
  d->f += n;
}

void Number::operator-= (double n)
{
  d->f -= n;
}

void Number::operator*= (double n)
{
  d->f *= n;
}

void Number::operator/= (double n)
{
  d->f /= n;
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

bool Number::operator<= (double n) const
{
  return (d->f <= n);
}

bool Number::operator< (double n) const
{
  return (d->f < n);
}

bool Number::operator== (double n) const
{
  return (d->f == n);
}

Number Number::abs () const
{
  return Number (fabs (d->f));
}

Number Number::neg () const
{
  return Number (-1 * d->f);
}

Number Number::pow (const Number &exp) const
{
  return Number (::pow (d->f, exp.d->f));
}

Number Number::pow (double exp) const
{
  return Number (::pow (d->f, exp));
}

Number Number::log (double base) const
{
  double logbase = log10 (base);
  return Number (log10 (d->f) / logbase);
}

Number Number::ln () const
{
  return Number (::log (d->f));
}

// goniometric functions
Number Number::sin () const
{
  return Number (::sin (d->f));
}

Number Number::cos () const
{
  return Number (::cos (d->f));
}

Number Number::tg () const
{
  return Number (::tan (d->f));
}

Number Number::cotg () const
{
  return Number (1 / ::tan (d->f));
}

Number Number::asin () const
{
  return Number (::asin (d->f));
}

Number Number::acos () const
{
  return Number (::acos (d->f));
}

Number Number::atg () const
{
  return Number (::atan (d->f));
}

Number Number::atan2 (const Number &y, const Number &x)
{
  return Number (::atan2 (y.d->f, x.d->f));
}


// hyperbolic functions
Number Number::sinh () const
{
  return Number (::sinh (d->f));
}

Number Number::cosh () const
{
  return Number (::cosh (d->f));
}

Number Number::tgh () const
{
  return Number (::tanh (d->f));
}

Number Number::asinh () const
{
  return Number (::asinh (d->f));
}

Number Number::acosh () const
{
  return Number (::acosh (d->f));
}

Number Number::atgh () const
{
  return Number (::atanh (d->f));
}

