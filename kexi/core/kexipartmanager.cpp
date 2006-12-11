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

#include <klibloader.h>
#include <ktrader.h>
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
	m_partlist.setAutoDelete(true);
	m_partsByMime.setAutoDelete(false);
	m_parts.setAutoDelete(false);//KApp will remove parts
	m_nextTempProjectPartID = -1;
}

void
Manager::lookup()
{
//js: TODO: allow refreshing!!!! (will need calling removeClient() by Part objects)
	if (m_lookupDone)
		return;
	m_lookupDone = true;
	m_partlist.clear();
	m_partsByMime.clear();
	m_parts.clear();
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/Handler", 
		"[X-Kexi-PartVersion] == " + QString::number(KEXI_PART_VERSION));
	
	KConfig conf("kexirc", true);
	conf.setGroup("Parts");
	QStringList sl_order = QStringList::split( ",", conf.readEntry("Order") );//we'll set parts in defined order
	const int size = QMAX( tlist.count(), sl_order.count() );
	QPtrVector<KService> ordered( size*2 );
	int offset = size; //we will insert not described parts from #offset
	
	//compute order
	for(KTrader::OfferList::ConstIterator it(tlist.constBegin()); it != tlist.constEnd(); ++it)
	{
		KService::Ptr ptr = (*it);
		QCString mime = ptr->property("X-Kexi-TypeMime").toCString();
		kdDebug() << "Manager::lookup(): " << mime << endl;
//<TEMP>: disable some parts if needed
		if (!Kexi::tempShowForms() && mime=="kexi/form")
			continue;
		if (!Kexi::tempShowReports() && mime=="kexi/report")
			continue;
		if (!Kexi::tempShowMacros() && mime=="kexi/macro")
			continue;
		if (!Kexi::tempShowScripts() && mime=="kexi/script")
			continue;
//</TEMP>
		int idx = sl_order.findIndex( ptr->library() );
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
				kdDebug() << "Manager::lookup(): inserting info to " << info->mimeType() << endl;
			}
			m_partlist.append(info);
		}
	}
}

Manager::~Manager()
{
}

Part *
Manager::part(Info *i)
{
	clearError();
	if(!i)
		return 0;

//	kdDebug() << "Manager::part( id = " << i->projectPartID() << " )" << endl;

	if (i->isBroken()) {
			setError(i->errorMessage());
			return 0;
	}

	Part *p = m_parts[i->projectPartID()];
	
	if(!p) {
//		kdDebug() << "Manager::part().." << endl;
		int error=0;
		p = KParts::ComponentFactory::createInstanceFromService<Part>(i->ptr(), this, 
			QString(i->objectName()+"_part").latin1(), QStringList(), &error);
		if(!p) {
			kdDebug() << "Manager::part(): failed :( (ERROR #" << error << ")" << endl;
			kdDebug() << "  " << KLibLoader::self()->lastErrorMessage() << endl;
			i->setBroken(true, i18n("Error while loading plugin \"%1\"").arg(i->objectName()));
			setError(i->errorMessage());
			return 0;
		}
		if (p->m_registeredPartID>0) {
			i->setProjectPartID( p->m_registeredPartID );
		}

		p->setInfo(i);
		m_parts.insert(i->projectPartID(),p);
		emit partLoaded(p);
	}
	else {
//		kdDebug() << "Manager::part(): cached: " << i->groupName() << endl;
	}

//	kdDebug() << "Manager::part(): fine!" << endl;
	return p;
}

#if 0
void
Manager::unloadPart(Info *i)
{
	m_parts.setAutoDelete(true);
	m_parts.remove(i->projectPartID());
	m_parts.setAutoDelete(false);
/*	if (!p)
		return;
	m_partsByMime.take(i->mime());
	m_partlist.removeRef(p);*/
}

void 
Manager::unloadAllParts()
{
//	m_partsByMime.clear();
	m_parts.setAutoDelete(true);
	m_parts.clear();
	m_parts.setAutoDelete(false);
//	m_partlist.clear();
}
#endif

/*void 
Manager::removeClients( KexiMainWindow *win )
{
	if (!win)
		return;
	QIntDictIterator<Part> it(m_parts);
	for (;i.current();++it) {
		i.current()->removeClient(win->guiFactory());
	}
}*/

Part *
Manager::partForMimeType(const QString &mimeType)
{
	return mimeType.isEmpty() ? 0 : part(m_partsByMime[mimeType.latin1()]);
}

Info *
Manager::infoForMimeType(const QString &mimeType)
{
	Info *i = mimeType.isEmpty() ? 0 : m_partsByMime[mimeType.latin1()];
	if (i)
		return i;
	setError(i18n("No plugin for mime type \"%1\"").arg(mimeType));
	return 0;
}


bool
Manager::checkProject(KexiDB::Connection *conn)
{
	clearError();
//	QString errmsg = i18n("Invalid project contents.");

//TODO: catch errors!
	if(!conn->isDatabaseUsed()) {
		setError(conn);
		return false;
	}

	KexiDB::Cursor *cursor = conn->executeQuery("SELECT * FROM kexi__parts");//, KexiDB::Cursor::Buffered);
	if(!cursor) {
		setError(conn);
		return false;
	}

//	int id=0;
//	QStringList parts_found;
	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
//		id++;
		Info *i = infoForMimeType(cursor->value(2).toCString());
		if(!i)
		{
			Missing m;
			m.name = cursor->value(1).toString();
			m.mime = cursor->value(2).toCString();
			m.url = cursor->value(3).toString();

			m_missing.append(m);
		}
		else
		{
			i->setProjectPartID(cursor->value(0).toInt());
			i->setIdStoredInPartDatabase(true);
//			parts_found+=cursor->value(2).toString();
		}
	}

	conn->deleteCursor(cursor);

#if 0 //js: moved to Connection::createDatabase()
	//add missing default part entries
	KexiDB::TableSchema *ts = conn->tableSchema("kexi__parts");
	if (!ts)
		return false;
	KexiDB::FieldList *fl = ts->subList("p_id", "p_name", "p_mime", "p_url");
	if (!fl)
		return false;
	if (!parts_found.contains("kexi/table")) {
		if (!conn->insertRecord(*fl, QVariant(1), QVariant("Tables"), QVariant("kexi/table"), QVariant("http://")))
			return false;
	}
	if (!parts_found.contains("kexi/query")) {
		if (!conn->insertRecord(*fl, QVariant(2), QVariant("Queries"), QVariant("kexi/query"), QVariant("http://")))
			return false;
	}
#endif
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
