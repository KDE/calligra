/***************************************************************************
 * pythonutils.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonutils.h"

#include <limits.h>
#include <kdebug.h>

using namespace Kross;

void PythonUtils::checkArgs(const Py::Tuple& args, uint minparams, uint maxparams)
{
    if(args.size() < minparams)
        throw Py::TypeError("Too few arguments.");
    if(args.size() > maxparams)
        throw Py::TypeError("Too many arguments.");
}

Py::Object PythonUtils::toPyObject(const QString& s)
{
    return s.isNull() ? Py::None() : Py::String(s.latin1());
}

Py::List PythonUtils::toPyObject(QStringList list)
{
    Py::List* l = new Py::List();
    for(QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
        l->append(toPyObject(*it));
    return *l;
}

Py::Dict PythonUtils::toPyObject(QMap<QString, QVariant> map)
{
    Py::Dict* dict = new Py::Dict();
    for(QMap<QString, QVariant>::Iterator it = map.begin(); it != map.end(); ++it)
        dict->setItem(it.key().latin1(), toPyObject(it.data()));
    return *dict;
}

Py::List PythonUtils::toPyObject(QValueList<QVariant> list)
{
    Py::List* l = new Py::List();
    for(QValueList<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
        l->append(toPyObject(*it));
    return *l;
}

Py::Object PythonUtils::toPyObject(QVariant variant)
{
    switch(variant.type()) {
        case QVariant::Bool:
            return Py::Int(variant.toBool());
        case QVariant::Int:
            return Py::Int(variant.toInt());
        case QVariant::UInt:
            return Py::Long((unsigned long)variant.toUInt());
        case QVariant::Double:
            return Py::Float(variant.toDouble());
        case QVariant::CString:
        case QVariant::String:
            return toPyObject(variant.toString());
        case QVariant::StringList:
            return toPyObject(variant.toStringList());
        case QVariant::Map:
            return toPyObject(variant.toMap());
        case QVariant::List:
            return toPyObject(variant.toList());
        //Date, Time, DateTime, ByteArray, BitArray
#if(QT_VERSION >= 0x030200)
        //LongLong, ULongLong
#endif
        default: {
            kdDebug() << "PythonUtils::variant2object() Not possible to convert the QVariant type '" <<  variant.typeName() << "' to a Py::Object." << endl;
            return Py::None();
        }
    }
}

uint PythonUtils::toUInt(Py::Object obj)
{
    if(! obj.isNumeric())
        throw Py::TypeError("Numeric value expected.");
    unsigned long v = Py::Long(obj);
    if(v > UINT_MAX)
        throw Py::OverflowError("Maximal value exceeded.");
    return (uint)v;
}
