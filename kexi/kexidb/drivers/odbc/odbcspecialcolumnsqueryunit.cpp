/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

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

#include "odbcspecialcolumnsqueryunit.h"

// kexi includes
#include "odbcconnection_p.h"

// odbc includes
#include <sqlext.h>

using namespace KexiDB;

ODBCSpecialColumnsQueryUnit::ODBCSpecialColumnsQueryUnit( QObject* parent , const QString& tableName)
    : ODBCQueryUnit( parent ), m_tableName( tableName )
{
}

SQLRETURN ODBCSpecialColumnsQueryUnit::execute()
{
    QByteArray ba = m_tableName.toAscii();
    const char* tableName = ba.constData();
    return SQLSpecialColumns(m_cursorData->statementHandle,
                     SQL_BEST_ROWID,
                     NULL, 0, // catalog name
                     NULL, 0, // schema name
                     (SQLCHAR* )tableName , m_tableName.length(), // table name
                     SQL_SCOPE_SESSION, // scope of the special column
                     SQL_NO_NULLS // nullable
            );
}
