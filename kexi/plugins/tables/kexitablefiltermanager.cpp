/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qvariant.h>

#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#include <kdebug.h>

#include "kexitablefiltermanager.h"
#include "kexitablefilter.h"

KexiTableFilterManager::KexiTableFilterManager(QObject *parent, const char *name)
 : QObject(parent, name)
{
	lookupFilters();
}

void
KexiTableFilterManager::lookupFilters()
{
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/TableFilter");
	KTrader::OfferList::Iterator it(tlist.begin());
	for(; it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);

		kdDebug() << "KexiTableFilterManager::lookupFilters(): found driver: " << ptr->name() << endl;
		kdDebug() << "KexiTableFilterManager::lookupFilters(): type: " << ptr->property("X-Kexi-Direction").toString() << endl;

		KexiTableFilterMeta *meta = new KexiTableFilterMeta(ptr);

		if(ptr->property("X-Kexi-Direction").toString() == "import")
			m_import.append(meta);
		else
			m_export.append(meta);
	}
}

KexiTableFilter *
KexiTableFilterManager::getFilter(KexiTableFilterMeta *m)
{
	if(!m || !m->service())
		return 0;

	KexiTableFilter *f = KParts::ComponentFactory::createInstanceFromService<KexiTableFilter>(m->service(),
	 this, "filter", QStringList());

	return f;
}

KexiTableFilterManager::~KexiTableFilterManager()
{
}

//META
KexiTableFilterMeta::KexiTableFilterMeta(KService::Ptr &s)
{
//	m_lib = s->library();
	m_comment = s.data()->comment();
	m_service = s;
}

KexiTableFilterMeta::~KexiTableFilterMeta()
{
}

#include "kexitablefiltermanager.moc"
