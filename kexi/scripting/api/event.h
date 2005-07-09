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
- ScriptContainer has list of class ScriptEvent
- so, fill up ScriptContainer before xml -> EventContainer ?
  - could also be done on runtime <- better!
*/

    /**
     * Events are used for dynamic connections between
     * \a Object instances and introduce an abstract Qt like
     * signals and slot concept to chain events together.
     */
    class Event : public Kross::Api::Class<Event>
    {
        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<Event> Ptr;

            /**
             * Constructor.
             *
             * \param name The unique name this \a Event has to
             *       easy identify it.
             */
            explicit Event(const QString& name);

            /**
             * Destructor.
             */
            virtual ~Event();

            //bool attach(Event::Ptr event);
            //bool detach(Event::Ptr event);
            /*
            Object::Ptr getParent() const;
            bool hasChild(const QString& name) const;
            Object::Ptr getChild(const QString& name) const;
            QMap<QString, Object::Ptr> getChildren() const;
            bool addChild(const QString& name, Object::Ptr object, bool replace = false);
            void removeChild(const QString& name);
            void removeAllChildren();
            */

//Override call
            //virtual Object::Ptr call(const QString& name, KSharedPtr<List> arguments) {}

//Maybe move to Kross::Api::Object ?!
            //bool connect(Event::Ptr event) { connect(event, SIGNAL(called(Event::Ptr)), SLOT(call(Event::Ptr))); }
            //bool disconnect(Event::Ptr) {}

        protected:
            //void called(Event::Ptr event) {}
            //void executed(Event* sender, int execstate, Kross::Api::Object*);

            //void call(Event::Ptr event) {}
            //void call(Event* sender, const QString& name, Kross::Api::List*) {}

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

