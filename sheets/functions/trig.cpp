// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in trigonometric functions
#include "TrigonometryModule.h"

#include "engine/Function.h"
#include "engine/ValueCalc.h"

using namespace Calligra::Sheets;

// prototypes (sort alphabetically)
Value func_acos(valVector args, ValueCalc *calc, FuncExtra *);
Value func_acosh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_acot(valVector args, ValueCalc *calc, FuncExtra *);
Value func_acoth(valVector args, ValueCalc *calc, FuncExtra *);
Value func_asinh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_asin(valVector args, ValueCalc *calc, FuncExtra *);
Value func_atan(valVector args, ValueCalc *calc, FuncExtra *);
Value func_atan2(valVector args, ValueCalc *calc, FuncExtra *);
Value func_atanh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cos(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cosh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cot(valVector args, ValueCalc *calc, FuncExtra *);
Value func_coth(valVector args, ValueCalc *calc, FuncExtra *);
Value func_csc(valVector args, ValueCalc *calc, FuncExtra *);
Value func_csch(valVector args, ValueCalc *calc, FuncExtra *);
Value func_degrees(valVector args, ValueCalc *calc, FuncExtra *);
Value func_radians(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sec(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sech(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sin(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sinh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tan(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tanh(valVector args, ValueCalc *calc, FuncExtra *);
Value func_pi(valVector args, ValueCalc *calc, FuncExtra *);

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadtrigonometrymodule.json", TrigonometryModule)

TrigonometryModule::TrigonometryModule(QObject *parent, const QVariantList &)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("ACOS", func_acos);
    add(f);
    f = new Function("ACOSH", func_acosh);
    add(f);
    f = new Function("ACOT", func_acot);
    add(f);
    f = new Function("ACOTH", func_acoth);
    add(f);
    f = new Function("ASIN", func_asin);
    add(f);
    f = new Function("ASINH", func_asinh);
    add(f);
    f = new Function("ATAN", func_atan);
    add(f);
    f = new Function("ATAN2", func_atan2);
    f->setParamCount(2);
    add(f);
    f = new Function("ATANH", func_atanh);
    add(f);
    f = new Function("COS", func_cos);
    add(f);
    f = new Function("COSH", func_cosh);
    add(f);
    f = new Function("COT", func_cot);
    add(f);
    f = new Function("COTH", func_coth);
    add(f);
    f = new Function("CSC", func_csc);
    add(f);
    f = new Function("CSCH", func_csch);
    add(f);
    f = new Function("DEGREES", func_degrees);
    add(f);
    f = new Function("RADIANS", func_radians);
    add(f);
    f = new Function("SEC", func_sec);
    add(f);
    f = new Function("SECH", func_sech);
    add(f);
    f = new Function("SIN", func_sin);
    add(f);
    f = new Function("SINH", func_sinh);
    add(f);
    f = new Function("TAN", func_tan);
    add(f);
    f = new Function("TANH", func_tanh);
    add(f);
    f = new Function("PI", func_pi);
    f->setParamCount(0);
    add(f);
}

QString TrigonometryModule::descriptionFileName() const
{
    return QString("trig.xml");
}

// Function: sin
Value func_sin(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->sin(args[0]);
}

// Function: cos
Value func_cos(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->cos(args[0]);
}

// Function: tan
Value func_tan(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->tg(args[0]);
}

// Function: atan
Value func_atan(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->atg(args[0]);
}

// Function: asin
Value func_asin(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->asin(args[0]);
}

// Function: acos
Value func_acos(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->acos(args[0]);
}

Value func_acot(valVector args, ValueCalc *calc, FuncExtra *)
{
    // PI/2 - atg (val)
    return calc->sub(calc->div(calc->pi(), 2), calc->atg(args[0]));
}

// function: ACOTH
Value func_acoth(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->lower(calc->abs(args[0]), Value(1.0)))
        return Value::errorNUM();

    return calc->mul(Value(0.5), calc->ln(calc->div(calc->add(args[0], Value(1.0)), calc->sub(args[0], Value(1.0)))));
}

// Function: asinh
Value func_asinh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->asinh(args[0]);
}

// Function: acosh
Value func_acosh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->acosh(args[0]);
}

// Function: atanh
Value func_atanh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->atgh(args[0]);
}

// Function: tanh
Value func_tanh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->tgh(args[0]);
}

// Function: sinh
Value func_sinh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->sinh(args[0]);
}

// Function: cosh
Value func_cosh(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->cosh(args[0]);
}

// Function: cot
Value func_cot(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->div(1, calc->tg(args[0]));
}

// Function: coth
Value func_coth(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->isZero(args[0]))
        return Value::errorNUM();

    return calc->div(1, calc->tgh(args[0]));
}

// Function: csc
Value func_csc(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->div(1, calc->sin(args[0]));
}

// Function: csch
Value func_csch(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->div(1, calc->sinh(args[0]));
}

// Function: sec
Value func_sec(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->div(1, calc->cos(args[0]));
}

// Function: sech
Value func_sech(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->div(1, calc->cosh(args[0]));
}

// Function: DEGREES
Value func_degrees(valVector args, ValueCalc *calc, FuncExtra *)
{
    // val * 180 / pi
    return calc->div(calc->mul(args[0], 180.0), calc->pi());
}

// Function: RADIANS
Value func_radians(valVector args, ValueCalc *calc, FuncExtra *)
{
    // val * pi / 180
    return calc->mul(calc->div(args[0], 180.0), calc->pi());
}

// Function: PI
Value func_pi(valVector, ValueCalc *calc, FuncExtra *)
{
    return calc->pi();
}

// Function: atan2
Value func_atan2(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->atan2(args[1], args[0]);
}

#include "trig.moc"
