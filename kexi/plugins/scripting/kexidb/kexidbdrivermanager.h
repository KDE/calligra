/***************************************************************************
 * kexidbdrivermanager.h
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

#ifndef KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H
#define KROSS_KEXIDB_KEXIDBDRIVERMANAGER_H

#include <qstring.h>

#include <kexidb/drivermanager.h>

namespace Kross { namespace KexiDB {

    // Forward declarations.
    class KexiDBDriver;
    class KexiDBConnectionData;
    class KexiDBField;
    class KexiDBTableSchema;
    class KexiDBQuerySchema;

    /**
     * The drivermanager is the base class to access KexiDBDriver objects and provides
     * common functionality to deal with the KexiDB module.
     *
     * Example (in Python) ;
     * @code
     * # Import the kexidb module.
     * import krosskexidb
     * # Get the drivermanager.
     * drivermanager = krosskexidb.DriverManager()
     * # Let's determinate the mimetype (e.g. "application/x-sqlite3").
     * mimetype = drivermanager.mimeForFile("/home/user/mykexidbfile.kexi")
     * # Now we use that mimetype to get the name of the driver to handle that file (e.g. "SQLite3")
     * drivername = drivermanager.lookupByMime(mimetype)
     * # We are able to create the driver now.
     * driver = drivermanager.driver(drivername)
     * @endcode
     */
    class KexiDBDriverManager : public QObject
    {
        public:
            KexiDBDriverManager(QObject* parent = 0);
            virtual ~KexiDBDriverManager();

#if 0
        private:

            /** Returns a list with avaible drivernames. */
            const QStringList driverNames();

            /** Return the to the defined drivername matching KexiDBDriver object. */
            KexiDBDriver* driver(const QString& drivername);

            /** Return the to the defined mimetype-string matching drivername. */
            const QString lookupByMime(const QString& mimetype);

            /** Return the matching mimetype for the defined file. */
            const QString mimeForFile(const QString& filename);

            /** Return a new KexiDBConnectionData object. */
            KexiDBConnectionData* createConnectionData();

            /** Create and return a KexiDBConnectionData object. Fill the content of the
            KexiDBConnectionData object with the defined file as. The file could be e.g.
            a *.kexi file or a *.kexis file. */
            KexiDBConnectionData* createConnectionDataByFile(const QString& filename);

            /** Return a new KexiDBField object. */
            KexiDBField* field();

            /** Return a new KexiDBTableSchema object. */
            KexiDBTableSchema* tableSchema(const QString& tablename);

            /** Return a new KexiDBQuerySchema object. */
            KexiDBQuerySchema* querySchema();

        private:
            inline ::KexiDB::DriverManager& driverManager();
            ::KexiDB::DriverManager m_drivermanager;
#endif
    };

}}

#endif

