/***************************************************************************
 * list.h
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

#ifndef KROSS_API_LIST_H
#define KROSS_API_LIST_H

#include <qstring.h>
#include <qvaluelist.h>

#include "object.h"
#include "value.h"

namespace Kross { namespace Api {

    /**
     * The List class implementates \a Value to handle
     * lists and collections.
     */
    class List : public Value< List, QValueList<Object*> >
    {
            friend class Value< List, QValueList<Object*> >;
        protected:
            List(const QValueList<Object*>& value, const QString& name = "list");
        public:
            virtual ~List();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            /**
             * Return the \a Object with defined index from the
             * QValueList this list holds.
             *
             * \throw TypeException If index is out of bounds.
             * \param idx The QValueList-index.
             * \return The \a Object.
             */
            Object* item(uint idx);

            /**
             * Return the number of items in the QValueList this
             * list holds.
             *
             * \return The number of items.
             */
            uint count();
    };

}}

#endif

