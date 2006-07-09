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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

            ::KexiDB::Field* field() { return m_field; }

        private:
            ::KexiDB::Field* m_field;

            /// Returns the type string for this field, e.g. "Integer" for Integer type.
            Kross::Api::Object::Ptr type(Kross::Api::List::Ptr);
            /// Sets the type string for this field, e.g. "Integer" for Integer type.
            Kross::Api::Object::Ptr setType(Kross::Api::List::Ptr);

            /// Returns the QVariant::typeName which is equivalent to the type this field has.
            Kross::Api::Object::Ptr variantType(Kross::Api::List::Ptr);
            /// Set the type this field has according to the defined QVariant::typeName.
            Kross::Api::Object::Ptr setVariantType(Kross::Api::List::Ptr);

            /** Returns the optional subtype for this field. Subtype is a string 
            providing additional hint for field's type. E.g. for BLOB type, it 
            can be a MIME type or certain QVariant type name, for 
            example: "QPixmap", "QColor" or "QFont". */
            Kross::Api::Object::Ptr subType(Kross::Api::List::Ptr);
            /// Sets the optional subtype for this field.
            Kross::Api::Object::Ptr setSubType(Kross::Api::List::Ptr);

            /** Returns type group string for this field, e.g. "IntegerGroup" 
            for IntegerGroup type. */
            Kross::Api::Object::Ptr typeGroup(Kross::Api::List::Ptr);

            /// \return true if the field is autoincrement (e.g. integer/numeric).
            Kross::Api::Object::Ptr isAutoInc(Kross::Api::List::Ptr);
            /// Sets auto increment flag.
            Kross::Api::Object::Ptr setAutoInc(Kross::Api::List::Ptr);

            /// \return true if the field is member of single-field unique key.
            Kross::Api::Object::Ptr isUniqueKey(Kross::Api::List::Ptr);
            /** Specifies whether the field has single-field unique constraint 
            or not. */
            Kross::Api::Object::Ptr setUniqueKey(Kross::Api::List::Ptr);

            /// \return true if the field is member of single-field primary key.
            Kross::Api::Object::Ptr isPrimaryKey(Kross::Api::List::Ptr);
            /// Specifies whether the field is single-field primary key or not.
            Kross::Api::Object::Ptr setPrimaryKey(Kross::Api::List::Ptr);

            /// \return true if the field is member of single-field foreign key.
            Kross::Api::Object::Ptr isForeignKey(Kross::Api::List::Ptr);
            /// Sets whether the field has to be declared with single-field foreign key.
            Kross::Api::Object::Ptr setForeignKey(Kross::Api::List::Ptr);

            /// \return true if the field is not allowed to be null.
            Kross::Api::Object::Ptr isNotNull(Kross::Api::List::Ptr);
            /// Specifies whether the field has single-field unique constraint or not.
            Kross::Api::Object::Ptr setNotNull(Kross::Api::List::Ptr);

            /// \return true if the field is not allowed to be empty.
            Kross::Api::Object::Ptr isNotEmpty(Kross::Api::List::Ptr);
            /// Specifies whether the field has single-field unique constraint or not.
            Kross::Api::Object::Ptr setNotEmpty(Kross::Api::List::Ptr);

            /// \return true if the field is indexed using single-field database index.
            Kross::Api::Object::Ptr isIndexed(Kross::Api::List::Ptr);
            /// Specifies whether the field is indexed or not.
            Kross::Api::Object::Ptr setIndexed(Kross::Api::List::Ptr);

            /// \return true if the field is an unsigned integer.
            Kross::Api::Object::Ptr isUnsigned(Kross::Api::List::Ptr);
            /// Specifies whether the field is an unsigned integer or not.
            Kross::Api::Object::Ptr setUnsigned(Kross::Api::List::Ptr);

            /// \return the name of this field.
            Kross::Api::Object::Ptr name(Kross::Api::List::Ptr);
            /// Sets the name of this field.
            Kross::Api::Object::Ptr setName(Kross::Api::List::Ptr);

            /// \return the caption of this field.
            Kross::Api::Object::Ptr caption(Kross::Api::List::Ptr);
            /// Sets the caption of this field.
            Kross::Api::Object::Ptr setCaption(Kross::Api::List::Ptr);

            /// \return the descriptive text for this field.
            Kross::Api::Object::Ptr description(Kross::Api::List::Ptr);
            /// Set the description for this field.
            Kross::Api::Object::Ptr setDescription(Kross::Api::List::Ptr);

            /// \return the length of text if the field type is text.
            Kross::Api::Object::Ptr length(Kross::Api::List::Ptr);
            /** Sets the length for this field. Only works for Text 
            Type (not including LongText). */
            Kross::Api::Object::Ptr setLength(Kross::Api::List::Ptr);

            /** Returns precision for numeric and other fields that have
            both length and precision (floating point types). */
            Kross::Api::Object::Ptr precision(Kross::Api::List::Ptr);
            /// Sets the precision for numeric and other fields.
            Kross::Api::Object::Ptr setPrecision(Kross::Api::List::Ptr);

            /** Returns the width of this field (usually in pixels or points).
            0 (the default) means there is no hint for the width. */
            Kross::Api::Object::Ptr width(Kross::Api::List::Ptr);
            /// Sets the width of this field.
            Kross::Api::Object::Ptr setWidth(Kross::Api::List::Ptr);

            /// \return the default value this field has.
            Kross::Api::Object::Ptr defaultValue(Kross::Api::List::Ptr);
            /// Sets the default value this field has.
            Kross::Api::Object::Ptr setDefaultValue(Kross::Api::List::Ptr);

    };

}}

#endif

