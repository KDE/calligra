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

            operator QStringList () {
                //QValueList<Object::Ptr> getValue()
                krossdebug("999999999999 ...........................");
                return QStringList();
            }

            /**
             * Constructor.
             *
             * \param value The list of \a Object instances this
             *        list has initialy.
             */
            List(QValueList<Object::Ptr> value = QValueList<Object::Ptr>());

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
             * \param defaultobject The default \a Object which should
             *        be used if there exists no item with such an
             *        index. This \a Object instance will be returned
             *        if not NULL and if the index is out of bounds. If
             *        its NULL a \a TypeException will be thrown.
             * \return The \a Object instance.
             */
            Object::Ptr item(uint idx, Object* defaultobject = 0);

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

            template<typename TYPE>
            static Object::Ptr toObject(TYPE t) { return t; }
    };

    /**
     * This template class extends the \a List class with
     * generic functionality to deal with lists.
     */
    template< class OBJECT >
    class ListT : public List
    {
        public:
            ListT() : List() {}
            ListT(QValueList<OBJECT> values) : List(values) {}

            template< typename TYPE >
            ListT(QIntDict<TYPE> values) : List()
            {
                QIntDictIterator<TYPE> it( values );
                TYPE *t;
                while( (t = it.current()) != 0 ) {
                    this->append( new OBJECT(t) );
                    ++it;
                }
            }

            template< typename TYPE >
            ListT(const QPtrList<TYPE> values) : List()
            {
                QPtrListIterator<TYPE> it(values);
                TYPE *t;
                while( (t = it.current()) != 0 ) {
                    this->append( new OBJECT(t) );
                    ++it;
                }
            }

            virtual ~ListT() {}

            template<typename TYPE>
            static Object::Ptr toObject(TYPE t)
            {
                return new ListT(t);
            }
    };

}}

#endif

