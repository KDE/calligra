/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QKeyEvent>
#include <QEvent>
#include <Q3CString>

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

#include <kexi.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <kexidialogbase.h>
#include <keximainwindow.h>
#include <kexiutils/identifier.h>
#include <widget/utils/kexiflowlayout.h>
#include <widget/kexismalltoolbutton.h>
#include <kexidb/utils.h>

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

KexiBrowser::KexiBrowser(QWidget* parent, KexiMainWindow *mainWin, int features)
 : QWidget(parent, "KexiBrowser")
 , m_mainWin(mainWin)
 , m_features(features)
 , m_actions( new KActionCollection(this) )
 , m_baseItems(199, false)
 , m_normalItems(199)
 , m_prevSelectedPart(0)
 , m_singleClick(false)
// , m_nameEndsWithAsterisk(false)
 , m_readOnly(false)
// , m_enableExecuteArea(true)
{
	setCaption(i18n("Project Navigator"));
	setIcon(*m_mainWin->icon());

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
	connect(m_list, SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem *, const QPoint&)));
	connect(m_list, SIGNAL(selectionChanged(Q3ListViewItem*)), this,
		SLOT(slotSelectionChanged(Q3ListViewItem*)));
	
	KConfig *config = KGlobal::config();
	config->setGroup("MainWindow");
	if ((m_features & SingleClickOpensItemOptionEnabled) 
		&& config->readBoolEntry("SingleClickOpensItem", false))
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
	m_openAction = new KAction(i18n("&Open"), "fileopen", 0/*Qt::Key_Enter conflict!*/, this, 
		SLOT(slotOpenObject()), this, "open_object");
	m_openAction->setToolTip(i18n("Open object"));
	m_openAction->setWhatsThis(i18n("Opens object selected in the list"));

//	m_openAction->plug(m_toolbar);
	KexiSmallToolButton *btn;
	if (m_features & Toolbar) {
		btn = new KexiSmallToolButton(this, m_openAction);
		buttons_flyr->add(btn);
	}

	if (m_mainWin->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
		m_deleteAction = 0;
		m_renameAction = 0;
		m_designAction = 0;
		m_editTextAction = 0;
		m_newObjectAction = 0;
		m_newObjectPopup = 0;
	}
	else {
		m_deleteAction = new KAction(i18n("&Delete"), "editdelete", 0/*Qt::Key_Delete*/, 
			this, SLOT(slotRemove()), m_actions, "edit_delete");
	//! @todo 1.1: just add "Delete" tooltip and what's this
		m_deleteAction->setToolTip(i18n("&Delete").replace("&",""));

		m_renameAction = new KAction(i18n("&Rename"), "", 0, 
			this, SLOT(slotRename()), m_actions, "edit_rename");
#ifdef KEXI_SHOW_UNIMPLEMENTED
		//todo	plugSharedAction("edit_cut",SLOT(slotCut()));
		//todo	plugSharedAction("edit_copy",SLOT(slotCopy()));
		//todo	plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

		m_designAction = new KAction(i18n("&Design"), "edit", 0/*Qt::CTRL + Qt::Key_Enter conflict!*/, this, 
			SLOT(slotDesignObject()), this, "design_object");
		m_designAction->setToolTip(i18n("Design object"));
		m_designAction->setWhatsThis(i18n("Starts designing of the object selected in the list"));
		if (m_features & Toolbar) {
			btn = new KexiSmallToolButton(this, m_designAction);
			buttons_flyr->add(btn);
		}

		m_editTextAction = new KAction(i18n("Open in &Text View"), "", 0, this, 
			SLOT(slotEditTextObject()), this, "editText_object");
		m_editTextAction->setToolTip(i18n("Open object in text view"));
		m_editTextAction->setWhatsThis(i18n("Opens selected object in the list in text view"));

		m_newObjectAction = new KAction("", "filenew", 0, this, SLOT(slotNewObject()), this, "new_object");
		if (m_features & Toolbar) {
			m_newObjectToolButton = new KexiSmallToolButton(this, "", QIcon(), "new_object");
			m_newObjectPopup = new KMenu(this, "newObjectPopup");
			connect(m_newObjectPopup, SIGNAL(aboutToShow()), this, SLOT(slotNewObjectPopupAboutToShow()));
		//	KexiPart::Part* part = Kexi::partManager().part("kexi/table");
		//	m_newObjectPopup->insertItem( KIcon(part->info()->createItemIcon()), part->instanceName() );
			m_newObjectToolButton->setPopup(m_newObjectPopup);
			m_newObjectToolButton->setPopupDelay(QApplication::startDragTime());
			connect(m_newObjectToolButton, SIGNAL(clicked()), this, SLOT(slotNewObject()));
			buttons_flyr->add(m_newObjectToolButton);

			m_deleteObjectToolButton = new KexiSmallToolButton(this, m_deleteAction);
			m_deleteObjectToolButton->setTextLabel("");
			buttons_flyr->add(m_deleteObjectToolButton);
		}
	}

	m_executeAction = new KAction(i18n("Execute"), "player_play", 0, this,
		SLOT(slotExecuteObject()), this, "data_execute");

	m_exportActionMenu = new KActionMenu(i18n("Export"));
	m_dataExportAction = new KAction(i18n("Export->To File As Data &Table... ", "To &File As Data Table..."), 
		"table", 0, this, SLOT(slotExportAsDataTable()), this, "exportAsDataTable");
	m_dataExportAction->setWhatsThis(
		i18n("Exports data from the currently selected table or query data to a file."));
	m_exportActionMenu->insert( m_dataExportAction );

	m_printAction = new KAction(i18n("&Print..."), "fileprint", 0, this, 
		SLOT(slotPrintItem()), this, "printItem");
	m_printAction->setWhatsThis(
		i18n("Prints data from the currently selected table or query."));
	m_pageSetupAction = new KAction(i18n("Page Setup..."), "", 0, this, 
		SLOT(slotPageSetupForItem()), this, "pageSetupForItem");
	m_pageSetupAction->setWhatsThis(
		i18n("Shows page setup for printing the active table or query."));

	if (m_mainWin->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
		m_partPopup = 0;
	}
	else {
		m_partPopup = new KMenu(this, "partPopup");
		m_partPopupTitle_id = m_partPopup->insertTitle("");
		m_newObjectAction->plug(m_partPopup);
#ifdef KEXI_SHOW_UNIMPLEMENTED
		m_partPopup->insertSeparator();
		plugSharedAction("edit_paste", m_partPopup);
#endif
	}

	if (m_features & ContextMenus) {
		//init popups
		m_itemPopup = new KMenu(this, "itemPopup");
		m_itemPopupTitle_id = m_itemPopup->insertTitle("");
		m_openAction->plug(m_itemPopup);
		m_openAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);

		if (m_designAction) {
			m_designAction->plug(m_itemPopup);
			m_designAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);
		}

		if (m_editTextAction) {
			m_editTextAction->plug(m_itemPopup);
			m_editTextAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);
		}

		if (m_newObjectAction) {
			m_newObjectAction->plug(m_itemPopup);
			m_itemPopup->insertSeparator();
		}
