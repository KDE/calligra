/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <qpixmapcache.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kimageeffect.h>

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
#include "kexi.h"


KexiBrowser::KexiBrowser(KexiMainWindow *mainWin)
 : KexiViewBase(mainWin, mainWin, "KexiBrowser")
 , m_baseItems(199, false)
 , m_normalItems(199)
 , m_prevSelectedPart(0)
{
	QVBoxLayout *lyr = new QVBoxLayout(this);
	m_toolbar = new KToolBar(this, "kexibrowser_toolbar", false);
	m_toolbar->setIconSize( kapp->iconLoader()->currentSize(KIcon::Small) );
	m_toolbar->insertSeparator();
	lyr->addWidget(m_toolbar);
	m_list = new KexiBrowserListView(this);
	lyr->addWidget(m_list);
//	setFocusProxy(m_list);
	m_list->renameLineEdit()->installEventFilter(this);
//	m_ac = m_parent->actionCollection();
//	KexiActionProxy ap;
	//shared actions
	plugSharedAction("edit_delete",SLOT(slotRemove()));
	plugSharedAction("edit_edititem", SLOT(slotRename()));
#ifdef KEXI_SHOW_UNIMPLEMENTED
	plugSharedAction("edit_cut",SLOT(slotCut()));
	plugSharedAction("edit_copy",SLOT(slotCopy()));
	plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif
	
	setCaption(i18n("Project Navigator"));
	setIcon(*mainWin->icon());

	m_list->header()->hide();
	m_list->addColumn("");
	m_list->setShowToolTips(true);
	m_list->setRootIsDecorated(true);
	m_list->setSorting(0);
	m_list->sort();
	m_list->setAllColumnsShowFocus(true);
	m_list->setTooltipColumn(0);
	m_list->renameLineEdit()->setValidator( new Kexi::IdentifierValidator(this) );
//	setResizeMode(QListView::LastColumn);

	connect(m_list, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
//js todo: ADD OPTION for enable this:
//connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
	connect(m_list, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotSelectionChanged(QListViewItem*)));
//	connect(m_list, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotClicked(QListViewItem*)));

	connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
	connect(m_list, SIGNAL(executed(QListViewItem*)), this, SLOT(slotExecuteItem(QListViewItem*)));
//	connect(m_list->renameLineEdit(), SIGNAL(done(QListViewItem *, int)), this, SLOT(slotItemRenameDone(QListViewItem *, int)));
//	connect(m_list, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotItemRenamed(QListViewItem*)));

	//init popups
	m_itemPopup = new KPopupMenu(this, "itemPopup");
	m_itemPopupTitle_id = m_itemPopup->insertTitle("");
	m_openAction = new KAction(i18n("&Open"), "fileopen", Key_Enter, this, 
		SLOT(slotOpenObject()), this, "open_object");
	m_openAction->plug(m_itemPopup);
	m_openAction->plug(m_toolbar);
	m_designAction = new KAction(i18n("&Design"), "edit", CTRL + Key_Enter, this, 
		SLOT(slotDesignObject()), this, "design_object");
	m_designAction->plug(m_itemPopup);
	m_designAction->plug(m_toolbar);
	m_newObjectAction = new KAction("", "filenew", 0, this, SLOT(slotNewObject()), this, "new_object");

/*	QImage img = SmallIcon("table").convertToImage();
	QImage img2 = SmallIcon("new_sign").convertToImage();

	bitBlt( &img, 0, 0, &img2, 0, 0, img2.width(), img2.height(), 0);

	QPixmap pix;
	pix.convertFromImage(img);
//	m_newObjectAction->setIconSet( pix );
//$kico_kexi_32_0:1:notrans_0
	QPixmapCache::insert("$kico_table_newobj_16_0:1:notrans_0", pix);*/
//	m_newObjectAction->setIconSet( SmallIcon("table_newobj") );

	m_newObjectAction->plug(m_itemPopup);
	m_newObjectToolbarAction = new KAction("", 0, this, SLOT(slotNewObject()), this, "new_object");
	m_toolbar->insertSeparator();
	m_newObjectToolbarAction->plug(m_toolbar);
	m_itemPopup->insertSeparator();
	plugSharedAction("edit_cut", m_itemPopup);
	plugSharedAction("edit_copy", m_itemPopup);
	m_itemPopup->insertSeparator();
	plugSharedAction("edit_edititem", i18n("&Rename"), m_itemPopup);
//	m_renameObjectAction = new KAction(i18n("&Rename"), 0, Key_F2, this, 
//		SLOT(slotRename()), this, "rename_object");
//	m_renameObjectAction->plug(m_itemPopup);
	plugSharedAction("edit_delete", m_itemPopup);

	m_partPopup = new KPopupMenu(this, "partPopup");
	m_partPopupTitle_id = m_partPopup->insertTitle("");
	m_newObjectAction->plug(m_partPopup);
	m_partPopup->insertSeparator();
	plugSharedAction("edit_paste", m_partPopup);
}

