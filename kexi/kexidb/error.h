/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef _KEXI_ERROR_H_
#define _KEXI_ERROR_H_

#define ERR_NONE 0
#define ERR_DRIVERMANAGER 10
#define ERR_MISSING_DB_LOCATION 20
#define ERR_ALREADY_CONNECTED 30
#define ERR_NO_CONNECTION 40 //!< when opened connection was expected using KexiDB::Connection
#define ERR_NO_DB_USED 41 //!< when used database was expected using KexiDB::Connection
#define ERR_OBJECT_EXISTS 50
#define ERR_OBJECT_NOT_EXISTING 60
#define ERR_ACCESS_RIGHTS 70
#define ERR_TRANSACTION_ACTIVE 80
#define ERR_NO_TRANSACTION_ACTIVE 90
#define ERR_DB_SPECIFIC 100
#define ERR_CURSOR_NOT_OPEN 110
#define ERR_SINGLE_DB_NAME_MISMATCH 120
#define ERR_CURSOR_RECORD_FETCHING 130 //!< eg. for Cursor::drv_getNextRecord()
#define ERR_UNSUPPORTED_DRV_FEATURE 140 //!< given driver's feature is unsupported (eg. transactins)
#define ERR_ROLLBACK_OR_COMMIT_TRANSACTION 150 //!< error during transaction rollback or commit
#define ERR_SYSTEM_NAME_RESERVED 160 //!< system name is reserved and cannot be used (e.g. for table of field name)
#define ERR_CANNOT_CREATE_EMPTY_OBJECT 170 //!< empty object cannot be created (e.g. table without fields)
#define ERR_INVALID_DRIVER_IMPL 180 //! driver's implementation is not invalid

#define ERR_OTHER 0xffff //!< use this if you have not (yet?) the name for given error 
#endif
