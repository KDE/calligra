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

#include <api/object.h>
#include <api/variant.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/cursor.h>

namespace Kross { namespace KexiDB {

    // Forward declaration.
    class KexiDBConnection;

    /**
     * The KexiDBCursor class is a wrapper around the
     * \a ::KexiDB::Cursor class to provide database
     * cursor functionality.
     */
    class KexiDBCursor : public Kross::Api::Class<KexiDBCursor>
    {
        public:

            /**
             * Constructor.
             *
             * \param cursor The \a ::KexiDB::Cursor this class
             *        wraps.
             */
            KexiDBCursor(::KexiDB::Cursor* cursor);

            /**
             * Destructor.
             */
            virtual ~KexiDBCursor();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

            /// Opens the cursor.
            bool open();
            /// Returns true if the cursor is opened else false.
            bool isOpened();
            /// Closes and then opens again the same cursor.
            bool reopen();
            /// Closes previously opened cursor.
            bool close();
            /// Moves current position to the first record and retrieves it.
            bool moveFirst();
            /// Moves current position to the last record and retrieves it.
            bool moveLast();
            /// Moves current position to the previous record and retrieves it.
            bool movePrev();
            /// Moves current position to the next record and retrieves it.
            bool moveNext();
            /// Returns true if current position is before first record.
            bool bof();
            /// Returns true if current position is after last record.
            bool eof();
            /// Returns current internal position of the cursor's query. Records 
            /// are numbered from 0; the value -1 means that the cursor does not 
            /// point to a valid record.
            Q_LLONG at();
            /// Returns the number of fields available for this cursor.
            uint fieldCount();
            /// Returns the value stored in the passed column number (counting from 0).
            QVariant value(uint index);

        private:
            ::KexiDB::Cursor* m_cursor;
    };

}}

#endif

