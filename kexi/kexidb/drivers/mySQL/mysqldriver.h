/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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

#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <kexidb/driver.h>

namespace KexiDB {

class KEXIDB_MYSQL_DRIVER_EXPORT MySqlDriver : public Driver
{
	Q_OBJECT

	public:
		MySqlDriver(QObject *parent=0, const char *name="mysq", const QStringList &args=QStringList());
		~MySqlDriver();

	protected:
		virtual Connection *drv_createConnection( ConnectionData &conn_data );
};
}

#endif
