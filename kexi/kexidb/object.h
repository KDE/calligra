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

#ifndef KEXIDB_OBJECT_H
#define KEXIDB_OBJECT_H

#include <qstring.h>

namespace KexiDB {

/*!

*/
class KEXI_DB_EXPORT Object
{
	public:
		//! \return true if there was error during last operation on the object.
		bool error() { return m_hasError; }

		/*! \return (localized) error message if there was error during last operation on the object,
			else: 0. */
		const QString& errorMsg() { return m_errMsg; }

		/*! \return error number of if there was error during last operation on the object,
			else: 0. */
		int errorNum() { return m_errno; }

	protected:
		/*! Sets the (localized) error message to \a msg. Use this in subclasses. */
		void setErrorMsg( const QString &msg );

		/*! Clears error flag. */
		void clearError() { m_errno = 0; m_hasError = false; m_errMsg = QString::null; }

		Object();
		virtual ~Object();

		int m_errno;
		bool m_hasError;
		QString m_errMsg;

	private:
		class Private;
		Private *d;
};

} //namespace KexiDB

#endif


