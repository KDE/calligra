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
#include <klistview.h>
#include <kmessagebox.h>

#include "kexi.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiproject.h"
#include "kexidialogbase.h"
#include "keximainwindow.h"
#include "kexi_utils.h"


//KexiBrowser::KexiBrowser(KexiMainWindow *parent, QString mime, KexiPart::Info *part )
KexiBrowser::KexiBrowser(KexiMainWindow *parent )
 : KexiDockBase(parent,"KexiBrowser")
{
	QHBoxLayout *lyr = new QHBoxLayout(this);
	m_list = new KListView(this, "list");
	lyr->addWidget(m_list);
	setFocusProxy(m_list);
//	m_list->installEventFilter(this);
//	m_ac = m_parent->actionCollection();
//	KexiActionProxy ap;
	//shared actions
	plugSharedAction("edit_delete",SLOT(slotRemove()));
	plugSharedAction("edit_cut",SLOT(slotCut()));
	plugSharedAction("edit_copy",SLOT(slotCopy()));
	plugSharedAction("edit_paste",SLOT(slotPaste()));

	setCaption(i18n("Navigator"));
	setIcon(*parent->icon());

	m_list->header()->hide();
	m_list->addColumn("");
	m_list->setShowToolTips(true);
	m_list->setRootIsDecorated(true);
	m_list->setSorting(0);
	m_list->sort();
//	setResizeMode(QListView::LastColumn);

	connect(m_list, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	connect(m_list, SIGNAL(executed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
//js todo: ADD OPTION for enable this:
//connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
	connect(m_list, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotSelectionChanged(QListViewItem*)));
//	connect(m_list, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotClicked(QListViewItem*)));
	connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));

	//init popups
	m_itemPopup = new KPopupMenu(this, "itemPopup");
	m_itemPopupTitle_id = m_itemPopup->insertTitle("");
	KAction *a = new KAction(i18n("&Open"), SmallIcon("fileopen"), Key_Enter, this, 
		SLOT(slotOpenObject()), this, "open_object");
	a->plug(m_itemPopup);
	a = new KAction(i18n("&Design"), SmallIcon("edit"), CTRL + Key_Enter, this, 
		SLOT(slotDesignObject()), this, "design_object");
	a->plug(m_itemPopup);
	m_itemPopup->insertSeparator();
	plugSharedAction("edit_cut", m_itemPopup);
	plugSharedAction("edit_copy", m_itemPopup);
	m_itemPopup->insertSeparator();
	plugSharedAction("edit_delete", m_itemPopup);

	m_partPopup = new KPopupMenu(this, "partPopup");
	m_newObjectAction = new KAction("", 0, this, SLOT(slotNewObject()), this, "new_object");
	m_newObjectAction->plug(m_partPopup);
	m_partPopup->insertSeparator();
	plugSharedAction("edit_paste", m_partPopup);
}

void
KexiBrowser::clear()
{
	m_list->clear();
}

void
KexiBrowser::addGroup(KexiPart::Info *info)
{
	if(!info->addTree())
		return;


//	KexiBrowserItem *item = new KexiBrowserItem(this, info->mime(), info->groupName(), 0, info);
	KexiBrowserItem *item = new KexiBrowserItem(m_list, info);
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
KexiBrowser::slotRemoveItem(const KexiPart::Item &item)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	QListViewItem *new_item = it->itemBelow();//nearest item to select
	if (!new_item || new_item->parent()!=it->parent()) {
		new_item = it->itemAbove();
	}
	if (it->item()==&item)
		delete it;
	if (new_item)
		m_list->setSelected(new_item, true);
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
		QString title_text = bit->text(0);
		KexiBrowserItem *par_it = static_cast<KexiBrowserItem*>(bit->parent());
		KexiPart::Part* par_part = 0;
		if (par_it && par_it->info() && ((par_part = Kexi::partManager().part(par_it->info()))) && !par_part->instanceName().isEmpty())
			title_text +=  (" : " + par_part->instanceName());
		pm->changeTitle(m_itemPopupTitle_id, *bit->pixmap(0), title_text);
	}
	else {
		pm = m_partPopup;
		KexiPart::Part* part = Kexi::partManager().part(bit->info());
		if (part)
			m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceName() ));
		else
			m_newObjectAction->setText(i18n("&Create Object..."));
		m_newObjectAction->setIconSet( SmallIconSet(bit->info()->itemIcon()) );
		m_list->setCurrentItem(item);
		m_list->repaintItem(item);
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
	setAvailable("edit_delete",gotitem);
	setAvailable("edit_cut",gotitem);
	setAvailable("edit_copy",gotitem);
	setAvailable("edit_paste",!gotitem);
}

/*void
KexiBrowser::slotClicked(QListViewItem* i)
{
	//workaround for non-selectable item
	//if (!i || !static_cast<KexiBrowserItem*>(i)->item())
		//slotSelectionChanged(i);
}*/

void KexiBrowser::installEventFilter ( const QObject * filterObj )
{
	m_list->installEventFilter ( filterObj );
	KexiDockBase::installEventFilter ( filterObj );
}

void KexiBrowser::slotRemove()
{
	kdDebug() << "KexiBrowser::slotRemove()" << endl;
	if (!isAvailable("edit_delete"))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit removeItem( it->item() );
}

void KexiBrowser::slotNewObject()
{
	kdDebug() << "KexiBrowser::slotNewObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->info())
		return;
	emit newItem( it->info() );
}

void KexiBrowser::slotOpenObject()
{
	kdDebug() << "KexiBrowser::slotOpenObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), false/*!designMode*/ );
}

void KexiBrowser::slotDesignObject()
{
	kdDebug() << "KexiBrowser::slotDesignObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), true/*designMode*/ );
}

void KexiBrowser::slotCut()
{
	KEXI_UNFINISHED_SHARED_ACTION("edit_cut");
	//TODO
}

void KexiBrowser::slotCopy()
{
	KEXI_UNFINISHED_SHARED_ACTION("edit_copy");
	//TODO
}

void KexiBrowser::slotPaste()
{
	KEXI_UNFINISHED_SHARED_ACTION("edit_paste");
	//TODO
}


#include "kexibrowser.moc"

