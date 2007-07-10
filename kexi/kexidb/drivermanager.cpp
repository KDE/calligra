/* This file is part of the KDE project
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kexidb/drivermanager.h>
#include <kexidb/drivermanager_p.h>

#include <kexidb/driver.h>
#include <kexidb/driver_p.h>
#include <kexidb/error.h>

#include <klibloader.h>
#include <kservicetypetrader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kservice.h>

#include <assert.h>

#include <qapplication.h>

//remove debug
#undef KexiDBDbg
#define KexiDBDbg if (0) kDebug()

using namespace KexiDB;

DriverManagerInternal* DriverManagerInternal::s_self = 0L;


DriverManagerInternal::DriverManagerInternal() /* protected */
	: QObject( 0 )
	, Object()
	, m_refCount(0)
	, lookupDriversNeeded(true)
{
	setObjectName( "KexiDB::DriverManager" );
	m_serverResultNum=0;
}

DriverManagerInternal::~DriverManagerInternal()
{
	KexiDBDbg << "DriverManagerInternal::~DriverManagerInternal()" << endl;
	qDeleteAll(m_drivers);
	if ( s_self == this )
		s_self = 0;
	KexiDBDbg << "DriverManagerInternal::~DriverManagerInternal() ok" << endl;
}

void DriverManagerInternal::slotAppQuits()
{
	if (qApp && !qApp->topLevelWidgets().isEmpty()
		&& qApp->topLevelWidgets().first()->isVisible())
	{
		return; //what a hack! - we give up when app is still there
	}
	KexiDBDbg << "DriverManagerInternal::slotAppQuits(): let's clear drivers..." << endl;
	qDeleteAll(m_drivers);
}

DriverManagerInternal *DriverManagerInternal::self()
{
	if (!s_self)
		s_self = new DriverManagerInternal();

	return s_self;
}

bool DriverManagerInternal::lookupDrivers()
{
	if (!lookupDriversNeeded)
		return true;

	if (qApp) {
		connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(slotAppQuits()));
	}
//TODO: for QT-only version check for KComponentData wrapper
//		KexiDBWarn << "DriverManagerInternal::lookupDrivers(): cannot work without KComponentData (KGlobal::mainComponent()==0)!" << endl;
//		setError("Driver Manager cannot work without KComponentData (KGlobal::mainComponent()==0)!");

	lookupDriversNeeded = false;
	clearError();
	KService::List tlist = KServiceTypeTrader::self()->query("Kexi/DBDriver");
	KService::List::ConstIterator it(tlist.constBegin());
	for(; it != tlist.constEnd(); ++it)
	{
		KService::Ptr ptr = (*it);
		if (!ptr->property("Library").toString().startsWith("kexidb_")) {
			KexiDBWarn << "DriverManagerInternal::lookupDrivers():"
				" X-KDE-Library == " << ptr->property("Library").toString()
				<< ": no \"kexidb_\" prefix -- skipped to avoid potential conflicts!" << endl;
			continue;
		}
		QString srv_name = ptr->property("X-Kexi-DriverName").toString().toLower();
		if (srv_name.isEmpty()) {
			KexiDBWarn << "DriverManagerInternal::lookupDrivers():"
				" X-Kexi-DriverName must be set for KexiDB driver \""
				<< ptr->property("Name").toString() << "\" service!\n -- skipped!" << endl;
			continue;
		}
		if (m_services_lcase.contains( srv_name ) ) {
			KexiDBWarn << "DriverManagerInternal::lookupDrivers(): more than one driver named '"
				<< srv_name << "'\n -- skipping this one!" << endl;
			continue;
		}

		QString srv_ver_str = ptr->property("X-Kexi-KexiDBVersion").toString();
		QStringList lst( srv_ver_str.split( "." ) );
		uint minor_ver, major_ver;
		bool ok = (lst.count() == 2);
		if (ok)
			major_ver = lst[0].toUInt(&ok);
		if (ok)
			minor_ver = lst[1].toUInt(&ok);
		if (!ok) {
			KexiDBWarn << "DriverManagerInternal::lookupDrivers(): problem with detecting '"
			<< srv_name << "' driver's version -- skipping it!" << endl;
			continue;
		}
		if (major_ver != KexiDB::version().major || minor_ver != KexiDB::version().minor) {
			KexiDBWarn << QString("DriverManagerInternal::lookupDrivers(): '%1' driver"
				" has version '%2' but required KexiDB driver version is '%3.%4'\n"
				" -- skipping this driver!").arg( srv_name ).arg( srv_ver_str )
				.arg(KexiDB::version().major).arg(KexiDB::version().minor) << endl;
			possibleProblems += QString("\"%1\" database driver has version \"%2\" "
				"but required driver version is \"%3.%4\"")
				.arg( srv_name ).arg( srv_ver_str )
				.arg(KexiDB::version().major).arg(KexiDB::version().minor);
			continue;
		}

		QString drvType = ptr->property("X-Kexi-DriverType").toString().toLower();
		if (drvType=="file") {
			//new property: a list of supported mime types
			QStringList mimes( ptr->property("X-Kexi-FileDBDriverMimeList").toStringList() );
			//single mime is obsolete, but we're handling it:
			{
				QString mime( ptr->property("X-Kexi-FileDBDriverMime").toString().toLower() );
				if (!mime.isEmpty())
					mimes.append( mime );
			}

			//store association of this driver with all listed mime types
			for (QStringList::ConstIterator mime_it = mimes.constBegin(); mime_it!=mimes.constEnd(); ++mime_it) {
				QString mime( (*mime_it).toLower() );
				if (!m_services_by_mimetype.contains(mime)) {
					m_services_by_mimetype.insert(mime, ptr);
				}
				else {
					KexiDBWarn << "DriverManagerInternal::lookupDrivers(): more than one driver for '"
					<< mime << "' mime type!" << endl;
				}
			}
		}
		m_services.insert( srv_name, ptr );
		m_services_lcase.insert( srv_name,  ptr );
		KexiDBDbg << "KexiDB::DriverManager::lookupDrivers(): registered driver: "
		          << ptr->name() << "(" << ptr->library() << ")" << endl;
	}

	if (tlist.isEmpty())
	{
		setError(ERR_DRIVERMANAGER, i18n("Could not find any database drivers.") );
		return false;
	}
	return true;
}

