/***************************************************************************
 * class.h
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

#include "object.h"
#include "list.h"
#include "exception.h"
#include "argument.h"

namespace Kross { namespace Api {

    /**
     * From \a Object inherited template-class to represent
     * class-structures. Classes implemetating this template
     * are able to dynamicly define callable functions
     * accessible from within scripts.
     */
    template<class T>
    class Class : public Object
    {
        private:

            /// Definition of funtion-pointers.
            typedef Object*(T::*FunctionPtr)(List*);

            /**
             * The Function class is an internal container
             * for callable functions.
             */
            class Function
            {
                public:
                    /// Pointer to the memberfunction.
                    FunctionPtr function;
                    /// List of arguments this function supports.
                    ArgumentList arglist;
                    /// Some documentation to describe the function.
                    QString documentation;
            };

            /// List of callable functions this instance spends.
            QMap<QString, Function*> m_functions;

        protected:

            /**
             * Add a callable function to the list of functions this
             * Object supports.
             *
             * \param name The functionname. Each function this object
             *        holds shgould have an unique name to be
             *        still accessable.
             * \param function A pointer to the methodfunction that
             *        should handle calls.
             * \param documentation Some documentation used to describe
             *        what the function does.
             */
            void addFunction(const QString& name, FunctionPtr function, ArgumentList arglist, const QString& documentation)
            {
                Function* f = new Function();
                f->function = function;
                f->arglist = arglist;
                f->documentation = documentation;
                m_functions.replace(name, f);
            }

        public:

            /**
             * Constructor.
             *
             * \param name The name this class has.
             * \param parentmodule The parent \a Module object
             *        or NULL if this class has no parent.
             */
            explicit Class(const QString& name, Object* parentmodule = 0)
                : Object(name, parentmodule)
            {
            }

            /**
             * Destructor.
             */
            virtual ~Class()
            {
            }

            /**
             * Overloaded method to handle function-calls.
             *
             * \throw AttributeException if argumentparameters
             *        arn't valid.
             * \param name The functionname. Each function this
             *        Object holds should have a different
             *        name cause they are access by they name.
             * \param arguments The list of arguments.
             * \return An Object representing the call result
             *         or NULL if there doesn't exists such a
             *         function with defined name.
             */
            virtual Object* call(const QString& name, List* arguments)
            {
                Function* f = m_functions[name];
                if(! f) return 0;
                T *self = static_cast<T*>(this);

                QValueList<Object*> arglist = arguments->getValue();
                if(arglist.size() < f->arglist.getMinParams())
                    throw AttributeException(i18n("To less parameters for '%1'.").arg(name));
                if(arglist.size() > f->arglist.getMaxParams())
                    throw AttributeException(i18n("To much parameters for '%1'.").arg(name));

                FunctionPtr function = f->function;
                return (self->*function)(arguments);
            }

            /**
             * Return a list of avaible functions.
             *
             * \return List of avaible functions.
             */
            QStringList getCalls()
            {
                QStringList list;
                QMap<QString, Function*>::Iterator it(m_functions.begin());
                for(; it != m_functions.end(); ++it)
                    list.append(it.key());
                return list;
            }
    };

}}

#endif

