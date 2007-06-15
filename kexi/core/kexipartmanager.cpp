/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <KLibLoader>
#include <KServiceType>
#include <KMimeTypeTrader>
#include <kdebug.h>
#include <kconfig.h>
#include <kparts/componentfactory.h>

#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexistaticpart.h"
#include "kexi_version.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

using namespace KexiPart;

Manager::Manager(QObject *parent)
 : QObject(parent)
{
	m_lookupDone = false;
	m_lookupResult = false;
	m_nextTempProjectPartID = -1;
}

Manager::~Manager()
{
	qDeleteAll(m_partlist);
}

bool Manager::lookup()
{
//js: TODO: allow refreshing!!!! (will need calling removeClient() by Part objects)
	if (m_lookupDone)
		return m_lookupResult;
	m_lookupDone = true;
	m_lookupResult = false;
	m_partlist.clear();
	m_partsByMime.clear();
	m_parts.clear();
	
	if (!KServiceType::serviceType("Kexi/Handler")) {
		kWarning() << "KexiPart::Manager::lookup(): No 'Kexi/Handler' service type installed! Aborting." << endl;
		setError(i18n("No \"%1\" service type installed! Check your Kexi installation. Aborting.",
			QString("Kexi/Handler")));
		return false;
	}
	KService::List tlist = KServiceTypeTrader::self()->query("Kexi/Handler", 
		"[X-Kexi-PartVersion] == " + QString::number(KEXI_PART_VERSION));

	KConfigGroup cg( KGlobal::config()->group("Parts") );
	QStringList sl_order = cg.readEntry("Order").split( "," );//we'll set parts in defined order
	const int size = qMax( tlist.count(), sl_order.count() );
	QList<KService::Ptr> ordered;
	int offset = size; //we will insert not described parts from #offset
	
	//compute order
	foreach(KService::Ptr ptr, tlist) {
		QString mime = ptr->property("X-Kexi-TypeMime").toString();
		kDebug() << "Manager::lookup(): " << mime << endl;
//<TEMP>: disable some parts if needed
		if (!Kexi::tempShowReports() && mime=="kexi/report")
			continue;
		if (!Kexi::tempShowMacros() && mime=="kexi/macro")
			continue;
		if (!Kexi::tempShowScripts() && mime=="kexi/script")
			continue;
//</TEMP>
		const int idx = sl_order.indexOf( ptr->library() );
		if (idx!=-1)
			ordered.insert(idx, ptr);
		else //add to end
			ordered.insert(offset++, ptr);
	}
	//fill final list using computed order
	for (int i = 0; i< (int)ordered.size(); i++) {
		KService::Ptr ptr = ordered[i];
		if (ptr) {
			Info *info = new Info(ptr);
			info->setProjectPartID(m_nextTempProjectPartID--); // temp. part id are -1, -2, and so on, 
			                                                   // to avoid duplicates
			if (!info->mimeType().isEmpty()) {
				m_partsByMime.insert(info->mimeType(), info);
				kDebug() << "Manager::lookup(): inserting info to " << info->mimeType() << endl;
			}
			m_partlist.append(info);
		}
	}
	m_lookupResult = true;
	return true;
}

Part* Manager::part(Info *i)
{
	clearError();
	if(!i)
		return 0;

	if (i->isBroken()) {
			setError(i->errorMessage());
			return 0;
	}

	Part *p = m_parts.value(i->projectPartID());
	if (!p) {
		int error=0;
		p = KService::createInstance<Part>(i->ptr(), this, QStringList(), &error);
		if(!p) {
			kDebug() << "Manager::part(): failed :( (ERROR #" << error << ")" << endl;
			kDebug() << "  " << KLibLoader::self()->lastErrorMessage() << endl;
			i->setBroken(true, i18n("Error while loading plugin \"%1\"", i->objectName()));
			setError(i->errorMessage());
			return 0;
		}
		if (p->registeredPartID() > 0) {
			i->setProjectPartID( p->registeredPartID() );
		}
		p->setInfo(i);
		p->setObjectName( QString("%1 part").arg(i->objectName()) );
		m_parts.insert(i->projectPartID(), p);
		emit partLoaded(p);
	}
	return p;
}

Part* Manager::partForMimeType(const QString &mimeType)
{
	return mimeType.isEmpty() ? 0 : part(m_partsByMime.value(mimeType));
}

Info* Manager::infoForMimeType(const QString &mimeType)
{
	Info *i = mimeType.isEmpty() ? 0 : m_partsByMime.value(mimeType);
	if (i)
		return i;
	setError(i18n("No plugin for mime type \"%1\"", mimeType));
	return 0;
}

bool Manager::checkProject(KexiDB::Connection *conn)
{
	clearError();
//	QString errmsg = i18n("Invalid project contents.");

//! @todo catch errors!
	if (!conn->isDatabaseUsed()) {
		setError(conn);
		return false;
	}

	KexiDB::Cursor *cursor = conn->executeQuery("SELECT * FROM kexi__parts"); 
	if (!cursor) {
		setError(conn);
		return false;
	}

	for (cursor->moveFirst(); !cursor->eof(); cursor->moveNext()) {
		Info *i = infoForMimeType(cursor->value(2).toString());
		if(!i) {
			Missing m;
			m.name = cursor->value(1).toString();
			m.mime = cursor->value(2).toString();
			m.url = cursor->value(3).toString();

			m_missing.append(m);
		}
		else {
			i->setProjectPartID(cursor->value(0).toInt());
			i->setIdStoredInPartDatabase(true);
		}
	}

	conn->deleteCursor(cursor);
	return true;
}

void Manager::insertStaticPart(StaticPart* part)
{
	if (!part)
		return;
	part->info()->setProjectPartID(m_nextTempProjectPartID--); // temp. part id are -1, -2, and so on, 
	m_partlist.append(part->info());
	if (!part->info()->mimeType().isEmpty())
		m_partsByMime.insert(part->info()->mimeType(), part->info());
	m_parts.insert(part->info()->projectPartID(), part);
}

#include "kexipartmanager.moc"
