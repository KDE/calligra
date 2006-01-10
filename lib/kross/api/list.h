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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_API_LIST_H
#define KROSS_API_LIST_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qintdict.h>

#include "object.h"
#include "value.h"

namespace Kross { namespace Api {

    /**
     * The List class implementates \a Value to handle
     * lists and collections.
     */
    class List : public Value< List, QValueList<Object::Ptr> >
    {
            friend class Value< List, QValueList<Object::Ptr> >;
        public:

            /**
             * Shared pointer to implement reference-counting.
             */
            typedef KSharedPtr<List> Ptr;

            /**
             * Constructor.
             *
             * \param value The list of \a Object instances this
             *        list has initialy.
             * \param name A name this list has.
             */
            List(QValueList<Object::Ptr> value = QValueList<Object::Ptr>(), const QString& name = "list");

            /**
             * Destructor.
             */
            virtual ~List();

            /**
             * See \see Kross::Api::Object::getClassName()
             */
            virtual const QString getClassName() const;

            /**
             * \return a string representation of the whole list.
             *
             * \see Kross::Api::Object::toString()
             */
            virtual const QString toString();

            /**
             * Return the \a Object with defined index from the
             * QValueList this list holds.
             *
             * \throw TypeException If index is out of bounds.
             * \param idx The QValueList-index.
             * \return The \a Object instance.
             */
            Object::Ptr item(uint idx);

            /**
             * Return the number of items in the QValueList this
             * list holds.
             *
             * \return The number of items.
             */
            uint count();

            /**
             * Append an \a Kross::Api::Object to the list.
             *
             * \param object The \a Kross::Api::Object instance to
             *       append to this list.
             */
            void append(Object::Ptr object);

    };

    /**
     * This template class extends the \a List class with a more
     * generic way to deal with lists.
     */
    template< class OBJECT, class TYPE >
    class ListT : public List
    {
        public:
            ListT(QValueList<TYPE> values) : List(values) {}

            ListT(QIntDict<TYPE> values) : List() {
                QIntDictIterator<TYPE> it( values );
                TYPE *t;
                while( (t = it.current()) != 0 ) {
                    this->append( new OBJECT(t) );
                    ++it;
                }
            }

            ListT(const QPtrList<TYPE> values) : List() {
                QPtrListIterator<TYPE> it( values );
                TYPE *t;
                while( (t = it.current()) != 0 ) {
                    this->append( new OBJECT(t) );
                    ++it;
                }
            }

            virtual ~ListT() {}
    };

}}

#endif

