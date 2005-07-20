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

#include "object.h"
#include "argument.h"
#include "callable.h"

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
            /// Pointer to the memberfunction.
            FunctionPtr m_function;

        public:
            Event(const QString& name) //FIXME
                : Callable(name, 0, ArgumentList(), "TODO: Documentation"), m_function(0) {}
            Event(const QString& name, Object::Ptr parent, FunctionPtr function, ArgumentList arglist, const QString& documentation)
                : Callable(name, parent, arglist, documentation), m_function(function) {}
            virtual ~Event() {}

            virtual Object::Ptr call(const QString& name, List::Ptr arguments)
            {
                kdDebug() << QString("Event::call() name='%1'").arg(getName()) << endl;

                if(! name.isEmpty()) {
                    // If the name isn't empty this function shouldn't be executed. But
                    // does it really make sense to redirect the call to a child object
                    // of this function? Just let's throw an exception as long as we
                    // don't need this functionality :-)
                    throw RuntimeException(QString("Event::call() name='%1': Invalid functionname '%2'.").arg(getName()).arg(name));
                }

                // Check the arguments. Throws an exception if failed.
                checkArguments(arguments);

                //T *self = static_cast<T*>( getParent() ); //FIXME don't refcount parent's
                T *self = static_cast<T*>( getParent().data() );
                if(! self)
                    throw RuntimeException(QString("The event '%1' points to an invalid instance.").arg(getName()));

                // Call the classfunction via our remembered method-pointer.
                return (self->*m_function)(arguments);
            }

    };

}}

#endif

