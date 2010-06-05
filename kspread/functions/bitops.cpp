/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2006 Brad Hards <bradh@frogmouth.net>

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

// built-in logical functions

#include "BitOpsModule.h"
#include "FunctionModuleRegistry.h"
#include "Functions.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_bitand(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitxor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitlshift(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitrshift(valVector args, ValueCalc *calc, FuncExtra *);


KSPREAD_EXPORT_FUNCTION_MODULE("bitops", BitOpsModule)


BitOpsModule::BitOpsModule(QObject* parent, const QVariantList&)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("BITAND", func_bitand);
    f->setParamCount(2);
  add(f);
    f = new Function("BITOR", func_bitor);
    f->setParamCount(2);
  add(f);
    f = new Function("BITXOR", func_bitxor);
    f->setParamCount(2);
  add(f);
    f = new Function("BITLSHIFT", func_bitlshift);
    f->setParamCount(2);
  add(f);
    f = new Function("BITRSHIFT", func_bitrshift);
    f->setParamCount(2);
  add(f);
}

QString BitOpsModule::descriptionFileName() const
{
    return QString("bitops.xml");
}


// Function: BITAND
Value func_bitand(valVector args, ValueCalc *, FuncExtra *)
{
    const quint64 x = args[0].asInteger();
    const quint64 y = args[1].asInteger();
    return Value(static_cast<qint64>(x & y));
}

// Function: BITOR
Value func_bitor(valVector args, ValueCalc *, FuncExtra *)
{
    const quint64 x = args[0].asInteger();
    const quint64 y = args[1].asInteger();
    return Value(static_cast<qint64>(x | y));
}

// Function: BITXOR
Value func_bitxor(valVector args, ValueCalc *, FuncExtra *)
{
    const quint64 x = args[0].asInteger();
    const quint64 y = args[1].asInteger();
    return Value(static_cast<qint64>(x ^ y));
}

// Function: BITLSHIFT
Value func_bitlshift(valVector args, ValueCalc *, FuncExtra *)
{
    const quint64 x = args[0].asInteger();
    const int numshift = args[1].asInteger();
    if (numshift == 0)
        return Value(static_cast<qint64>(x));
    else if (numshift > 0)
        return Value(static_cast<qint64>(x << numshift));
    else // negative left shift, becomes right shift
        return Value(static_cast<qint64>(x >>(-1 * numshift)));
}

// Function: BITRSHIFT
Value func_bitrshift(valVector args, ValueCalc *, FuncExtra *)
{
    const quint64 x = args[0].asInteger();
    const int numshift = args[1].asInteger();
    if (numshift == 0)
        return Value(static_cast<qint64>(x));
    else if (numshift > 0)
        return Value(static_cast<qint64>(x >> numshift));
    else // negative right shift, becomes left shift
        return Value(static_cast<qint64>(x << (-1 * numshift)));
}

#include "BitOpsModule.moc"
