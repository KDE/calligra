/***************************************************************************
 * value.h
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

#ifndef KROSS_API_VALUE_H
#define KROSS_API_VALUE_H

#include <qstring.h>
#include <qvariant.h>
//#include <kdebug.h>

#include "object.h"

namespace Kross { namespace Api {

    /**
     * Template class to represent values. Any kind
     * of value like \a Variant are implementating
     * this class. That way we have a common base
     * for all kind of values.
     */
    template<class T, class V>
    class Value : public Object
    {
        protected:

            /**
             * Constructor.
             *
             * \param value The initial value this
             *        Value has.
             * \param name The name this Value has.
             */
            Value(V value, const QString& name)
                : Object(name)
                , m_value(value) {}

        public:

            /**
             * Destructor.
             */
            virtual ~Value() {}

            /**
             * Create a new Value instance. This static
             * function is the only way to instanciate
             * value-object's.
             */
            static T* create(V value, const QString& name = QString::null) {
                static ObjectList m_objectlist;
                T* t = 0;
                if(name.isNull())
                    t = new T(value);
                else
                    t = new T(value, name);
                m_objectlist.append(t); // garbage collector
                return t;
            }

            /**
             * Return the value.
             *
             * \return The value this Value-class holds.
             */
            inline V& getValue() { return m_value; }

            /**
             * Set the value.
             *
             * \param value The value to set.
             */
            inline void setValue(V& value) { m_value = value; }

        private:
            V m_value;
    };

}}

#endif

