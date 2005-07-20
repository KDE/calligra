/***************************************************************************
 * eventcollection.h
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

#ifndef KROSS_API_EVENTCOLLECTION_H
#define KROSS_API_EVENTCOLLECTION_H

#include <qstring.h>
//#include <qvariant.h>
#include <qobject.h>
//#include <kdebug.h>
//#include "object.h"
#include <ksharedptr.h>

//#include "../api/event.h"
#include "../api/eventaction.h"
#include "../api/eventsignal.h"
#include "../api/eventslot.h"

namespace Kross { namespace Api {

    // Forward declarations.
    //class Object;
    class Callable;
    class EventCollectionPrivate;

    /**
     * The \a EventCollection class is used as container for
     * a bunch of \a Event instances.
     *
     * Use the \a EventCollection to add Qt signals and slots,
     * QObject or KAction instances or \a ScriptContainer
     * events, collect them together and work with them.
     */
    class EventCollection : public KShared
    {
            // We protected the constructor cause EventCollection
            // instances should be created only within the
            // Manager::getEventCollection() method.
            friend class Manager;

        protected:

            /**
             * Constructor.
             *
             * \param collectionname The unique name this
             *       EventCollection has. It's used e.g.
             *       at the \a Manager to identify the
             *       EventCollection instance.
             */
            EventCollection(const QString& collectionname);

        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<EventCollection> Ptr;

            /// List of events.
            typedef QValueList<Callable::Ptr> EventList;

            /**
             * Destructor.
             */
            virtual ~EventCollection();

            /**
             * Return the name this EventCollection is reachable as.
             */
            const QString& getName();

            /**
             * Return the \a Event associtated with the name or
             * NULL if there is no such event in this collection.
             */
            EventList getEvents();

            /**
             * Add a \a QObject to the eventcollection. All
             * signals and slots the QObject has will be
             * added to a new \a EventCollection instance
             * which is child of this \a EventCollection
             * instance.
             */
            //EventCollection::Ptr addQObject(QObject* object);

            /**
             * Add a \a KAction to the eventcollection. The
             * KAction will be wrapped by a \a EventAction
             * and will be added to this collection.
             */
            EventAction::Ptr addKAction(KAction* action);

            /**
             * Add a Qt signal to the eventcollection by
             * creating an \a EventSignal for it.
             */
            EventSignal::Ptr addSignal(QObject* sender, QCString signal);

            /**
             * Add a Qt slot to the eventcollection by
             * creating an \a EventSlot for it.
             */
            EventSlot::Ptr addSlot(QObject* receiver, QCString slot);

            /*NOTE
            At least for serialization we need crossreferences between eventcollections.
            E.g. "myeventcollection1/event1" points to "myeventcollection2/event2",
                 but in that case we need to build the collections _before_ unserialize!
            We need also a security model for that cases!
            */
            //const QString& serializeToXML();
            //void unserializeFromXML(const QString& xml);

        private:
            /// Internaly used private d-pointer.
            EventCollectionPrivate* d;
    };

}}

#endif