KexiDB::Driver::Info DriverManagerInternal::driverInfo(const QString &name)
{
	KexiDB::Driver::Info i = m_driversInfo[name.toLower()];
	if (!error() && i.name.isEmpty())
		setError(ERR_DRIVERMANAGER, i18n("Could not find database driver \"%1\".", name) );
	return i;
}

Driver* DriverManagerInternal::driver(const QString& name)
{
	if (!lookupDrivers())
		return 0;

	clearError();
	KexiDBDbg << "DriverManagerInternal::driver(): loading " << name << endl;

	Driver *drv = 0;
	if (!name.isEmpty())
		drv = m_drivers.value( name.toLower() );
	if (drv)
		return drv; //cached

	if (!m_services_lcase.contains( name.toLower() )) {
		setError(ERR_DRIVERMANAGER, i18n("Could not find database driver \"%1\".", name) );
		return 0;
	}

	KService::Ptr ptr= *(m_services_lcase.find( name.toLower() ));
	QString srv_name = ptr->property("X-Kexi-DriverName").toString();

	KexiDBDbg << "KexiDBInterfaceManager::driver(): library: "<<ptr->library()<<endl;
	drv = KService::createInstance<KexiDB::Driver>(ptr,
		this,
		QStringList(),
		&m_serverResultNum);

	if (!drv) {
		setError(ERR_DRIVERMANAGER, i18n("Could not load database driver \"%1\".", name) );
		if (m_componentLoadingErrors.isEmpty()) {//fill errtable on demand
			m_componentLoadingErrors[KLibLoader::ErrNoServiceFound]="ErrNoServiceFound";
			m_componentLoadingErrors[KLibLoader::ErrServiceProvidesNoLibrary]="ErrServiceProvidesNoLibrary";
			m_componentLoadingErrors[KLibLoader::ErrNoLibrary]="ErrNoLibrary";
			m_componentLoadingErrors[KLibLoader::ErrNoFactory]="ErrNoFactory";
			m_componentLoadingErrors[KLibLoader::ErrNoComponent]="ErrNoComponent";
		}
		m_serverResultName=m_componentLoadingErrors[m_serverResultNum];
		return 0;
	}
	KexiDBDbg << "DriverManagerInternal::driver(): loading succeed: " << name <<endl;
//	KexiDBDbg << "drv="<<(long)drv <<endl;

	drv->setObjectName( srv_name );
	drv->d->service = ptr.data(); //store info
	drv->d->fileDBDriverMimeType = ptr->property("X-Kexi-FileDBDriverMime").toString();
	drv->d->initInternalProperties();

	if (!drv->isValid()) {
		setError(drv);
		delete drv;
		return 0;
	}
	m_drivers.insert(name.lower(), drv); //cache it
	return drv;
}

void DriverManagerInternal::incRefCount()
{
	m_refCount++;
	KexiDBDbg << "DriverManagerInternal::incRefCount(): " << m_refCount << endl;
}

