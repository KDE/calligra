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

#include "odbctablesqueryunit.h"

// kexi includes
#include "odbcconnection_p.h"

using namespace KexiDB;

ODBCTablesQueryUnit::ODBCTablesQueryUnit( QObject* parent )
    : ODBCQueryUnit( parent ), m_tablesFilter( QString() )
{
}

void ODBCTablesQueryUnit::setFilter( const QString& filter ) {
    m_tablesFilter = filter;
}

SQLRETURN ODBCTablesQueryUnit::execute()
{
    QByteArray tableNameBA;
    const char* tableName = 0;
    if ( !m_tablesFilter.isEmpty() ) {
        tableNameBA = m_tablesFilter.toAscii();
        tableName = tableNameBA.constData();
    }

    return SQLTables(m_cursorData->statementHandle,
                     NULL, 0, // catalog name
                     NULL, 0, // schema name
                     (SQLCHAR* )tableName , tableName? m_tablesFilter.length() : 0, // table name
                     (SQLCHAR*)"TABLE",
                     SQL_NTS
            );
}
