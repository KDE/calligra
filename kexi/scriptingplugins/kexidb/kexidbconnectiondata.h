/***************************************************************************
 * kexidbconnectiondata.h
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

#ifndef KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H
#define KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H

#include <qstring.h>

#include <scripting/api/object.h>
#include <scripting/api/variant.h>
#include <scripting/api/list.h>
#include <scripting/api/class.h>

#include <kexidb/connection.h>
#include <kexidb/connectiondata.h>

namespace Kross { namespace KexiDB {

    /**
     * The KexiDBConnection class handles \a ::KexiDB::Connection
     * instances and wraps parts of the functionality.
     */
    class KexiDBConnectionData : public Kross::Api::Class<KexiDBConnectionData>
    {
        public:

            /**
             * Constructor.
             *
             * \param data The \a ::KexiDB::ConnectionData instance
             *        this class wraps.
             */
            KexiDBConnectionData(::KexiDB::ConnectionData* data);

            /**
             * Destructor.
             */
            virtual ~KexiDBConnectionData();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// See \see Kross::Api::Object::getDescription
            virtual const QString getDescription() const;

            ::KexiDB::ConnectionData* getConnectionData() const;

        private:
            ::KexiDB::ConnectionData* m_data;

            Kross::Api::Object::Ptr caption(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setCaption(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr description(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setDescription(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr driverName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setDriverName(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr localSocketFileName(Kross::Api::List::Ptr);

            // For serverbased drivers
            Kross::Api::Object::Ptr hostName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setHostName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr port(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setPort(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr password(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setPassword(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr userName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setUserName(Kross::Api::List::Ptr);

            // For filebased drivers
            Kross::Api::Object::Ptr fileName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr setFileName(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr dbPath(Kross::Api::List::Ptr);
            Kross::Api::Object::Ptr dbFileName(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr serverInfoString(Kross::Api::List::Ptr);
    };

}}

#endif

