/***************************************************************************
 * mainmodule.cpp
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

#include "mainmodule.h"

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// \internal
    class MainModulePrivate
    {
        public:
           /**
            * The \a Exception this \a MainModule throwed or
            * NULL if we don't had an exception.
            */
            Exception::Ptr exception;
    };

}}

MainModule::MainModule(const QString& name)
    : Module(name)
    , d(new MainModulePrivate())
{
    d->exception = 0;
}

MainModule::~MainModule()
{
    delete d;
}

const QString MainModule::getClassName() const
{
    return "Kross::Api::MainModule";
}

bool MainModule::hadException()
{
    return d->exception != 0;
}

Exception::Ptr MainModule::getException()
{
    return d->exception;
}

void MainModule::setException(Exception::Ptr exception)
{
    d->exception = exception;
}

#if 0
bool MainModule::hasChild(const QString& name) const
{
    return Callable::hasChild(name);
}
#endif

EventSignal::Ptr MainModule::addSignal(const QString& name, QObject* sender, QCString signal)
{
    EventSignal* event = new EventSignal(name, sender, signal);
    if(! addChild(event)) {
        krosswarning( QString("Failed to add signal name='%1' signature='%2'").arg(name).arg(signal) );
        return 0;
    }
    return event;
}

EventSlot::Ptr MainModule::addSlot(const QString& name, QObject* receiver, QCString slot)
{
    EventSlot* event = new EventSlot(name, receiver, slot);
    if(! addChild(event)) {
        krosswarning( QString("Failed to add slot name='%1' signature='%2'").arg(name).arg(slot) );
        return 0;
    }
    return event;
}

QtObject::Ptr MainModule::addQObject(QObject* object, const QString& name)
{
    QtObject* qtobject = new QtObject(object, name);
    if(! addChild(qtobject)) {
        krosswarning( QString("Failed to add QObject name='%1'").arg(object->name()) );
        return 0;
    }
    return qtobject;
}

EventAction::Ptr MainModule::addKAction(KAction* action, const QString& name)
{
    EventAction* event = new EventAction(name, action);
    if(! addChild(event)) {
        krosswarning( QString("Failed to add KAction name='%1'").arg(action->name()) );
        return 0;
    }
    return event;
}

