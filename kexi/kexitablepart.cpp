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
#include <klineeditdlg.h>

#include "kexitablepart.h"
#include "kexiprojectpartitem.h"

KexiTablePart::KexiTablePart(KexiProject *project)
 : KexiProjectPart(project)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;

	m_project = project;
	m_items = new ItemList();

	getTables();
}

QString
KexiTablePart::name()
{
	return QString(i18n("Tables"));
}

QString
KexiTablePart::mime()
{
	return QString("kexi/table");
}

bool
KexiTablePart::visible()
{
	return true;
}

KexiPartPopupMenu*
KexiTablePart::groupContext()
{
	kdDebug() << "KexiTablePart::groupContext()" << endl;
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Talbe"), SLOT(slotCreate()));

	return m;
}

KexiPartPopupMenu*
KexiTablePart::itemContext()
{
	kdDebug() << "KexiTablePart::itemContext()" << endl;
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Table"), SLOT(slotOpen()));
	m->insertAction(i18n("Alter Table"), SLOT(slotAlter()));
	m->insertAction(i18n("Delete Table"), SLOT(slotDrop()));
	
	return m;
}

/*
KexiBrowserItem*
KexiTablePart::group()
{
	return 0;
}


KexiBrowserItem*
KexiTablePart::itemTemplate()
{
	return 0;
}
*/

ItemList*
KexiTablePart::items()
{
	return m_items;
}


QPixmap
KexiTablePart::groupPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("tables"), KIcon::Small);
//	return QPixmap();
}

QPixmap
KexiTablePart::itemPixmap()
{
	return QPixmap();
}


void
KexiTablePart::open(QString identifier)
{
}

KexiDialogBase*
KexiTablePart::view(QString identifier)
{
}

void
KexiTablePart::getTables()
{
	QStringList tables = m_project->db()->tables();

	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it )
	{
		kdDebug() << "KexiTablePart::getTables() added " << (*it) << endl;
		m_items->append(new KexiProjectPartItem(this, (*it), "kexi/table", (*it)));
	}

	emit itemListChanged(this);
}

void
KexiTablePart::slotCreate()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New Table"), i18n("Table Name:"), "", &ok, 0);

	if(ok && name.length() > 0)
	{
//		if(m_view->project()->db()->query("CREATE TABLE " + name + " (id INT(10))"))
//		{
//			KexiAlterTable* kat = new KexiAlterTable(m_view, 0,name, "alterTable");
//			kat->show();
//			KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, parent, name);
//			item->setPixmap(0, iconLoader->loadIcon("table", KIcon::Small));
//		}
	}
}

KexiTablePart::~KexiTablePart()
{
}

#include "kexitablepart.moc"
