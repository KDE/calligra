/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
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

#include <qheader.h>
#include <qpoint.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include "kexi.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiproject.h"
#include "kexidialogbase.h"
#include "keximainwindow.h"


//KexiBrowser::KexiBrowser(KexiMainWindow *parent, QString mime, KexiPart::Info *part )
KexiBrowser::KexiBrowser(KexiMainWindow *parent )
 : KListView(parent,"KexiBrowser")
 , KexiActionProxy(parent, this)
{
	m_parent = parent;
//	m_ac = m_parent->actionCollection();
//	KexiActionProxy ap;
	plugAction("edit_remove",SLOT(slotRemove()));

	setCaption(i18n("Navigator"));
	setIcon(*parent->icon());

	header()->hide();
	addColumn("");
	setShowToolTips(true);
	setRootIsDecorated(true);
	setSorting(0);
	sort();
//	setResizeMode(QListView::LastColumn);

	connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
//js todo: ADD OPTION for enable this:
//connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
	connect(this, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotSelectionChanged(QListViewItem*)));
	connect(this, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotClicked(QListViewItem*)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));

	//init popups
	m_itemPopup = new KPopupMenu(this, "itemPopup");
	KAction *a = new KAction(i18n("&Open"), SmallIcon("fileopen"), Key_Enter, this, 
		SLOT(slotOpenObject()), this, "open_object");
	a->plug(m_itemPopup);
	a = new KAction(i18n("&Design"), SmallIcon("edit"), CTRL + Key_Enter, this, 
		SLOT(slotDesignObject()), this, "design_object");
	a->plug(m_itemPopup);
	action("edit_remove")->plug(m_itemPopup);

	m_partPopup = new KPopupMenu(this, "partPopup");
	m_newObjectAction = new KAction("", 0, this, SLOT(slotNewObject()), this, "new_object");
	m_newObjectAction->plug(m_partPopup);
}

void
KexiBrowser::addGroup(KexiPart::Info *info)
{
//	KexiBrowserItem *item = new KexiBrowserItem(this, info->mime(), info->groupName(), 0, info);
	KexiBrowserItem *item = new KexiBrowserItem(this, info);
//	item->setPixmap(0, SmallIcon(info->groupIcon()));
//	item->setOpen(true);
//	item->setSelectable(false);
	m_baseItems.insert(info->mime().lower(), item);

	kdDebug() << "KexiBrowser::addGroup()" << endl;
//js: now it's executed by hand from keximainwindow:	slotItemListChanged(info);
}

void
KexiBrowser::addItem(KexiPart::Item *item)
{
	if (!item)
		return;
	//part object for this item
	KexiBrowserItem *parent = m_baseItems.find(item->mime().lower());
	if (!parent) //TODO: add "Other" part group for that
		return;
	kdDebug() << "KexiBrowser::addItem() found parent:" << parent << endl;
//	KexiBrowserItem *bitem = new KexiBrowserItem(parent, item.mime(), item.name(), item.identifier());
	KexiBrowserItem *bitem = new KexiBrowserItem(parent, parent->info(), item);
//	bitem->setPixmap(0, SmallIcon(parent->info()->itemIcon()));
}

void
KexiBrowser::slotContextMenu(KListView *list, QListViewItem *item, const QPoint &pos)
{
	if(!item)
		return;
	KexiBrowserItem *bit = static_cast<KexiBrowserItem*>(item);
	KPopupMenu *pm;
	if (bit->item()) {
		pm = m_itemPopup;
	}
	else {
		pm = m_partPopup;
		KexiPart::Part * part = Kexi::partManager().part(bit->info());
		if (part)
			m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceName() ));
		else
			m_newObjectAction->setText(i18n("&Create Object..."));
		m_newObjectAction->setIconSet( SmallIconSet(bit->info()->itemIcon()) );
	}
	pm->exec(pos);
}

void
KexiBrowser::slotExecuteItem(QListViewItem *vitem)
{
	kdDebug() << "KexiBrowser::slotExecuteItem()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(vitem);

	if (!it->item())
		return;
	emit openItem( it->item(), false/*!designMode*/ );

/*	if(m_parent->activateWindow(it->item().identifier()))
		return;

	if(!it || it->info())
		return;

	KexiPart::Item item;
	item.setName(it->name());
	item.setIdentifier(it->identifier());
	item.setMime(it->mime());

	kdDebug() << "KexiBrowser::slotExecuteItem() searching stuff for mime: " << it->mime() << endl;
//	KexiPart::Info *info = m_parent->project()->partManager()->info(it->mime());
//	if(!info)
//		return;

//	kdDebug() << "KexiBrowser::slotExecuteItem() info=" << info << endl;

	KexiPart::Part *part = Kexi::partManager().part(it->mime());
//	if(!info->instance())
	if (!part)
		return;

//	info->instance()->execute(m_parent, it->name());
	part->execute(m_parent, item);
	*/
}

void
KexiBrowser::slotSelectionChanged(QListViewItem* i)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(i);
	bool gotitem = it && it->item();
	setAvailable("edit_remove",gotitem);
}

void
KexiBrowser::slotClicked(QListViewItem* i)
{
	//workaround for non-selectable item
	if (!i || !static_cast<KexiBrowserItem*>(i)->item())
		slotSelectionChanged(i);
}

/*bool KexiBrowser::actionAvailable(const char *name)
{
	if (qstrcmp(name,"edit_remove")==0)
		return selectedItem() && static_cast<KexiBrowserItem*>(selectedItem())->item();

	return false;
}
*/

void KexiBrowser::slotRemove()
{
	kdDebug() << "KexiBrowser::slotRemove()" << endl;
	if (!isAvailable("edit_remove"))
		return;
	//TODO
}

void KexiBrowser::slotNewObject()
{
	kdDebug() << "slotNewObject()" << endl;
}

void KexiBrowser::slotOpenObject()
{
	kdDebug() << "slotOpenObject()" << endl;
}

void KexiBrowser::slotDesignObject()
{
	kdDebug() << "slotDesignObject()" << endl;
}

#include "kexibrowser.moc"

