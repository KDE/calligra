/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_PREPAREDSTATEMENT_H
#define KEXIDB_PREPAREDSTATEMENT_H

#include <qvariant.h>
#include <ksharedptr.h>

#include "field.h"

namespace KexiDB {

class ConnectionInternal;
class TableSchema;
class FieldList;

class KEXI_DB_EXPORT PreparedStatement : public KShared
{
	public:
		typedef KSharedPtr<PreparedStatement> Ptr;
		enum StatementType {
			SelectStatement,
			InsertStatement
		};

		PreparedStatement(StatementType type, ConnectionInternal& conn, TableSchema& tableSchema,
			const QStringList& where = QStringList());
		virtual ~PreparedStatement();
		PreparedStatement& operator<< ( const QVariant& value );
		void clearArguments();
		virtual bool execute() = 0;

	protected:
		QCString generateStatementString();

		StatementType m_type;
//		ConnectionInternal *m_conn;
		FieldList *m_fields;
		QValueList<QVariant> m_args;
		QStringList* m_where;
		Field::List* m_whereFields;
};

} //namespace KexiDB

#endif
