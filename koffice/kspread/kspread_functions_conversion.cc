/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in conversion functions

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

using namespace KSpread;

// prototypes
Value func_arabic (valVector args, ValueCalc *calc, FuncExtra *);
Value func_carx (valVector args, ValueCalc *calc, FuncExtra *);
Value func_cary (valVector args, ValueCalc *calc, FuncExtra *);
Value func_decsex (valVector args, ValueCalc *calc, FuncExtra *);
Value func_polr (valVector args, ValueCalc *calc, FuncExtra *);
Value func_pola (valVector args, ValueCalc *calc, FuncExtra *);
Value func_roman (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sexdec (valVector args, ValueCalc *calc, FuncExtra *);
Value func_AsciiToChar (valVector args, ValueCalc *calc, FuncExtra *);
Value func_CharToAscii (valVector args, ValueCalc *calc, FuncExtra *);
Value func_inttobool (valVector args, ValueCalc *calc, FuncExtra *);
Value func_booltoint (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ToString (valVector args, ValueCalc *calc, FuncExtra *);

// registers all conversion functions
void RegisterConversionFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;
  
  f = new Function ("ARABIC", func_arabic);
  repo->add (f);
  f = new Function ("CARX", func_carx);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("CARY", func_cary);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("DECSEX", func_decsex);
  repo->add (f);
  f = new Function ("POLR", func_polr);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("POLA", func_pola);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ROMAN", func_roman);
  repo->add (f);
  f = new Function ("SEXDEC", func_sexdec);
  f->setParamCount (1, 3);
  repo->add (f);
  f = new Function ("ASCIITOCHAR", func_AsciiToChar);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("CHARTOASCII", func_CharToAscii);
  repo->add (f);
  f = new Function ("BOOL2INT", func_booltoint);
  repo->add (f);
  f = new Function ("INT2BOOL", func_inttobool);
  repo->add (f);
  f = new Function ("BOOL2STRING", func_ToString);
  repo->add (f);
  f = new Function ("NUM2STRING", func_ToString);
  repo->add (f);
  f = new Function ("STRING", func_ToString);
  repo->add (f);
}

// Function: POLR
Value func_polr (valVector args, ValueCalc *calc, FuncExtra *)
{
  // sqrt (a^2 + b^2)
  Value a = args[0];
  Value b = args[1];
  Value res = calc->sqrt (calc->add (calc->sqr (a), calc->sqr (b)));
  return res;
}

// Function: POLA
Value func_pola (valVector args, ValueCalc *calc, FuncExtra *)
{
  // acos (a / polr(a,b))
  Value polr = func_polr (args, calc, 0);
  if (calc->isZero (polr))
    return Value::errorDIV0();
  Value res = calc->acos (calc->div (args[0], polr));
  return res;
}

// Function: CARX
Value func_carx (valVector args, ValueCalc *calc, FuncExtra *)
{
  // a * cos(b)
  Value res = calc->mul (args[0], calc->cos (args[1]));
  return res;
}

// Function: CARY
Value func_cary (valVector args, ValueCalc *calc, FuncExtra *)
{
  // a * sin(b)
  Value res = calc->mul (args[0], calc->sin (args[1]));
  return res;
}

// Function: DECSEX
Value func_decsex (valVector args, ValueCalc *calc, FuncExtra *)
{
  // original function was very compicated, but I see no reason for that,
  // when it can be done as simply as this ...
  // maybe it was due to all the infrastructure not being ready back then
  return calc->conv()->asTime (calc->div (args[0], 24));
}

// Function: SEXDEC
Value func_sexdec (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 1)
  {
    // convert given value to number
    Value time = calc->conv()->asTime (args[0]);
    return calc->mul (calc->conv()->asFloat (time), 24);
  }
  
  // convert h/m/s to number of hours
  Value h = args[0];
  Value m = args[1];

  Value res = calc->add (h, calc->div (m, 60));
  if (args.count() == 3) {
    Value s = args[2];
    res = calc->add (res, calc->div (s, 3600));
  }
  return res;
}

// Function: ROMAN
Value func_roman (valVector args, ValueCalc *calc, FuncExtra *)
{
  const QCString RNUnits[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
  const QCString RNTens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
  const QCString RNHundreds[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
  const QCString RNThousands[] = {"", "M", "MM", "MMM"};

  // precision loss is not a problem here, as we only use the 0-3999 range
  long value = calc->conv()->asInteger (args[0]).asInteger ();
  if ((value < 0) || (value > 3999))
    return Value::errorNA();
  QString result;
  result = QString::fromLatin1 (RNThousands[(value / 1000)] +
                                RNHundreds[(value / 100) % 10] +
                                RNTens[(value / 10 ) % 10] +
                                RNUnits[(value) % 10]);
  return Value (result);
}

// convert single roman character to decimal
// return < 0 if invalid
int func_arabic_helper (QChar c)
{
  switch (c.upper().unicode())
  {
    case 'M': return 1000;
    case 'D': return 500;
    case 'C': return 100;
    case 'L': return 50;
    case 'X': return 10;
    case 'V': return 5;
    case 'I': return 1;
  }
  return -1;
}

// Function: ARABIC
Value func_arabic (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString roman = calc->conv()->asString (args[0]).asString();
  if( roman.isEmpty() ) return Value::errorVALUE();

  int val = 0, lastd = 0, d = 0;

  for (unsigned i = 0; i < roman.length(); i++) {
    d = func_arabic_helper( roman[i] );
    if( d < 0 ) return Value::errorVALUE();

    if( lastd < d ) val -= lastd;
      else val += lastd;
    lastd = d;
  }
  if( lastd < d ) val -= lastd;
    else val += lastd;

  return Value (val);
}

// helper for AsciiToChar
void func_a2c_helper (ValueCalc *calc, QString &s, Value val)
{
  if (val.isArray()) {
    for (unsigned int row = 0; row < val.rows(); ++row)
      for (unsigned int col = 0; col < val.columns(); ++col)
        func_a2c_helper (calc, s, val.element (col, row));
  } else {
    int v = calc->conv()->asInteger (val).asInteger();
    if (v == 0) return;
    QChar c (v);
    s = s + c;
  }
}

// Function: AsciiToChar
Value func_AsciiToChar (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str;
  for (unsigned int i = 0; i < args.count(); i++)
    func_a2c_helper (calc, str, args[i]);
  return Value (str);
}

// Function: CharToAscii
Value func_CharToAscii (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString val = calc->conv()->asString (args[0]).asString ();
  if (val.length() == 1)
    return Value (QString (val[0]));
  return Value::errorVALUE();
}

// Function: inttobool
Value func_inttobool (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asBoolean (args[0]);
}

// Function: booltoint
Value func_booltoint (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asInteger (args[0]);
}

// Function: BoolToString, NumberToString, String
Value func_ToString (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asString (args[0]);
}
