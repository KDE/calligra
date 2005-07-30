/***************************************************************************
 * qtobject.cpp
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "qtobject.h"
#include "object.h"
#include "variant.h"
#include "event.h"

//#include "eventmanager.h"
#include "../main/manager.h"
//#include "../main/scriptcontainer.h"
#include "eventslot.h"
#include "eventsignal.h"

#include <qobject.h>
#include <qsignal.h>
//#include <qglobal.h>
//#include <qobjectdefs.h>
#include <qmetaobject.h>

using namespace Kross::Api;

QtObject::QtObject(Object::Ptr parent, QObject* object)
    : Kross::Api::Class<QtObject>(object->name(), parent)
    , m_object(object)
{
kdDebug() << QString("================================================================================0") << endl;
    kdDebug() << QString("QtObject::QtObject(%1) Constructor").arg(object->name()) << endl;

    QStrList slotnames = m_object->metaObject()->slotNames(false);
    for(char* c = slotnames.first(); c; c = slotnames.next()) {
        QCString s = c;
        addChild( new EventSlot(s, this, object, s) );
    }

    QStrList signalnames = m_object->metaObject()->signalNames(false);
    for(char* c = signalnames.first(); c; c = signalnames.next()) {
        QCString s = c;
        addChild( new EventSignal(s, this, object, s) );
    }

//TODO port functions to the new event-framework.

    addFunction("propertyNames", &QtObject::propertyNames,
        Kross::Api::ArgumentList(),
        "Return a list of property names."
    );
    addFunction("hasProperty", &QtObject::hasProperty,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Return true if the property exists else false."
    );
    addFunction("getProperty", &QtObject::getProperty,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Return the property."
    );
    addFunction("setProperty", &QtObject::setProperty,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant"),
        "Set the property."
    );

    addFunction("slotNames", &QtObject::slotNames,
        Kross::Api::ArgumentList(),
        "Return a list of slot names."
    );
    addFunction("hasSlot", &QtObject::hasSlot,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Return true if the slot exists else false."
    );

    addFunction("signalNames", &QtObject::signalNames,
        Kross::Api::ArgumentList(),
        "Return a list of signal names."
    );
    addFunction("hasSignal", &QtObject::hasSignal,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Return true if the signal exists else false."
    );

    addFunction("connect", &QtObject::connectSignal,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::QtObject")
            << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Connect signal with a QObject slot."
    );

    addFunction("disconnect", &QtObject::disconnectSignal,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Disconnect signal from QObject slot."
    );

    addFunction("signal", &QtObject::emitSignal,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Emit a signal."
    );

    addFunction("slot", &QtObject::callSlot,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        "Call a slot."
    );
}

QtObject::~QtObject()
{
}

const QString QtObject::getClassName() const
{
    return "Kross::Api::QtObject";
}

const QString QtObject::getDescription() const
{
    return i18n("Class to wrap QObject instances.");
}

QObject* QtObject::getObject()
{
    return m_object;
}

Kross::Api::Object::Ptr QtObject::propertyNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(
        QStringList::fromStrList(m_object->metaObject()->propertyNames(false)),
        "Kross::Api::QtObject::propertyNames::Variant::StringList");
}

Kross::Api::Object::Ptr QtObject::hasProperty(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
        m_object->metaObject()->findProperty(Kross::Api::Variant::toString(args->item(0)).latin1(), false),
        "Kross::Api::QtObject::hasProperty::Variant::Bool");
}

Kross::Api::Object::Ptr QtObject::getProperty(Kross::Api::List::Ptr args)
{
    QVariant variant = m_object->property(Kross::Api::Variant::toString(args->item(0)).latin1());
    if(variant.type() == QVariant::Invalid)
        return 0;
    return new Kross::Api::Variant(variant,
        "Kross::Api::QtObject::getProperty::Variant");
}

Kross::Api::Object::Ptr QtObject::setProperty(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           m_object->setProperty(
               Kross::Api::Variant::toString(args->item(0)).latin1(),
               Kross::Api::Variant::toVariant(args->item(1))
           ),
           "Kross::Api::QtObject::setProperty::Variant::Bool");
}

Kross::Api::Object::Ptr QtObject::slotNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(
           QStringList::fromStrList(m_object->metaObject()->slotNames(false)),
           "Kross::Api::QtObject::slotNames::Variant::StringList");
}

Kross::Api::Object::Ptr QtObject::hasSlot(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(m_object->metaObject()->slotNames(false).find(
               Kross::Api::Variant::toString(args->item(0)).latin1()
           ) != -1),
           "Kross::Api::QtObject::hasSlot::Variant::Bool");
}

Kross::Api::Object::Ptr QtObject::signalNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(
           QStringList::fromStrList(m_object->metaObject()->signalNames(false)),
           "Kross::Api::QtObject::signalNames::Variant::StringList");
}

Kross::Api::Object::Ptr QtObject::hasSignal(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(m_object->metaObject()->signalNames(false).find(
               Kross::Api::Variant::toString(args->item(0)).latin1()
           ) != -1),
           "Kross::Api::QtObject::hasSignal::Variant::Bool");
}

Kross::Api::Object::Ptr QtObject::connectSignal(Kross::Api::List::Ptr args)
{
    QString signalname = Kross::Api::Variant::toString(args->item(0));
    QString signalsignatur = QString("2%1").arg(signalname);
    const char* signalsig = signalsignatur.latin1();

    QtObject* obj = Kross::Api::Object::fromObject<Kross::Api::QtObject>(args->item(1));
    QObject* o = obj->getObject();
    if(! o)
        throw TypeException(i18n("No such QObject receiver in '%1'.").arg(obj->getName()));

    QString slotname = Kross::Api::Variant::toString(args->item(2));
    QString slotsignatur = QString("1%1").arg(slotname);
    const char* slotsig = slotsignatur.latin1();

    return new Kross::Api::Variant(
           QObject::connect(m_object, signalsig, o, slotsig),
           "Kross::Api::QtObject::connect::Bool");
}

Kross::Api::Object::Ptr QtObject::disconnectSignal(Kross::Api::List::Ptr)
{
    //TODO
    return 0;
}

Kross::Api::Object::Ptr QtObject::emitSignal(Kross::Api::List::Ptr args)
{
    QString name = Kross::Api::Variant::toString(args->item(0));
    int signalid = m_object->metaObject()->findSignal(name.latin1(), false);
    if(signalid < 0)
        throw TypeException(i18n("No such signal '%1'.").arg(name));
    m_object->qt_invoke(signalid, 0); //TODO convert Kross::Api::List::Ptr => QUObject*
    return 0;
}

Kross::Api::Object::Ptr QtObject::callSlot(Kross::Api::List::Ptr args)
{
/*TODO
    QUObject uo[12] = { QUObject(), QUObject(), QUObject(),
                        QUObject(), QUObject(), QUObject(),
                        QUObject(), QUObject(), QUObject(),
                        QUObject(), QUObject(), QUObject() };
*/
    QString name = Kross::Api::Variant::toString(args->item(0));
    int slotid = m_object->metaObject()->findSlot(name.latin1(), false);
    if(slotid < 0)
        throw TypeException(i18n("No such slot '%1'.").arg(name));
    m_object->qt_invoke(slotid, 0); //TODO convert Kross::Api::List::Ptr => QUObject*
    return 0;
}

