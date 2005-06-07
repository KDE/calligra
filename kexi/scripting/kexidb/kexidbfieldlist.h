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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBFIELDLIST_H
#define KROSS_KEXIDB_KEXIDBFIELDLIST_H

#include <qstring.h>

#include "../api/object.h"
//#include "../api/variant.h"
#include "../api/list.h"
#include "../api/class.h"

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

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            ::KexiDB::FieldList* fieldlist() { return m_fieldlist; }

        private:
            ::KexiDB::FieldList* m_fieldlist;

            Kross::Api::Object::Ptr fieldCount(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr field(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr fields(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr hasField(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr names(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr addField(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr insertField(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr removeField(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr clear(Kross::Api::List::Ptr);
    };

}}

#endif

