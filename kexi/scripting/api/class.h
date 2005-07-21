/***************************************************************************
 * class.h
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

#ifndef KROSS_API_CLASS_H
#define KROSS_API_CLASS_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qobject.h>
#include <klocale.h>
#include <kdebug.h>

#include "object.h"
#include "classbase.h"
#include "list.h"
#include "event.h"
#include "exception.h"
#include "argument.h"

namespace Kross { namespace Api {

    /**
     * From \a ClassBase inherited template-class to represent
     * class-structures. Classes implemetating this template
     * are able to dynamicly define \a Callable methodfunctions
     * accessible from within scripts.
     */
    template<class T>
    class Class : public ClassBase
    {
        private:
            /// Definition of funtion-pointers.
            typedef Object::Ptr(T::*FunctionPtr)(List::Ptr);
            /// List of memberfunctions.
            QMap<QString, Event<T> * > m_functions;
            /// List of memberfunction names.
            QStringList m_functionnames;

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
             * \param documentation Some documentation used to describe
             *        what the function does.
	     *
	     * \todo Is that template arguments or concrete arguments?
             */
            void addFunction(const QString& name, FunctionPtr function, ArgumentList arglist, const QString& documentation)
            {
                //if(m_functions.contains(name)) throw... needed?
                //FIXME pass this instance as parent to Event ?!
                Event<T> *event = new Event<T>(name, this, function, arglist, documentation);
                m_functions.replace(name, event);
                m_functionnames.append(name);
            }

        public:

            /**
             * Constructor.
             *
             * \param name The name this class has.
             * \param parentmodule The parent \a Module object
             *        or NULL if this class has no parent.
             */
            explicit Class(const QString& name, Object::Ptr parentmodule = 0)
                : ClassBase(name, parentmodule)
            {
            }

            /**
             * Destructor.
             */
            virtual ~Class()
            {
                /*FIXME maybe we should't trust the reference counter here?
                typename QMap<QString, Class<T>::Function* >::Iterator it = m_functions.begin();
                for(; it != m_functions.end(); ++it)
                    delete it.data();
                */
            }

            /**
             * Overloaded method to handle function-calls.
             *
             * \throw AttributeException if argumentparameters
             *        arn't valid.
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
                kdDebug() << QString("Class::call(%1)").arg(name) << endl;

                Event<T> *f = m_functions[name];
                if(! f) // no function with that name, pass call to super class
                    return Object::call(name, arguments);

                QString s = ""; //FIXME implement namespace/url resolution?!
                return f->call(s, arguments);
            }

            /**
             * Return a list of avaible functionnames.
             *
             * \return List of avaible functionnames.
             */
            QStringList getCalls()
            {
                return m_functionnames;
            }
    };

}}

#endif

