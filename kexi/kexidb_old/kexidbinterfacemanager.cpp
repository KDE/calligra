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

#include "kexidb.h"
#include "kexidbinterfacemanager.h"

KexiDBInterfaceManager::KexiDBInterfaceManager(QObject *parent, const char *name) : QObject(parent, name)
{
	//some initialisations
//	m_libLoader = KLibLoader::self();
	lookupDrivers();
// no idea, why this loops infinitly here	load("mySQL");

}

KexiDB*
KexiDBInterfaceManager::require(QString driver)
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

void
KexiDBInterfaceManager::load(QString driver)
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
					this, "driverPlugin", QStringList());
				
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
KexiDBInterfaceManager::getDrivers() const
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
KexiDBInterfaceManager::getDriverInfo(QString driver)
{
	
}


KexiDBInterfaceManager::~KexiDBInterfaceManager()
{
}

#include "kexidbinterfacemanager.moc"
