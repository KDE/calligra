/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>

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

namespace KexiDB
{

class Connection;
class QuerySchema;
class TableSchema;

/**
 * parser for sql statements
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
		 * @param db is used for things like wildcard resolvation, if 0 parser wroks in "pure mode"
		 */
		Parser(Connection *connection);
		~Parser();

		/**
		 * clears previous results and runs the parser
		 */
		void		parse(const QString &statement);

		/**
		 * rests results
		 */
		void		clear();

		/**
		 * @returns the resulting operation or OP_Error if faild
		 */
		OPCode		operation() { return m_operation; }

		/**
		 * @returns a pointer to a KexiDBTable on CREATE TABLE or 0 on any other operation or error
		 */
		TableSchema	*table() { return m_table; }

		/**
		 * @returns a pointer to KexiDBSelect if 'SELECT ...' was called or 0 on any other operation or error
		 */
		QuerySchema	*select() { return m_select; }

		/**
		 * @returns a pointer to the used database connection or 0 if not set
		 */
		Connection	*db() { return m_db; }


		/**
		 * sets the operation (only parser will need to call that)
		 */
		void		setOperation(OPCode op) { m_operation = op; }

		/**
		 * creates a new table (only parser will need to call that)
		 */
		void		createTable(const char *t);

		/**
		 * creates a new select object (only parser will need to call that)
		 */
		void		createSelect();


	private:
		OPCode		m_operation;
//		KexiDBTable	*m_table;
		TableSchema	*m_table;
		QuerySchema	*m_select;
		Connection	*m_db;
};

}

#endif

