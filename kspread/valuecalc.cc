/* This file is part of the KDE project
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "valuecalc.h"

#include "valueconverter.h"

#include <math.h>

#include <kdebug.h>

using namespace KSpread;

KLocale *ValueCalc::locale = 0;

ValueCalc* ValueCalc::_self = 0;

ValueCalc * ValueCalc::self ()
{
  if (!_self)
  {
    _self = new ValueCalc;
  }
  return _self;
}

ValueCalc::~ValueCalc ()
{
  _self = 0;
}

KSpreadValue ValueCalc::add (const KSpreadValue &a, const KSpreadValue &b)
{
  ValueConverter *vc = ValueConverter::self();
  double aa, bb;
  aa = vc->asFloat (a, locale).asFloat();
  bb = vc->asFloat (b, locale).asFloat();
  KSpreadValue res = KSpreadValue (aa + bb);

  if (a.isNumber())
    res.setFormat (format (a.format(), b.format()));

  return res;
}

KSpreadValue ValueCalc::sub (const KSpreadValue &a, const KSpreadValue &b)
{
  ValueConverter *vc = ValueConverter::self();
  double aa, bb;
  aa = vc->asFloat (a, locale).asFloat();
  bb = vc->asFloat (b, locale).asFloat();
  KSpreadValue res = KSpreadValue (aa - bb);

  if (a.isNumber())
    res.setFormat (format (a.format(), b.format()));

  return res;
}

KSpreadValue ValueCalc::mul (const KSpreadValue &a, const KSpreadValue &b)
{
  ValueConverter *vc = ValueConverter::self();
  double aa, bb;
  aa = vc->asFloat (a, locale).asFloat();
  bb = vc->asFloat (b, locale).asFloat();
  KSpreadValue res = KSpreadValue (aa * bb);

  if (a.isNumber())
    res.setFormat (format (a.format(), b.format()));

  return res;
}

KSpreadValue ValueCalc::div (const KSpreadValue &a, const KSpreadValue &b)
{
  ValueConverter *vc = ValueConverter::self();
  double aa, bb;
  aa = vc->asFloat (a, locale).asFloat();
  bb = vc->asFloat (b, locale).asFloat();
  KSpreadValue res;
  if (bb == 0.0)
    return KSpreadValue::errorDIV0();
  else
    res = KSpreadValue (aa / bb);

  if (a.isNumber())
    res.setFormat (format (a.format(), b.format()));

  return res;
}

KSpreadValue ValueCalc::pow (const KSpreadValue &a, const KSpreadValue &b)
{
  ValueConverter *vc = ValueConverter::self();
  double aa, bb;
  aa = vc->asFloat (a, locale).asFloat();
  bb = vc->asFloat (b, locale).asFloat();
  KSpreadValue res = KSpreadValue (::pow (aa, bb));

  if (a.isNumber())
    res.setFormat (format (a.format(), b.format()));

  return res;
}

KSpreadValue ValueCalc::add (const KSpreadValue &a, double b)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res = KSpreadValue (vc->asFloat(a, locale).asFloat() + b);

  if (a.isNumber())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::sub (const KSpreadValue &a, double b)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res = KSpreadValue (vc->asFloat(a, locale).asFloat() - b);

  if (a.isNumber())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::mul (const KSpreadValue &a, double b)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res = KSpreadValue (vc->asFloat(a, locale).asFloat() * b);

  if (a.isNumber())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::div (const KSpreadValue &a, double b)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res;
  if (b == 0.0)
    return KSpreadValue::errorDIV0();

  res = KSpreadValue (vc->asFloat(a, locale).asFloat() / b);

  if (a.isNumber())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::pow (const KSpreadValue &a, double b)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res = KSpreadValue (::pow (vc->asFloat(a, locale).asFloat(), b));

  if (a.isNumber())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::log (const KSpreadValue &number,
    const KSpreadValue &base)
{
  ValueConverter *vc = ValueConverter::self();

  double logbase = vc->asFloat (base, locale).asFloat();
  if (logbase == 1.0)
    return KSpreadValue::errorDIV0();
  if (logbase <= 0.0)
    return KSpreadValue::errorNA();

  logbase = log10 (logbase);
  KSpreadValue res = KSpreadValue (log10 (vc->asFloat (number, locale).asFloat()) / logbase);

  if (number.isNumber())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::ln (const KSpreadValue &number)
{
  ValueConverter *vc = ValueConverter::self();
  KSpreadValue res = KSpreadValue (::log (vc->asFloat (number, locale).asFloat()));

  if (number.isNumber())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::log (const KSpreadValue &number, double base)
{
  ValueConverter *vc = ValueConverter::self();

  if (base <= 0.0)
    return KSpreadValue::errorNA();
  if (base == 1.0)
    return KSpreadValue::errorDIV0();

  double num = vc->asFloat (number, locale).asFloat();
  KSpreadValue res = KSpreadValue (log10 (num) / log10 (base));

  if (number.isNumber())
    res.setFormat (number.format());

  return res;
}

// ------------------------------------------------------

KSpreadValue ValueCalc::sum (const KSpreadValue &range)
{
  ValueConverter *vc = ValueConverter::self();

  if (!range.isArray())
    return vc->asFloat (range, locale);

  //if we are here, we have an array
  KSpreadValue res;
  KSpreadValue::Format fmt = KSpreadValue::fmt_None;

  int rows = range.rows ();
  int cols = range.columns ();
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      if (v.isArray())
        res = add (res, sum (v));
      else
        res = add (res, v);
      if (fmt == KSpreadValue::fmt_None)
        fmt = range.element (c, r).format ();
    }

  res.setFormat (fmt);
  return res;
}

int ValueCalc::count (const KSpreadValue &range)
{
  if (!range.isArray())
    return range.isEmpty() ? 0 : 1;

  int res = 0;

  int cols = range.columns ();
  int rows = range.rows ();
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      if (v.isArray())
        res += count (v);
      else
        if (!range.element (c, r).isEmpty())
          res++;
    }

  return res;
}

KSpreadValue ValueCalc::avg (const KSpreadValue &range)
{
  int cnt = count (range);
  if (cnt)
    return div (sum (range), cnt);
  return KSpreadValue (0.0);
}

KSpreadValue ValueCalc::max (const KSpreadValue &range)
{
  ValueConverter *vc = ValueConverter::self();

  if (!range.isArray())
    return vc->asFloat (range, locale);

  //if we are here, we have an array
  KSpreadValue res;

  int rows = range.rows ();
  int cols = range.columns ();
  bool got = false;
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      if (v.isArray())
        v = max (v);
      if (!v.isEmpty ())
        if (got)
        {
          if (v.greater (res))
            res = v;
        }
        else
        {
          res = v;
          got = true;
        }
    }

  return res;
}

KSpreadValue ValueCalc::min (const KSpreadValue &range)
{
  ValueConverter *vc = ValueConverter::self();

  if (!range.isArray())
    return vc->asFloat (range, locale);

  //if we are here, we have an array
  KSpreadValue res;

  int rows = range.rows ();
  int cols = range.columns ();
  bool got = false;
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      if (v.isArray())
        v = max (v);
      if (!v.isEmpty ())
        if (got)
        {
          if (v.less (res))
            res = v;
        }
        else
        {
          res = v;
          got = true;
        }
    }

  return res;
}

KSpreadValue::Format ValueCalc::format (KSpreadValue::Format a,
    KSpreadValue::Format b)
{
  if ((a == KSpreadValue::fmt_None) || (a == KSpreadValue::fmt_Boolean))
    return b;
  return a;
}