KexiBrowser::~KexiBrowser()
{
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

	KexiBrowserItem *item = new KexiBrowserItem(m_list, info);
	m_baseItems.insert(info->mime().lower(), item);

//	kdDebug() << "KexiBrowser::addGroup()" << endl;
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
//	kdDebug() << "KexiBrowser::addItem() found parent:" << parent << endl;
	
	KexiBrowserItem *bitem = new KexiBrowserItem(parent, parent->info(), item);
	m_normalItems.insert(item->identifier(), bitem);
}

void 
KexiBrowser::slotRemoveItem(const KexiPart::Item &item)
{
	KexiBrowserItem *to_remove=m_normalItems.take(item.identifier());
	if (!to_remove)
		return;

	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	
	QListViewItem *new_item_to_select = 0;
	if (it==to_remove) {//compute item to select if current one will be removed
		new_item_to_select = it->itemBelow();//nearest item to select
		if (!new_item_to_select || new_item_to_select->parent()!=it->parent()) {
			new_item_to_select = it->itemAbove();
		}
	}
	delete to_remove;

	if (new_item_to_select)
		m_list->setSelected(new_item_to_select, true);
}

void
KexiBrowser::slotContextMenu(KListView* /*list*/, QListViewItem *item, const QPoint &pos)
{
	if(!item)
		return;
	KexiBrowserItem *bit = static_cast<KexiBrowserItem*>(item);
	KPopupMenu *pm;
	if (bit->item()) {
		pm = m_itemPopup;
		//update popup title
		QString title_text = bit->text(0).stripWhiteSpace();
		KexiBrowserItem *par_it = static_cast<KexiBrowserItem*>(bit->parent());
		KexiPart::Part* par_part = 0;
		if (par_it && par_it->info() && ((par_part = Kexi::partManager().part(par_it->info()))) && !par_part->instanceName().isEmpty()) {
			//add part type name
			title_text +=  (" : " + par_part->instanceName());
		}
		pm->changeTitle(m_itemPopupTitle_id, *bit->pixmap(0), title_text);
	}
	else {
		pm = m_partPopup;
		QString title_text = bit->text(0).stripWhiteSpace();
		pm->changeTitle(m_partPopupTitle_id, *bit->pixmap(0), title_text);
/*		KexiPart::Part* part = Kexi::partManager().part(bit->info());
		if (part)
			m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceName() ));
		else
			m_newObjectAction->setText(i18n("&Create Object..."));
		m_newObjectAction->setIconSet( SmallIconSet(bit->info()->itemIcon()) );*/
		m_list->setCurrentItem(item);
		m_list->repaintItem(item);
	}
	pm->exec(pos);
}

void
KexiBrowser::slotExecuteItem(QListViewItem *vitem)
{
//	kdDebug() << "KexiBrowser::slotExecuteItem()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(vitem);

	if (!it->item())
		return;
	emit openOrActivateItem( it->item(), Kexi::DataViewMode );
}

void
KexiBrowser::slotSelectionChanged(QListViewItem* i)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(i);
	bool gotitem = it && it->item();
	bool gotgroup = it && !it->item();
 //TODO: also check if the item is not read only
	setAvailable("edit_delete",gotitem);
	setAvailable("edit_cut",gotitem);
	setAvailable("edit_copy",gotitem);
