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

// built-in information functions


#include <config.h>
#include <sys/utsname.h>

#include <qdir.h>
#include <kdebug.h>
#include <klocale.h>

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

#include "kspread_doc.h"
#include "kspread_sheet.h"

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_filename (valVector args, ValueCalc *calc, FuncExtra *);
Value func_info (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isblank (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isdate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_iserr (valVector args, ValueCalc *calc, FuncExtra *);
Value func_iserror (valVector args, ValueCalc *calc, FuncExtra *);
Value func_iseven (valVector args, ValueCalc *calc, FuncExtra *);
Value func_islogical (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isna (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isnottext (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isnum (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isodd (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isref (valVector args, ValueCalc *calc, FuncExtra *);
Value func_istext (valVector args, ValueCalc *calc, FuncExtra *);
Value func_istime (valVector args, ValueCalc *calc, FuncExtra *);
Value func_n (valVector args, ValueCalc *calc, FuncExtra *);
Value func_type (valVector args, ValueCalc *calc, FuncExtra *);
Value func_version (valVector args, ValueCalc *calc, FuncExtra *);

// registers all information functions
void RegisterInformationFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("FILENAME", func_filename);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("INFO", func_info);
  repo->add (f);
  f = new Function ("ISBLANK", func_isblank);
  repo->add (f);
  f = new Function ("ISDATE", func_isdate);
  repo->add (f);
  f = new Function ("ISERR", func_iserr);
  repo->add (f);
  f = new Function ("ISERROR", func_iserror);
  repo->add (f);
  f = new Function ("ISEVEN", func_iseven);
  repo->add (f);
  f = new Function ("ISLOGICAL", func_islogical);
  repo->add (f);
  f = new Function ("ISNA", func_isna);
  repo->add (f);
  f = new Function ("ISNONTEXT", func_isnottext);
  repo->add (f);
  f = new Function ("ISNOTTEXT", func_isnottext);
  repo->add (f);
  f = new Function ("ISNUM", func_isnum);
  repo->add (f);
  f = new Function ("ISNUMBER", func_isnum);
  repo->add (f);
  f = new Function ("ISODD", func_isodd);
  repo->add (f);
  f = new Function ("ISREF", func_isref);
  f->setNeedsExtra (true);
  repo->add (f);
  f = new Function ("ISTEXT", func_istext);
  repo->add (f);
  f = new Function ("ISTIME", func_istime);
  repo->add (f);
  f = new Function ("N", func_n);
  repo->add (f);
  f = new Function ("TYPE", func_type);
  f->setAcceptArray ();
  repo->add (f);
}

// Function: INFO
Value func_info (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString type = calc->conv()->asString (args[0]).asString().lower();

  if (type == "directory")
    return Value (QDir::currentDirPath());

  if (type == "release")
    return Value (QString (VERSION));

  if ( type == "numfile" )
    return Value ((int) Doc::documents().count());

  if (type == "recalc")
  {
    QString result;
    if (calc->doc()) {
      if (calc->doc()->delayCalculation())
        result = i18n ("Manual");
      else
        result = i18n ("Automatic");
    }
    return Value (result);
  }

  if (type == "memavail")
    // not supported
    return Value::errorVALUE();
  if (type == "memused")
    // not supported
    return Value::errorVALUE();
  if (type == "origin")
    // not supported
    return Value::errorVALUE();

  if (type == "system") {
    struct utsname name;
    if (uname (&name) >= 0)
      return Value (QString (name.sysname));
  }

  if (type == "totmem")
    // not supported
    return Value::errorVALUE();

  if (type == "osversion")
  {
    struct utsname name;
    if (uname (&name) >= 0)
    {
       QString os = QString("%1 %2 (%3)").arg( name.sysname ).
         arg( name.release ).arg( name.machine );
       return Value (os);
    }
  }

  return Value::errorVALUE();
}

// Function: ISBLANK
Value func_isblank (valVector args, ValueCalc *, FuncExtra *)
{
  return Value (args[0].isEmpty());
}

// Function: ISLOGICAL
Value func_islogical (valVector args, ValueCalc *, FuncExtra *)
{
  return Value (args[0].isBoolean());
}

// Function: ISTEXT
Value func_istext (valVector args, ValueCalc *, FuncExtra *)
{
  return Value (args[0].isString());
}

// Function: ISREF
Value func_isref (valVector, ValueCalc */*calc*/, FuncExtra *e)
{
  // no reference ?
  if ((e == 0) || (e->ranges[0].col1 == -1) || (e->ranges[0].row1 == -1))
    return Value (false);
  // if we are here, it is a reference (cell/range)
  return Value (true);
}

// Function: ISNOTTEXT
Value func_isnottext (valVector args, ValueCalc *, FuncExtra *)
{
  return Value (args[0].isString() ? false : true);
}

// Function: ISNUM
Value func_isnum (valVector args, ValueCalc *, FuncExtra *)
{
  return Value (args[0].isNumber());
}

// Function: ISTIME
Value func_istime (valVector args, ValueCalc *, FuncExtra *)
{
  return Value ((args[0].format() == Value::fmt_Time)
      || (args[0].format() == Value::fmt_DateTime));
}

// Function: ISDATE
Value func_isdate (valVector args, ValueCalc *, FuncExtra *)
{
  return Value ((args[0].format() == Value::fmt_Date)
      || (args[0].format() == Value::fmt_DateTime));
}

// Function: ISODD
Value func_isodd (valVector args, ValueCalc *calc, FuncExtra *)
{
  return Value (calc->isEven(args[0]) ? false : true);
}

// Function: ISEVEN
Value func_iseven (valVector args, ValueCalc *calc, FuncExtra *)
{
  return Value (calc->isEven(args[0]));
}

// Function: ISERR
Value func_iserr (valVector args, ValueCalc *, FuncExtra *)
{
  return (args[0].isError() &&
      (args[0].errorMessage() != Value::errorNA().errorMessage()));
}

// Function: ISERROR
Value func_iserror (valVector args, ValueCalc *, FuncExtra *)
{
  return args[0].isError();
}

// Function: ISNA
Value func_isna (valVector args, ValueCalc *, FuncExtra *)
{
  return (args[0].isError() &&
      (args[0].errorMessage() == Value::errorNA().errorMessage()));
}

// Function: TYPE
Value func_type (valVector args, ValueCalc *, FuncExtra *)
{
  // Returns 1 for numbers, 2 for text, 4 for boolean, 16 for error,
  // 64 for arrays
  if (args[0].isArray())
    return Value (64);
  if (args[0].isNumber())
    return Value (1);
  if (args[0].isString())
    return Value (2);
  if (args[0].isBoolean())
    return Value (4);
  if (args[0].isError())
    return Value (16);
  
  // something else ?
  return Value (0);
}

Value func_filename (valVector, ValueCalc *calc, FuncExtra *)
{
  return Value (calc->doc()->url().prettyURL());
}

// Function: N
Value func_n (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asFloat (args[0]);
}
