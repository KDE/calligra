/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_CURSOR_H
#define KEXIDB_CURSOR_H

#include <qstring.h>
#include <qvariant.h>

#include <kexidb/connection.h>

namespace KexiDB {

//class CursorData;

/*! 

*/
class KEXI_DB_EXPORT Cursor
{
	public:
		virtual ~Cursor();
		/*! \return connection used for the cursor */
		Connection* connection() { return m_conn; }
		/*! Opens the cursor using \a statement */
		bool open( const QString& statement = QString::null );
		/*! Closes previously opened cursor. 
			If the cursor is closed, nothing happens. */
		virtual bool close();
		/*! Moves current position to the first record and retrieves it. */
		bool moveFirst();
		/*! Moves current position to the last record and retrieves it. */
		virtual bool moveLast();
		/*! Moves current position to the next record and retrieves it. */
		virtual bool moveNext();
		/*! \return true if current position is after last record. */
		bool eof();
		/*! \return current internal position of the query. */
		int at();
		QString statement() { return m_statement; }
		bool isOpened() { return m_opened; }
		/*! Closes and then opens again the same cursor. 
			Cursor must be opened before calling this method. */
		bool reopen();

		virtual QVariant value(int i) = 0;

	protected:
		/*! Cursor will operate on \a conn */
		Cursor(Connection* conn, const QString& statement = QString::null );
		virtual bool drv_open() = 0;
		virtual bool drv_close() = 0;
		virtual bool drv_moveFirst() = 0;
		virtual bool drv_getRecord() = 0;

		Connection *m_conn;
//		CursorData *m_data;
		QString m_statement;
		bool m_opened;
//		bool m_beforeFirst;
		bool m_atLast;
		bool m_afterLast;
//		bool m_atLast;
		bool m_validRecord; //! true if valid record is currently retrieved @ current position
		bool m_readAhead;
		int m_at;
	private:
		class Private;
		Private *d;
};

} //namespace KexiDB

#endif


