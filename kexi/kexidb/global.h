/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_GLOBAL_H
#define KEXIDB_GLOBAL_H

#include <kexidb/kexidb_export.h>

//global public definitions

/*! KexiDB implementation version. 
 It is altered after every change: 
 - major number is increased after KexiDB storage format change, 
 - minor is increased after adding binary-incompatible change.
 In external code: do not use this to get library version information:
 use KexiDB::versionMajor() and KexiDB::versionMinor() instead to get real version.
*/
#define KEXIDB_VERSION_MAJOR 1
#define KEXIDB_VERSION_MINOR 7


/*! \namespace KexiDB 
\brief Kexi database backend drivers.

\section Framework
DriverManager 

Database access
 - Connection
 - ConnectionData
 
Database structure
 - Schema
  - tableschema
  - queryschema
  - indexschema

Stored in the database.


Data representation
 - Record
 - Field

 
\section Drivers

Drivers are loaded using DriverManager::driver(const QString& name).  The names
of drivers are given in their drivers .desktop file in the
X-Kexi-DriverName field.

KexiDB supports two kinds of databases: file-based and network-based databases.
The type of a driver is available from several places. The X-Kexi-DriverType
field in the driver's .desktop file, is read by the DriverManager and
available by calling DriverManager::driverInfo(const QString &name) and using
the Driver::Info#fileBased member from the result. Given a reference to a
Driver, its type can also be found directly using Driver::isFileDriver() const.

Each database backend driver consists of three main classes: a driver,
a connection and a cursor class, e.g SQLiteDriver, SQLiteConnection,
SQLiteCursor.

The driver classes subclass the Driver class.  They set Driver#m_typeNames,
which maps KexiDB's Field::Type on to the types supported by the database.  They also
provide functions for escaping strings and checking table names.  These may be
used, for example, on a database backend that uses the database name as a
filename.  In this case, it should be ensured that all the characters in the
database name are valid characters in a filename.

The connection classes subclass the Connection class, and include most of the
calls to the native database API.

The cursor classes subclass Cursor, and implement cursor functionality specific
to the database backend.

*/
namespace KexiDB {

#define KexiDBDbg  kdDebug(44000)   //! Debug area for core KexiDB code
#define KexiDBDrvDbg kdDebug(44001) //! Debug area for KexiDB's drivers implementation code
#define KexiDBWarn  kdWarning(44000)
#define KexiDBDrvWarn kdWarning(44001)

//! \return KexiDB version info (most significant part)
KEXI_DB_EXPORT int versionMajor();

//! \return KexiDB version info (least significant part)
KEXI_DB_EXPORT int versionMinor();

/*! Object types set like table or query. */
enum ObjectTypes {
	UnknownObjectType = -1, //!< helper
	AnyObjectType = 0,      //!< helper
	TableObjectType = 1,
	QueryObjectType = 2,
	LastObjectType = 2, //ALWAYS UPDATE THIS

	KexiDBSystemTableObjectType = 128,//!< helper, not used in storage 
	                                  //!< (allows to select kexidb system tables
	                                  //!< may be or'd with TableObjectType)
	IndexObjectType = 256 //!< special
};

}

#endif

