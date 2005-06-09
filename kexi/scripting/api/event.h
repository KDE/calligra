/***************************************************************************
 * event.h
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

#ifndef KROSS_API_EVENT_H
#define KROSS_API_EVENT_H

#include <qstring.h>
#include <qobject.h>
//#include <kaction.h>
#include <ksharedptr.h>

//#include "class.h"
#include "qtobject.h"

namespace Kross { namespace Api {

    // Forward declarations.
    class ScriptContainer;

/*
    class Event{Translator|Attacher|Dispatcher}
    {
        public:
            QCString getSignal(QCString slot);
            QCString getSlot(QCString signal);
    };
*/

    /**
     * Events are used for dynamic connections between
     * objects and introduce an abstract Qt signals and
     * slot chain functionality.
     */
    class Event
        : public QObject
        , public Kross::Api::Class<Event>
    {
            Q_OBJECT

        public:
            typedef KSharedPtr<Event> Ptr;

            /**
             * Constructor.
             */
            explicit Event(const QString& name = "Event" /*, QObject* object*/);

            /**
             * Destructor.
             */
            virtual ~Event();

            //ScriptContainer* getScriptContainer();
            //void setScriptContainer(ScriptContainer* scriptcontainer);

            //bool attach(Event::Ptr event);
            //bool detach(Event::Ptr event);

        signals:
            void executed(Event::Ptr event);
            //void executed(Event* sender, int execstate, Kross::Api::Object*);

        public slots:
            void execute(Event::Ptr event) {}
            //void execute(Event* sender, const QString& name, Kross::Api::List*) {}

        private:
            // QProperty's
            //Kross::Api::Object::Ptr propertyNames(Kross::Api::List::Ptr);
            //Kross::Api::Object::Ptr hasProperty(Kross::Api::List::Ptr);
            //Kross::Api::Object::Ptr getProperty(Kross::Api::List::Ptr);
            //Kross::Api::Object::Ptr setProperty(Kross::Api::List::Ptr);
    };

    //class EventCondition : public Event {};

}}

#endif

