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
#include <qptrlist.h>

namespace KexiDB {

/*! Connection data, once configured, can be later stored for reuse.
*/
class KEXI_DB_EXPORT ConnectionData
{
	public:
		typedef QPtrList<ConnectionData> List;
//		typedef QPtrList<const ConnectionData> ConstList;

		ConnectionData();

		ConnectionData(const ConnectionData&);
		
		~ConnectionData();

		/*! Name is optional for identyfying given connection
			by name eg. for users. */
		QString name;

		/*! Optional id used for identyfiyng any single data in a set (ConnectionData::ConstList).
		 This is set automatically when needed. By default: -1. */
		int id;

		/*! Name (unique, not i18n'd) of driver that is used (o should be used) to 
		 create a connection.
		 If you pass this ConnectionData object to KexiDB::Driver::createConnection() 
		 to create connection, \a driverName member will be updated with a valid KexiDB
		 driver name.
		 In other situations \a driverName member may be used to store information what 
		 driver should be used to perform connection, before we get an appropriate 
		 driver object from DriverManager.
		*/
		QString driverName;

		/*! Host name used for remote connection. Can be empty
			if connection is not remote, then \a hostName can be described 
			to the user as "localhost". */
		QString hostName;

		/*! \return a user-friendly string like:
		 - "myhost.org:12345" if a host and port is specified;
		 - "localhost:12345" of only port is specified; 
		 - "user@myhost.org:12345" if also user is specified
		 - "<file>" if file-based driver is assigned but no filename is assigned
		 - "file: pathto/mydb.kexi" if file-based driver is assigned and 
		    filename is assigned
		 
		 User's name is added if \a addUser is true (the default).
		*/
		QString serverInfoString(bool addUser = true) const;
		
		/*! Port used for remote connection. Default is 0, what means we use don't
			change database engine's default port. */
		unsigned short int port;

		/*! Name of local (named) socket file, for local connections only. */
		QString localSocketFileName;
		
		/*! Password used for connection. Can be empty. */
		QString password;

		/*! Username used for connection. Can be empty. */
		QString userName;

		void setFileName( const QString& fn );

		/*! For file-based database engines like SQLite, \a fileName is used
			instead hostName and port. This method returns filename with a path. */
		QString fileName() const { return m_fileName; }

		/*! \return file path (for file-based engines) but without a file name */
		QString dbPath() const { return m_dbPath; }
		
		/*! \return file name (for file-based engines) but without a full path */
		QString dbFileName() const { return m_dbFileName; }
	protected:
		/*! For file-based database engines like SQLite, \a fileName is used
			instead hostName and port */
		QString m_fileName;

		/*! Absolute path to the database file (or empty if database is not file-based) */
		QString m_dbPath;
		/*! Filename of the database file (or empty if database is not file-based) */
		QString m_dbFileName;

		class Private;
		Private *d;

	friend class Connection;
};

}; //namespace KexiDB

#endif
