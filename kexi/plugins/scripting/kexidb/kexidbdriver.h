/***************************************************************************
 * kexidbdriver.h
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

#ifndef KROSS_KEXIDB_KEXIDBDRIVER_H
#define KROSS_KEXIDB_KEXIDBDRIVER_H

#include <qstring.h>
//#include <qguardedptr.h>

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/driver.h>

namespace Kross { namespace KexiDB {

    /**
     * Class to handle \a ::KexiDB::Driver instances. Such
     * drivers are managed by the \a KexiDBDriverManager.
     */
    class KexiDBDriver : public Kross::Api::Class<KexiDBDriver>
    {
        public:

            /**
             * Constructor.
             *
             * \param driver The \a ::KexiDB::Driver instance
             *        this driver wraps.
             */
            KexiDBDriver(::KexiDB::Driver* driver);

            /**
             * Destructor.
             */
            virtual ~KexiDBDriver();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// Return true if this driver is valid else false.
            bool isValid();
            /// The drivers major versionnumber.
            int versionMajor();
            /// The drivers minor versionnumber.
            int versionMinor();
            /// Driver-specific SQL string escaping. For example the " or ' char may
            /// need to be escaped for values used within SQL-statements.
            QString escapeString(const QString& s);
            /// Returns true if this driver is file-based.
            bool isFileDriver();
            /// Return a name of MIME type of files handled by this driver if it is a
            /// file-based database's driver otherwise returns null string.
            QString fileDBDriverMimeType();
            /// Returns true if the passed string is a system object's name, eg. name
            /// of build-in system table that cannot be used or created by a user.
            bool isSystemObjectName(const QString& name);
            /// Returns true if the passed string is a system database's name, eg. name
            /// of build-in, system database that cannot be used or created by a user.
            bool isSystemDatabaseName(const QString& name);
            /// Returns true if the passed string is a system field's name, build-in
            /// system field that cannot be used or created by a user.
            bool isSystemFieldName(const QString& name);
            /// The as second argument passed string got escaped to be usuable within
            /// a SQL-statement and those escaped string got returned by the method.
            /// The first argument defines the fieldtype to what we should escape the
            /// second argument to.
            QString valueToSQL(const QString& fieldtype, const QVariant& value);
            /// Create a new KexiDBConnection object and return it.
            KexiDBConnection* createConnection(KexiDBConnectionData* data);
            /// Return a list of KexiDBConnection objects.
            Q3PtrList< ::KexiDB::Connection > connectionsList();

        private:
            ::KexiDB::Driver* m_driver;
    };

}}

#endif

