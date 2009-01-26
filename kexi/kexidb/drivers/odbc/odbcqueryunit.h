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

#ifndef KEXIDB_ODBC_QUERY_UNIT_H
#define KEXIDB_ODBC_QUERY_UNIT_H

// qt includes
#include <QObject>

// kexi includes

// odbc includes
#include <sql.h>

namespace KexiDB
{

class ODBCCursorData;

class ODBCQueryUnit : public QObject
{
   Q_OBJECT
	    
  public:
    ODBCQueryUnit(QObject* parent = 0);
    
    void setCursorData(ODBCCursorData* cursorData);

    virtual SQLRETURN execute() = 0;

  protected:
    ODBCCursorData* m_cursorData;

};

}

#endif
