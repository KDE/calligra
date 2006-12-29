/* This file is part of the KDE project
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "migratemanager.h"
#include "migratemanager_p.h"
#include "keximigrate.h"

#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kservice.h>

#include <assert.h>

#include <qapplication.h>

//remove debug
#undef KexiDBDbg
#define KexiDBDbg if (0) kdDebug()

using namespace KexiMigration;

MigrateManagerInternal* MigrateManagerInternal::s_self = 0L;

/*! @todo
 Temporary, needed because MigrateManagerInternal::m_drivers is autodeleted
 drivers currently own KexiMigrate::Data members so these are destroyed when 
 last MigrateManager instance is deleted. Remove this hack when 
 KexiMigrate is splitted into Driver and Connection. */
MigrateManager __manager;

MigrateManagerInternal::MigrateManagerInternal() /* protected */
	: QObject( 0, "KexiMigrate::MigrateManagerInternal" )
	, Object()
	, m_drivers(17, false)
	, m_refCount(0)
	, lookupDriversNeeded(true)
{
	m_drivers.setAutoDelete(true);
	m_serverResultNum=0;

}

MigrateManagerInternal::~MigrateManagerInternal()
{
	KexiDBDbg << "MigrateManagerInternal::~MigrateManagerInternal()" << endl;
	m_drivers.clear();
	if ( s_self == this )
		s_self = 0;
	KexiDBDbg << "MigrateManagerInternal::~MigrateManagerInternal() ok" << endl;
}

void MigrateManagerInternal::slotAppQuits()
{
	if (qApp->mainWidget() && qApp->mainWidget()->isVisible())
	    return; //what a hack! - we give up when app is still there
	KexiDBDbg << "MigrateManagerInternal::slotAppQuits(): let's clear drivers..." << endl;
	m_drivers.clear();
}

MigrateManagerInternal *MigrateManagerInternal::self()
{
	if (!s_self)
		s_self = new MigrateManagerInternal();

	return s_self;
}

bool MigrateManagerInternal::lookupDrivers()
{
	if (!lookupDriversNeeded)
		return true;

	if (qApp) {
		connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(slotAppQuits()));
	}
//TODO: for QT-only version check for KInstance wrapper
//		KexiDBWarn << "DriverManagerInternal::lookupDrivers(): cannot work without KInstance (KGlobal::instance()==0)!" << endl;
//		setError("Driver Manager cannot work without KInstance (KGlobal::instance()==0)!");

	lookupDriversNeeded = false;
	clearError();
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/MigrationDriver");
	KTrader::OfferList::ConstIterator it(tlist.constBegin());
	for(; it != tlist.constEnd(); ++it)
	{
		KService::Ptr ptr = (*it);
		QString srv_name = ptr->property("X-Kexi-MigrationDriverName").toString();
		if (srv_name.isEmpty()) {
			KexiDBWarn << "MigrateManagerInternal::lookupDrivers(): "
				"X-Kexi-MigrationDriverName must be set for migration driver \"" 
				<< ptr->property("Name").toString() << "\" service!\n -- skipped!" << endl;
			continue;
		}
		if (m_services_lcase.contains(srv_name.lower())) {
			continue;
		}

//! @todo could be merged. Copied from KexiDB::DriverManager.
//<COPIED>
		QString srv_ver_str = ptr->property("X-Kexi-KexiMigrationVersion").toString();
		QStringList lst( QStringList::split(".", srv_ver_str) );
		int minor_ver, major_ver;
		bool ok = (lst.count() == 2);
		if (ok)
			major_ver = lst[0].toUInt(&ok);
		if (ok)
			minor_ver = lst[1].toUInt(&ok);
		if (!ok) {
			KexiDBWarn << "MigrateManagerInternal::lookupDrivers(): problem with detecting '"
			<< srv_name.lower() << "' driver's version -- skipping it!" << endl;
			possibleProblems += QString("\"%1\" migration driver has unrecognized version; "
				"required driver version is \"%2.%3\"")
				.arg(srv_name.lower())
				.arg(KexiMigration::versionMajor()).arg(KexiMigration::versionMinor());
			continue;
		}
		if (major_ver != KexiMigration::versionMajor() || minor_ver != KexiMigration::versionMinor()) {
			KexiDBWarn << QString("MigrateManagerInternal::lookupDrivers(): '%1' driver" 
				" has version '%2' but required migration driver version is '%3.%4'\n"
				" -- skipping this driver!").arg(srv_name.lower()).arg(srv_ver_str)
				.arg(KexiMigration::versionMajor()).arg(KexiMigration::versionMinor()) << endl;
			possibleProblems += QString("\"%1\" migration driver has version \"%2\" "
				"but required driver version is \"%3.%4\"")
				.arg(srv_name.lower()).arg(srv_ver_str)
				.arg(KexiMigration::versionMajor()).arg(KexiMigration::versionMinor());
			continue;
		}
//</COPIED>

		QString mime = ptr->property("X-Kexi-FileDBDriverMime").toString().lower();
		QString drvType = ptr->property("X-Kexi-MigrationDriverType").toString().lower();
		if (drvType=="file") {
			if (!mime.isEmpty()) {
				if (!m_services_by_mimetype.contains(mime)) {
					m_services_by_mimetype.insert(mime, ptr);
				}
				else {
					KexiDBWarn << "MigrateManagerInternal::lookupDrivers(): more than one driver for '" 
						<< mime << "' mime type!" << endl;
				}
			}
		}
		m_services.insert(srv_name, ptr);
		m_services_lcase.insert(srv_name.lower(), ptr);
		KexiDBDbg << "MigrateManager::lookupDrivers(): registered driver: " << ptr->name() 
			<< "(" << ptr->library() << ")" << endl;
	}

	if (tlist.isEmpty())
	{
		setError(ERR_DRIVERMANAGER, i18n("Could not find any import/export database drivers.") );
		return false;
	}
	return true;
}

