/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexibrowser.h"
#include "kexibrowser_p.h"
#include "kexibrowseritem.h"

#include <q3header.h>
#include <qpoint.h>
#include <qpixmapcache.h>
#include <qtoolbutton.h>
#include <Q3VBoxLayout>
#include <QKeyEvent>
#include <QEvent>

#include <kglobalsettings.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>
#include <k3listview.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kimageeffect.h>
#include <kconfig.h>
#include <kglobal.h>
#include <KActionCollection>
#include <KActionMenu>

#include <kexi.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/identifier.h>
#include <widget/utils/kexiflowlayout.h>
#include <widget/kexismalltoolbutton.h>
#include <kexidb/utils.h>
#include <kexi_global.h>

/*
class KexiBrowserView : public QWidget
{
	KexiBrowserView*(
};

KexiBrowserView::KexiBrowserView(KexiMainWindow *mainWin)
 : QWidget(mainWin, "KexiBrowserView")
{
	QVBoxLayout *lyr = new QVBoxLayout(this);
	KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);
	m_browser = KexiBrowser(this, mainWin);
	lyr->addWidget(m_browser);
	setFocusProxy(m_browser);
}*/

KexiBrowser::KexiBrowser(QWidget* parent, Features features)
 : QWidget(parent)
 , m_features(features)
 , m_actions( new KActionCollection(this) )
 , m_prevSelectedPart(0)
 , m_singleClick(false)
// , m_nameEndsWithAsterisk(false)
 , m_readOnly(false)
// , m_enableExecuteArea(true)
{
	setWindowTitle(i18n("Project Navigator"));
	setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
	KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);

	m_list = new KexiBrowserListView(this);
	lyr->addWidget(m_list);
	m_list->installEventFilter(this);
	m_list->renameLineEdit()->installEventFilter(this);
	connect( KGlobalSettings::self(), SIGNAL( settingsChanged(int) ), SLOT( slotSettingsChanged(int) ) );
	slotSettingsChanged(0);

	m_list->header()->hide();
	m_list->addColumn("");
	m_list->setShowToolTips(true);
	m_list->setSorting(0);
	m_list->sort();
	m_list->setAllColumnsShowFocus(true);
	m_list->setTooltipColumn(0);
	m_list->renameLineEdit()->setValidator( new KexiUtils::IdentifierValidator(this) );
	m_list->setResizeMode(Q3ListView::LastColumn);
	connect(m_list, SIGNAL(selectionChanged(Q3ListViewItem*)), this,
		SLOT(slotSelectionChanged(Q3ListViewItem*)));
	connect(m_list, SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem *, const QPoint&)));
	
	KConfigGroup mainWindowGroup = KGlobal::config()->group("MainWindow");
	if ((m_features & SingleClickOpensItemOptionEnabled) 
		&& mainWindowGroup.readEntry<bool>("SingleClickOpensItem", false))
	{
		connect(m_list, SIGNAL(executed(Q3ListViewItem*)), this,
			SLOT(slotExecuteItem(Q3ListViewItem*)));
	}
	else {
		connect(m_list, SIGNAL(doubleClicked(Q3ListViewItem*)), this,
			SLOT(slotExecuteItem(Q3ListViewItem*)));
		m_list->enableExecuteArea = false;
	}

	// actions
	m_openAction = addAction("open_object", KIcon("document-open"), i18n("&Open"), 
		i18n("Open object"), i18n("Opens object selected in the list"),
		SLOT(slotOpenObject()));

