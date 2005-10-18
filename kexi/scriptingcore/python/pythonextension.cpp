/***************************************************************************
 * pythonextension.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "pythonextension.h"
#include "pythonobject.h"

#include "../api/variant.h"
#include "../api/dict.h"
#include "../api/exception.h"

#include <kdebug.h>

using namespace Kross::Python;

PythonExtension::PythonExtension(Kross::Api::Object::Ptr object)
    : Py::PythonExtension<PythonExtension>()
    , m_object(object)
{
#ifdef KROSS_PYTHON_EXTENSION_CTOR_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::Constructor objectname='%1' objectclass='%2'").arg(m_object->getName()).arg(m_object->getClassName()) << endl;
#endif

    //TODO determinate and return real dynamic objectname and documentation.
    behaviors().name("KrossPythonExtension");
    behaviors().doc(
        "The common KrossPythonExtension object enables passing "
        "of Kross::Api::Object's from C/C++ to Python and "
        "backwards in a transparent way."
    );
    behaviors().supportGetattr();
    add_varargs_method(
        "_call_",
        &PythonExtension::_call_,
        "Internal method use to wrap method- and attribute-calls. "
        "Something like 'myobj.myfunc(myarguments)' got wrapped "
        "into a 'myobj._call_('myfunc',myarguments)' methodcall. "
        "That way all callbacks are passed back through the "
        "_call_() member-function."
    );
}

PythonExtension::~PythonExtension()
{
#ifdef KROSS_PYTHON_EXTENSION_DTOR_DEBUG
    kdDebug() << "Kross::Python::PythonExtension::Destructor" << endl;
#endif
}

Py::Object PythonExtension::str()
{
    QString s = m_object->getName();
    return toPyObject(s.isEmpty() ? m_object->getClassName() : s);
}

Py::Object PythonExtension::repr()
{
    return toPyObject( m_object->toString() );
}

Py::Object PythonExtension::getattr(const char* n)
{
    QString name(n);
#ifdef KROSS_PYTHON_EXTENSION_GETATTR_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::getattr name='%1'").arg(name) << endl;
#endif

    if(name == "__methods__") {
        Py::List methods;
        QStringList calls = m_object->getCalls();
        for(QStringList::Iterator it = calls.begin(); it != calls.end(); ++it) {
#ifdef KROSS_PYTHON_EXTENSION_GETATTR_DEBUG
            kdDebug() << QString("Kross::Python::PythonExtension::getattr name='%1' callable='%2'").arg(name).arg(*it) << endl;
#endif
            methods.append(Py::String( (*it).latin1() ));
        }
        return methods;
    }

    if(name == "__members__") {
        Py::List members;
        QMap<QString, Kross::Api::Object::Ptr> children = m_object->getChildren();
        QMap<QString, Kross::Api::Object::Ptr>::Iterator it( children.begin() );
        for(; it != children.end(); ++it) {
#ifdef KROSS_PYTHON_EXTENSION_GETATTR_DEBUG
            kdDebug() << QString("Kross::Python::PythonExtension::getattr name='%1' child='%2'").arg(name).arg(it.key()) << endl;
#endif
            members.append(Py::String( it.key().latin1() ));
        }
        return members;
    }

    if(name == "__dict__") { kdDebug()<<QString("PythonExtension::getattr(%1) __dict__").arg(name)<<endl; return Py::None(); }
    if(name == "__class__") { kdDebug()<<QString("PythonExtension::getattr(%1) __class__").arg(name)<<endl; return Py::None(); }

    if(name.startsWith("__")) {
#ifdef KROSS_PYTHON_EXTENSION_GETATTR_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::getattr name='%1' is a internal name.").arg(name) << endl;
#endif
        return Py::PythonExtension<PythonExtension>::getattr_methods(n);
    }

    // use our methodproxy "_call_" to dynamicly redirect the call
    // to the correct method. It's a dirty hack, but it's the easiest way ;)
    m_methodname = name;
    return Py::PythonExtension<PythonExtension>::getattr_methods("_call_");
}

Py::Object PythonExtension::getattr_methods(const char* n)
{
    QString name(n);
#ifdef KROSS_PYTHON_EXTENSION_GETATTRMETHOD_DEBUG
    kdDebug()<<"PythonExtension::getattr_methods name="<<name<<endl;
#endif
    return Py::PythonExtension<PythonExtension>::getattr_methods(n);
}

int PythonExtension::setattr(const char* name, const Py::Object& value)
{
#ifdef KROSS_PYTHON_EXTENSION_SETATTR_DEBUG
    kdDebug() << QString("PythonExtension::setattr name=%1 value=%2").arg(name).arg(value.as_string().c_str()) << endl;
#endif
    return Py::PythonExtension<PythonExtension>::setattr(name, value);
}

Kross::Api::Object::Ptr PythonExtension::getObject()
{
    return m_object;
}

Kross::Api::List::Ptr PythonExtension::toObject(const Py::Tuple& tuple)
{
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Tuple)") << endl;
#endif

    QValueList<Kross::Api::Object::Ptr> list;
    for(uint i = 0; i < tuple.size(); i++) {
        Py::Object po = tuple[i];
        Kross::Api::Object::Ptr o = toObject(po);
        if(o) list.append(o);
    }
    return new Kross::Api::List(list);
}

Kross::Api::Object::Ptr PythonExtension::toObject(const Py::Object& object)
{
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) object='%1'").arg(object.as_string().c_str()) << endl;
#endif

    if(object == Py::None()) {
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) isNone") << endl;
#endif
        return 0;
    }

    if(object.isInstance()) {
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) isInstance") << endl;
#endif
        return new PythonObject(object);
    }

    if(object.isTuple()) {
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) isTuple") << endl;
#endif
        Py::Tuple tuple = object;
        return toObject(tuple).data();
    }

    if(object.isNumeric()) {
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) isNumeric") << endl;
#endif
        //FIXME add isUnsignedLong() to Py::Long (or create
        // an own Py::UnsignedLong class) and if true used it
        // rather then long to prevent overflows (needed to
        // handle e.g. uint correct!)
        Py::Long l = object;
        return new Kross::Api::Variant(Q_LLONG(long(l)));
    }

    if(object.isString()) {
#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toObject(Py::Object) isString='%1'").arg(object.as_string().c_str()) << endl;
#endif
        return new Kross::Api::Variant(object.as_string().c_str());
    }

    /*TODO
    if(object.isUnicode()) {
        Py::String s = object;
        return Kross::Api::Variant::create(QVariant(s.as_unicodestring().c_str()));
    }
    isDict()
    isList()
    isMapping()
    isNumeric()
    isSequence()
    isTrue()
    */

    Py::ExtensionObject<PythonExtension> extobj(object);
    PythonExtension* extension = extobj.extensionObject();
    if(! extension)
        throw Py::TypeError("Failed to determinate PythonExtension object.");
    Kross::Api::Object::Ptr obj = extension->getObject();
    if(! obj)
        throw Py::TypeError("Failed to convert the PythonExtension object into a Kross::Api::Object.");

