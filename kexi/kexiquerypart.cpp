/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qpixmap.h>

#include <koApplication.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>

#include "kexiquerypart.h"

KexiQueryPart::KexiQueryPart(KexiProject *project)
 : KexiProjectPart(project)
{
	kdDebug() << "KexiQueryPart::KexiQueryPart()" << endl;
}

QString
KexiQueryPart::name()
{
	return QString("Queries");
}

QString
KexiQueryPart::mime()
{
	return QString("kexi/query");
}

bool
KexiQueryPart::visible()
{
	return true;
}

KexiPartPopupMenu*
KexiQueryPart::groupContext(KexiView* view)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Query"), SLOT(slotCreateQuery()));
	setCurrentView(view);

	return m;
}

KexiPartPopupMenu*
KexiQueryPart::itemContext(KexiView* view)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Query"), SLOT(slotOpenQuery()));
	m->insertAction(i18n("Delete Query"), SLOT(slotDeleteQuery()));
	setCurrentView(view);
	
	return m;
}

/*
KexiBrowserItem*
KexiQueryPart::group()
{
	return 0;
}


KexiBrowserItem*
KexiQueryPart::itemTemplate()
{
	return 0;
}
*/

ItemList*
KexiQueryPart::items()
{
	return new ItemList();
}


QPixmap
KexiQueryPart::groupPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("queries"), KIcon::Small);
//	return QPixmap();
}

QPixmap
KexiQueryPart::itemPixmap()
{
	return QPixmap();
}

void
KexiQueryPart::executeItem(KexiView* view, QString identifier)
{
	setCurrentView(view);
}

#include "kexiquerypart.moc"
