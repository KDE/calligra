/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <qheader.h>
#include <qpoint.h>

#include <kiconloader.h>
#include <kdebug.h>

#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiproject.h"
//#include "kexihandlerpopupmenu.h"
#include "kexidialogbase.h"
#include "keximainwindow.h"

KexiBrowser::KexiBrowser(KexiMainWindow *parent, QString mime, KexiPart::Info *part, const char *name )
 : KListView(parent,name)
{
	m_mime = mime;
	m_part = part;
	m_parent = parent;

	header()->hide();

	addColumn("");
	setShowToolTips(true);
	setRootIsDecorated(true);
	setSorting(0);
	sort();
//	setResizeMode(QListView::LastColumn);

	connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotExecuteItem(QListViewItem*)));

	if(part)
	{
//		connect(part, SIGNAL(itemListChanged(KexiProjectHandler *)), this, SLOT(slotItemListChanged(KexiProjectHandler *)));
//		slotItemListChanged(part);
	}

}

void
KexiBrowser::addGroup(KexiPart::Info *info)
{
	KexiBrowserItem *item = new KexiBrowserItem(this, info->mime(), info->groupName(), 0, info);

	item->setPixmap(0, SmallIcon(info->groupIcon()));
	item->setOpen(true);
	item->setSelectable(false);
	m_baseItems.insert(info->mime(), item);
	kdDebug() << "KexiBrowser::addGroup()" << endl;
	slotItemListChanged(info);
}

void
KexiBrowser::addItem(KexiPart::Item item)
{
	if(m_mime == "kexi/db" && m_baseItems.find(item.mime()))
	{
		//part object
		KexiBrowserItem *parent = m_baseItems.find(item.mime());
		kdDebug() << "KexiBrowser::addItem() found parent:" << parent << endl;
		KexiBrowserItem *bitem = new KexiBrowserItem(parent, item.mime(), item.name(), item.identifier());
		bitem->setPixmap(0, SmallIcon(parent->info()->itemIcon()));
	}
	else if(m_mime == item.mime())
	{
		//part objects group
		KexiBrowserItem *bitem = new KexiBrowserItem(this, item.mime(), item.name(), item.identifier(), 0);
		if(m_part)
			bitem->setPixmap(0, SmallIcon(m_part->itemIcon()));
	}
}

void
KexiBrowser::slotItemListChanged(KexiPart::Info *parent)
{
	kdDebug() << "KexiBrowser::slotItemListChanged()" << endl;
	if(m_mime == "kexi/db")
	{
		KexiBrowserItem *group = m_baseItems.find(parent->mime());
		kdDebug() << "KexiBrowser::slotItemListChanged(): group=" << group  << " mime=" << parent->mime()<< endl;
		group->clearChildren();
	}
	else
	{
		clear();
	}

	ItemList list = m_parent->project()->items(parent);
	kdDebug() << "KexiBrowser::slotItemListChanged(): list count:" << list.count() << endl;
	for(ItemList::Iterator it = list.begin(); it != list.end(); ++it)
	{
		kdDebug() << "KexiBrowser::slotItemListChanged() adding item" << (*it).mime() << endl;
		addItem(*it);
	}
}

void
KexiBrowser::slotContextMenu(KListView *, QListViewItem *, const QPoint &)
{
#if 0
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if(!it)
		return;

	if(it->proxy() || it->item())
	{
		KexiPartPopupMenu *pg = 0;
		if(it->identifier().isNull())
		{
			// FIXME: Make this less hacky please :)
			pg = it->proxy()->groupContextMenu();
		}
		else
		{
			kdDebug() << "KexiBrowser::slotContextMenu() item @ " << it->item() << endl;
			//a littlebit hacky
			pg = it->item()->handler()->proxy(
			static_cast<KexiDialogBase*>(parent()->parent())->kexiView()
			)->itemContextMenu(it->identifier());
		}

		pg->setPartItemId(it->identifier());
		pg->exec(pos);
//		delete pg;
	}
#endif
}

void
KexiBrowser::slotExecuteItem(QListViewItem *item)
{
	kdDebug() << "KexiBrowser::slotExecuteItem()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if(!it || it->info())
		return;

	kdDebug() << "KexiBrowser::slotExecuteItem() searching stuff for mime: " << it->mime() << endl;
	KexiPart::Info *info = m_parent->project()->partManager()->info(it->mime());
	if(!info)
		return;

	kdDebug() << "KexiBrowser::slotExecuteItem() info=" << info << endl;

	if(!info->instance())
		return;

	info->instance()->execute(m_parent, it->name());
}

#include "kexibrowser.moc"

