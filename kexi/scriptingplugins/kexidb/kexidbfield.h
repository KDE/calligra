/***************************************************************************
 * kexidbfield.h
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

#ifndef KROSS_KEXIDB_KEXIDBFIELD_H
#define KROSS_KEXIDB_KEXIDBFIELD_H

#include <qstring.h>

#include <api/object.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/drivermanager.h>
#include <kexidb/field.h>

namespace Kross { namespace KexiDB {

    class KexiDBField : public Kross::Api::Class<KexiDBField>
    {
        public:

            /**
             * Constructor.
             *
             * \param field The \a ::KexiDB::Field object this
             *        instance wraps.
             */
            KexiDBField(::KexiDB::Field* field);

            /**
             * Destructor.
             */
            virtual ~KexiDBField();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            ::KexiDB::Field* field() { return m_field; }

        private:
            ::KexiDB::Field* m_field;

            Kross::Api::Object::Ptr type(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setType(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr subType(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setSubType(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr typeGroup(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isAutoInc(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setAutoInc(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isUniqueKey(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setUniqueKey(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isPrimaryKey(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setPrimaryKey(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isForeignKey(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setForeignKey(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isNotNull(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setNotNull(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isNotEmpty(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setNotEmpty(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isIndexed(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setIndexed(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr isUnsigned(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setUnsigned(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr name(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setName(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr caption(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setCaption(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr description(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setDescription(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr length(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setLength(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr precision(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setPrecision(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr width(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setWidth(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr defaultValue(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setDefaultValue(Kross::Api::List::Ptr);

    };

}}

#endif

