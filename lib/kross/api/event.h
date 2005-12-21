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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
#include <qvaluelist.h>
#include <qmap.h>
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

            /**
             * Definition of funtion-pointers.
             */
            typedef Object::Ptr(T::*FunctionPtr)(List::Ptr);

            /**
             * The function-pointer this event points too.
             */
            FunctionPtr m_function;

            /**
             * List of memberfunctions. Each function is accessible
             * by the functionname.
             */
            QMap<QString, Event<T>* > m_functions;

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
#ifdef KROSS_API_EVENT_CALL_DEBUG
                if(m_functions.contains(name))
                    kdDebug() << QString("Event::addFunction() Replaced function %1 with arguments %2").arg(name).arg(arglist.toString()) << endl;
                else
                    kdDebug() << QString("Event::addFunction() Added function %1 with arguments %2").arg(name).arg(arglist.toString()) << endl;
#endif
                m_functions.replace(name, new Event<T>(name, this, function, arglist));
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
            virtual ~Event()
            {
                for(QMapIterator<QString, Event<T>* > it = m_functions.begin(); it != m_functions.end(); ++it)
                    delete it.data();
            }

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

                Event<T>* event = m_functions[name];
                if(event) {
#ifdef KROSS_API_EVENT_CALL_DEBUG
                    kdDebug() << QString("Event::call() event.name='%1' event.tostring='%2' event.args.tostring='%3'")
                                 .arg(event->getName()).arg(event->toString()).arg(event->m_arglist.toString()) << endl;
#endif

                    // We have an valid event, so just call it...
                    return event->call(QString::null, arguments);
                }

                if(name.isNull()) {
                    if(m_function) {
#ifdef KROSS_API_EVENT_CALL_DEBUG
                        kdDebug() << QString("Event::call() Call builtin function in object '%1'").arg(getParent().data() ? getParent()->getName() : "!NULL!") << endl;
#endif

                        // Check the arguments. Throws an exception if failed.
                        checkArguments(arguments);

                        // We try to redirect the call to the m_function pointer the
                        // parent object defines.
                        T *self = static_cast<T*>( getParent().data() );
                        if(! self)
                            throw Exception::Ptr( new Exception(QString("The event '%1' points to an invalid instance.").arg(getName())) );

                        return (self->*m_function)(arguments);
                    }

                    // If no name and no function are defined, we return a reference to our instance.
                    return this;
                }

                // Redirect the call to the Kross::Api::Callable we are inheritated from.
                return Callable::call(name, arguments);
            }

    };

}}

#endif

