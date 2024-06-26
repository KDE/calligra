// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// Local
#include "Function.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Function::Private
{
public:
    QString name;
    QString alternateName;
    FunctionPtr ptr;
    int paramMin, paramMax;
    bool acceptArray;
    bool ne; // need FunctionExtra* when called ?
};

Function::Function(const QString &name, FunctionPtr ptr)
    : d(new Private)
{
    d->name = name;
    d->ptr = ptr;
    d->acceptArray = false;
    d->paramMin = 1;
    d->paramMax = 1;
    d->ne = false;
}

Function::~Function()
{
    delete d;
}

QString Function::name() const
{
    return d->name;
}

QString Function::alternateName() const
{
    return d->alternateName;
}

void Function::setAlternateName(const QString &name)
{
    d->alternateName = name;
}

void Function::setParamCount(int min, int max)
{
    d->paramMin = min;
    d->paramMax = (max == 0) ? min : max;
}

bool Function::paramCountOkay(int count)
{
    // less than needed
    if (count < d->paramMin)
        return false;
    // no upper limit
    if (d->paramMax == -1)
        return true;
    // more than needed
    if (count > d->paramMax)
        return false;
    // okay otherwise
    return true;
}

void Function::setAcceptArray(bool accept)
{
    d->acceptArray = accept;
}

bool Function::needsExtra()
{
    return d->ne;
}
void Function::setNeedsExtra(bool extra)
{
    d->ne = extra;
}

Value Function::exec(valVector args, ValueCalc *calc, FuncExtra *extra)
{
    // check number of parameters
    if (!paramCountOkay(args.count()))
        return Value::errorVALUE();

    if (extra)
        extra->function = this;

    // do we need to perform array expansion ?
    bool mustExpandArray = false;
    if (!d->acceptArray)
        for (int i = 0; i < args.count(); ++i) {
            if (args[i].isArray())
                mustExpandArray = true;
        }

    if (!d->ptr)
        return Value::errorVALUE();

    // perform the actual array expansion if need be

    if (mustExpandArray) {
        // compute number of rows/cols of the result
        int rows = 0;
        int cols = 0;
        for (int i = 0; i < args.count(); ++i) {
            int x = 1;
            if (extra)
                x = extra->ranges[i].rows();
            if (x > rows)
                rows = x;
            if (extra)
                x = extra->ranges[i].columns();
            if (x > cols)
                cols = x;
        }
        // allocate the resulting array
        Value res(Value::Array);
        // perform the actual computation for each element of the array
        for (int row = 0; row < rows; ++row)
            for (int col = 0; col < cols; ++col) {
                // fill in the parameter vector
                valVector vals(args.count());
                FuncExtra extra2 = *extra;
                for (int i = 0; i < args.count(); ++i) {
                    int r = extra->ranges[i].rows();
                    int c = extra->ranges[i].columns();
                    vals[i] = args[i].isArray() ? args[i].element(col % c, row % r) : args[i];

                    // adjust the FuncExtra structure to refer to the correct cells
                    extra2.ranges[i].col1 += col;
                    extra2.ranges[i].row1 += row;
                    extra2.ranges[i].col2 = extra2.ranges[i].col1;
                    extra2.ranges[i].row2 = extra2.ranges[i].row1;
                }
                // execute the function on each element
                res.setElement(col, row, exec(vals, calc, &extra2));
            }
        return res;
    } else
        // call the function
        return (*d->ptr)(args, calc, extra);
}

FunctionCaller::FunctionCaller(FunctionPtr ptr, const valVector &args, ValueCalc *calc, FuncExtra *extra)
    : m_ptr(ptr)
    , m_args(args)
    , m_calc(calc)
    , m_extra(extra)
{
}

Value FunctionCaller::exec()
{
    return (*m_ptr)(m_args, m_calc, m_extra);
}

Value FunctionCaller::exec(const valVector &args)
{
    return (*m_ptr)(args, m_calc, m_extra);
}
