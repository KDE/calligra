/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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

#ifndef ORACLEPREPAREDSTATEMENT_H
#define ORACLEPREPAREDSTATEMENT_H

#include <kexidb/preparedstatement.h>
#include "oracleconnection_p.h"

//todo 1.1 - unfinished: #define KEXI_USE_MYSQL_STMT
// for 1.0 we're using unoptimized version

namespace KexiDB 
{

/*! Implementation of prepared statements for MySQL driver. */
class OraclePreparedStatement : public PreparedStatement, public OracleConnectionInternal
{
	public:
		OraclePreparedStatement(StatementType type, ConnectionInternal& conn, FieldList& fields);

		virtual ~OraclePreparedStatement();

		virtual bool execute();

		QByteArray m_tempStatementString;

		bool m_resetRequired : 1;
private:
		Connection* m_conn;

	/*
		protected:
		bool init();
		void done();
	*/
};
}
#endif