KexiMigrate* MigrateManagerInternal::driver(const QString& name)
{
	if (!lookupDrivers())
		return 0;
	
	clearError();
	KexiDBDbg << "MigrationrManagerInternal::migrationDriver(): loading " << name << endl;

	KexiMigrate *drv = name.isEmpty() ? 0 : m_drivers.find(name.latin1());
	if (drv)
		return drv; //cached

	if (!m_services_lcase.contains(name.lower())) {
		setError(ERR_DRIVERMANAGER, i18n("Could not find import/export database driver \"%1\".").arg(name) );
		return 0;
	}

	KService::Ptr ptr= *(m_services_lcase.find(name.lower()));
	QString srv_name = ptr->property("X-Kexi-MigrationDriverName").toString();

	KexiDBDbg << "MigrateManagerInternal::driver(): library: "<<ptr->library()<<endl;
	drv = KParts::ComponentFactory::createInstanceFromService<KexiMigrate>(ptr,
		this, srv_name.latin1(), QStringList(),&m_serverResultNum);

	if (!drv) {
		setError(ERR_DRIVERMANAGER, i18n("Could not load import/export database driver \"%1\".")
				.arg(name) );
		if (m_componentLoadingErrors.isEmpty()) {//fill errtable on demand
			m_componentLoadingErrors[KParts::ComponentFactory::ErrNoServiceFound]="ErrNoServiceFound";
			m_componentLoadingErrors[KParts::ComponentFactory::ErrServiceProvidesNoLibrary]="ErrServiceProvidesNoLibrary";
			m_componentLoadingErrors[KParts::ComponentFactory::ErrNoLibrary]="ErrNoLibrary";
			m_componentLoadingErrors[KParts::ComponentFactory::ErrNoFactory]="ErrNoFactory";
			m_componentLoadingErrors[KParts::ComponentFactory::ErrNoComponent]="ErrNoComponent";
		}
		m_serverResultName=m_componentLoadingErrors[m_serverResultNum];
		return 0;
	}
	KexiDBDbg << "MigrateManagerInternal::driver(): loading succeed: " << name <<endl;
	KexiDBDbg << "drv="<<(long)drv <<endl;

//	drv->setName(srv_name.latin1());
//	drv->d->service = ptr; //store info
//	drv->d->fileDBDriverMimeType = ptr->property("X-Kexi-FileDBDriverMime").toString();
//	drv->d->initInternalProperties();

	if (!drv->isValid()) {
		setError(drv);
		delete drv;
		return 0;
	}

	m_drivers.insert(name.latin1(), drv); //cache it
	return drv;
}

