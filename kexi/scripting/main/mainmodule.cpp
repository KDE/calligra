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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "mainmodule.h"

#include <kdebug.h>

using namespace Kross::Api;

MainModule::MainModule(const QString& name)
    : Module<MainModule>(name)
{
}

MainModule::~MainModule()
{
}

const QString MainModule::getClassName() const
{
    return "Kross::Api::MainModule";
}

const QString MainModule::getDescription() const
{
    return i18n("TODO: Documentation");
}

EventSignal::Ptr MainModule::addSignal(const QString& name, QObject* sender, QCString signal)
{
    EventSignal* event = new EventSignal(name, this, sender, signal);
    if(! addChild(event))
        throw new Exception( QString("Failed to add signal name='%1' signature='%2'").arg(name).arg(signal) );
    return event;
}

EventSlot::Ptr MainModule::addSlot(const QString& name, QObject* receiver, QCString slot)
{
    EventSlot* event = new EventSlot(name, this, receiver, slot);
    if(! addChild(event))
        throw new Exception( QString("Failed to add slot name='%1' signature='%2'").arg(name).arg(slot) );
    return event;
}

QtObject::Ptr MainModule::addQObject(QObject* object)
{
    QtObject* qtobject = new QtObject(this, object);
    if(! addChild(qtobject))
        throw new Exception( QString("Failed to add QObject name='%1'").arg(object->name()) );
    return qtobject;
}

EventAction::Ptr MainModule::addKAction(const QString& name, KAction* action)
{
    EventAction* event = new EventAction(name, this, action);
    if(! addChild(event))
        throw new Exception( QString("Failed to add KAction name='%1'").arg(action->name()) );
    return event;
}