//	m_openAction->plug(m_toolbar);
	KexiSmallToolButton *btn;
	if (m_features & Toolbar) {
		btn = new KexiSmallToolButton(m_openAction, this);
		buttons_flyr->add(btn);
	}

	if (KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
		m_deleteAction = 0;
		m_renameAction = 0;
		m_designAction = 0;
		m_editTextAction = 0;
		m_newObjectAction = 0;
		m_newObjectMenu = 0;
	}
	else {
		m_deleteAction = addAction("edit_delete", KIcon("edit-delete"), i18n("&Delete"),
			i18n("Delete object"), i18n("Deletes the object selected in the list"),
			SLOT(slotRemove()));

		m_renameAction = addAction("edit_rename", KIcon(), i18n("&Rename"),
			i18n("Rename object"), i18n("Renames the object selected in the list"),
			SLOT(slotRename()));
#ifdef KEXI_SHOW_UNIMPLEMENTED
		//todo	plugSharedAction("edit_cut",SLOT(slotCut()));
		//todo	plugSharedAction("edit_copy",SLOT(slotCopy()));
		//todo	plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

		m_designAction = addAction("design_object", KIcon("edit"), i18n("&Design"),
			i18n("Design object"), i18n("Starts designing of the object selected in the list"),
			SLOT(slotDesignObject()));
		if (m_features & Toolbar) {
			btn = new KexiSmallToolButton(m_designAction, this);
//			m_designAction->setEnabled(false);
			buttons_flyr->add(btn);
		}

		m_editTextAction = addAction("editText_object", KIcon(), i18n("Open in &Text View"),
			i18n("Open object in text view"), i18n("Opens selected object in the list in text view"),
			SLOT(slotEditTextObject()));

		m_newObjectAction = addAction("new_object", KIcon("document-new"), QString(),
			QString(), QString(),
			SLOT(slotNewObject()));
			
		if (m_features & Toolbar) {
			m_newObjectToolButton = new KexiSmallToolButton(this);
			m_newObjectMenu = new KMenu(this);
			m_newObjectMenu->setObjectName("newObjectMenu");
			connect(m_newObjectMenu, SIGNAL(aboutToShow()),
				this, SLOT(slotNewObjectMenuAboutToShow()));
		//	KexiPart::Part* part = Kexi::partManager().part("kexi/table");
		//	m_newObjectMenu->insertItem( KIcon(part->info()->createItemIcon()), part->instanceName() );
			m_newObjectToolButton->setMenu(m_newObjectMenu);
//			m_newObjectToolButton->setPopupDelay(QApplication::startDragTime());
			connect(m_newObjectToolButton, SIGNAL(clicked()), this, SLOT(slotNewObject()));
			buttons_flyr->add(m_newObjectToolButton);

			m_deleteObjectToolButton = new KexiSmallToolButton(m_deleteAction, this);
			m_deleteObjectToolButton->setText(QString());
			buttons_flyr->add(m_deleteObjectToolButton);
		}
	}

	m_executeAction = addAction("data_execute", KIcon("media-playback-start"), i18n("Execute"),
//! @todo tooltip, what's this
		QString(), QString(),
		SLOT(slotExecuteObject()));

	m_actions->addAction("export_object",
		m_exportActionMenu = new KActionMenu(i18n("Export"), this) );
	m_dataExportAction = addAction( "exportAsDataTable", KIcon("table"),
		i18nc("Export->To File As Data &Table... ", "To &File As Data Table..."),
		i18n("Export data to a file"),
		i18n("Exports data from the currently selected table or query data to a file."),
		SLOT(slotExportAsDataTable()));
	m_exportActionMenu->addAction( m_dataExportAction );

	m_printAction = addAction("print_object", KIcon("document-print"), i18n("&Print..."),
		i18n("Print data"),
		i18n("Prints data from the currently selected table or query."),
		SLOT(slotPrintObject()));
		
	m_pageSetupAction = addAction("pageSetupForObject", KIcon(), i18n("Page Setup..."),
		i18n("Page setup for data"),
		i18n("Shows page setup for printing the active table or query."),
		SLOT(slotPageSetupForObject()));

	if (KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
		m_partMenu = 0;
	}
	else {
		m_partMenu = new KexiGroupMenu(this, m_actions);
	}

	if (m_features & ContextMenus) {
		m_itemMenu = new KexiItemMenu(this, m_actions);
	}
	else {
		m_itemMenu = 0;
	}

	if (!(m_features & Writable)) {
		setReadOnly(true);
	}
}

KexiBrowser::~KexiBrowser()
{
}

