/***************************************************************************
 * kexidbparser.h
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

#ifndef KROSS_KEXIDB_KEXIDBPARSER_H
#define KROSS_KEXIDB_KEXIDBPARSER_H

#include <qstring.h>

#include <api/object.h>
#include <api/list.h>
#include <api/class.h>

#include <kexidb/drivermanager.h>
#include <kexidb/parser/parser.h>

namespace Kross { namespace KexiDB {

    // Forward declaration.
    class KexiDBConnection;

    class KexiDBParser : public Kross::Api::Class<KexiDBParser>
    {
        public:

            /**
             * Constructor.
             */
            KexiDBParser(KexiDBConnection* connection, ::KexiDB::Parser* parser);

            /**
             * Destructor.
             */
            virtual ~KexiDBParser();

            /// See \see Kross::Api::Object::getClassName
            virtual const QString getClassName() const;

        private:
            KexiDBConnection* m_connection;
            ::KexiDB::Parser* m_parser;

            /** Clears previous results and runs the parser on the 
            SQL statement passed as an argument. */
            Kross::Api::Object::Ptr parse(Kross::Api::List::Ptr);
            /// Clears results.
            Kross::Api::Object::Ptr clear(Kross::Api::List::Ptr);
            /// \return the resulting operation.
            Kross::Api::Object::Ptr operation(Kross::Api::List::Ptr);

            /// \return the \a KexiDBTableSchema object on a CREATE TABLE operation.
            Kross::Api::Object::Ptr table(Kross::Api::List::Ptr);
            /// \return the \a KexiDBQuerySchema object on a SELECT operation."
            Kross::Api::Object::Ptr query(Kross::Api::List::Ptr);
            /// \return the \a KexiDBConnection object pointing to the used database connection.
            Kross::Api::Object::Ptr connection(Kross::Api::List::Ptr);
            /// \return the SQL query statement.
            Kross::Api::Object::Ptr statement(Kross::Api::List::Ptr);

            /// \return the type string of the last error.
            Kross::Api::Object::Ptr errorType(Kross::Api::List::Ptr);
            /// \return the message of the last error.
            Kross::Api::Object::Ptr errorMsg(Kross::Api::List::Ptr);
            /// \return the position where the last error occurred.
            Kross::Api::Object::Ptr errorAt(Kross::Api::List::Ptr);
    };

}}

#endif

