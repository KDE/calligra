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
	for(KTrader::OfferList::Iterator it(tlist.begin()); it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		kdDebug() << "Manager::lookup(): " << ptr->property("X-Kexi-TypeMime").toString() << endl;
		Info *info = new Info(ptr);
		m_partsByMime.insert(ptr->property("X-Kexi-TypeMime").toString(), info);
		m_partlist.append(info);
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


void
Manager::checkProject(KexiDB::Connection *conn)
{
//TODO: catch errors!
	if(!conn->isDatabaseUsed())
		return;

	KexiDB::Cursor *cursor = conn->executeQuery("SELECT * FROM kexi__parts", KexiDB::Cursor::Buffered);
	if(!cursor)
		return;

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
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
		}
	}

	conn->deleteCursor(cursor);

	return;
}

#include "kexipartmanager.moc"