void KexiBrowser::setProject(KexiProject* prj, const QString& itemsMimeType,
	QString* partManagerErrorMessages)
{
	clear();
	m_itemsMimeType = itemsMimeType;
	m_list->setRootIsDecorated(m_itemsMimeType.isEmpty());

	KexiPart::PartInfoList* plist = Kexi::partManager().partInfoList();
	foreach (KexiPart::Info *info, *plist) {
		if (!info->isVisibleInNavigator())
			continue;
		if (!m_itemsMimeType.isEmpty() && info->mimeType()!=m_itemsMimeType.toLatin1())
			continue;

//		kDebug() << "KexiMainWindowImpl::initNavigator(): adding " << it->groupName() << endl;

/*			KexiPart::Part *p=Kexi::partManager().part(it);
		if (!p) {
			//TODO: js - OPTIONALLY: show error
			continue;
		}
		p->createGUIClient(this);*/

		//load part - we need this to have GUI merged with part's actions
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
		KexiPart::Part *p=Kexi::partManager().part(info);
		if (p) {
			KexiBrowserItem *groupItem = 0;
			if (m_itemsMimeType.isEmpty()) {
				groupItem = addGroup(*info);
				if (!groupItem)
					continue;
			}
			//lookup project's objects (part items)
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
			KexiPart::ItemDict *item_dict = prj->items(info);
			if (!item_dict)
				continue;
			for (KexiPart::ItemDictIterator item_it( *item_dict ); item_it.current(); ++item_it)
				addItem(*item_it.current(), groupItem, info);
			if (!m_itemsMimeType.isEmpty())
				break; //the only group added, so our work is completed
		}
		else {
			//add this error to the list that will be displayed later
			QString msg, details;
			KexiDB::getHTMLErrorMesage(&Kexi::partManager(), msg, details);
			if (!msg.isEmpty() && partManagerErrorMessages) {
				if (partManagerErrorMessages->isEmpty()) {
					*partManagerErrorMessages = QString("<qt><p>")
						+i18n("Errors encountered during loading plugins:")+"<ul>";
				}
				partManagerErrorMessages->append( QString("<li>") + msg );
				if (!details.isEmpty())
					partManagerErrorMessages->append(QString("<br>")+details);
				partManagerErrorMessages->append("</li>");
			}
		}
	}
	if (partManagerErrorMessages && !partManagerErrorMessages->isEmpty())
		partManagerErrorMessages->append("</ul></p>");
}

KAction* KexiBrowser::addAction(const QString& name, const KIcon& icon, const QString& text,
	const QString& toolTip, const QString& whatsThis, const char* slot)
{
	KAction *action = new KAction(icon, text, this);
	m_actions->addAction(name, action);
	action->setToolTip(toolTip);
	action->setWhatsThis(whatsThis);
	connect(action, SIGNAL(triggered()), this, slot);
	return action;
}

QString KexiBrowser::itemsMimeType() const
{
	return m_itemsMimeType;
}

KexiBrowserItem *KexiBrowser::addGroup(KexiPart::Info& info)
{
	if(!info.isVisibleInNavigator())
		return 0;

	KexiBrowserItem *item = new KexiBrowserItem(m_list, &info);
	m_baseItems.insert(info.mimeType().toLower(), item);
	return item;
//	kDebug() << "KexiBrowser::addGroup()" << endl;
}

KexiBrowserItem* KexiBrowser::addItem(KexiPart::Item& item)
{
	//part object for this item
	KexiBrowserItem *parent = item.mimeType().isEmpty() 
		? 0 : m_baseItems.value( item.mimeType().toLower() );
	return addItem(item, parent, parent->partInfo());
}

KexiBrowserItem* KexiBrowser::addItem(KexiPart::Item& item, KexiBrowserItem *parent, KexiPart::Info* info)
{
//	if (!parent) //TODO: add "Other" part group for that
	//	return 0;
//	kDebug() << "KexiBrowser::addItem() found parent:" << parent << endl;
	KexiBrowserItem *bitem;
	if (parent)
		bitem = new KexiBrowserItem(parent, info, &item);
	else
		bitem = new KexiBrowserItem(m_list, info, &item);
	m_normalItems.insert(item.identifier(), bitem);
	return bitem;
}

