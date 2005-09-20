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

#include "../main/krossconfig.h"
#include "object.h"
#include "argument.h"
#include "callable.h"
#include "list.h"
#include "exception.h"

#include <qstring.h>
#include <kdebug.h>

namespace Kross { namespace Api {

    /**
     * Template class for all kinds of callable events. An
     * event is the abstract base for callable objects like
     * methodfunctions in \a Class instances or \a EventSlot
     * and \a EventSignal to access Qt signals and slots.
     */
    template<class T>
    class Event : public Callable
    {
        private:

            /// Definition of funtion-pointers.
            typedef Object::Ptr(T::*FunctionPtr)(List::Ptr);

            /// The function-pointer this event points too.
            FunctionPtr m_function;

            /// List of memberfunctions.
            QMap<QString, Event<T> * > m_functions;

        protected:

            /**
             * Add a \a Callable methodfunction to the list of functions
             * this Object supports.
             *
             * \param name The functionname. Each function this object
             *        holds should have an unique name to be
             *        still accessable.
             * \param function A pointer to the methodfunction that
             *        should handle calls.
             * \param arglist A list of arguments for the function.
             *
             * \todo Is that template arguments or concrete arguments?
             */
            void addFunction(const QString& name, FunctionPtr function, ArgumentList arglist = ArgumentList())
            {
                if(m_functions.contains(name))
                    throw Exception::Ptr( new Exception( QString("Class::addFunction(%1 failed cause there exists already a function with such name.)").arg(name) ) );
                Event<T> *event = new Event<T>(name, this, function, arglist);
                m_functions.replace(name, event);
            }

        public:


            /**
             * Constructor.
             */
            Event(const QString& name, Object::Ptr parent)
                : Callable(name, parent, ArgumentList())
                , m_function(0) {}

            /**
             * Constructor.
             */
            Event(const QString& name, Object::Ptr parent, FunctionPtr function, ArgumentList arglist)
                : Callable(name, parent, arglist)
                , m_function(function) {}

            /**
             * Destructor.
             */
            virtual ~Event() {}

            /**
             * Overloaded method to handle function-calls.
             *
             * \throw AttributeException if argumentparameters
             *        arn't valid.
             * \throw RuntimeException if the functionname isn't
             *        valid.
             * \param name The functionname. Each function this
             *        Object holds should have a different
             *        name cause they are access by they name.
             *        If name is QString::null or empty, a
             *        self-reference to this instance is
             *        returned.
             * \param arguments The list of arguments.
             * \return An Object representing the call result
             *         or NULL if there doesn't exists such a
             *         function with defined name.
             */
            virtual Object::Ptr call(const QString& name, List::Ptr arguments)
            {
#ifdef KROSS_API_EVENT_CALL_DEBUG
                kdDebug() << QString("Event::call() name='%1' getName()='%2'").arg(name).arg(getName()) << endl;
#endif

                if(name.isEmpty() && m_function) {

                    // Check the arguments. Throws an exception if failed.
                    checkArguments(arguments);

                    // We try to redirect the call to the m_function of
                    // the parent event.
                    Object::Ptr parent = getParent(); //FIXME don't rely on the parent
                    if(parent) {
                        T *self = static_cast<T*>( parent.data() );
                        if(self)
                            return (self->*m_function)(arguments);
                    }

                    // Something went wrong.
                    throw Exception::Ptr( new Exception(QString("The event '%1' points to an invalid instance.").arg(getName())) );
                }

                // Check if we've a registered event with that name and
                // if that's the case, just redirect the call to the event.
                Event<T> *event = m_functions[name];
                if(event) {
                    QString s = ""; // empty string cause we like to execute the event itself.
                    return event->call(s, arguments);
                }

                // Redirect the call to Kross::Api::Object
                return Callable::call(name, arguments); //Callable::call(name, arguments);
            }

    };

}}

#endif