#ifdef KEXI_SHOW_UNIMPLEMENTED
	//todo	plugSharedAction("edit_cut", m_itemPopup);
	//todo	plugSharedAction("edit_copy", m_itemPopup);
	//todo	m_itemPopup->insertSeparator();
#endif
		m_executeAction->plug(m_itemPopup);
		m_executeAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);

		m_exportActionMenu->plug(m_itemPopup);
		m_exportActionMenu_id = m_exportActionMenu->menuId(0);
		m_itemPopup->insertSeparator();
		m_exportActionMenu_id_sep = m_itemPopup->idAt(m_itemPopup->count()-1);

		m_printAction->plug(m_itemPopup);
		m_printAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);

		m_pageSetupAction->plug(m_itemPopup);
		m_pageSetupAction_id = m_itemPopup->idAt(m_itemPopup->count()-1);
		if (m_renameAction || m_deleteAction) {
			m_itemPopup->insertSeparator();
			m_pageSetupAction_id_sep = m_itemPopup->idAt(m_itemPopup->count()-1);
		}
		else {
			m_pageSetupAction_id_sep = -1;
		}

		if (m_renameAction)
			m_renameAction->plug(m_itemPopup);
		if (m_deleteAction)
			m_deleteAction->plug(m_itemPopup);
	}
	else {
		m_itemPopup = 0;
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

	KexiPart::PartInfoList *pl = Kexi::partManager().partInfoList();
	for (KexiPart::Info *info = pl->first(); info; info = pl->next()) {
		if (!info->isVisibleInNavigator())
			continue;
		if (!m_itemsMimeType.isEmpty() && info->mimeType()!=m_itemsMimeType.latin1())
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

QString KexiBrowser::itemsMimeType() const
{
	return m_itemsMimeType;
}

KexiBrowserItem *KexiBrowser::addGroup(KexiPart::Info& info)
{
	if(!info.isVisibleInNavigator())
		return 0;

	KexiBrowserItem *item = new KexiBrowserItem(m_list, &info);
	m_baseItems.insert(info.mimeType().lower(), item);
	return item;
//	kDebug() << "KexiBrowser::addGroup()" << endl;
}

KexiBrowserItem* KexiBrowser::addItem(KexiPart::Item& item)
{
	//part object for this item
	KexiBrowserItem *parent = item.mimeType().isEmpty() ? 0 : m_baseItems.find(item.mimeType().lower());
	return addItem(item, parent, parent->info());
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
	KexiBrowserItem *to_remove=m_normalItems.take(item.identifier());
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
	if (bit->item()) {
		pm = m_itemPopup;
		//update popup title
		QString title_text = bit->text(0).trimmed();
		KexiBrowserItem *par_it = static_cast<KexiBrowserItem*>(bit->parent());
		KexiPart::Part* par_part = 0;
		if (par_it && par_it->info() && ((par_part = Kexi::partManager().part(par_it->info()))) && !par_part->instanceCaption().isEmpty()) {
			//add part type name
			title_text +=  (" : " + par_part->instanceCaption());
		}
		pm->changeTitle(m_itemPopupTitle_id, *bit->pixmap(0), title_text);
	}
	else if (m_partPopup) {
		pm = m_partPopup;
		QString title_text = bit->text(0).trimmed();
		pm->changeTitle(m_partPopupTitle_id, *bit->pixmap(0), title_text);
/*		KexiPart::Part* part = Kexi::partManager().part(bit->info());
		if (part)
			m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceName() ));
		else
			m_newObjectAction->setText(i18n("&Create Object..."));
		m_newObjectAction->setIcon( KIcon(bit->info()->itemIcon()) );*/
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
	if (!it->item() && !m_singleClick /*annoying when in single click mode*/) {
		m_list->setOpen( vitem, !vitem->isOpen() );
		return;
	}
	if (it->info()->isExecuteSupported())
		emit executeItem( it->item() );
	else
		emit openOrActivateItem( it->item(), Kexi::DataViewMode );
}

void
KexiBrowser::slotSelectionChanged(Q3ListViewItem* i)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(i);
	if (!it)
		return;
	KexiPart::Part* part = Kexi::partManager().part(it->info());
	if (!part) {
		it = static_cast<KexiBrowserItem*>(it->parent());
		if (it) {
			part = Kexi::partManager().part(it->info());
		}
	}

	const bool gotitem = it && it->item();
	//bool gotgroup = it && !it->item();
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
	if (m_designAction)
		m_designAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
	if (m_editTextAction)
		m_editTextAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::TextViewMode));

	if (m_features & ContextMenus) {
		m_itemPopup->setItemVisible(m_openAction_id, m_openAction->isEnabled());
		if (m_designAction)
			m_itemPopup->setItemVisible(m_designAction_id, m_designAction->isEnabled());
		if (m_editTextAction)
			m_itemPopup->setItemVisible(m_editTextAction_id, part && m_editTextAction->isEnabled());
		if (m_executeAction)
			m_itemPopup->setItemVisible(m_executeAction_id, gotitem && it->info()->isExecuteSupported());
		if (m_exportActionMenu) {
			m_itemPopup->setItemVisible(m_exportActionMenu_id, gotitem && it->info()->isDataExportSupported());
			m_itemPopup->setItemVisible(m_exportActionMenu_id_sep, gotitem && it->info()->isDataExportSupported());
		}
		if (m_printAction)
			m_itemPopup->setItemVisible(m_printAction_id, gotitem && it->info()->isPrintingSupported());
		if (m_pageSetupAction) {
			m_itemPopup->setItemVisible(m_pageSetupAction_id, gotitem && it->info()->isPrintingSupported());
			if (m_pageSetupAction_id_sep!=-1)
				m_itemPopup->setItemVisible(m_pageSetupAction_id_sep, gotitem && it->info()->isPrintingSupported());
		}
	}

	if (m_prevSelectedPart != part) {
	m_prevSelectedPart = part;
		if (part) {
			if (m_newObjectAction) {
				m_newObjectAction->setText(i18n("&Create Object: %1...").arg( part->instanceCaption() ));
				m_newObjectAction->setIcon( part->info()->createItemIcon() );
				if (m_features & Toolbar) {
					m_newObjectToolButton->setIconSet( part->info()->createItemIcon() );
					QToolTip::add(m_newObjectToolButton, 
						i18n("Create object: %1").arg( part->instanceCaption().lower() ));
					Q3WhatsThis::add(m_newObjectToolButton, 
						i18n("Creates a new object: %1").arg( part->instanceCaption().lower() ));
				}
			}
		} else {
			if (m_newObjectAction) {
				m_newObjectAction->setText(i18n("&Create Object..."));
	//			m_newObjectToolbarAction->setIconSet( KIcon("filenew") );
	//			m_newObjectToolbarAction->setText(m_newObjectAction->text());
				if (m_features & Toolbar) {
					m_newObjectToolButton->setIconSet( "filenew" );
					QToolTip::add(m_newObjectToolButton, i18n("Create object"));
					Q3WhatsThis::add(m_newObjectToolButton, i18n("Creates a new object"));
				}
			}
		}
	}
	emit selectionChanged(it ? it->item() : 0);
}

