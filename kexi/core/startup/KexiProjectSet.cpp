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

#include "KexiProjectSet.h"
#include "kexi.h"

#include <kexidb/driver.h>
#include <kexidb/connection.h>

#include <kdebug.h>

class KexiProjectSetPrivate
{
public:
	KexiProjectSetPrivate()
	{
//		list.setAutoDelete(true);
	}
	KexiProjectData::List list;
};

KexiProjectSet::KexiProjectSet()
: d(new KexiProjectSetPrivate())
{
}

KexiProjectSet::KexiProjectSet(KexiDB::ConnectionData &conndata)
: d(new KexiProjectSetPrivate())
{
	KexiDB::Driver *drv = Kexi::driverManager.driver(conndata.driverName);
	if (!drv) {
		setError(&Kexi::driverManager);
		return;
	}
	KexiDB::Connection *conn = drv->createConnection(conndata);
	if (!conn) {
		setError(drv);
		return;
	}
	if (!conn->connect()) {
		setError(conn);
		delete conn;
		return;
	}
	QStringList dbnames = conn->databaseNames(false/*skip system*/);
	KexiDBDbg << dbnames.count() << endl;
	if (conn->error()) {
		setError(conn);
		delete conn;
		return;
	}
	delete conn;
	conn = 0;
	for (QStringList::Iterator it = dbnames.begin(); it!=dbnames.end(); ++it) {
		// project's caption is just the same as database name - nothing better is available
		KexiProjectData *pdata = new KexiProjectData(conndata, *it, *it);
		d->list.append( pdata );
	}
	clearError();
}


KexiProjectSet::~KexiProjectSet()
{
	delete d;
}

void KexiProjectSet::addProjectData(KexiProjectData *data)
{
	d->list.append(data);
}

KexiProjectData::List KexiProjectSet::list() const
{
	return d->list;
}

KexiProjectData* KexiProjectSet::findProject(const QString &dbName) const
{
	const QString _dbName = dbName.lower();
	QPtrListIterator<KexiProjectData> it( d->list );
	for (;it.current();++it) {
		if (it.current()->databaseName().lower()==_dbName)
			return it.current();
	}
	return 0;
}
