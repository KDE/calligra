/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <klibloader.h>
#include <ktrader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kparts/componentfactory.h>

#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexi_global.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

using namespace KexiPart;

Manager::Manager(QObject *parent)
 : QObject(parent)
{
	m_partlist.setAutoDelete(true);
	m_partsByMime.setAutoDelete(false);
	m_parts.setAutoDelete(false);//KApp will remove parts
}

void
Manager::lookup()
{
	
	m_partlist.clear();
	m_partsByMime.clear();
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/Handler", "[X-Kexi-PartVersion] == " + QString::number(KEXI_PART_VERSION));
	
	KConfig conf("kexirc", true);
	conf.setGroup("Parts");
	QStringList sl_order = QStringList::split( ",", conf.readEntry("Order") );//we'll set parts in defined order
	const int size = QMAX( tlist.count(), sl_order.count() );
	QPtrVector<KService> ordered( size*2 );
	int offset = size; //we will insert not described parts from #offset
	
	//compute order
	for(KTrader::OfferList::Iterator it(tlist.begin()); it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		kdDebug() << "Manager::lookup(): " << ptr->property("X-Kexi-TypeMime").toString() << endl;
		
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
			m_partsByMime.insert(ptr->property("X-Kexi-TypeMime").toString(), info);
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
	if(!i || i->broken())
		return 0;

	kdDebug() << "Manager::part( id = " << i->projectPartID() << " )" << endl;

	Part *p = m_parts[i->projectPartID()];
	
	if(!p) {
		kdDebug() << "Manager::part().." << endl;
		p = KParts::ComponentFactory::createInstanceFromService<Part>(i->ptr(), this, 0, QStringList());
		if(!p) {
			kdDebug() << "Manager::part(): failed :(" << endl;
			i->setBroken(true);
			return 0;
		}
		p->setInfo(i);
		m_parts.insert(i->projectPartID(),p);
		emit partLoaded(p);
	}
	else {
		kdDebug() << "Manager::part(): cached: " << i->groupName() << endl;
	}

	kdDebug() << "Manager::part(): fine!" << endl;
	return p;
}

Part *
Manager::part(const QString &mime)
{
	return part(m_partsByMime[mime]);
}

Info *
Manager::info(const QString &mime)
{
	return m_partsByMime[mime];
}


bool
Manager::checkProject(KexiDB::Connection *conn)
{
//TODO: catch errors!
	if(!conn->isDatabaseUsed())
		return false;

	KexiDB::Cursor *cursor = conn->executeQuery("SELECT * FROM kexi__parts", KexiDB::Cursor::Buffered);
	if(!cursor)
		return false;

	int id=0;
//	QStringList parts_found;
	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		id++;
		Info *i = info(cursor->value(2).toString());
		if(!i)
		{
			Missing m;
			m.name = cursor->value(1).toString();
			m.mime = cursor->value(2).toString();
			m.url = cursor->value(3).toString();

			m_missing.append(m);
		}
		else
		{
			i->setProjectPartID(cursor->value(0).toInt());
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

#include "kexipartmanager.moc"