void KexiBrowser::installEventFilter ( const QObject * filterObj )
{
	if (!filterObj)
		return;
	m_list->installEventFilter ( filterObj );
	QWidget::installEventFilter ( filterObj );
}

bool KexiBrowser::eventFilter ( QObject *o, QEvent * e )
{
	if (o==m_list && e->type()==QEvent::Resize) {
		kDebug() << "resize!" << endl;
	}
	if (o==m_list->renameLineEdit()) {
		if (e->type()==QEvent::Hide) 
			itemRenameDone();
	}
	else if (e->type()==QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
			if (0==(ke->state() & (Qt::ShiftButton|Qt::ControlButton|Qt::AltButton))) {
				Q3ListViewItem *it = m_list->selectedItem();
				if (it)
					slotExecuteItem(it);
			}
			else if (Qt::ControlButton==(ke->state() & (Qt::ShiftButton|Qt::ControlButton|Qt::AltButton))) {
				slotDesignObject();
			}
		}
	}
	else if (e->type()==QEvent::ShortcutOverride) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		//override delete action
		if (ke->key()==Qt::Key_Delete && ke->state()==Qt::NoButton) {
			slotRemove();
			ke->accept();
			return true;
		}
		//override rename action
		if (ke->key()==Qt::Key_F2 && ke->state()==Qt::NoButton) {
			slotRename();
			ke->accept();
			return true;
		}
