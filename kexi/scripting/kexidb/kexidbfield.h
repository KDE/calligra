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

#include "../api/object.h"
#include "../api/list.h"
#include "../api/class.h"

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

            Kross::Api::Object* type(Kross::Api::List*);
            Kross::Api::Object* setType(Kross::Api::List*);

            Kross::Api::Object* subType(Kross::Api::List*);
            Kross::Api::Object* setSubType(Kross::Api::List*);

            Kross::Api::Object* typeGroup(Kross::Api::List*);

            Kross::Api::Object* isAutoInc(Kross::Api::List*);
            Kross::Api::Object* setAutoInc(Kross::Api::List*);

            Kross::Api::Object* isUniqueKey(Kross::Api::List*);
            Kross::Api::Object* setUniqueKey(Kross::Api::List*);

            Kross::Api::Object* isPrimaryKey(Kross::Api::List*);
            Kross::Api::Object* setPrimaryKey(Kross::Api::List*);

            Kross::Api::Object* isForeignKey(Kross::Api::List*);
            Kross::Api::Object* setForeignKey(Kross::Api::List*);

            Kross::Api::Object* isNotNull(Kross::Api::List*);
            Kross::Api::Object* setNotNull(Kross::Api::List*);

            Kross::Api::Object* isNotEmpty(Kross::Api::List*);
            Kross::Api::Object* setNotEmpty(Kross::Api::List*);

            Kross::Api::Object* isIndexed(Kross::Api::List*);
            Kross::Api::Object* setIndexed(Kross::Api::List*);

            Kross::Api::Object* isUnsigned(Kross::Api::List*);
            Kross::Api::Object* setUnsigned(Kross::Api::List*);

            Kross::Api::Object* name(Kross::Api::List*);
            Kross::Api::Object* setName(Kross::Api::List*);

            Kross::Api::Object* caption(Kross::Api::List*);
            Kross::Api::Object* setCaption(Kross::Api::List*);

            Kross::Api::Object* description(Kross::Api::List*);
            Kross::Api::Object* setDescription(Kross::Api::List*);

            Kross::Api::Object* length(Kross::Api::List*);
            Kross::Api::Object* setLength(Kross::Api::List*);

            Kross::Api::Object* precision(Kross::Api::List*);
            Kross::Api::Object* setPrecision(Kross::Api::List*);

            Kross::Api::Object* width(Kross::Api::List*);
            Kross::Api::Object* setWidth(Kross::Api::List*);

            Kross::Api::Object* defaultValue(Kross::Api::List*);
            Kross::Api::Object* setDefaultValue(Kross::Api::List*);

    };

}}

#endif