void 
KexiBrowser::slotRemoveItem(const KexiPart::Item &item)
{
	KexiBrowserItem *to_remove = m_normalItems.take(item.identifier());
	if (!to_remove)
		return;

	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	
	Q3ListViewItem *new_item_to_select = 0;
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
KexiBrowser::slotContextMenu(K3ListView* /*list*/, Q3ListViewItem *item, const QPoint &pos)
{
	if(!item || !(m_features & ContextMenus))
		return;
	KexiBrowserItem *bit = static_cast<KexiBrowserItem*>(item);
	KMenu *pm = 0;
	if (bit->partItem()) {
		pm = m_itemMenu;
		KexiBrowserItem *par_it = static_cast<KexiBrowserItem*>(bit->parent());
		m_itemMenu->update(par_it->partInfo(), bit->partItem());
	}
	else if (m_partMenu) {
		pm = m_partMenu;
		m_newObjectAction->setIcon( KIcon(bit->partInfo()->itemIcon()) );
		m_partMenu->update(bit->partInfo());
		m_list->setCurrentItem(item);
		m_list->repaintItem(item);
	}
	if (pm)
		pm->exec(pos);
}

void
KexiBrowser::slotExecuteItem(Q3ListViewItem *vitem)
{
//	kDebug() << "KexiBrowser::slotExecuteItem()" << endl;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(vitem);
	if (!it)
		return;
	if (!it->partItem() && !m_singleClick /*annoying when in single click mode*/) {
		m_list->setOpen( vitem, !vitem->isOpen() );
		return;
	}
	if (it->partInfo()->isExecuteSupported())
		emit executeItem( it->partItem() );
	else
		emit openOrActivateItem( it->partItem(), Kexi::DataViewMode );
}

void
KexiBrowser::slotSelectionChanged(Q3ListViewItem* i)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(i);
	if (!it)
		return;
	KexiPart::Part* part = Kexi::partManager().part(it->partInfo());
	if (!part) {
		it = static_cast<KexiBrowserItem*>(it->parent());
		if (it) {
			part = Kexi::partManager().part(it->partInfo());
		}
	}

	const bool gotitem = it && it->partItem();
	//bool gotgroup = it && !it->partItem();
//TODO: also check if the item is not read only
	if (m_deleteAction) {
		m_deleteAction->setEnabled(gotitem && !m_readOnly);
		if (m_features & Toolbar) {
			m_deleteObjectToolButton->setEnabled(gotitem && !m_readOnly);
		}
	}
#ifdef KEXI_SHOW_UNIMPLEMENTED
//todo	setAvailable("edit_cut",gotitem);
//todo	setAvailable("edit_copy",gotitem);
//todo	setAvailable("edit_edititem",gotitem);
#endif

	m_openAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::DataViewMode));
	if (m_designAction) {
//		m_designAction->setVisible(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
		m_designAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
	}
	if (m_editTextAction)
		m_editTextAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::TextViewMode));

//	if (m_features & ContextMenus) {
//		m_openAction->setVisible(m_openAction->isEnabled());
//		if (m_designAction)
	//		m_designAction->setVisible(m_designAction->isEnabled());
//		if (m_editTextAction)
//			m_editTextAction->setVisible(part && m_editTextAction->isEnabled());
//		if (m_executeAction)
//			m_executeAction->setVisible(gotitem && it->partInfo()->isExecuteSupported());
//		if (m_exportActionMenu) {
			//m_exportActionMenu->setVisible(gotitem && it->partInfo()->isDataExportSupported());
//		}
//		if (m_printAction)
//			m_printAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
//		if (m_pageSetupAction) {
//			m_pageSetupAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
//		}
//	}

	if (m_prevSelectedPart != part) {
		m_prevSelectedPart = part;
		if (part) {
			if (m_newObjectAction) {
				m_newObjectAction->setText(
					i18n("&Create Object: %1...", part->instanceCaption() ));
				m_newObjectAction->setIcon( KIcon(part->info()->createItemIcon()) );
				if (m_features & Toolbar) {
					m_newObjectToolButton->setIcon( KIcon(part->info()->createItemIcon()) );
					m_newObjectToolButton->setToolTip(
						i18n("Create object: %1", part->instanceCaption().toLower() ));
					m_newObjectToolButton->setWhatsThis(
						i18n("Creates a new object: %1", part->instanceCaption().toLower() ));
				}
			}
		} else {
			if (m_newObjectAction) {
				m_newObjectAction->setText(i18n("&Create Object..."));
	//			m_newObjectToolbarAction->setIcon( KIcon("document-new") );
	//			m_newObjectToolbarAction->setText(m_newObjectAction->text());
				if (m_features & Toolbar) {
					m_newObjectToolButton->setIcon( KIcon("document-new") );
					m_newObjectToolButton->setToolTip(i18n("Create object"));
					m_newObjectToolButton->setWhatsThis(i18n("Creates a new object"));
				}
			}
		}
	}
	emit selectionChanged(it ? it->partItem() : 0);
}

