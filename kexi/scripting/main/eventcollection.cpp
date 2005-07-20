/***************************************************************************
 * eventcollection.cpp
 * This file is part of the KDE project
 * copyright (C)2005 by Sebastian Sauer (mail@dipe.org)
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

#include "eventcollection.h"
//#include "../api/object.h"
//#include "../api/list.h"
#include "../main/manager.h"
//#include "../api/qtobject.h"
//#include "../api/interpreter.h"
//#include "../api/script.h"
//#include "../api/eventmanager.h"

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class EventCollectionPrivate
    {
        public:
            /// The name the collection has.
            QString m_name;
            /// List of \a Event instances this collection has.
            EventCollection::EventList m_events;
    };

}}

EventCollection::EventCollection(const QString& collectionname)
    : KShared() // initialize reference-counter
    , d( new EventCollectionPrivate() )
{
    d->m_name = collectionname;
}

EventCollection::~EventCollection()
{
    delete d;
}

const QString& EventCollection::getName()
{
    return d->m_name;
}

EventCollection::EventList EventCollection::getEvents()
{
    return d->m_events;
}

EventAction::Ptr EventCollection::addKAction(KAction* action)
{
/*TODO
    EventAction* event = new EventAction(action);
    d->m_events.append(event);
    return event;
*/
}

EventSignal::Ptr EventCollection::addSignal(QObject* sender, QCString signal)
{
/*TODO
    EventSignal* event = new EventSignal(sender, signal);
    d->m_events.append(event);
    return event;
*/
}

EventSlot::Ptr EventCollection::addSlot(QObject* receiver, QCString slot)
{
/*TODO
    EventSlot* event = new EventSlot(receiver, slot);
    d->m_events.append(event);
    return event;
*/
}
