/* This file is part of the KDE project
Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>

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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#ifndef _MYSQLCURSOR_H_
#define _MYSQLCURSOR_H_

#include <kexidb/cursor.h>
#include <kexidb/connection.h>

#ifdef Q_WS_WIN
# include <config-win.h>
#endif
#include <mysql.h>
#define BOOL bool

namespace KexiDB {

class KEXIDB_MYSQL_DRIVER_EXPORT MySqlCursor: public Cursor {
public:
	MySqlCursor(Connection* conn, const QString& statement = QString::null, uint cursor_options = 0 );
	MySqlCursor(Connection* conn, QuerySchema& query, uint options = 0 );
	~MySqlCursor();
        virtual bool drv_open(const QString& statement);
        virtual bool drv_close();
        virtual bool drv_moveFirst();
        virtual bool drv_getNextRecord();
        virtual bool drv_getPrevRecord();
	virtual QVariant value(int);

	//TODO:
	virtual QVariant value(int i) const { return QVariant(); }

	//TODO:
	/*! [PROTOTYPE] \return current record data or NULL if there is no current records. */
	virtual const char ** recordData() const { return 0; }

	//TODO:
	virtual void storeCurrentRecord(RecordData &data) const {}

private:
	MYSQL_RES *m_res;	
	MYSQL_ROW m_row;
	unsigned long *m_lengths;
//js: int m_numFields;
	unsigned long m_numRows;
};

}

#endif
