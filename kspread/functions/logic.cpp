/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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
#include "LogicModule.h"

#include "Function.h"
#include "FunctionModuleRegistry.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_and(valVector args, ValueCalc *calc, FuncExtra *);
Value func_false(valVector args, ValueCalc *calc, FuncExtra *);
Value func_if(valVector args, ValueCalc *calc, FuncExtra *);
Value func_nand(valVector args, ValueCalc *calc, FuncExtra *);
Value func_nor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_not(valVector args, ValueCalc *calc, FuncExtra *);
Value func_or(valVector args, ValueCalc *calc, FuncExtra *);
Value func_true(valVector args, ValueCalc *calc, FuncExtra *);
Value func_xor(valVector args, ValueCalc *calc, FuncExtra *);


KSPREAD_EXPORT_FUNCTION_MODULE("logic", LogicModule)


LogicModule::LogicModule(QObject* parent, const QVariantList&)
        : FunctionModule(parent)
{
    Function *f;

    f = new Function("FALSE", func_false);
    f->setParamCount(0);
    add(f);
    f = new Function("TRUE", func_true);
    f->setParamCount(0);
    add(f);
    f = new Function("NOT", func_not);
    f->setParamCount(1);
    add(f);
    f = new Function("AND", func_and);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("NAND", func_nand);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("NOR", func_nor);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("OR", func_or);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("XOR", func_xor);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("IF", func_if);
    f->setParamCount(2, 3);
    add(f);
}

QString LogicModule::descriptionFileName() const
{
    return QString("logic.xml");
}


// helper for most logical functions
static bool asBool(Value val, ValueCalc *calc, bool* ok = 0)
{
    return calc->conv()->asBoolean(val, ok).asBoolean();
}

///////////////////////////////////////////////////////////////////////////////


//
// ArrayWalker: AND
//
void awAnd(ValueCalc *calc, Value &res, Value value, Value)
{
    if (res.asBoolean())
        res = Value(asBool(value, calc));
}


//
// ArrayWalker: OR
//
void awOr(ValueCalc *calc, Value &res, Value value, Value)
{
    if (!res.asBoolean())
        res = Value(asBool(value, calc));
}


//
// ArrayWalker: XOR
//
void awXor(ValueCalc *calc, Value &count, Value value, Value)
{
    if (asBool(value, calc))
        count = Value(count.asInteger() + 1);
}

///////////////////////////////////////////////////////////////////////////////


//
// Function: AND
//
Value func_and(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result(true);
    int cnt = args.count();
    for (int i = 0; i < cnt; ++i) {
        if (args[i].isError())
            return args[i];
    }
    for (int i = 0; i < cnt; ++i) {
        calc->arrayWalk(args[i], result, awAnd, Value(0));
        if (! result.asBoolean())
            // if any value is false, return false
            return result;
    }
    // nothing is false -> return true
    return result;
}


//
// Function: FALSE
//
Value func_false(valVector, ValueCalc *, FuncExtra *)
{
    return Value(false);
}


//
// Function: IF
//
Value func_if(valVector args, ValueCalc *calc, FuncExtra *)
{
    if ((args[0].isError()))
        return args[0];
    bool ok = true;
    bool guard = asBool(args[0], calc, &ok);
    if (!ok)
        return Value::errorVALUE();
    if (guard)
        return args[1];
    // evaluated to false
    if (args.count() == 3) {
        if (args[2].isEmpty()) {
            return Value(0);
        } else {
            return args[2];
        }
    } else {
        // only two arguments
        return Value(false);
    }
}


//
// Function: NAND
//
Value func_nand(valVector args, ValueCalc *calc, FuncExtra *extra)
{
    // AND in reverse
    return Value(! func_and(args, calc, extra).asBoolean());
}


//
// Function: NOR
//
Value func_nor(valVector args, ValueCalc *calc, FuncExtra *extra)
{
    // OR in reverse
    return Value(! func_or(args, calc, extra).asBoolean());
}


//
// Function: NOT
//
Value func_not(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isError())
        return args[0];

    bool ok = true;
    bool val = !asBool(args[0], calc, &ok);
    if (!ok) return Value::errorVALUE();
    return Value(val);
}


//
// Function: OR
//
Value func_or(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result(false);
    int cnt = args.count();
    for (int i = 0; i < cnt; ++i) {
        if (args[i].isError())
            return args[i];
    }
    for (int i = 0; i < cnt; ++i) {
        calc->arrayWalk(args[i], result, awOr, Value(0));
        if (result.asBoolean())
            // if any value is true, return true
            return result;
    }
    // nothing is true -> return false
    return result;
}


//
// Function: TRUE
//
Value func_true(valVector, ValueCalc *, FuncExtra *)
{
    return Value(true);
}


//
// Function: XOR
//
Value func_xor(valVector args, ValueCalc *calc, FuncExtra *)
{
    // exclusive OR - exactly one value must be true
    int cnt = args.count();
    Value count(0);
    for (int i = 0; i < cnt; ++i) {
        if (args[i].isError())
            return args[i];
    }
    for (int i = 0; i < cnt; ++i)
        calc->arrayWalk(args[i], count, awXor, Value(0));
    return Value(count.asInteger() == 1);
}

#include "LogicModule.moc"
