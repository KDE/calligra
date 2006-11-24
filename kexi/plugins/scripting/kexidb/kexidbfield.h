/***************************************************************************
 * kexidbfield.h
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef SCRIPTING_KEXIDBFIELD_H
#define SCRIPTING_KEXIDBFIELD_H

#include <qstring.h>
#include <qobject.h>

#include <kexidb/drivermanager.h>
#include <kexidb/field.h>

namespace Scripting {

    /**
    * A field in a record.
    */
    class KexiDBField : public QObject
    {
            Q_OBJECT
        public:
            KexiDBField(QObject* parent, ::KexiDB::Field* field, bool owner);
            virtual ~KexiDBField();
            ::KexiDB::Field* field() { return m_field; }

        public slots:

            /** Returns the type string for this field, e.g. "Integer" for Integer type. */
            const QString type();
            /** Sets the type string for this field, e.g. "Integer" for Integer type. */
            void setType(const QString type);

            /** Returns the optional subtype for this field. Subtype is a string providing
            additional hint for field's type. E.g. for BLOB type, it can be a MIME type or
            certain QVariant type name, for example: "QPixmap", "QColor" or "QFont". */
            const QString subType();
            /** Sets the optional subtype for this field. */
            void setSubType(const QString& subtype);

            /** Returns the QVariant::typeName which is equivalent to the type this field has. */
            const QString variantType();
            /** Returns type group string for this field, e.g. "IntegerGroup" for IntegerGroup type. */
            const QString typeGroup();

            /** Returns true if the field is autoincrement (e.g. integer/numeric). */
            bool isAutoInc();
            /** Sets auto increment flag. */
            void setAutoInc(bool autoinc);

            /** Returns true if the field is member of single-field unique key. */
            bool isUniqueKey();
            /** Specifies whether the field has single-field unique constraint or not. */
            void setUniqueKey(bool unique);

            /** Returns true if the field is member of single-field primary key. */
            bool isPrimaryKey();
            /** Specifies whether the field is single-field primary key or not. */
            void setPrimaryKey(bool primary);

            /** Returns true if the field is member of single-field foreign key. */
            bool isForeignKey();
            /** Sets whether the field has to be declared with single-field foreign key. */
            void setForeignKey(bool foreign);

            /** Returns true if the field is not allowed to be null. */
            bool isNotNull();
            /** Specifies whether the field has single-field unique constraint or not. */
            void setNotNull(bool notnull);

            /** Returns true if the field is not allowed to be empty. */
            bool isNotEmpty();
            /** Specifies whether the field has single-field unique constraint or not. */
            void setNotEmpty(bool notempty);

            /** Returns true if the field is indexed using single-field database index. */
            bool isIndexed();
            /** Specifies whether the field is indexed or not. */
            void setIndexed(bool indexed);

            /** Returns true if the field is an unsigned integer. */
            bool isUnsigned();
            /** Specifies whether the field is an unsigned integer or not. */
            void setUnsigned(bool isunsigned);

            /** Returns the name of this field. */
            const QString name();
            /** Sets the name of this field. */
            void setName(const QString& name);

            /** Returns the caption of this field. */
            const QString caption();
            /** Sets the caption of this field. */
            void setCaption(const QString& caption);

            /** Returns the descriptive text for this field. */
            const QString description();
            /** Set the description for this field. */
            void setDescription(const QString& desc);

            /** Returns the length of text if the field type is text. */
            uint length();
            /** Sets the length for this field. Only works for Text Type (not including LongText). */
            void setLength(uint length);

            /** Returns precision for numeric and other fields that have both length and
            precision (floating point types). */
            uint precision();
            /** Sets the precision for numeric and other fields. */
            void setPrecision(uint precision);

            /** Returns the width of this field (usually in pixels or points).
            0 (the default) means there is no hint for the width. */
            uint width();
            /** Sets the width of this field. */
            void setWidth(uint width);

            /** Returns the default value this field has. */
            QVariant defaultValue();
            /** Sets the default value this field has. */
            void setDefaultValue(const QVariant& defaultvalue);

        private:
            ::KexiDB::Field* m_field;
            bool m_owner;
    };

}

#endif

