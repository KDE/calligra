/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "sqliteadmin.h"
#include "driver_p.h"

#ifndef SQLITE2
# include "sqlitevacuum.h"
#endif

SQLiteAdminTools::SQLiteAdminTools()
 : KexiDB::AdminTools()
{
}

SQLiteAdminTools::~SQLiteAdminTools()
{
}

bool SQLiteAdminTools::vacuum(KexiDB::Connection& conn)
{
#ifndef SQLITE2
	const bool wasConnected = conn.isConnected();
	if (wasConnected) {
		if (!conn.disconnect())
			return false;
	}
	SQLiteVacuum vacuum(conn.data()->fileName());
	tristate result = vacuum.run();
	if (wasConnected) {
		if (!conn.connect())
			return false;
		if (!conn.useDatabase(conn.data()->dbFileName()))
			return false;
	}
	return result || ~result;
#else
	return false;
#endif
}

