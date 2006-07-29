/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDBPARSER_H
#define KEXIDBPARSER_H

#include <qobject.h>
#include <qptrlist.h>

#include <kexidb/field.h>
#include <kexidb/expression.h>

namespace KexiDB
{

class Connection;
class QuerySchema;
class TableSchema;
class Field;

/**
 * class which contains detailed i18n'ed error description
 */
class KEXI_DB_EXPORT ParserError
{
	public:

                /**
		 * Empty constructor.
		 */
		ParserError();

                /**
		 * Constructor.
		 *
		 * \param type The errortype.
		 * \param error A description of the error.
		 * \param hint Token where the error happend.
		 * \param at The position where the error happend.
		 */
		ParserError(const QString &type, const QString &error, const QString &hint, int at);

                /**
		 * Destructor.
		 */
		~ParserError();

                /**
		 * \return the errortype.
		 */
		QString	type() { return m_type; }

                /**
		 * \return a descriping error message.
		 */
		QString	error() { return m_error; }

                /**
		 * \return position where the error happend.
		 */
		int	at() { return m_at; }

	private:
		QString m_type;
		QString m_error;
		QString m_hint;
		int m_at;
//		bool	m_isNull;
};

class ParserPrivate;

/**
 * Parser for sql statements.
 */
class KEXI_DB_EXPORT Parser
{
	public:

                /**
                 * The operation-code of the statement.
                 */
		enum OPCode
		{
			OP_None = 0,	/// No statement parsed or reseted.
			OP_Error,	/// Error while parsing.
			OP_CreateTable, /// Create a table.
			OP_AlterTable,  /// Alter an existing table
			OP_Select,      /// Query-statement.
			OP_Insert,      /// Insert new content.
			OP_Update,      /// Update existing content.
			OP_Delete       /// Delete existing content.
		};

		/**
		 * constructs an empty object of the parser
		 * \param connection is used for things like wildcard resolution. If 0 parser works in "pure mode"
		 */
		Parser(Connection *connection);
		~Parser();

		/**
		 * clears previous results and runs the parser
		 */
		bool parse(const QString &statement);

		/**
		 * rests results
		 */
		void clear();

		/**
		 * \return the resulting operation or OP_Error if failed
		 */
		OPCode operation() const;

                /**
                 * \return the resulting operation as string.
                 */
                QString operationString() const;

		/**
		 * \return a pointer to a KexiDBTable on CREATE TABLE
		 * or 0 on any other operation or error. Returned object is owned by you.
		 * You can call this method only once every time after doing parse().
		 * Next time, the call will return 0.
		 */
		TableSchema *table();

		/**
		 * \return a pointer to KexiDBSelect if 'SELECT ...' was called
		 * or 0 on any other operation or error. Returned object is owned by you.
		 * You can call this method only once every time after doing parse().
		 * Next time, the call will return 0.
		 */
		QuerySchema *query();

		/**
		 * \return a pointer to the used database connection or 0 if not set
		 * You can call this method only once every time after doing parse().
		 * Next time, the call will return 0.
		 */
		Connection	*db() const;

		/**
		 * returns detailed information about last error.
		 * If no error occured ParserError isNull()
		 */
		ParserError error() const;

		/**
                 * \return the statement passed on the last \a parse method-call.
                 */
		QString statement() const;

		/**
		 * \internal
		 * sets the operation (only parser will need to call this)
		 */
		void setOperation(OPCode op);

		/**
		 * \internal
		 * creates a new table (only parser will need to call this)
		 */
		void createTable(const char *t);

		/**
		 * \internal
		 * sets \a query schema object (only parser will need to call this)
		 */
//todo: other query types
		void setQuerySchema(QuerySchema *query);

		/**
		 * \internal
		 * \return query schema
		 */
		QuerySchema *select() const;

		/**
		 * \internal
		 * INTERNAL use only: sets a error
		 */
		void setError(const ParserError &err);

                /**
                 * \a return true if the \param str is an reserved
                 * keyword (see tokens.cpp for a list of reserved
                 * keywords).
                 */
		bool isReservedKeyword(const char *str);

	protected:
		void init();

		ParserError m_error;
		ParserPrivate *d;
};

}

#endif