#ifdef KROSS_PYTHON_EXTENSION_TOOBJECT_DEBUG
    kdDebug() << "Kross::Python::PythonExtension::toObject(Py::Object) successfully converted into Kross::Api::Object." << endl;
#endif
    return obj;
}

Py::Object PythonExtension::toPyObject(const QString& s)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(QString)") << endl;
#endif
    return s.isNull() ? Py::String() : Py::String(s.latin1());
}

Py::List PythonExtension::toPyObject(QStringList list)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(QStringList)") << endl;
#endif
    Py::List* l = new Py::List();
    for(QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
        l->append(toPyObject(*it));
    return *l;
}

Py::Dict PythonExtension::toPyObject(QMap<QString, QVariant> map)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(QMap<QString,QVariant>)") << endl;
#endif
    Py::Dict* d = new Py::Dict();
    for(QMap<QString, QVariant>::Iterator it = map.begin(); it != map.end(); ++it)
        d->setItem(it.key().latin1(), toPyObject(it.data()));
    return *d;
}

Py::List PythonExtension::toPyObject(QValueList<QVariant> list)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(QValueList<QVariant>)") << endl;
#endif
    Py::List* l = new Py::List();
    for(QValueList<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
        l->append(toPyObject(*it));
    return *l;
}

Py::Object PythonExtension::toPyObject(const QVariant& variant)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(QVariant) typename='%1'").arg(variant.typeName()) << endl;
#endif

    switch(variant.type()) {
        case QVariant::Invalid:
            return Py::None();
        case QVariant::Bool:
            return Py::Int(variant.toBool());
        case QVariant::Int:
            return Py::Int(variant.toInt());
        case QVariant::UInt:
            return Py::Long((unsigned long)variant.toUInt());
        case QVariant::Double:
            return Py::Float(variant.toDouble());
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
        case QVariant::ByteArray:
        case QVariant::BitArray:
        case QVariant::CString:
        case QVariant::String:
            return toPyObject(variant.toString());
        case QVariant::StringList:
            return toPyObject(variant.toStringList());
        case QVariant::Map:
            return toPyObject(variant.toMap());
        case QVariant::List:
            return toPyObject(variant.toList());

        // To handle following both cases is a bit difficult
        // cause Python doesn't spend an easy possibility
        // for such large numbers (TODO maybe BigInt?). So,
        // we risk overflows here, but well...
        case QVariant::LongLong: {
            Q_LLONG l = variant.toLongLong();
            //return (l < 0) ? Py::Long((long)l) : Py::Long((unsigned long)l);
            return Py::Long((long)l);
            //return Py::Long(PyLong_FromLong( (long)l ), true);
        } break;
        case QVariant::ULongLong: {
            return Py::Long((unsigned long)variant.toULongLong());
        } break;

        default: {
            kdWarning() << QString("Kross::Python::PythonExtension::toPyObject(QVariant) Not possible to convert the QVariant type '%1' to a Py::Object.").arg(variant.typeName()) << endl;
            return Py::None();
        }
    }
}

