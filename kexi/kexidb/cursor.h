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
#include <kexidb/object.h>

namespace KexiDB {

//class CursorData;

/*! 

*/
class KEXI_DB_EXPORT Cursor: public Object
{
	public:
		virtual ~Cursor();
		/*! \return connection used for the cursor */
		Connection* connection() { return m_conn; }
		/*! Opens the cursor using \a statement. 
		 Omit \a statement if cursor is already initialized with statement 
		 at creation time. If \a statement is not empty, existing statement
		 (if any) is overwritten. */
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
		/*! Moves current position to the next record and retrieves it. */
		virtual bool movePrev();
		/*! \return true if current position is after last record. */
		bool eof();
		/*! \return true if current position is before first record. */
		bool bof();
		/*! \return current internal position of the cursor's query. 
		 We are counting records from 0.
		 Value -1 means that cursor does not point to any valid record
		 (this happens eg. after open(), close(), 
		 and after moving after last record or before first one. */
		int at();
		QString statement() { return m_statement; }
		bool isOpened() { return m_opened; }
		/*! Closes and then opens again the same cursor. 
			Cursor must be opened before calling this method. */
		bool reopen();
		/*! \return number of fields available for this cursor. */
		int fieldCount() { return m_fieldCount; }
		virtual QVariant value(int i) = 0;

	protected:
		/*! Cursor will operate on \a conn */
		Cursor(Connection* conn, const QString& statement = QString::null );
		virtual bool drv_open() = 0;
		virtual bool drv_close() = 0;
//		virtual bool drv_moveFirst() = 0;
		virtual bool drv_getNextRecord() = 0;
		virtual bool drv_getPrevRecord() = 0;

		Connection *m_conn;
//		CursorData *m_data;
		QString m_statement;
		bool m_opened;
		bool m_beforeFirst;
		bool m_atLast;
		bool m_afterLast;
//		bool m_atLast;
		bool m_validRecord; //! true if valid record is currently retrieved @ current position
		bool m_readAhead;
		int m_at;
		int m_fieldCount;
	private:
		class Private;
		Private *d;
};

} //namespace KexiDB

#endif


