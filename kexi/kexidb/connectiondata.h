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

#ifndef KEXIDB_CONNECTION_DATA_H
#define KEXIDB_CONNECTION_DATA_H

#include <qstring.h>

namespace KexiDB {

/*! Connection data, once configured, can be later stored for reuse.
*/
class KEXI_DB_EXPORT ConnectionData
{
	public:

		ConnectionData();

		/*! Name is optional for identyfying given connection
			by name eg. for users. */
		QString name;

		/*! Host name used for remote connection. Can be empty
			if connection is no-remote. */
		QString hostName;

		/*! Port used for remote connection. Default is 0, what means we use don't
			change database engine's default port. */
		unsigned short int port;

		/*! Password used for connection. Can be empty. */
		QString password;

		/*! Username used for connection. Can be empty. */
		QString userName;

		void setFileName( const QString& fn );

		/*! For file-based database engines like SQLite, \a fileName is used
			instead hostName and port */
		QString fileName() { return m_fileName; }

		QString dbPath() { return m_dbPath; }
		QString dbFileName() { return m_dbFileName; }
	protected:
		/*! For file-based database engines like SQLite, \a fileName is used
			instead hostName and port */
		QString m_fileName;

		/*! Absolute path to the database file (or empty if database is not file-based) */
		QString m_dbPath;
		/*! Filename of the database file (or empty if database is not file-based) */
		QString m_dbFileName;

	private:
		class Private;
		Private *d;

	friend class Connection;
};

}; //namespace KexiDB

#endif