void KexiBrowser::installEventFilter ( QObject * filterObj )
{
	if (!filterObj)
		return;
	m_list->installEventFilter ( filterObj );
	QWidget::installEventFilter ( filterObj );
}

bool KexiBrowser::eventFilter ( QObject *o, QEvent * e )
{
	if (o==m_list && e->type()==QEvent::Resize) {
		kexidbg << "resize!" << endl;
	}
	if (o==m_list->renameLineEdit()) {
		if (e->type()==QEvent::Hide) 
			itemRenameDone();
	}
	else if (e->type()==QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
			if (0==(ke->modifiers() & (Qt::ShiftButton|Qt::ControlButton|Qt::AltButton))) {
				Q3ListViewItem *it = m_list->selectedItem();
				if (it)
					slotExecuteItem(it);
			}
			else if (Qt::ControlButton==(ke->modifiers() & (Qt::ShiftButton|Qt::ControlButton|Qt::AltButton))) {
				slotDesignObject();
			}
		}
	}
	else if (e->type()==QEvent::ShortcutOverride) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		//override delete action
		if (ke->key()==Qt::Key_Delete && ke->modifiers()==Qt::NoButton) {
			slotRemove();
			ke->accept();
			return true;
		}
		//override rename action
		if (ke->key()==Qt::Key_F2 && ke->modifiers()==Qt::NoButton) {
			slotRename();
			ke->accept();
			return true;
		}
/*		else if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
			if (ke->modifiers()==Qt::ControlModifier) {
				slotDesignObject();
			}
			else if (ke->modifiers()==0 && !m_list->renameLineEdit()->isVisible()) {
				Q3ListViewItem *it = m_list->selectedItem();
				if (it)
					slotExecuteItem(it);
			}
			ke->accept();
			return true;
		}*/
	}
	return false;
}

void KexiBrowser::slotRemove()
{
	if (!m_deleteAction || !m_deleteAction->isEnabled() || !(m_features & Writable))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->partItem())
		return;
	emit removeItem( it->partItem() );
}

void KexiBrowser::slotNewObject()
{
	if (!m_newObjectAction || !(m_features & Writable))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->partInfo())
		return;
	emit newItem( it->partInfo() );
}

void KexiBrowser::slotOpenObject()
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->partItem())
		return;
	emit openItem( it->partItem(), Kexi::DataViewMode );
}

void KexiBrowser::slotDesignObject()
{
	if (!m_designAction)
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->partItem())
		return;
	emit openItem( it->partItem(), Kexi::DesignViewMode );
}

void KexiBrowser::slotEditTextObject()
{
	if (!m_editTextAction)
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->partItem())
		return;
	emit openItem( it->partItem(), Kexi::TextViewMode );
}

void KexiBrowser::slotCut()
{
	if (!(m_features & Writable))
		return;
//	KEXI_UNFINISHED_SHARED_ACTION("edit_cut");
	//TODO
}

void KexiBrowser::slotCopy()
{
//	KEXI_UNFINISHED_SHARED_ACTION("edit_copy");
	//TODO
}

void KexiBrowser::slotPaste()
{
	if (!(m_features & Writable))
		return;
//	KEXI_UNFINISHED_SHARED_ACTION("edit_paste");
	//TODO
}

void KexiBrowser::slotRename()
{
	if (!m_renameAction || !(m_features & Writable))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (it)
		m_list->rename(it, 0);
}

void KexiBrowser::itemRenameDone()
{
	if (!(m_features & Writable))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it)
		return;
	QString txt = it->text(0).trimmed();
	bool ok = it->partItem()->name().toLower()!=txt.toLower(); //the new name must be different
	if (ok) {
		/* TODO */
		emit renameItem(it->partItem(), txt, ok);
	}
	if (!ok) {
		txt = it->partItem()->name(); //revert
	}
	//"modified" flag has been removed before editing: readd it
	if (m_list->nameEndsWithAsterisk) {
		txt += "*";
		m_list->nameEndsWithAsterisk = false;
	}
	it->setText(0, txt);
	it->parent()->sort();
	setFocus();
}

