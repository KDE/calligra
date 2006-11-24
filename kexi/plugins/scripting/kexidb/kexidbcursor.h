/***************************************************************************
 * kexidbcursor.h
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

#ifndef KROSS_KEXIDB_KEXIDBCURSOR_H
#define KROSS_KEXIDB_KEXIDBCURSOR_H

#include <qstring.h>
#include <qobject.h>

#include <kexidb/cursor.h>
#include <kexidb/roweditbuffer.h>

namespace Kross { namespace KexiDB {

    // Forward declaration.
    class KexiDBConnection;

    /**
     * The cursor provides a control structure for the successive traversal
     * of records in a result set as returned e.g. by a query.
     *
     * Example (in Python) that shows how to iterate over the result of a query;
     * @code
     * # Once we have a KexiDBConnection object we are able to execute a query string and get a cursor as result.
     * cursor = connection.executeQueryString("SELECT * from emp")
     * # Let's check if the query was successfully.
     * if not cursor: raise("Query failed")
     * # Walk through all items in the table.
     * while(not cursor.eof()):
     *     # Iterate over the fields the record has.
     *     for i in range( cursor.fieldCount() ):
     *         # Print some informations.
     *         print "%s %s %s" % (cursor.at(), i, cursor.value(i))
     *     # and move on to the next record.
     *     cursor.moveNext()
     * @endcode
     *
     * Example (in Python) that shows how to use a cursor to strip
     * all whitespaces at the beginning and the end from the values
     * in a table;
     * @code
     * import krosskexidb
     * drivermanager = krosskexidb.DriverManager()
     * connectiondata = drivermanager.createConnectionDataByFile("/home/me/kexiprojectfile.kexi")
     * driver = drivermanager.driver( connectiondata.driverName() )
     * connection = driver.createConnection(connectiondata)
     * if not connection.connect(): raise "Failed to connect"
     * if not connection.useDatabase( connectiondata.databaseName() ):
     *     if not connection.useDatabase( connectiondata.fileName() ):
     *         raise "Failed to use database"
     *
     * table = connection.tableSchema("emp")
     * query = table.query()
     * cursor = connection.executeQuerySchema(query)
     * if not cursor: raise("Query failed")
     * while(not cursor.eof()):
     *     for i in range( cursor.fieldCount() ):
     *         v = str( cursor.value(i) )
     *         if v.startswith(' ') or v.endswith(' '):
     *             cursor.setValue(i, v.strip())
     *     cursor.moveNext()
     * if not cursor.save(): raise "Failed to save changes"
     * @endcode
     */
    class KexiDBCursor : public QObject
    {
            Q_OBJECT
        public:
            KexiDBCursor(QObject* parent, ::KexiDB::Cursor* cursor, bool owner);
            virtual ~KexiDBCursor();

        public slots:

            /** Opens the cursor. */
            bool open();
            /** Returns true if the cursor is opened else false. */
            bool isOpened();
            /** Closes and then opens again the same cursor. */
            bool reopen();
            /** Closes previously opened cursor. */
            bool close();

            /** Moves current position to the first record and retrieves it. */
            bool moveFirst();
            /** Moves current position to the last record and retrieves it. */
            bool moveLast();
            /** Moves current position to the previous record and retrieves it. */
            bool movePrev();
            /** Moves current position to the next record and retrieves it. */
            bool moveNext();

            /** Returns true if current position is before first record. */
            bool bof();
            /** Returns true if current position is after last record. */
            bool eof();

            /** Returns current internal position of the cursor's query. Records
            are numbered from 0; the value -1 means that the cursor does not
            point to a valid record. */
            int at();
            /** Returns the number of fields available for this cursor. */
            uint fieldCount();
            /** Returns the value stored in the passed column number (counting from 0). */
            QVariant value(uint index);
            /** Set the value for the field defined with index. The new value is buffered
            and does not got written as long as save() is not called. */
            bool setValue(uint index, QVariant value);

            /** Save any changes done with setValue(). You should call this only once at
            the end of all value/setValue iterations cause the cursor is closed once
            the changes got saved successfully. */
            bool save();

        private:
            class Record {
                public:
                    ::KexiDB::RowData rowdata;
                    ::KexiDB::RowEditBuffer* buffer;
                    Record(::KexiDB::Cursor* cursor)
                        : buffer( new ::KexiDB::RowEditBuffer(true) )
                    {
                        cursor->storeCurrentRow(rowdata);
                    }
                    ~Record()
                    {
                        delete buffer;
                    }
            };
            QMap<Q_LLONG, Record*> m_modifiedrecords;
            void clearBuffers();

            ::KexiDB::Cursor* m_cursor;
            bool m_owner;
    };

}}

#endif

