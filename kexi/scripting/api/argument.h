/***************************************************************************
 * argument.h
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

#ifndef KROSS_API_ARGUMENT_H
#define KROSS_API_ARGUMENT_H

#include <qstring.h>
#include <qvaluelist.h>

#include "object.h"

namespace Kross { namespace Api {

    // Forward declaration.
    class ArgumentList;

    /**
     * Each \a Class::Function holds a list of arguments
     * the function supports. The Argument-class represents
     * such a single argument in a \a ArgumentList collection.
     */
    class Argument
    {
        public:

            /**
             * Constructor.
             *
             * \param classname The name of the class this
             *        argument expects.
             * \param object The optional default \a Object this
             *        class holds.
             */
            Argument(const QString& classname = QString::null, Object* object = 0);

            /**
             * Destructor.
             */
            ~Argument();

            /**
             * Return the name of the class this argument expects.
             *
             * \return Name of the class this argument expects.
             */
            inline const QString& getClassName();

            /**
             * Return the optional default \a Object this class
             * holds.
             *
             * \return The default object or NULL if the argument
             *         isn't optional and therefore doesn't have
             *         an default object.
             */
            inline Object* getObject();

            /**
             * Implementation of the << operator.
             *
             * \param arglist The \a ArgumentList the
             *        operator is applied on.
             * \param arg The \a Argument to add to the
             *        passed \a ArgumentList.
             * \return The changed \a ArgumentList.
             */
            ArgumentList& operator << (ArgumentList& arglist);

        private:
            /// The classname of the argument.
            QString m_classname;
            /// The optional default Object* this argument holds.
            Object* m_object;
    };

    /**
     * An ArgumentList is a collection of \a Argument
     * objects used in \a Class::Function.
     */
    class ArgumentList
    {
        public:

            /**
             * Constructor.
             */
            ArgumentList();

            /**
             * Destructor.
             */
            ~ArgumentList();

            /**
             * Implementation of the << operator.
             *
             * \param arg The passed \a Argument.
             * \return The changed \a ArgumentList.
             */
            ArgumentList& operator << (Argument arg);

            /**
             * Return number of minimal needed parameters.
             *
             * \return Minimal needed parameters.
             */
            uint getMinParams();

            /**
             * Return the number of maximal allowed parameters.
             *
             * \return Maximal needed parameters.
             */
            uint getMaxParams();

        private:
            /// Minimal needed parameters.
            uint m_minparams;
            /// Maximal needed parameters.
            uint m_maxparams;
            /// List of \a Argument objects.
            QValueList<Argument> m_arguments;
    };

}}

#endif

