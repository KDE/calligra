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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H
#define KROSS_KEXIDB_KEXIDBCONNECTIONDATA_H

#include <qstring.h>

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

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

            operator ::KexiDB::ConnectionData& () { return *m_data; }
            operator ::KexiDB::ConnectionData* () { return m_data; }

            /**
             * Destructor.
             */
            virtual ~KexiDBConnectionData();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            ::KexiDB::ConnectionData* data() { return m_data; }
            QString databaseName() { return m_dbname; }
            void setDatabaseName(const QString& s) { m_dbname = s; }

        private:
            ::KexiDB::ConnectionData* m_data;
            QString m_dbname;

            /// \return connection name.
            Kross::Api::Object::Ptr caption(Kross::Api::List::Ptr);
            /// Set connection name.
            Kross::Api::Object::Ptr setCaption(Kross::Api::List::Ptr);

            /// \return the description.
            Kross::Api::Object::Ptr description(Kross::Api::List::Ptr);
            /// Set the description.
            Kross::Api::Object::Ptr setDescription(Kross::Api::List::Ptr);

            /// \return drivername.
            Kross::Api::Object::Ptr driverName(Kross::Api::List::Ptr);
            /// Set the drivername.
            Kross::Api::Object::Ptr setDriverName(Kross::Api::List::Ptr);

            /// \return true if a local socket file is used else false.
            Kross::Api::Object::Ptr localSocketFileUsed(Kross::Api::List::Ptr);
            /// Set if the local socket file should be used.
            Kross::Api::Object::Ptr setLocalSocketFileUsed(Kross::Api::List::Ptr);
            /// \return local socket filename.
            Kross::Api::Object::Ptr localSocketFileName(Kross::Api::List::Ptr);
            /// Set the local socket filename.
            Kross::Api::Object::Ptr setLocalSocketFileName(Kross::Api::List::Ptr);

            // For serverbased drivers
            /// \return the database name.
            Kross::Api::Object::Ptr databaseName(Kross::Api::List::Ptr);
            /// Set the database name.
            Kross::Api::Object::Ptr setDatabaseName(Kross::Api::List::Ptr);

            /// \return hostname.
            Kross::Api::Object::Ptr hostName(Kross::Api::List::Ptr);
            /// Set the hostname.
            Kross::Api::Object::Ptr setHostName(Kross::Api::List::Ptr);

            /// \return port
            Kross::Api::Object::Ptr port(Kross::Api::List::Ptr);
            /// Set port.
            Kross::Api::Object::Ptr setPort(Kross::Api::List::Ptr);

            /// \return password.
            Kross::Api::Object::Ptr password(Kross::Api::List::Ptr);
            /// Set password.
            Kross::Api::Object::Ptr setPassword(Kross::Api::List::Ptr);

            /// \return username.
            Kross::Api::Object::Ptr userName(Kross::Api::List::Ptr);
            /// Set username.
            Kross::Api::Object::Ptr setUserName(Kross::Api::List::Ptr);

            // For filebased drivers

            /// \return filename.
            Kross::Api::Object::Ptr fileName(Kross::Api::List::Ptr);
            /// Set filename.
            Kross::Api::Object::Ptr setFileName(Kross::Api::List::Ptr);

            /// \return database path.
            Kross::Api::Object::Ptr dbPath(Kross::Api::List::Ptr);
            /// \return database filename.
            Kross::Api::Object::Ptr dbFileName(Kross::Api::List::Ptr);

            /// Return a user-friendly string representation.
            Kross::Api::Object::Ptr serverInfoString(Kross::Api::List::Ptr);
    };

}}

#endif