void MigrateManagerInternal::incRefCount()
{
	m_refCount++;
	KexiDBDbg << "MigrateManagerInternal::incRefCount(): " << m_refCount << endl;
}

void MigrateManagerInternal::decRefCount()
{
	m_refCount--;
	KexiDBDbg << "MigrateManagerInternal::decRefCount(): " << m_refCount << endl;
//	if (m_refCount<1) {
//		KexiDBDbg<<"KexiDB::DriverManagerInternal::decRefCount(): reached m_refCount<1 -->deletelater()"<<endl;
//		s_self=0;
//		deleteLater();
//	}
}

// ---------------------------
// --- DriverManager impl. ---
// ---------------------------

MigrateManager::MigrateManager()
	: QObject( 0, "KexiMigrate::MigrateManager" )
	, Object()
	, d_int( MigrateManagerInternal::self() )
{
	d_int->incRefCount();
//	if ( !s_self )
//		s_self = this;
//	lookupDrivers();
}

MigrateManager::~MigrateManager()
{
	KexiDBDbg << "MigrateManager::~MigrateManager()" << endl;
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
	KexiDBDbg << "MigrateManager::~MigrateManager() ok" << endl;
}


const QStringList MigrateManager::driverNames()
{
	if (!d_int->lookupDrivers()) {
		kdDebug() << "MigrateManager::driverNames() lookupDrivers failed" << endl;
		return QStringList();
	}
	
	if (d_int->m_services.isEmpty()) {
		kdDebug() << "MigrateManager::driverNames() MigrateManager::ServicesMap is empty" << endl;
		return QStringList();
	}

	if (d_int->error()) {
		kdDebug() << "MigrateManager::driverNames() Error: " << d_int->errorMsg() << endl;
		return QStringList();
	}

	return d_int->m_services.keys();
}

QString MigrateManager::driverForMimeType(const QString &mimeType)
{
	if (!d_int->lookupDrivers()) {
		kdDebug() << "MigrateManager::driverForMimeType() lookupDrivers() failed" << endl;
		setError(d_int);
		return 0;
	}
	
	KService::Ptr ptr = d_int->m_services_by_mimetype[mimeType.lower()];
	if (!ptr) {
		kdDebug() << QString("MigrateManager::driverForMimeType(%1) No such mimetype").arg(mimeType) << endl;
		return QString::null;
	}

	return ptr->property("X-Kexi-MigrationDriverName").toString();
}

KexiMigrate* MigrateManager::driver(const QString& name)
{
	KexiMigrate *drv = d_int->driver(name);
	if (d_int->error()) {
		kdDebug() << QString("MigrateManager::driver(%1) Error: %2").arg(name).arg(d_int->errorMsg()) << endl;
		setError(d_int);
	}
	return drv;
}

QString MigrateManager::serverErrorMsg()
{
	return d_int->m_serverErrMsg;
}

int MigrateManager::serverResult()
{
	return d_int->m_serverResultNum;
}

QString MigrateManager::serverResultName()
{
	return d_int->m_serverResultName;
}

void MigrateManager::drv_clearServerResult()
{
	d_int->m_serverErrMsg=QString::null;
	d_int->m_serverResultNum=0;
	d_int->m_serverResultName=QString::null;
}

QString MigrateManager::possibleProblemsInfoMsg() const
{
	if (d_int->possibleProblems.isEmpty())
		return QString::null;
	QString str;
	str.reserve(1024);
	str = "<ul>";
	for (QStringList::ConstIterator it = d_int->possibleProblems.constBegin();
		it!=d_int->possibleProblems.constEnd(); ++it)
	{
		str += (QString::fromLatin1("<li>") + *it + QString::fromLatin1("</li>"));
	}
	str += "</ul>";
	return str;
}

//------------------------

int KexiMigration::versionMajor()
{
	return KEXI_MIGRATION_VERSION_MAJOR;
}

int KexiMigration::versionMinor()
{
	return KEXI_MIGRATION_VERSION_MINOR;
}

#include "migratemanager_p.moc"
