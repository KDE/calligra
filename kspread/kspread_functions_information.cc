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
KSpreadValue func_filename (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_info (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isblank (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isdate (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_iseven (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_islogical (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isnottext (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isnum (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isodd (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_isref (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_istext (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_istime (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_n (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_type (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_version (valVector args, ValueCalc *calc, FuncExtra *);

// registers all information functions
void KSpreadRegisterInformationFunctions()
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
  f = new Function ("ISEVEN", func_iseven);
  repo->add (f);
  f = new Function ("ISLOGICAL", func_islogical);
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
KSpreadValue func_info (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString type = calc->conv()->asString (args[0]).asString().lower();

  if (type == "directory")
    return KSpreadValue (QDir::currentDirPath());

  if (type == "release")
    return KSpreadValue (QString (VERSION));

  if ( type == "numfile" )
    return KSpreadValue ((int) KSpreadDoc::documents().count());

  if (type == "recalc")
  {
    QString result;
    if (calc->doc()) {
      if (calc->doc()->delayCalculation())
        result = i18n ("Manual");
      else
        result = i18n ("Automatic");
    }
    return KSpreadValue (result);
  }

  if (type == "memavail")
    // not supported
    return KSpreadValue::errorVALUE();
  if (type == "memused")
    // not supported
    return KSpreadValue::errorVALUE();
  if (type == "origin")
    // not supported
    return KSpreadValue::errorVALUE();

  if (type == "system") {
    struct utsname name;
    if (uname (&name) >= 0)
      return KSpreadValue (QString (name.sysname));
  }

  if (type == "totmem")
    // not supported
    return KSpreadValue::errorVALUE();

  if (type == "osversion")
  {
    struct utsname name;
    if (uname (&name) >= 0)
    {
       QString os = QString("%1 %2 (%3)").arg( name.sysname ).
         arg( name.release ).arg( name.machine );
       return KSpreadValue (os);
    }
  }

  return KSpreadValue::errorVALUE();
}

// Function: ISBLANK
KSpreadValue func_isblank (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue (args[0].isEmpty());
}

// Function: ISLOGICAL
KSpreadValue func_islogical (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue (args[0].isBoolean());
}

// Function: ISTEXT
KSpreadValue func_istext (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue (args[0].isString());
}

// Function: ISREF
KSpreadValue func_isref (valVector, ValueCalc */*calc*/, FuncExtra *e)
{
  // no reference ?
  if ((e == 0) || (e->ranges[0].col1 == -1) || (e->ranges[0].row1 == -1))
    return KSpreadValue (false);
  // if we are here, it is a reference (cell/range)
  return KSpreadValue (true);
}

// Function: ISNOTTEXT
KSpreadValue func_isnottext (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue (args[0].isString() ? false : true);
}

// Function: ISNUM
KSpreadValue func_isnum (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue (args[0].isNumber());
}

// Function: ISTIME
KSpreadValue func_istime (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue ((args[0].format() == KSpreadValue::fmt_Time)
      || (args[0].format() == KSpreadValue::fmt_DateTime));
}

// Function: ISDATE
KSpreadValue func_isdate (valVector args, ValueCalc *, FuncExtra *)
{
  return KSpreadValue ((args[0].format() == KSpreadValue::fmt_Date)
      || (args[0].format() == KSpreadValue::fmt_DateTime));
}

// Function: ISODD
KSpreadValue func_isodd (valVector args, ValueCalc *calc, FuncExtra *)
{
  return KSpreadValue (calc->isEven(args[0]) ? false : true);
}

// Function: ISEVEN
KSpreadValue func_iseven (valVector args, ValueCalc *calc, FuncExtra *)
{
  return KSpreadValue (calc->isEven(args[0]));
}

// Function: TYPE
KSpreadValue func_type (valVector args, ValueCalc *, FuncExtra *)
{
  // Returns 1 for numbers, 2 for text, 4 for boolean, 16 for error,
  // 64 for arrays
  if (args[0].isArray())
    return KSpreadValue (64);
  if (args[0].isNumber())
    return KSpreadValue (1);
  if (args[0].isString())
    return KSpreadValue (2);
  if (args[0].isBoolean())
    return KSpreadValue (4);
  if (args[0].isError())
    return KSpreadValue (16);
  
  // something else ?
  return KSpreadValue (0);
}

KSpreadValue func_filename (valVector, ValueCalc *calc, FuncExtra *)
{
  return KSpreadValue (calc->doc()->url().prettyURL());
}

// Function: N
KSpreadValue func_n (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asFloat (args[0]);
}
