/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef MYSQLPREPAREDSTATEMENT_H
#define MYSQLPREPAREDSTATEMENT_H
#include <kexidb/preparedstatement.h>
#include <kexidb/connection_p.h>

namespace KexiDB 
{
class MySQLPreparedStatement : public PreparedStatement
{
	public:
		MySQLPreparedStatement(StatementType type, ConnectionInternal& conn, TableSchema& tableSchema);

		virtual ~MySQLPreparedStatement();

		virtual bool execute();
		bool m_resetRequired : 1;

	private:
		Connection* m_conn;
};
}
#endif
