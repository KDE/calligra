/* This file is part of the KDE project
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>
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

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/error.h>

#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kservice.h>

#include <assert.h>

using namespace KexiDB;

DriverManager* DriverManager::s_self = 0L;

DriverManager::DriverManager() /* protected */
	: QObject( 0, "KexiDB::DriverManager" )
	, Object()
{
	if ( !s_self )
		s_self = this;
	lookupDrivers();
}

DriverManager::~DriverManager()
{
/*	Connection *conn;
	for ( conn = m_connections.first(); conn ; conn = m_connections.next() ) {
		conn->disconnect();
		conn->m_driver = 0; //don't let the connection touch our driver now
		m_connections.remove();
		delete conn;
	}*/

	if ( s_self == this )
		s_self = 0;
}

DriverManager *DriverManager::self()
{
	if (!s_self)
		s_self = new DriverManager();

	return s_self;
}

void DriverManager::lookupDrivers()
{
	clearError();
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/DBDriver");
	KTrader::OfferList::Iterator it(tlist.begin());
	for(; it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		m_services.insert(ptr->property("X-Kexi-DriverName").toString(), ptr);
		kdDebug() << "KexiDB::DriverManager::lookupDrivers(): registered driver: " << ptr->name() << "(" << ptr->library() << ")" << endl;
	}

	if (tlist.isEmpty())
	{
		setErrorMsg(ERR_DRIVERMANAGER, i18n("Could not find any database drivers.") );
	}
}

const QStringList DriverManager::driverNames()
{
	if (m_services.isEmpty() && error())
		return QStringList();
	return m_services.keys();
}

KService::Ptr DriverManager::serviceInfo(const QString &name)
{
	clearError();
	if (m_services.contains(name)) {
		return *m_services.find(name);
	} else {
		setErrorMsg(ERR_DRIVERMANAGER, i18n("No such driver service: '%1'.").arg(name) );
		return 0;
	}
}


Driver* DriverManager::driver(const QCString& name)
{
	clearError();
	kdDebug() << "DriverManager::driver(): loading " << name << endl;

	Driver *drv = m_drivers.find(name);
	if (drv)
		return drv; //cached

	if (!m_services.contains(name)) {
		setErrorMsg(ERR_DRIVERMANAGER, i18n("Could not find database driver '%1'.").arg(name) );
		return 0;
	}

	KLibLoader *libLoader = KLibLoader::self();
	KService::Ptr d= *(m_services.find(name));

	kdDebug() << "KexiDBInterfaceManager::load(): library: "<<d->library()<<endl;
	drv = KParts::ComponentFactory::createInstanceFromService<KexiDB::Driver>(d,
		this, "db", QStringList());

	if (!drv) {
		setErrorMsg(ERR_DRIVERMANAGER, i18n("Could not load database driver '%1'.").arg(name) );
		return 0;
	}
	kdDebug() << "KexiDBInterfaceManager::load(): loading succeed: " << name << endl;

	drv->m_service = d; //store info
	m_drivers.insert(name, drv); //cache it
	return drv;
}
void KexiDB::DriverManager::incRefCount()
{
	m_refCount++;
}

void KexiDB::DriverManager::decRefCount()
{
	m_refCount--;
	if (m_refCount<1) {
		kdDebug()<<"KexiDBInterfaceManager::remRef: reached m_ref<1 -->deletelater()"<<endl;
		s_self=0;
		deleteLater();
	}
}