void DriverManagerInternal::decRefCount()
{
	m_refCount--;
	KexiDBDbg << "DriverManagerInternal::decRefCount(): " << m_refCount << endl;
//	if (m_refCount<1) {
//		KexiDBDbg<<"KexiDB::DriverManagerInternal::decRefCount(): reached m_refCount<1 -->deletelater()"<<endl;
//		s_self=0;
//		deleteLater();
//	}
}

void DriverManagerInternal::aboutDelete( Driver* drv )
{
	m_drivers.remove( drv->name() );
}



// ---------------------------
// --- DriverManager impl. ---
// ---------------------------

DriverManager::DriverManager()
	: QObject( 0 )
	, Object()
	, d_int( DriverManagerInternal::self() )
{
	setObjectName("KexiDB::DriverManager");
	d_int->incRefCount();
//	if ( !s_self )
//		s_self = this;
//	lookupDrivers();
}

DriverManager::~DriverManager()
{
	KexiDBDbg << "DriverManager::~DriverManager()" << endl;
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
	KexiDBDbg << "DriverManager::~DriverManager() ok" << endl;
}

const KexiDB::Driver::InfoMap DriverManager::driversInfo()
{
	if (!d_int->lookupDrivers())
		return KexiDB::Driver::InfoMap();

	if (!d_int->m_driversInfo.isEmpty())
		return d_int->m_driversInfo;
	ServicesMap::ConstIterator it;
	for ( it=d_int->m_services.constBegin() ; it != d_int->m_services.constEnd(); ++it ) {
		Driver::Info info;
		KService::Ptr ptr = it.value();
		info.name = ptr->property("X-Kexi-DriverName").toString();
		info.caption = ptr->property("Name").toString();
		info.comment = ptr->property("Comment").toString();
		if (info.caption.isEmpty())
			info.caption = info.name;
		info.fileBased = (ptr->property("X-Kexi-DriverType").toString().toLower()=="file");
		if (info.fileBased)
			info.fileDBMimeType = ptr->property("X-Kexi-FileDBDriverMime").toString().toLower();
		QVariant v = ptr->property("X-Kexi-DoNotAllowProjectImportingTo");
		info.allowImportingTo = v.isNull() ? true : !v.toBool();
		d_int->m_driversInfo.insert(info.name.toLower(), info);
	}
	return d_int->m_driversInfo;
}

const QStringList DriverManager::driverNames()
{
	if (!d_int->lookupDrivers())
		return QStringList();

	if (d_int->m_services.isEmpty() && d_int->error())
		return QStringList();
	return d_int->m_services.keys();
}

KexiDB::Driver::Info DriverManager::driverInfo(const QString &name)
{
	driversInfo();
	KexiDB::Driver::Info i = d_int->driverInfo(name);
	if (d_int->error())
		setError(d_int);
	return i;
}

KService::Ptr DriverManager::serviceInfo(const QString &name)
{
	if (!d_int->lookupDrivers()) {
		setError(d_int);
		return KService::Ptr();
	}

	clearError();
	if (d_int->m_services_lcase.contains( name.toLower() ) ) {
		return *d_int->m_services_lcase.find( name.toLower() );
	} else {
		setError(ERR_DRIVERMANAGER, i18n("No such driver service: \"%1\".", name) );
		return KService::Ptr();
	}
}

const DriverManager::ServicesMap& DriverManager::services()
{
	d_int->lookupDrivers();
	return d_int->m_services;
}

QString DriverManager::lookupByMime(const QString &mimeType)
{
	if (!d_int->lookupDrivers()) {
		setError(d_int);
		return 0;
	}

	KService::Ptr ptr = d_int->m_services_by_mimetype[mimeType.toLower()];
	if (!ptr)
		return QString();
	return ptr->property("X-Kexi-DriverName").toString();
}

Driver* DriverManager::driver(const QString& name)
{
	Driver *drv = d_int->driver(name);
	if (d_int->error())
		setError(d_int);
	return drv;
}

QString DriverManager::serverErrorMsg()
{
	return d_int->m_serverErrMsg;
}

int DriverManager::serverResult()
{
	return d_int->m_serverResultNum;
}

QString DriverManager::serverResultName()
{
	return d_int->m_serverResultName;
}

void DriverManager::drv_clearServerResult()
{
	d_int->m_serverErrMsg.clear();
	d_int->m_serverResultNum=0;
	d_int->m_serverResultName.clear();
}

QString DriverManager::possibleProblemsInfoMsg() const
{
	if (d_int->possibleProblems.isEmpty())
		return QString();
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

#include "drivermanager_p.moc"

