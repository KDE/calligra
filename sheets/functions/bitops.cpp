// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2006 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in logical functions

#include "BitOpsModule.h"

#include "engine/Function.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

#include <cmath>

using namespace Calligra::Sheets;

// prototypes (sorted alphabetically)
Value func_bitand(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitxor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitlshift(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitrshift(valVector args, ValueCalc *calc, FuncExtra *);

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadbitopsmodule.json", BitOpsModule)

BitOpsModule::BitOpsModule(QObject *parent, const QVariantList &)
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

// operand must be a non-negative integer below 2^48; returns an error, or a null Value if valid
static Value checkBitOperand(ValueCalc *calc, const Value &v)
{
    if (v.isError())
        return v;
    bool ok = true;
    const Number n = calc->conv()->asFloat(v, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();
    if (n != std::floor(n) || n < 0 || n >= 281474976710656.0) // 2^48
        return Value::errorNUM();
    return Value();
}

// a shift amount must be an integer with |amount| <= 53
static Value checkShiftAmount(ValueCalc *calc, const Value &v)
{
    if (v.isError())
        return v;
    bool ok = true;
    const Number n = calc->conv()->asFloat(v, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();
    if (n != std::floor(n) || n < -53 || n > 53)
        return Value::errorNUM();
    return Value();
}

// Function: BITAND
Value func_bitand(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value err = checkBitOperand(calc, args[0]);
    if (!err.isEmpty())
        return err;
    err = checkBitOperand(calc, args[1]);
    if (!err.isEmpty())
        return err;

    const uint64_t x = args[0].asInteger();
    const uint64_t y = args[1].asInteger();
    return Value(static_cast<int64_t>(x & y));
}

// Function: BITOR
Value func_bitor(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value err = checkBitOperand(calc, args[0]);
    if (!err.isEmpty())
        return err;
    err = checkBitOperand(calc, args[1]);
    if (!err.isEmpty())
        return err;

    const uint64_t x = args[0].asInteger();
    const uint64_t y = args[1].asInteger();
    return Value(static_cast<int64_t>(x | y));
}

// Function: BITXOR
Value func_bitxor(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value err = checkBitOperand(calc, args[0]);
    if (!err.isEmpty())
        return err;
    err = checkBitOperand(calc, args[1]);
    if (!err.isEmpty())
        return err;

    const uint64_t x = args[0].asInteger();
    const uint64_t y = args[1].asInteger();
    return Value(static_cast<int64_t>(x ^ y));
}

// Function: BITLSHIFT
Value func_bitlshift(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value err = checkBitOperand(calc, args[0]);
    if (!err.isEmpty())
        return err;
    err = checkShiftAmount(calc, args[1]);
    if (!err.isEmpty())
        return err;

    const uint64_t x = args[0].asInteger();
    const int numshift = args[1].asInteger();
    if (numshift == 0)
        return Value(static_cast<int64_t>(x));
    else if (numshift > 0)
        return Value(static_cast<int64_t>(x << numshift));
    else // negative left shift, becomes right shift
        return Value(static_cast<int64_t>(x >> (-1 * numshift)));
}

// Function: BITRSHIFT
Value func_bitrshift(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value err = checkBitOperand(calc, args[0]);
    if (!err.isEmpty())
        return err;
    err = checkShiftAmount(calc, args[1]);
    if (!err.isEmpty())
        return err;

    const uint64_t x = args[0].asInteger();
    const int numshift = args[1].asInteger();
    if (numshift == 0)
        return Value(static_cast<int64_t>(x));
    else if (numshift > 0)
        return Value(static_cast<int64_t>(x >> numshift));
    else // negative right shift, becomes left shift
        return Value(static_cast<int64_t>(x << (-1 * numshift)));
}

#include "bitops.moc"
