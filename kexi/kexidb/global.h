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

#ifndef KEXIDB_GLOBAL_H
#define KEXIDB_GLOBAL_H

//global public definitions

/*! KexiDB implementation version.
 In external code: do not use this to get library version information:
 use KexiDB::versionMajor() and KexiDB::versionMinor() instead to get real version.
*/
#define KEXIDB_VERSION_MAJOR 1
#define KEXIDB_VERSION_MINOR 1

namespace KexiDB {

#ifndef KEXI_DB_EXPORT
# ifndef Q_WS_WIN
#  define KEXI_DB_EXPORT
# endif
#endif

#define KexiDBDbg  kdDebug(44000)   //! Debug area for core KexiDB code
#define KexiDBDrvDbg kdDebug(44001) //! Debug area for KexiDB's drivers implementation code
#define KexiDBWarning  kdWarning(44000)
#define KexiDBDrvWarning kdWarning(44001)

//! returns KexiDB version info (most significant part)
int KEXI_DB_EXPORT versionMajor();

//! returns KexiDB version info (least significant part)
int KEXI_DB_EXPORT versionMinor();

/*! Object types set like table or query. */
enum ObjectTypes {
	UnknownObjectType = -1, //!< helper
	AnyObjectType = 0,      //!< helper
	TableObjectType = 1,
	QueryObjectType = 2,
	IndexObjectType = 3,
	LastObjectType = 3, //ALWAYS UPDATE THIS

	KexiDBSystemTableObjectType = 128 //!< helper, not used in storage 
	                                  //!< (allows to select kexidb system tables
	                                  //!< may be or'd with TableObjectType)
};

}

#endif