void KexiBrowser::setFocus()
{
	if (!m_list->selectedItem() && m_list->firstChild())//select first
		m_list->setSelected(m_list->firstChild(), true);
	m_list->setFocus();
}

void KexiBrowser::updateItemName( KexiPart::Item& item, bool dirty )
{
	if (!(m_features & Writable))
		return;
	KexiBrowserItem *bitem = m_normalItems.value( item.identifier() );
	if (!bitem)
		return;
	bitem->setText( 0, item.name() + (dirty ? "*" : "") );
}

void KexiBrowser::slotSettingsChanged(int)
{
	m_singleClick = KGlobalSettings::singleClick();
}

void KexiBrowser::selectItem(KexiPart::Item& item)
{
	KexiBrowserItem *bitem = m_normalItems.value( item.identifier() );
	if (!bitem)
		return;
	m_list->setSelected(bitem, true);
	m_list->ensureItemVisible(bitem);
	m_list->setCurrentItem(bitem);
}

void KexiBrowser::clearSelection()
{
	m_list->clearSelection();
	Q3ListViewItem *item = m_list->firstChild();
	if (item) {
		m_list->ensureItemVisible(item);
	}
}

void KexiBrowser::slotNewObjectMenuAboutToShow()
{
	kexidbg << "KexiBrowser::slotNewObjectMenuAboutToShow()" << endl;
	if ((m_features & Toolbar) && m_newObjectMenu && m_newObjectMenu->isEmpty()) {
		//preload items
		KexiPart::PartInfoList *plist
			= Kexi::partManager().partInfoList(); //this list is properly sorted
		foreach (KexiPart::Info *info, *plist) {
			//add an item to "New object" toolbar menu 
			QAction *action = KexiMainWindowIface::global()->actionCollection()->action( 
				KexiPart::nameForCreateAction(*info) );
			if (action) {
				m_newObjectMenu->addAction(action);
			}
			else {
				//! @todo err
			}
		}
	}
}

void KexiBrowser::slotExecuteObject()
{
	if (!m_executeAction)
		return;
	KexiPart::Item* item = selectedPartItem();
	if (item)
		emit executeItem( item );
}

void KexiBrowser::slotExportAsDataTable()
{
	if (!m_dataExportAction)
		return;
	KexiPart::Item* item = selectedPartItem();
	if (item)
		emit exportItemAsDataTable( item );
}

KexiPart::Item* KexiBrowser::selectedPartItem() const
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	return it ? it->partItem() : 0;
}

bool KexiBrowser::actionEnabled(const QString& actionName) const
{
	if (actionName=="project_export_data_table" && (m_features & ContextMenus))
		return m_exportActionMenu->isVisible();
	kWarning() << "KexiBrowser::actionEnabled() no such action: " << actionName << endl;
	return false;
}

void KexiBrowser::slotPrintObject()
{
	if (!m_printAction)
		return;
	KexiPart::Item* item = selectedPartItem();
	if (item)
		emit printItem( item );
}

void KexiBrowser::slotPageSetupForObject()
{
	if (!m_pageSetupAction)
		return;
	KexiPart::Item* item = selectedPartItem();
	if (item)
		emit pageSetupForItem( item );
}


void KexiBrowser::setReadOnly(bool set)
{
	m_readOnly = set;
	if (m_deleteAction)
		m_deleteAction->setEnabled(!m_readOnly);
	if (m_renameAction)
		m_renameAction->setEnabled(!m_readOnly);
	if (m_newObjectAction) {
		m_newObjectAction->setEnabled(!m_readOnly);
		if (m_features & Toolbar) {
			m_newObjectMenu->setEnabled(!m_readOnly);
			m_newObjectToolButton->setEnabled(!m_readOnly);
		}
	}
}

bool KexiBrowser::isReadOnly() const
{
	return m_readOnly;
}

void KexiBrowser::clear()
{
	m_list->clear();
}

//--------------------------------------------
KexiBrowserListView::KexiBrowserListView(QWidget *parent)
 : K3ListView(parent)
 , nameEndsWithAsterisk(false)
 , enableExecuteArea(true)
{
	setObjectName("KexiBrowserListView");
}

KexiBrowserListView::~KexiBrowserListView()
{
}

