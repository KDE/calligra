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

namespace KexiDB {

/*!

*/
class KEXI_DB_EXPORT DriverManagerInternal : public QObject, public KexiDB::Object
{
	public:
		~DriverManagerInternal();

		/*! Tries to load db driver \a name.
			\return db driver, or 0 if error (then error message is also set) */
		KexiDB::Driver* driver(const QCString& name);

		static DriverManagerInternal *self();

		/*! returns list of available drivers names. That drivers can be loaded
			by first use of driver() method. */
//		const QStringList driverNames();

		/*! \return info. about driver (service info) */
//		KService::Ptr serviceInfo(const QString &name);

		//! Returns a map structure of the services. Not necessary for everyday use.
//		const ServicesMap& services() { return m_services; }

		/*! increments the refcount for the manager*/
		void incRefCount();
		/*! decrements the refcount for the manager
			if the refcount reaches a value less than 1 the manager is freed */
		void decRefCount();
	protected:
		/*! Used by self() */
		DriverManagerInternal();

		void lookupDrivers();

		static KexiDB::DriverManagerInternal* s_self;

		KexiDB::DriverManager::ServicesMap m_services; //! services map
		KexiDB::DriverManager::ServicesMap m_services_lcase; //! as above but service names in lowercase
		QDict<KexiDB::Driver> m_drivers;
		ulong m_refCount;

	friend class DriverManager;
};
} //KexiDB

using namespace KexiDB;

DriverManagerInternal::DriverManagerInternal() /* protected */
	: QObject( 0, "KexiDB::DriverManager" )
	, Object()
	, m_refCount(0)
{
	m_drivers.setAutoDelete(true);
	lookupDrivers();
}

DriverManagerInternal::~DriverManagerInternal()
{
	kdDebug() << "DriverManagerInternal::~DriverManagerInternal()" << endl;
	m_drivers.clear();
	if ( s_self == this )
		s_self = 0;
	kdDebug() << "DriverManagerInternal::~DriverManagerInternal() ok" << endl;
}

DriverManagerInternal *DriverManagerInternal::self()
{
	if (!s_self)
		s_self = new DriverManagerInternal();

	return s_self;
}

void DriverManagerInternal::lookupDrivers()
{
	clearError();
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/DBDriver");
	KTrader::OfferList::Iterator it(tlist.begin());
	for(; it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		QString srv_name = ptr->property("X-Kexi-DriverName").toString();
		if (!m_services_lcase.contains(srv_name.lower())) {
			m_services.insert(srv_name, ptr);
			m_services_lcase.insert(srv_name.lower(), ptr);
			kdDebug() << "KexiDB::DriverManager::lookupDrivers(): registered driver: " << ptr->name() << "(" << ptr->library() << ")" << endl;
		}
	}

	if (tlist.isEmpty())
	{
		setError(ERR_DRIVERMANAGER, i18n("Could not find any database drivers.") );
	}
}

Driver* DriverManagerInternal::driver(const QCString& name)
{
	clearError();
	kdDebug() << "DriverManager::driver(): loading " << name << endl;

	Driver *drv = m_drivers.find(name.lower());
	if (drv)
		return drv; //cached

	if (!m_services_lcase.contains(name.lower())) {
		setError(ERR_DRIVERMANAGER, i18n("Could not find database driver '%1'.").arg(name) );
		return 0;
	}

//	KLibLoader *libLoader = KLibLoader::self();
	KService::Ptr ptr= *(m_services_lcase.find(name.lower()));

	kdDebug() << "KexiDBInterfaceManager::load(): library: "<<ptr->library()<<endl;
	drv = KParts::ComponentFactory::createInstanceFromService<KexiDB::Driver>(ptr,
		this, "db", QStringList());

	if (!drv) {
		setError(ERR_DRIVERMANAGER, i18n("Could not load database driver '%1'.").arg(name) );
		return 0;
	}
	kdDebug() << "KexiDBInterfaceManager::load(): loading succeed: " << name << endl;

	drv->m_service = ptr; //store info
	m_drivers.insert(name.lower(), drv); //cache it
	return drv;
}

void DriverManagerInternal::incRefCount()
{
	m_refCount++;
}

void DriverManagerInternal::decRefCount()
{
	m_refCount--;
	if (m_refCount<1) {
		kdDebug()<<"KexiDB::DriverManagerInternal::decRefCount(): reached m_refCount<1 -->deletelater()"<<endl;
//		s_self=0;
//		deleteLater();
	}
}

DriverManagerInternal* DriverManagerInternal::s_self = 0L;



// ---------------------------
// --- DriverManager impl. ---
// ---------------------------

DriverManager::DriverManager()
	: QObject( 0, "KexiDB::DriverManager" )
	, Object()
	, d_int( DriverManagerInternal::self() )
{
	d_int->incRefCount();
//	if ( !s_self )
//		s_self = this;
//	lookupDrivers();
}

DriverManager::~DriverManager()
{
	kdDebug() << "DriverManager::~DriverManager()" << endl;
/*	Connection *conn;
	for ( conn = m_connections.first(); conn ; conn = m_connections.next() ) {
		conn->disconnect();
		conn->m_driver = 0; //don't let the connection touch our driver now
		m_connections.remove();
		delete conn;
	}*/

	d_int->decRefCount();
	if (d_int->m_refCount==0) {
		//delete internal drv manager!
		delete d_int;
	}
//	if ( s_self == this )
		//s_self = 0;
	kdDebug() << "DriverManager::~DriverManager() ok" << endl;
}

const QStringList DriverManager::driverNames()
{
	if (d_int->m_services.isEmpty() && d_int->error())
		return QStringList();
	return d_int->m_services.keys();
}

KService::Ptr DriverManager::serviceInfo(const QString &name)
{
	clearError();
	if (d_int->m_services_lcase.contains(name.lower())) {
		return *d_int->m_services_lcase.find(name.lower());
	} else {
		setError(ERR_DRIVERMANAGER, i18n("No such driver service: '%1'.").arg(name) );
		return 0;
	}
}

const DriverManager::ServicesMap& DriverManager::services()
{
	return d_int->m_services;
}

Driver* DriverManager::driver(const QCString& name)
{
	Driver *drv = d_int->driver(name);
	if (d_int->error())
		setError(d_int);
	return drv;
}

