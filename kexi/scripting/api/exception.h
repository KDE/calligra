/***************************************************************************
 * exception.h
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

#ifndef KROSS_API_EXCEPTION_H
#define KROSS_API_EXCEPTION_H

#include "object.h"

#include <qstring.h>
#include <ksharedptr.h>
#include <klocale.h>

namespace Kross { namespace Api {

    /**
     * Common exception class used for representing exceptions
     * in Kross.
     *
     * Internal we use \a Exception instances to throw and handle
     * exceptions. Those exceptions are inherited from \a Object
     * and therefore they are first class citizens in Kross.
     */
    class Exception : public Object
    {
        private:

            /// The error message.
            QString m_error;

        public:

            /// Shared pointer to implement reference-counting.
            typedef KSharedPtr<Exception> Ptr;

            /**
             * Constructor.
             *
             * \param error The error message.
             * \param parent The parent \a Object or NULL if
             *        this exception object doesn't has a
             *        parent.
             */
            Exception(const QString& error, Object::Ptr parent = 0)
                : Object("Exception", parent)
                , m_error(error)
            {
            }

            /**
             * Destructor.
             */
            virtual ~Exception()
            {
            }

            /// \see Kross::Api::Object::getClassName()
            virtual const QString getClassName() const
            {
                return "Kross::Api::Exception";
            }

            /// \see Kross::Api::Object::getDescription()
            virtual const QString getDescription() const
            {
                return "Exception object.";
            }

            /// \see Kross::Api::Object::toString()
            virtual const QString toString()
            {
                return i18n("Exception: %1").arg( getError() );
            }

            /**
             * \return the error message.
             */
            const QString& getError()
            {
                return m_error;
            }

    };

}}

#endif