Py::Object PythonExtension::toPyObject(Kross::Api::Object::Ptr object)
{
    if(! object) {
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
        kdDebug() << "Kross::Python::PythonExtension::toPyObject(Kross::Api::Object) is NULL => Py::None" << endl;
#endif
        return Py::None();
    }

    if(object->getClassName() == "Kross::Api::Variant") {
        QVariant v = static_cast<Kross::Api::Variant*>( object.data() )->getValue();
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
        kdDebug() << QString("Kross::Python::PythonExtension::toPyObject(Kross::Api::Object) is Kross::Api::Variant %1").arg(v.toString()) << endl;
#endif
        return toPyObject(v);
    }

    if(object->getClassName() == "Kross::Api::List") {
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
        kdDebug() << "Kross::Python::PythonExtension::toPyObject(Kross::Api::Object) is Kross::Api::List" << endl;
#endif
        Py::List pylist;
        Kross::Api::List* list = static_cast<Kross::Api::List*>( object.data() );
        QValueList<Kross::Api::Object::Ptr> valuelist = list->getValue();
        for(QValueList<Kross::Api::Object::Ptr>::Iterator it = valuelist.begin(); it != valuelist.end(); ++it)
            pylist.append( toPyObject(*it) ); // recursive
        return pylist;
    }

    if(object->getClassName() == "Kross::Api::Dict") {
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
        kdDebug() << "Kross::Python::PythonExtension::toPyObject(Kross::Api::Object) is Kross::Api::Dict" << endl;
#endif
        Py::Dict pydict;
        Kross::Api::Dict* dict = static_cast<Kross::Api::Dict*>( object.data() );
        QMap<QString, Kross::Api::Object::Ptr> valuedict = dict->getValue();
        for(QMap<QString, Kross::Api::Object::Ptr>::Iterator it = valuedict.begin(); it != valuedict.end(); ++it)
            pydict[ it.key().latin1() ] = toPyObject( it.data() ); // recursive
        return pydict;
    }

#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Trying to handle PythonExtension::toPyObject(%1) as PythonExtension").arg(object->getClassName()) << endl;
#endif
    return Py::asObject( new PythonExtension(object) );
}

Py::Tuple PythonExtension::toPyTuple(Kross::Api::List::Ptr list)
{
#ifdef KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::toPyTuple(Kross::Api::List) name='%1'").arg(list ? list->getName() : "NULL") << endl;
#endif
    uint count = list ? list->count() : 0;
    Py::Tuple tuple(count);
    for(uint i = 0; i < count; i++)
        tuple.setItem(i, toPyObject(list->item(i)));
    return tuple;
}

Py::Object PythonExtension::_call_(const Py::Tuple& args)
{
    Kross::Api::List::Ptr arguments = toObject(args);
    Kross::Api::Object::Ptr obj;

#ifdef KROSS_PYTHON_EXTENSION_CALL_DEBUG
    kdDebug() << QString("Kross::Python::PythonExtension::_call_(Py::Tuple) m_methodname='%1' arguments='%2'").arg(m_methodname).arg(arguments->toString()) << endl;
#endif

    try {
        if(m_object->hasChild(m_methodname)) {
#ifdef KROSS_PYTHON_EXTENSION_CALL_DEBUG
            kdDebug() << QString("Kross::Python::PythonExtension::_call_ name='%1' is a child object of '%2'.").arg(m_methodname).arg(m_object->getName()) << endl;
#endif
            obj = m_object->getChild(m_methodname)->call(QString::null, arguments);
        }
        else {
#ifdef KROSS_PYTHON_EXTENSION_CALL_DEBUG
            kdDebug() << QString("Kross::Python::PythonExtension::_call_ try to call function with name '%1' in object '%2'.").arg(m_methodname).arg(m_object->getName()) << endl;
#endif
            obj = m_object->call(m_methodname, arguments);
        }
    }
    catch(Kross::Api::Exception::Ptr e) {
        kdDebug() << "EXCEPTION in PythonExtension::_call_" << endl;
        throw Py::RuntimeError( e->toString().latin1() );
    }

    return obj ? toPyObject(obj) : Py::None();
}

