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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
		ParserError();
		ParserError(const QString &type, const QString &error, const QString &hint, int at);
		~ParserError();

		QString	type() { return m_type; }
		QString	error() { return m_error; }
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
		enum OPCode
		{
			OP_None = 0,	// no statement parsed or reseted
			OP_Error,	// error while parsing
			OP_CreateTable,
			OP_AlterTable,
			OP_Select,
			OP_Insert,
			OP_Update,
			OP_Delete
		};

		/**
		 * constructs an empty object of the parser
		 * @param db is used for things like wildcard resolution. If 0 parser works in "pure mode"
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

		QString statement() const;


		/**
		 * @internal
		 * sets the operation (only parser will need to call this)
		 */
		void setOperation(OPCode op);

		/**
		 * @internal
		 * creates a new table (only parser will need to call this)
		 */
		void createTable(const char *t);

		/**
		 * @internal
		 * sets \a query schema object (only parser will need to call this)
		 */
//todo: other query types
		void setQuerySchema(QuerySchema *query);

		/**
		 * @internal
		 * \return query schema
		 */
		QuerySchema *select() const;

		/**
		 * @internal
		 * INTERNAL use only: sets a error
		 */
		void setError(const ParserError &err);

		bool isReservedKeyword(const char *str);

	protected:
		void init();

		ParserError m_error;
		ParserPrivate *d;
};

}

#endif

