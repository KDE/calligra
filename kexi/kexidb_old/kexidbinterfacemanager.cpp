
/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "kexidb.h"
#include "kexidbinterfacemanager.h"


KexiDBInterfaceManager *KexiDBInterfaceManager::s_kexidbinterfacemanager=0;

KexiDBInterfaceManager *KexiDBInterfaceManager::self()
{
	if (s_kexidbinterfacemanager==0)
		s_kexidbinterfacemanager=new KexiDBInterfaceManager("kexidbifacemngr");

	return s_kexidbinterfacemanager;
}

void KexiDBInterfaceManager::addRef()
{
	m_ref++;
}

void KexiDBInterfaceManager::remRef()
{
	m_ref--;
	if (m_ref<1) {
		kdDebug()<<"KexiDBInterfaceManager::remRef: reached m_ref<1 -->deletelater()"<<endl;
		s_kexidbinterfacemanager=0;
		deleteLater();
	}
}


KexiDBInterfaceManager::KexiDBInterfaceManager(const char *name) : QObject(0, name),m_ref(0)
{
	//some initialisations
//	m_libLoader = KLibLoader::self();
	lookupDrivers();
	//m_part = new KexiDB(this, "database");
// no idea, why this loops infinitly here	load("mySQL");

}

void
KexiDBInterfaceManager::lookupDrivers()
{
	KTrader::OfferList tlist = KTrader::self()->query("KexiDB/Interface");
	if(tlist.count() > 0) {
		KTrader::OfferList::Iterator it(tlist.begin());
		for(; it != tlist.end(); ++it)
		{
			KService::Ptr ptr = (*it);
			m_driverList.insert(ptr->property("X-Kexi-DriverName").toString(), ptr);
			kdDebug() << "KexiDBInterfaceManager::lookupDrivers(): registerd driver: " << ptr->name() << endl;
		}
	}
	else
	{
		KMessageBox::error(0, i18n("Kexi could not find any database drivers!"));
		exit(0);
	}
}

KexiDB*
KexiDBInterfaceManager::load(const QString &driver)
{
	kdDebug() << "KexiDBInterfaceManager::load(): loading " << driver << endl;
	if(m_driverList.contains(driver))
	{
		KLibLoader *libLoader = KLibLoader::self();

			kdDebug() << "KexiDBInterfaceManager::load(): d found" << endl;

			KService::Ptr d= *(m_driverList.find(driver));

			kdDebug() << "KexiDBInterfaceManager::load(): d->service found" << endl;
			kdDebug() << "KexiDBInterfaceManager::load(): library: "<<d->library()<<endl;
				KexiDB *plugin = KParts::ComponentFactory::createInstanceFromService<KexiDB>(d,
					this, "db", QStringList());

				if(plugin)
				{
					kdDebug() << "KexiDBInterfaceManager::load(): loading suceed: " << plugin << endl;
					return plugin;
				}
	}
	return 0;
}

QStringList
KexiDBInterfaceManager::drivers() const
{
	QStringList result;

	for(QMap<QString,KService::Ptr>::const_iterator it=m_driverList.begin(); it!=m_driverList.end(); ++it)
	{
		result.append(it.key());
	}

	return result;
}

KService::Ptr
KexiDBInterfaceManager::driverInfo(const QString &driver)
{
	if (m_driverList.contains(driver))
		return *m_driverList.find(driver);
	else
		return KService::Ptr();

}


KexiDBInterfaceManager::~KexiDBInterfaceManager()
{
/*
	for(QDictIterator<KexiDBDriver> it(m_driverList); it.current(); ++it)
	{
		KexiDBDriver *driver = it;
		if(driver->loaded())
		{
			delete driver;
		}
	}
*/
}

KexiDB*
KexiDBInterfaceManager::newDBInstance(const QString &driver)
{
	/*! this loads (if needed) the plugin
	    and creates an instance, wich is returned.
	    it should be used in replacement of that one
	    oh, how we love c++
	*/

	kdDebug() << "KexDB::add" << endl;

	KexiDB *d = load(driver);
	if(d)
	{
		kdDebug() << "got driver..." << endl;
		return d;
	}
	return 0;
}


#include "kexidbinterfacemanager.moc"
