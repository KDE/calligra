/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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

#ifndef ORACLEDB_H
#define ORACLEDB_H
#include <db/driver.h>

// Drivers found in drivers/ folder belong to KexiDB namespace although naming_conventions.txt says they do not.
namespace KexiDB {

//! Oracle database driver.
class OracleDriver : public KexiDB::Driver{
	// Macros which seem to be needed
	Q_OBJECT
	KEXIDB_DRIVER

	public:
		OracleDriver( QObject *parent, const QVariantList &args = QVariantList() );
		virtual ~OracleDriver();


		// From here on, almost everything is copied from mysqldriver.h

		//! isSystemDatabaseName
		virtual bool isSystemDatabaseName( const QString &n ) const;

		//! Escape a string for use as a value
		virtual QString escapeString(const QString& str) const;
		virtual QByteArray escapeString(const QByteArray& str) const;

		//! Escape BLOB value \a array
		virtual QString escapeBLOB(const QByteArray& array) const;

	protected:
		virtual QString drv_escapeIdentifier( const QString& str) const;
		virtual QByteArray drv_escapeIdentifier( const QByteArray& str) const;
		virtual Connection *drv_createConnection( ConnectionData &conn_data );
		virtual bool drv_isSystemFieldName( const QString& n ) const;
		inline virtual QString addLimitTo1(const QString& sql, bool add) {
			return add ? 
				(QString::fromLatin1("SELECT * FROM ( ")+sql+
				QString::fromLatin1(" ) WHERE ROWNUM<=1")) 
				:sql;
		}

	private:
		static const char *keywords[];

}; // class OracleDriver

} // namespace KexiDB

#endif // KEXIDB_ORACLE_DRIVER_H