//	setAvailable("edit_paste",gotgroup);
	m_openAction->setEnabled(gotitem);
	m_designAction->setEnabled(gotitem);
//	m_renameObjectAction->setEnabled(gotitem);
	setAvailable("edit_edititem",gotitem);

	KexiPart::Part* part = Kexi::partManager().part(it->info());
	if (!part) {
		it = static_cast<KexiBrowserItem*>(it->parent());
		if (it) {
			part = Kexi::partManager().part(it->info());
		}
	}

	if (m_prevSelectedPart != part) {
		m_prevSelectedPart = part;
		if (part) {
			m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceName() ));
			m_newObjectAction->setIcon( part->info()->createItemIcon() );
			m_newObjectToolbarAction->setIcon( part->info()->createItemIcon() );
			m_newObjectToolbarAction->setText(m_newObjectAction->text());
		} else {
			m_newObjectAction->setText(i18n("&Create Object..."));
			m_newObjectToolbarAction->setIconSet( SmallIconSet("filenew") );
			m_newObjectToolbarAction->setText(m_newObjectAction->text());
		}
	}
}

void KexiBrowser::installEventFilter ( const QObject * filterObj )
{
	m_list->installEventFilter ( filterObj );
	KexiViewBase::installEventFilter ( filterObj );
}

bool KexiBrowser::eventFilter ( QObject *o, QEvent * e )
{
	if (o==m_list->renameLineEdit() && e->type()==QEvent::Hide) {
		itemRenameDone();
	}
	return false;
}

void KexiBrowser::slotRemove()
{
//	kdDebug() << "KexiBrowser::slotRemove()" << endl;
	if (!isAvailable("edit_delete"))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit removeItem( it->item() );
}

void KexiBrowser::slotNewObject()
{
//	kdDebug() << "KexiBrowser::slotNewObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->info())
		return;
	emit newItem( it->info() );
}

void KexiBrowser::slotOpenObject()
{
//	kdDebug() << "KexiBrowser::slotOpenObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), Kexi::DataViewMode );
}

void KexiBrowser::slotDesignObject()
{
//	kdDebug() << "KexiBrowser::slotDesignObject()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), Kexi::DesignViewMode );
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

void KexiBrowser::slotRename()
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (it)
		m_list->rename(it, 0);
}

//slotItemRenamed(QListViewItem *item)
void KexiBrowser::itemRenameDone()
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it)
		return;
	QString txt = it->text(0).stripWhiteSpace();
	bool ok = it->item()->name().lower()!=txt.lower(); //the new name must be different
	if (ok) {
		/* TODO */
		emit renameItem(it->item(), txt, ok);
	}
	if (!ok) {
		txt = it->item()->name(); //revert
	}
	it->setText(0, QString(" ") + txt + " ");
	it->parent()->sort();
	m_list->setFocus();
}

void KexiBrowser::setFocus()
{
	m_list->setFocus();
	if (!m_list->selectedItem() && m_list->firstChild())//select first
		m_list->setSelected(m_list->firstChild(), true);
}

void KexiBrowser::updateItemName( KexiPart::Item *item, bool dirty )
{
	if (!item)
		return;
	KexiBrowserItem *bitem = m_normalItems[item->identifier()];
	if (!bitem)
		return;
	bitem->setText( 0, " "+ item->name() + (dirty ? "* " : " ") );
}

/*void KexiBrowser::focusInEvent( QFocusEvent *e )
{
	KexiDockBase::focusInEvent(e);
	if (m_list->selectedItem() && m_list->firstChild())//select first
		m_list->setSelected(m_list->firstChild(), true);
	m_list->setFocus();
}*/

//--------------------------------------------

KexiBrowserListView::KexiBrowserListView(QWidget *parent)
 : KListView(parent, "KexiBrowserListView")
{
}

void KexiBrowserListView::rename(QListViewItem *item, int c)
{
	if (renameLineEdit()->isVisible())
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if (it->item() && c==0) {
		//only edit 1st column for items, not item groups
//TODO: also check it this item is not read-only
		item->setText(0, item->text(0).mid(1,item->text(0).length()-2));
		KListView::rename(item, c);
		adjustColumn(0);
	}
}

#include "kexibrowser.moc"

