/***************************************************************************
 * pythonutils.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
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

QVariant PythonUtils::toVariant(Py::Object obj)
{
    if(obj.isNumeric()) {
        if(Py::Int().is(obj)) {
            return QVariant( (int)Py::Int(obj) );
        }
        if(Py::Float().is(obj)) {
            return QVariant( (double)Py::Float(obj) );
        }
        if(Py::Long().is(obj)) {
#if(QT_VERSION >= 0x030200)
            return QVariant( (Q_LLONG)((long)Py::Long(obj)) );
#else
            kdWarning() << "PythonUtils::toVariant() can't handle long. Trying to handle it as int." << endl;
            return QVariant( (int)Py::Int(obj) );
#endif
        }
        throw Py::TypeError("Invalid numeric variant value.");
    }

    if(obj.isString()) {
        return QVariant( obj.as_string().c_str() );
    }

    if(obj.isList()) {
        Py::List pylist(obj);
        QValueList<QVariant> vlist;
        uint length = pylist.length();
        for(uint i = 0; i < length; i++)
            vlist.append( toVariant(pylist[i]) );
        return vlist;
    }

    if(obj.isDict()) {
        QMap<QString, QVariant> vmap;
        Py::Dict pydict( obj.ptr() );
        Py::Dict::iterator it( pydict.begin() );
        for(; it != pydict.end(); ++it) {
            Py::Dict::value_type vt(*it);
            vmap.replace(vt.first.as_string().c_str(), toVariant(vt.second));
        }
        return vmap;
    }

    throw Py::TypeError("Invalid variant value.");
}


