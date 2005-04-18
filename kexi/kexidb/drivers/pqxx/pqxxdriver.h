/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <piggz@defiant.piggz.co.uk>

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

#ifndef KEXIDB_DRIVER_PQXX_H
#define KEXIDB_DRIVER_PQXX_H

#include <qstringlist.h>

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;
class DriverManager;

//! PostgreSQL database driver.
class pqxxSqlDriver : public Driver
{
	Q_OBJECT
	KEXIDB_DRIVER

	public:
		pqxxSqlDriver( QObject *parent, const char *name, const QStringList &args = QStringList() );
		~pqxxSqlDriver();

		virtual bool isSystemObjectName( const QString& n )const;
		virtual bool isSystemFieldName( const QString& n )const;
		virtual bool isSystemDatabaseName( const QString& n )const;

		//! Escape a string for use as a value
		virtual QString escapeString( const QString& str) const;
		virtual QCString escapeString( const QCString& str) const;
		virtual QString sqlTypeName(int id_t, int p=0) const;
		
	protected:
		virtual QString drv_escapeIdentifier( const QString& str) const;
		virtual QCString drv_escapeIdentifier( const QCString& str) const;
		
		virtual Connection *drv_createConnection( ConnectionData &conn_data );

	private:
		static const char *keywords[];
};

};

#endif