void KexiBrowserListView::rename(Q3ListViewItem *item, int c)
{
	if (renameLineEdit()->isVisible())
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if (it->partItem() && c==0) {
		//only edit 1st column for items, not item groups
//TODO: also check it this item is not read-only
//		item->setText(0, item->text(0).mid(1,item->text(0).length()-2));
		//remove "modified" flag for editing
		nameEndsWithAsterisk = item->text(0).endsWith("*");
		if (nameEndsWithAsterisk)
			item->setText(0, item->text(0).left(item->text(0).length()-1));
		K3ListView::rename(item, c);
		adjustColumn(0);
	}
}

bool KexiBrowserListView::isExecuteArea( const QPoint& point )
{
	return enableExecuteArea && K3ListView::isExecuteArea(point);
}

//--------------------------------------------
KexiMenuBase::KexiMenuBase(QWidget* parent, KActionCollection *col)
 : KMenu(parent)
 , m_actionCollection(col)
{
}

KexiMenuBase::~KexiMenuBase()
{
}

QAction* KexiMenuBase::addAction(const QString& actionName)
{
	QAction* action = m_actionCollection->action(actionName);
	if (action)
		KMenu::addAction(action);
	return action;
}

//--------------------------------------------
KexiItemMenu::KexiItemMenu(QWidget* parent, KActionCollection *col)
 : KexiMenuBase(parent, col)
{
}

KexiItemMenu::~KexiItemMenu()
{
}

void KexiItemMenu::update(KexiPart::Info* partInfo, KexiPart::Item* partItem)
{
	clear();
	QString title_text( partItem->name() );
	KexiPart::Part *part = partInfo ? Kexi::partManager().part(partInfo) : 0;
	if (part && !part->instanceCaption().isEmpty()) {
		//+ type name
		title_text +=  (" : " + part->instanceCaption());
	}
	addTitle( KIcon(partInfo->itemIcon()), title_text );
	
	if (m_actionCollection->action("open_object")
		&& m_actionCollection->action("open_object")->isEnabled()
		&& partItem && part && (part->supportedViewModes() & Kexi::DataViewMode))
	{
		addAction("open_object");
	}
	if (m_actionCollection->action("design_object")
		&& m_actionCollection->action("design_object")->isEnabled()
		&& partItem && part && (part->supportedViewModes() & Kexi::DesignViewMode))
	{
		addAction("design_object");
	}
	if (m_actionCollection->action("editText_object")
		&& m_actionCollection->action("editText_object")->isEnabled()
		&& partItem && part && (part->supportedViewModes() & Kexi::TextViewMode))
	{
		addAction("editText_object");
	}
	if (addAction("new_object"))
		addSeparator();

#ifdef KEXI_SHOW_UNIMPLEMENTED
	//todo	plugSharedAction("edit_cut", m_itemMenu);
	//todo	plugSharedAction("edit_copy", m_itemMenu);
	//todo	addSeparator();
#endif
	bool addSep = false;
	if (partItem && partInfo->isExecuteSupported()) {
		addAction("data_execute");
		addSep = true;
	}
	if (partItem && partInfo->isDataExportSupported()) {
		addAction("export_object");
		addSep = true;
	}
	if (addSep)
		addSeparator();
	
	if (partItem && partInfo->isPrintingSupported())
		addAction("print_object");
	if (partItem && partInfo->isPrintingSupported())
		addAction("pageSetupForObject");
	if (m_actionCollection->action("edit_rename") || m_actionCollection->action("edit_delete"))
		addSeparator();
	addAction("edit_rename");
	addAction("edit_delete");
}

//--------------------------------------------
KexiGroupMenu::KexiGroupMenu(QWidget* parent, KActionCollection *col)
 : KexiMenuBase(parent, col)
{
}

KexiGroupMenu::~KexiGroupMenu()
{
}

void KexiGroupMenu::update(KexiPart::Info* partInfo)
{
	clear();
	addTitle(KIcon(partInfo->itemIcon()), partInfo->groupName());
	addAction("new_object");
#ifdef KEXI_SHOW_UNIMPLEMENTED
//	addSeparator();
//	qobject_cast<KexiBrowser*>(parent())->plugSharedAction("edit_paste", this);
#endif
}

#include "kexibrowser.moc"
#include "kexibrowser_p.moc"
