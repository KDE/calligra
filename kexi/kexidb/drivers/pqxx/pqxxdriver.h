/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <adam@piggz.co.uk>

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

#ifndef KEXIDB_DRIVER_PQXX_H
#define KEXIDB_DRIVER_PQXX_H

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;

//! PostgreSQL database driver.
class pqxxSqlDriver : public Driver
{
	Q_OBJECT
	KEXIDB_DRIVER

	public:
		pqxxSqlDriver( QObject *parent, const QStringList &args = QStringList() );
		~pqxxSqlDriver();

		virtual bool isSystemObjectName( const QString& n )const;
		virtual bool isSystemDatabaseName( const QString& n )const;

		//! Escape a string for use as a value
		virtual QString escapeString( const QString& str) const;
		virtual QByteArray escapeString( const QByteArray& str) const;
		virtual QString sqlTypeName(int id_t, int p=0) const;

		//! Escape BLOB value \a array
		virtual QString escapeBLOB(const QByteArray& array) const;

		/*! Escapes and converts value \a v (for type \a ftype) 
		 to string representation required by SQL commands.
		 Reimplemented for boolean type only to use SQL compliant TRUE or FALSE */
		virtual QString valueToSQL( uint ftype, const QVariant& v ) const;

	protected:
		virtual QString drv_escapeIdentifier( const QString& str) const;
		virtual QByteArray drv_escapeIdentifier( const QByteArray& str) const;
		virtual Connection *drv_createConnection( ConnectionData &conn_data );
		virtual bool drv_isSystemFieldName( const QString& n )const;

	private:
		static const char *keywords[];
};

};

#endif
