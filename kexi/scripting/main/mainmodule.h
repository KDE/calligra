/***************************************************************************
 * mainmodule.h
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

#ifndef KROSS_API_MAINMODULE_H
#define KROSS_API_MAINMODULE_H

#include "../api/object.h"
#include "../api/variant.h"
#include "../api/module.h"
#include "../api/event.h"
#include "../api/eventsignal.h"
#include "../api/eventslot.h"
#include "../api/qtobject.h"
#include "../api/eventaction.h"

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>

#include <ksharedptr.h>
#include <kaction.h>

namespace Kross { namespace Api {

    /**
     * This class implements a global shared \a Module
     * singleton.
     */
    class MainModule : public Module<MainModule>
    {
        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<MainModule> Ptr;

            /**
             * Constructor.
             */
            explicit MainModule();

            /**
             * Destructor.
             */
            virtual ~MainModule();

            /// \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            /**
             * Add a Qt signal to the \a Module by
             * creating an \a EventSignal for it.
             */
            EventSignal::Ptr addSignal(const QString& name, QObject* sender, QCString signal);

            /**
             * Add a Qt slot to the \a Module by
             * creating an \a EventSlot for it.
             */
            EventSlot::Ptr addSlot(const QString& name, QObject* receiver, QCString slot);

            /**
             * Add a \a QObject to the eventcollection. All
             * signals and slots the QObject has will be
             * added to a new \a EventCollection instance
             * which is child of this \a EventCollection
             * instance.
             */
            QtObject::Ptr addQObject(QObject* object);

            /**
             * Add a \a KAction to the eventcollection. The
             * KAction will be wrapped by a \a EventAction
             * and will be added to this collection.
             */
            EventAction::Ptr addKAction(const QString& name, KAction* action);

            //typedef QValueList<Callable::Ptr> EventList;
            //EventList getEvents();
            //const QString& serializeToXML();
            //void unserializeFromXML(const QString& xml);
    };

}}

#endif

