// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include <engine/CS_Time.h>
#include <engine/Formula.h>
#include <engine/FunctionModuleRegistry.h>
#include <engine/Number.h>
#include <engine/Value.h>

#include <QTest>

#include <KLocalizedString>

#include <float.h> // DBL_EPSILON
#include <math.h>

using namespace Calligra::Sheets;

namespace QTest
{
template<>
char *toString(const Calligra::Sheets::Value &v)
{
    static QStringList formats{"fmt_None", "fmt_Boolean", "fmt_Number", "fmt_Percent", "fmt_Money", "fmt_DateTime", "fmt_Date", "fmt_Time", "fmt_String"};

    QString s = "Value[";
    s += formats.value(v.format());
    switch (v.type()) {
    case Calligra::Sheets::Value::Empty:
        s += ":Empty";
        break;
    case Calligra::Sheets::Value::Boolean:
        s += ':' + QString(v.asBoolean() ? "true" : "false");
        break;
    case Calligra::Sheets::Value::Integer:
        s += ':' + QString::number(v.asInteger());
        break;
    case Calligra::Sheets::Value::Float:
        s += ':' + QString::number((double)v.asFloat());
        break; // FIXME
    case Calligra::Sheets::Value::Complex:
        s += ':' + QStringLiteral("Complex");
        break; // TODO
    case Calligra::Sheets::Value::String:
        s += ':' + v.asString();
        break;
    case Calligra::Sheets::Value::Array:
        s += ':' + QStringLiteral("Array");
        break; // TODO
    case Calligra::Sheets::Value::CellRange:
        s += ':' + QStringLiteral("CellRange");
        break;
    case Calligra::Sheets::Value::Error:
        s += ':' + v.errorMessage();
        break;
    default:
        s += QStringLiteral("Unknown");
    }
    s += ']';
    return toString(s);
}

template<>
char *toString(const Calligra::Sheets::Time &t)
{
    QString s = "Time[%1]";
    s = s.arg(QString::number(numToDouble(t.duration()), 'g', 20));
    return toString(s);
}
}
