/***************************************************************************
 * kexidbconnection.h
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

#ifndef SCRIPTING_KEXIDBCONNECTION_H
#define SCRIPTING_KEXIDBCONNECTION_H

#include <qstring.h>
#include <qobject.h>
#include <qpointer.h>

#include <kexidb/connection.h>

namespace Scripting {

    // Forward declarations.
    class KexiDBDriver;
    class KexiDBConnectionData;
    class KexiDBCursor;
    class KexiDBTableSchema;
    class KexiDBQuerySchema;
    class KexiDBParser;

    /**
     * A connection to a database.
     *
     * Example (in Python) ;
     * @code
     * # Import the kexidb module.
     * import krosskexidb
     * # Get the drivermanager.
     * drivermanager = krosskexidb.DriverManager()
     * # We need a connectiondata object.
     * connectiondata = drivermanager.createConnectionData()
     * # Fill the new connectiondata object with what we need to connect.
     * connectiondata.setFileName("/home/user/kexisqlite3file.kexi")
     * # Create the database-driver to access the SQLite3 backend.
     * driver = drivermanager.driver("SQLite3")
     * # Create the connection now.
     * connection = driver.createConnection(connectiondata)
     * # Establish the connection.
     * if not connection.connect(): raise("Failed to connect with db")
     * # Open database for usage. The filebased driver uses the filename as databasename.
     * if not connection.useDatabase("/home/user/kexisqlite3file.kexi"): raise("Failed to use db")
     * @endcode
     */
    class KexiDBConnection : public QObject
    {
            Q_OBJECT
        public:
            KexiDBConnection(::KexiDB::Connection* connection, KexiDBDriver* driver = 0, KexiDBConnectionData* connectiondata = 0);
            virtual ~KexiDBConnection();

        public slots:

            /** Return true if there was an error during last operation on the database. */
            bool hadError() const;
            /** Return the last errormessage. */
            const QString lastError() const;

            /** Return the \a KexiDBConnectionData object used to create this connection. */
            QObject* data();
            /** Return the \a KexiDBDriver object this connection belongs too. */
            QObject* driver();

            /** Try to connect and return true if we are successfully connected now. */
            bool connect();
            /** Return true if we are connected. */
            bool isConnected();
            /** Disconnect and return true if we are successfully disconnected now. */
            bool disconnect();

            /** Returns true if the connection is read-only. */
            bool isReadOnly() const;

            /** Return true if the as argument passed databasename exists. */
            bool databaseExists(const QString& dbname);
            /** Return the name of currently used database for this connection or empty
            string if there is no used database. */
            const QString currentDatabase() const;
            /** Return list of database names for opened connection. */
            const QStringList databaseNames() const;
            /** Return true if connection is properly established. */
            bool isDatabaseUsed() const;
            /** Opens an existing database specified by the as argument passed databasename
            and returns true if the database is used now. */
            bool useDatabase(const QString& dbname);
            /** Closes currently used database for this connection. */
            bool closeDatabase();

            /** Return names of all table schemas stored in currently used database include the
            internal KexiDB system table names (kexi__*) */
            const QStringList allTableNames() const;
            /** Return names of all table schemas without the internal KexiDB system table names (kexi__*) */
            const QStringList tableNames() const;
            /** Return names of all query schemas stored in currently used database. */
            const QStringList queryNames() const;

            /** Executes query described by the as argument passed sqlstatement-string. Returns the
            opened cursor \a KexiDBCursor instance created for results of this query or NULL on error. */
            QObject* executeQueryString(const QString& sqlquery);
            /** Executes query described by the as argument passed KexiDBQuerySchema object. Returns the
            opened cursor \a KexiDBCursor instance created for results of this query or NULL on error. */
            QObject* executeQuerySchema(KexiDBQuerySchema* queryschema);
#if 0
            /** Inserts the as argument passed KexiDBField object. */
            Kross::Api::Object::Ptr insertRecord(Kross::Api::List::Ptr);
#endif
            /** Creates new database with the as argument passed databasename. */
            bool createDatabase(const QString& dbname);
            /** Drops the as argument passed databasename. */
            bool dropDatabase(const QString& dbname);

            /** Creates table defined by the as argument passed KexiTableSchema object. */
            bool createTable(KexiDBTableSchema* tableschema);
            /** Drops table defined by the as argument passed KexiDBTableSchema object. */
            bool dropTable(const QString& tablename);
            /** Alters the as first argument passed KexiDBTableSchema object using the as
            second argument passed KexiDBTableSchema. */
            bool alterTable(KexiDBTableSchema* fromschema, KexiDBTableSchema* toschema);
            /** Alters the tablename of the as first argument passed KexiDBTableSchema into
            the as second argument passed new tablename. */
            bool alterTableName(KexiDBTableSchema* tableschema, const QString& newtablename);

            /** Returns the \a KexiDBTableSchema object of the table matching to the as argument
            passed tablename. */
            QObject* tableSchema(const QString& tablename);
            /** Returns true if there is at least one valid record in the as argument passed tablename. */
            bool isEmptyTable(KexiDBTableSchema* tableschema) const;
            /** Returns the \a KexiDBQuerySchema object of the query matching to the as argument passed queryname. */
            QObject* querySchema(const QString& queryname);

            /** Return true if the \"auto commit\" option is on. */
            bool autoCommit() const;
            /** Set the auto commit option. This does not affect currently started transactions and can
            be changed even when connection is not established. */
            bool setAutoCommit(bool enabled);

#if 0
            /** Creates new transaction handle and starts a new transaction. */
            KexiDBTransaction* beginTransaction();
            /** Commits the as rgument passed KexiDBTransaction object. */
            bool commitTransaction(KexiDBTransaction* transaction);
            /** Rollback the as rgument passed KexiDBTransaction object. */
            bool rollbackTransaction(KexiDBTransaction* transaction);
            /** Return the KEXIDBTransaction object for default transaction for this connection. */
            KexiDBTransaction* defaultTransaction();
            /** Sets default transaction that will be used as context for operations on data in opened
            database for this connection. */
            void setDefaultTransaction(KexiDBTransaction* transaction);
            /** Returns list of currently active KexiDBTransaction objects. */
            Kross::Api::List* transactions();
            /** Return true if the transaction is active (ie. started). */
            //bool isTransactionActive() const { m_transaction.active(); }
            /** Return true if the transaction is uninitialized (null). */
            //bool isTransactionNull() const { m_transaction.isNull(); }
#endif

            /** Return a \a KexiDBParser object. */
            QObject* parser();

        private:
            ::KexiDB::Connection* m_connection;
            QPointer<KexiDBConnectionData> m_connectiondata;
            QPointer<KexiDBDriver> m_driver;
    };

}

#endif

