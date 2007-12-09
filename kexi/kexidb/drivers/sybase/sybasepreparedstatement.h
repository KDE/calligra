/* This file is part of the KDE project
   Copyright (C) 2006 Sharan Rao <sharanrao@gmail.com>

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

#ifndef SYBASEPREPAREDSTATEMENT_H
#define SYBASEPREPAREDSTATEMENT_H

#include <kexidb/preparedstatement.h>
#include <kexidb/connection_p.h>

namespace KexiDB 
{

class SybasePreparedStatement : public PreparedStatement
{
	public:
		SybasePreparedStatement(StatementType type, ConnectionInternal& conn, FieldList& fields);

		virtual ~SybasePreparedStatement();

		virtual bool execute();

		QByteArray m_tempStatementString;

		bool m_resetRequired : 1;

        private:
                Connection* m_conn;

};
}
#endif
