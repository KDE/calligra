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

/*
static KexiDBInterfaceManager *KexiDBInterfaceManager::s_kexidbinterfacemanager=0;

KexiDBInterfaceManager *KexiDBInterfaceManager::self();
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
		s_kexidbinterfacemanager=0;
		deleteLater();
	}
}
*/

KexiDBInterfaceManager::KexiDBInterfaceManager(const char *name) : QObject(0, name)//,m_ref(0)
{
	//some initialisations
//	m_libLoader = KLibLoader::self();
	lookupDrivers();
	//m_part = new KexiDB(this, "database");
// no idea, why this loops infinitly here	load("mySQL");

}

KexiDB*
KexiDBInterfaceManager::require(const QString &driver)
{
	kdDebug()<<"kexDBIface::require"<<endl;

	if(!m_driverList.find(driver))
		return 0;

	kdDebug()<<"driver found in list"<<endl;
	KexiDBDriver *d = m_driverList.find(driver);
	if(d->loaded())
	{
		return d->db();
	}
	else
	{
		load(driver);
		if (d->loaded()) return d->db();
		kdDebug()<<"KexiDBInterfaceManager::reqiure():couldn't load plugin"<<endl;
	}
	return 0;
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

			KexiDBDriver *driver = new KexiDBDriver(ptr->name());
			driver->setService(*it);
			m_driverList.insert(ptr->name(), driver);
			kdDebug() << "KexiDBInterfaceManager::lookupDrivers(): registerd driver: " << ptr->name() << endl;
		}
	}
	else
	{
		KMessageBox::error(0, i18n("Kexi could not find any database drivers!"));
		exit(0);
	}
}

void
KexiDBInterfaceManager::load(const QString &driver)
{
	kdDebug() << "KexiDBInterfaceManager::load(): loading " << driver << endl;
	if(m_driverList.find(driver))
	{
		KLibLoader *libLoader = KLibLoader::self();
		KexiDBDriver *d = m_driverList.find(driver);
		if(d)
		{
			kdDebug() << "KexiDBInterfaceManager::load(): d found" << endl;
			if(d->service())
			{
				kdDebug() << "KexiDBInterfaceManager::load(): d->service found" << endl;
				kdDebug() << "KexiDBInterfaceManager::load(): library: "<<d->service()->library()<<endl;
				KexiDB *plugin = KParts::ComponentFactory::createInstanceFromService<KexiDB>(d->service(),
					this, "db", QStringList());

				if(plugin)
				{
					d->m_db = plugin;
					d->m_loaded = true;
					kdDebug() << "KexiDBInterfaceManager::load(): loading suceed: " << plugin << endl;
				}
			}
		}
	}
}

QStringList
KexiDBInterfaceManager::drivers() const
{
	QStringList result;

	for(QDictIterator<KexiDBDriver> it(m_driverList); it.current(); ++it)
	{
		KexiDBDriver *currentDriver = it;
		result.append(currentDriver->driver());
	}

	return result;
}

KexiDBDriver*
KexiDBInterfaceManager::driverInfo(const QString &driver)
{
	KexiDBDriver *d = m_driverList.find(driver);
	if(d)
	{
		return d;
	}
	else
	{
		return 0;
	}

}


KexiDBInterfaceManager::~KexiDBInterfaceManager()
{
	for(QDictIterator<KexiDBDriver> it(m_driverList); it.current(); ++it)
	{
		KexiDBDriver *driver = it;
		if(driver->loaded())
		{
			delete driver;
		}
	}
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

	KexiDB *d =require(driver);
	if(d)
	{
		kdDebug() << "got driver..." << endl;
		return d;
	}
	return 0;
}


#include "kexidbinterfacemanager.moc"
