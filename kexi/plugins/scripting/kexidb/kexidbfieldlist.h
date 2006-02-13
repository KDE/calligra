/***************************************************************************
 * kexidbfieldlist.h
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

#ifndef KROSS_KEXIDB_KEXIDBFIELDLIST_H
#define KROSS_KEXIDB_KEXIDBFIELDLIST_H

#include <qstring.h>

#include <api/object.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/drivermanager.h>
#include <kexidb/fieldlist.h>

namespace Kross { namespace KexiDB {

    class KexiDBFieldList : public Kross::Api::Class<KexiDBFieldList>
    {
        public:

            /**
             * Constructor.
             */
            KexiDBFieldList(::KexiDB::FieldList* fieldlist);

            /**
             * Destructor.
             */
            virtual ~KexiDBFieldList();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            ::KexiDB::FieldList* fieldlist() { return m_fieldlist; }

        private:
            ::KexiDB::FieldList* m_fieldlist;

            /// \return the number of fields.
            Kross::Api::Object::Ptr fieldCount(Kross::Api::List::Ptr);
            /// \return the field specified by the number passed as an argument.
            Kross::Api::Object::Ptr field(Kross::Api::List::Ptr);
            /// \return a list of all fields.
            Kross::Api::Object::Ptr fields(Kross::Api::List::Ptr);
            /** \return true if the KexiDBField object passed as an argument is 
            in the field list. */
            Kross::Api::Object::Ptr hasField(Kross::Api::List::Ptr);
            /// \return a list of field names.
            Kross::Api::Object::Ptr names(Kross::Api::List::Ptr);

            /// Adds the KexiDBField object passed as an argument to the field list.
            Kross::Api::Object::Ptr addField(Kross::Api::List::Ptr);
            /** Inserts the KexiDBField object passed as the second argument
            into the field list at the position defined by the first argument. */
            Kross::Api::Object::Ptr insertField(Kross::Api::List::Ptr);
            /** Removes the KexiDBField object passed as an argument from the 
            field list. */
            Kross::Api::Object::Ptr removeField(Kross::Api::List::Ptr);
            /// Removes all KexiDBField objects from the fieldlist.
            Kross::Api::Object::Ptr clear(Kross::Api::List::Ptr);
            /// Set the fieldlist to the as argument passed list of fields.
            Kross::Api::Object::Ptr setFields(Kross::Api::List::Ptr);

            /// Creates and returns list that contain fields selected by name.
            Kross::Api::Object::Ptr subList(Kross::Api::List::Ptr);
    };

}}

#endif

