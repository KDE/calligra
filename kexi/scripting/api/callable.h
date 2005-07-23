/***************************************************************************
 * callable.h
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

#ifndef KROSS_API_FUNCTION_H
#define KROSS_API_FUNCTION_H

#include "object.h"
#include "list.h"
//#include "exception.h"
#include "argument.h"

#include <qstring.h>
#include <qvaluelist.h>
#include <ksharedptr.h>

namespace Kross { namespace Api {

    /**
     * Base class for a callable function. A function always
     * implements the \a Object::call() method to handle
     * the call.
     */
    class Callable : public Object
    {
        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<Callable> Ptr;

            /**
             * Constructor.
             *
             * \param name The name this callable object has and
             *       it is reachable as via \a Object::getChild() .
             * \param parent The parent \a Object this instance is
             *       child of.
             * \param arglist A list of arguments the callable
             *       object expects if it got called.
             * \param documentation The documentation to describe
             *       the callable object.
             */
            Callable(const QString& name, Object::Ptr parent, ArgumentList arglist, const QString& documentation);

            /**
             * Destructor.
             */
            virtual ~Callable();

            /**
             * \return the classname.
             */
            virtual const QString getClassName() const;

            /**
             * \return a description.
             */
            virtual const QString getDescription() const;

            /**
             * Call the object.
             */
            virtual Object::Ptr call(const QString& name, List::Ptr arguments) = 0;

        protected:
            /// List of arguments this callable object supports.
            ArgumentList m_arglist;
            /// Some documentation to describe the callable object.
            QString m_documentation;

            /// Check the passed arguments against the \a m_arglist and throws an exception if failed.
            void checkArguments(KSharedPtr<List> arguments);

            /// \see Kross::Api::Object::hasChild()
            Object::Ptr hasChild(List::Ptr args);
            /// \see Kross::Api::Object::getChild()
            Object::Ptr getChild(List::Ptr args);
//TODO? Object::Ptr setChild(List::Ptr args);
            /// \see Kross::Api::Object::getChildren()
            Object::Ptr childNames(List::Ptr args);
            /// \see Kross::Api::Object::call()
            Object::Ptr callChild(List::Ptr args);
    };

}}

#endif

