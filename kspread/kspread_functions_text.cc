/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
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


// built-in text functions
// please keep it in alphabetical order

#include <QRegExp>
#include <kdebug.h>
#include <klocale.h>
#include <math.h>

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

using namespace KSpread;

// Functions DOLLAR and FIXED convert data to double, hence they will not
// support arbitrary precision, when it will be introduced.

// prototypes
Value func_char (valVector args, ValueCalc *calc, FuncExtra *);
Value func_clean (valVector args, ValueCalc *calc, FuncExtra *);
Value func_code (valVector args, ValueCalc *calc, FuncExtra *);
Value func_compare (valVector args, ValueCalc *calc, FuncExtra *);
Value func_concatenate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollar (valVector args, ValueCalc *calc, FuncExtra *);
Value func_exact (valVector args, ValueCalc *calc, FuncExtra *);
Value func_find (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fixed (valVector args, ValueCalc *calc, FuncExtra *);
Value func_left (valVector args, ValueCalc *calc, FuncExtra *);
Value func_len (valVector args, ValueCalc *calc, FuncExtra *);
Value func_lower (valVector args, ValueCalc *calc, FuncExtra *);
Value func_mid (valVector args, ValueCalc *calc, FuncExtra *);
Value func_proper (valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexp (valVector args, ValueCalc *calc, FuncExtra *);
Value func_regexpre (valVector args, ValueCalc *calc, FuncExtra *);
Value func_replace (valVector args, ValueCalc *calc, FuncExtra *);
Value func_rept (valVector args, ValueCalc *calc, FuncExtra *);
Value func_rot (valVector args, ValueCalc *calc, FuncExtra *);
Value func_right (valVector args, ValueCalc *calc, FuncExtra *);
Value func_search (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sleek (valVector args, ValueCalc *calc, FuncExtra *);
Value func_substitute (valVector args, ValueCalc *calc, FuncExtra *);
Value func_t (valVector args, ValueCalc *calc, FuncExtra *);
Value func_text (valVector args, ValueCalc *calc, FuncExtra *);
Value func_toggle (valVector args, ValueCalc *calc, FuncExtra *);
Value func_trim (valVector args, ValueCalc *calc, FuncExtra *);
Value func_upper (valVector args, ValueCalc *calc, FuncExtra *);
Value func_value (valVector args, ValueCalc *calc, FuncExtra *);

// registers all text functions
void RegisterTextFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  // one-parameter functions
  f = new Function ("CHAR", func_char);
  repo->add (f);
  f = new Function ("CLEAN", func_clean);
  repo->add (f);
  f = new Function ("CODE", func_code);
  repo->add (f);
  f = new Function ("LEN", func_len);
  repo->add (f);
  f = new Function ("LOWER", func_lower);
  repo->add (f);
  f = new Function ("PROPER", func_proper);
  repo->add (f);
  f = new Function ("ROT", func_rot);
  repo->add (f);
  f = new Function ("SLEEK", func_sleek);
  repo->add (f);
  f = new Function ("T", func_t);
  repo->add (f);
  f = new Function ("TOGGLE", func_toggle);
  repo->add (f);
  f = new Function ("TRIM", func_trim);
  repo->add (f);
  f = new Function ("UPPER", func_upper);
  repo->add (f);
  f = new Function ("VALUE", func_value);
  repo->add (f);

  // other functions
  f = new Function ("COMPARE", func_compare);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("CONCATENATE", func_concatenate);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("DOLLAR", func_dollar);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("EXACT", func_exact);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("FIND", func_find);
  f->setParamCount (2, 3);
  repo->add (f);
  f = new Function ("FIXED", func_fixed);
  f->setParamCount (1, 3);
  repo->add (f);
  f = new Function ("LEFT", func_left);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("MID", func_mid);
  f->setParamCount (2, 3);
  repo->add (f);
  f = new Function ("REGEXP", func_regexp);
  f->setParamCount (2, 4);
  repo->add (f);
  f = new Function ("REGEXPRE", func_regexpre);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("REPLACE", func_replace);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("REPT", func_rept);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RIGHT", func_right);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("SEARCH", func_search);
  f->setParamCount (2, 3);
  repo->add (f);
  f = new Function ("SUBSTITUTE", func_substitute);
  f->setParamCount (3, 4);
  repo->add (f);
  f = new Function ("TEXT", func_text);
  f->setParamCount (1, 2);
  repo->add (f);
}


// Function: CHAR
Value func_char (valVector args, ValueCalc *calc, FuncExtra *)
{
  int val = calc->conv()->asInteger (args[0]).asInteger ();
  return Value (QString (QChar (val)));
}

// Function: CLEAN
Value func_clean (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str (calc->conv()->asString (args[0]).asString());
  QString result;
  QChar   c;
  int     i;
  int     l = str.length();

  for (i = 0; i < l; ++i)
  {
    c = str[i];
    if (c.isPrint())
      result += c;
  }

  return Value (result);
}

// Function: CODE
Value func_code (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str (calc->conv()->asString (args[0]).asString());
  if (str.length() <= 0)
     return Value::errorVALUE();

  return Value (str[0].unicode());
}

// Function: COMPARE
Value func_compare (valVector args, ValueCalc *calc, FuncExtra *)
{
  int  result = 0;
  bool exact = calc->conv()->asBoolean (args[2]).asBoolean();

  QString s1 = calc->conv()->asString (args[0]).asString();
  QString s2 = calc->conv()->asString (args[1]).asString();

  if (!exact)
    result = s1.toLower().localeAwareCompare(s2.toLower());
  else
    result = s1.localeAwareCompare(s2);

  if (result < 0)
    result = -1;
  else if (result > 0)
    result = 1;

  return Value (result);
}

void func_concatenate_helper (Value val, ValueCalc *calc,
    QString& tmp)
{
  if (val.isArray()) {
    for (unsigned int row = 0; row < val.rows(); ++row)
      for (unsigned int col = 0; col < val.columns(); ++col)
        func_concatenate_helper (val.element (col, row), calc, tmp);
  } else
    tmp += calc->conv()->asString (val).asString();
}

// Function: CONCATENATE
Value func_concatenate (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString tmp;
  for (int i = 0; i < args.count(); ++i)
    func_concatenate_helper (args[i], calc, tmp);

  return Value (tmp);
}

// Function: DOLLAR
Value func_dollar (valVector args, ValueCalc *calc, FuncExtra *)
{
  // ValueConverter doesn't support money directly, hence we need to
  // use the locale. This code has the same effect as the output
  // of ValueFormatter for money format.

  // This function converts data to double/int, hence it won't support
  // larger precision.

  double value = calc->conv()->asFloat (args[0]).asFloat();
  int precision = 2;
  if (args.count() == 2)
    precision = calc->conv()->asInteger (args[1]).asInteger();

  // do round, because formatMoney doesn't
  value = floor (value * pow (10.0, precision) + 0.5) / pow (10.0, precision);

  KLocale *locale = calc->conv()->locale();
  QString s = locale->formatMoney (value, locale->currencySymbol(), precision);

  return Value (s);
}

// Function: EXACT
Value func_exact (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString s1 = calc->conv()->asString (args[0]).asString();
  QString s2 = calc->conv()->asString (args[1]).asString();
  bool exact = (s1 == s2);
  return Value (exact);
}

// Function: FIND
Value func_find (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString find_text, within_text;
  int start_num = 1;

  find_text = calc->conv()->asString (args[0]).asString();
  within_text = calc->conv()->asString (args[1]).asString();
  if (args.count() == 3)
    start_num = calc->conv()->asInteger (args[2]).asInteger();

  // conforms to Excel behaviour
  if (start_num <= 0) return Value::errorVALUE();
  if (start_num > (int)within_text.length()) return Value::errorVALUE();

  int pos = within_text.indexOf(find_text, start_num - 1);
  if( pos < 0 ) return Value::errorNA();

  return Value (pos + 1);
}

// Function: FIXED
Value func_fixed (valVector args, ValueCalc *calc, FuncExtra *)
{
  // uses double, hence won't support big precision

  int decimals = 2;
  bool no_commas = false;

  double number = calc->conv()->asFloat (args[0]).asFloat();
  if (args.count() > 1)
    decimals = calc->conv()->asInteger (args[1]).asInteger();
  if (args.count() == 3)
    no_commas = calc->conv()->asBoolean (args[2]).asBoolean();

  QString result;
  KLocale *locale = calc->conv()->locale();

  // unfortunately, we can't just use KLocale::formatNumber because
  // * if decimals < 0, number is rounded
  // * if no_commas is true, thousand separators shouldn't show up

  if( decimals < 0 )
  {
    decimals = -decimals;
    number = floor( number/pow(10.0,decimals)+0.5 ) * pow(10.0,decimals);
    decimals = 0;
  }

  bool neg = number < 0;
  result = QString::number( neg ? -number:number, 'f', decimals );

  int pos = result.indexOf('.');
  if (pos == -1) pos = result.length();
    else result.replace(pos, 1, locale->decimalSymbol());
  if( !no_commas )
    while (0 < (pos -= 3))
      result.insert(pos, locale->thousandsSeparator());

  result.prepend( neg ? locale->negativeSign():
    locale->positiveSign() );

  return Value (result);
}

// Function: LEFT
Value func_left (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString();
  int nb = 1;
  if (args.count() == 2)
    nb = calc->conv()->asInteger (args[1]).asInteger();

  return Value (str.left (nb));
}

// Function: LEN
Value func_len (valVector args, ValueCalc *calc, FuncExtra *)
{
  int nb = calc->conv()->asString (args[0]).asString().length();
  return Value (nb);
}

// Function: LOWER
Value func_lower (valVector args, ValueCalc *calc, FuncExtra *)
{
  return Value (calc->conv()->asString (args[0]).asString().toLower());
}

// Function: MID
Value func_mid (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString();
  int pos = calc->conv()->asInteger (args[1]).asInteger();
  uint len = 0xffffffff;
  if (args.count() == 3)
    len = (uint) calc->conv()->asInteger (args[2]).asInteger();

  // Excel compatible
  pos--;

  return Value (str.mid (pos, len));
}

// Function: PROPER
Value func_proper (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString().toLower();

  QChar f;
  bool  first = true;

  for (int i = 0; i < str.length(); ++i)
  {
    if (first)
    {
      f = str[i];
      if (f.isNumber())
        continue;

      f = f.toUpper();

      str[i] = f;
      first = false;

      continue;
    }

    if (str[i] == ' ' || str[i] == '-')
      first = true;
  }

  return Value (str);
}

// Function: REGEXP
Value func_regexp (valVector args, ValueCalc *calc, FuncExtra *)
{
  // ensure that we got a valid regular expression
  QRegExp exp (calc->conv()->asString (args[1]).asString());
  if (!exp.isValid ())
    return Value::errorVALUE();

  QString s = calc->conv()->asString (args[0]).asString();
  QString defText;
  if (args.count() > 2)
    defText = calc->conv()->asString (args[2]).asString();
  int bkref = 0;
  if (args.count() == 4)
    bkref = calc->conv()->asInteger (args[3]).asInteger();
  if (bkref < 0)   // strange back-reference
    return Value::errorVALUE();

  QString returnValue;

  int pos = exp.indexIn(s);
  if (pos == -1)
    returnValue = defText;
  else
    returnValue = exp.cap (bkref);

  return Value (returnValue);
}

// Function: REGEXPRE
Value func_regexpre (valVector args, ValueCalc *calc, FuncExtra *)
{
  // ensure that we got a valid regular expression
  QRegExp exp (calc->conv()->asString (args[1]).asString());
  if (!exp.isValid ())
    return Value::errorVALUE();

  QString s = calc->conv()->asString (args[0]).asString();
  QString str = calc->conv()->asString (args[2]).asString();

  int pos = 0;
  while ((pos = exp.indexIn(s, pos)) != -1)
  {
    int i = exp.matchedLength();
    s = s.replace (pos, i, str);
    pos += str.length();
  }

  return Value (s);
}

// Function: REPLACE
Value func_replace (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString text = calc->conv()->asString (args[0]).asString();
  int pos = calc->conv()->asInteger (args[1]).asInteger();
  int len = calc->conv()->asInteger (args[2]).asInteger();
  QString new_text = calc->conv()->asString (args[3]).asString();

  if (pos < 0) pos = 0;

  QString result = text.replace (pos-1, len, new_text);
  return Value (result);
}

// Function: REPT
Value func_rept (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString s = calc->conv()->asString (args[0]).asString();
  int nb = calc->conv()->asInteger (args[1]).asInteger();

  QString result;
  for (int i = 0; i < nb; i++) result += s;
  return Value (result);
}

// Function: RIGHT
Value func_right (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString();
  int nb = 1;
  if (args.count() == 2)
    nb = calc->conv()->asInteger (args[1]).asInteger();

  return Value (str.right (nb));
}

// Function: ROT
Value func_rot (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString text = calc->conv()->asString (args[0]).asString();

  for( int i=0; i<text.length(); i++ )
  {
    unsigned c = text[i].toUpper().unicode();
    if( ( c >= 'A' ) && ( c <= 'M' ) )
      text[i] = QChar( text[i].unicode() + 13);
    if( ( c >= 'N' ) && ( c <= 'Z' ) )
      text[i] = QChar( text[i].unicode() - 13);
  }

  return Value (text);
}

// Function: SEARCH
Value func_search (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString find_text = calc->conv()->asString (args[0]).asString();
  QString within_text = calc->conv()->asString (args[1]).asString();
  int start_num = 1;
  if (args.count() == 3)
    start_num = calc->conv()->asInteger (args[2]).asInteger();

  // conforms to Excel behaviour
  if (start_num <= 0) return Value::errorVALUE();
  if (start_num > (int)within_text.length()) return Value::errorVALUE();

  // use globbing feature of QRegExp
  QRegExp regex( find_text, Qt::CaseInsensitive, QRegExp::Wildcard );
  int pos = within_text.indexOf( regex, start_num-1 );
  if( pos < 0 ) return Value::errorNA();

  return Value (pos + 1);
}

// Function: SLEEK
Value func_sleek (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString();
  QString result;
  QChar   c;
  int     i;
  int     l = str.length();

  for (i = 0; i < l; ++i)
  {
    c = str[i];
    if (!c.isSpace())
      result += c;
  }

  return Value (result);
}

// Function: SUBSTITUTE
Value func_substitute (valVector args, ValueCalc *calc, FuncExtra *)
{
  int num = 1;
  bool all = true;

  if (args.count() == 4)
  {
    num = calc->conv()->asInteger (args[3]).asInteger();
    all = false;
  }

  QString text = calc->conv()->asString (args[0]).asString();
  QString old_text = calc->conv()->asString (args[1]).asString();
  QString new_text = calc->conv()->asString (args[2]).asString();

  if( num <= 0 ) return Value::errorVALUE();
  if (old_text.length() == 0) return Value (text);

  QString result = text;

  int p = result.indexOf(old_text);
  while ((p != -1) && (num > 0))
  {
    result.replace( p, old_text.length(), new_text );
    // find another location, starting straight after the replaced text
    p = result.indexOf(old_text, p + new_text.length());
    if( !all ) num--;
  }

  return Value (result);
}

// Function: T
Value func_t (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asString (args[0]);
}

// Function: TEXT
Value func_text (valVector args, ValueCalc *calc, FuncExtra *)
{
  //Currently the same as the T function ...
  //Second parameter is format_text. It is currently ignored.
  return calc->conv()->asString (args[0]);
}

// Function: TOGGLE
Value func_toggle (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString str = calc->conv()->asString (args[0]).asString();
  int i;
  int l = str.length();

  for (i = 0; i < l; ++i)
  {
    QChar c = str[i];
    QChar lc = c.toLower();
    QChar uc = c.toUpper();

    if (c == lc) // it is in lowercase
      str[i] = c.toUpper();
    else if (c == uc) // it is in uppercase
      str[i] = c.toLower();
  }

  return Value (str);
}

// Function: TRIM
Value func_trim (valVector args, ValueCalc *calc, FuncExtra *)
{
  return Value (
      calc->conv()->asString (args[0]).asString().simplified());
}

// Function: UPPER
Value func_upper (valVector args, ValueCalc *calc, FuncExtra *)
{
  return Value (calc->conv()->asString (args[0]).asString().toUpper());
}

// Function: VALUE
Value func_value (valVector args, ValueCalc *calc, FuncExtra *)
{
  // same as the N function
  return calc->conv()->asFloat (args[0]);
}