/*		else if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
			if (ke->state()==Qt::ControlModifier) {
				slotDesignObject();
			}
			else if (ke->state()==0 && !m_list->renameLineEdit()->isVisible()) {
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
	if (!it || !it->item())
		return;
	emit removeItem( it->item() );
}

void KexiBrowser::slotNewObject()
{
	if (!m_newObjectAction || !(m_features & Writable))
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->info())
		return;
	emit newItem( it->info() );
}

void KexiBrowser::slotOpenObject()
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), Kexi::DataViewMode );
}

void KexiBrowser::slotDesignObject()
{
	if (!m_designAction)
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), Kexi::DesignViewMode );
}

void KexiBrowser::slotEditTextObject()
{
	if (!m_editTextAction)
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(m_list->selectedItem());
	if (!it || !it->item())
		return;
	emit openItem( it->item(), Kexi::TextViewMode );
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
	bool ok = it->item()->name().lower()!=txt.lower(); //the new name must be different
	if (ok) {
		/* TODO */
		emit renameItem(it->item(), txt, ok);
	}
	if (!ok) {
		txt = it->item()->name(); //revert
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
	KexiBrowserItem *bitem = m_normalItems[item.identifier()];
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
	KexiBrowserItem *bitem = m_normalItems[item.identifier()];
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

void KexiBrowser::slotNewObjectPopupAboutToShow()
{
	if ((m_features & Toolbar) && m_newObjectPopup && m_newObjectPopup->count()==0) {
		//preload items
		KexiPart::PartInfoList *list = Kexi::partManager().partInfoList(); //this list is properly sorted
		for (KexiPart::PartInfoListIterator it(*list); it.current(); ++it) {
			//add an item to "New object" toolbar popup 
			KAction *action = m_mainWin->actionCollection()->action( 
				KexiPart::nameForCreateAction(*it.current()) );
			if (action) {
				action->plug(m_newObjectPopup);
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
	return it ? it->item() : 0;
}

bool KexiBrowser::actionEnabled(const Q3CString& actionName) const
{
	if (actionName=="project_export_data_table" && (m_features & ContextMenus))
		return m_itemPopup->isItemVisible(m_exportActionMenu_id);
	kWarning() << "KexiBrowser::actionEnabled() no such action: " << actionName << endl;
	return false;
}

void KexiBrowser::slotPrintItem()
{
	if (!m_printAction)
		return;
	KexiPart::Item* item = selectedPartItem();
	if (item)
		emit printItem( item );
}

void KexiBrowser::slotPageSetupForItem()
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
			m_newObjectPopup->setEnabled(!m_readOnly);
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
 : K3ListView(parent, "KexiBrowserListView")
 , nameEndsWithAsterisk(false)
 , enableExecuteArea(true)
{
}

KexiBrowserListView::~KexiBrowserListView()
{
}

void KexiBrowserListView::rename(Q3ListViewItem *item, int c)
{
	if (renameLineEdit()->isVisible())
		return;
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if (it->item() && c==0) {
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

#include "kexibrowser.moc"
#include "kexibrowser_p.moc"
