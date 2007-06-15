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
	connect(m_list, SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
		this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem *, const QPoint&)));
	connect(m_list, SIGNAL(selectionChanged(Q3ListViewItem*)), this,
		SLOT(slotSelectionChanged(Q3ListViewItem*)));
	
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
	m_actions->addAction("open_object",
		m_openAction = new KAction(KIcon("document-open"), i18n("&Open"), this) );
	m_openAction->setToolTip(i18n("Open object"));
	m_openAction->setWhatsThis(i18n("Opens object selected in the list"));
	connect(m_openAction, SIGNAL(triggered()), this, SLOT(slotOpenObject()));

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
		m_actions->addAction("edit_delete",
			m_deleteAction = new KAction(KIcon("edit-delete"), i18n("&Delete"), this) );
		m_deleteAction->setToolTip(i18n("Delete object"));
		m_deleteAction->setWhatsThis(i18n("Deletes the object selected in the list"));
		connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotRemove()));

		m_actions->addAction("edit_rename",
			m_renameAction = new KAction(i18n("&Rename"), this) );
		connect(m_renameAction, SIGNAL(triggered()), this, SLOT(slotRename()));
#ifdef KEXI_SHOW_UNIMPLEMENTED
		//todo	plugSharedAction("edit_cut",SLOT(slotCut()));
		//todo	plugSharedAction("edit_copy",SLOT(slotCopy()));
		//todo	plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

		m_designAction = new KAction(KIcon("edit"), i18n("&Design"), this);
		m_designAction->setObjectName("design_object");
		m_designAction->setToolTip(i18n("Design object"));
		m_designAction->setWhatsThis(i18n("Starts designing of the object selected in the list"));
		connect(m_designAction, SIGNAL(triggered()), this, SLOT(slotDesignObject()));
		if (m_features & Toolbar) {
			btn = new KexiSmallToolButton(m_designAction, this);
			buttons_flyr->add(btn);
		}

		m_editTextAction = new KAction(i18n("Open in &Text View"), this);
		m_editTextAction->setObjectName("editText_object");
		m_editTextAction->setToolTip(i18n("Open object in text view"));
		m_editTextAction->setWhatsThis(i18n("Opens selected object in the list in text view"));
		connect(m_editTextAction, SIGNAL(triggered()), this, SLOT(slotEditTextObject()));

		m_newObjectAction = new KAction(KIcon("document-new"), QString(), this);
		m_newObjectAction->setObjectName("new_object");
		connect(m_newObjectAction, SIGNAL(triggered()), this, SLOT(slotNewObject()));
		if (m_features & Toolbar) {
			m_newObjectToolButton = new KexiSmallToolButton(this);
			m_newObjectToolButton->setObjectName("new_object");
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

	m_executeAction = new KAction(KIcon("media-playback-start"), i18n("Execute"), this);
	m_executeAction->setObjectName("data_execute");
	connect(m_executeAction, SIGNAL(triggered()), this, SLOT(slotExecuteObject()));

	m_exportActionMenu = new KActionMenu(i18n("Export"), this);
	m_dataExportAction = new KAction( KIcon("table"),
		i18nc("Export->To File As Data &Table... ", "To &File As Data Table..."), this);
	m_dataExportAction->setObjectName("exportAsDataTable");
	connect(m_dataExportAction, SIGNAL(triggered()), 
		this, SLOT(slotExportAsDataTable()));
	m_dataExportAction->setWhatsThis(
		i18n("Exports data from the currently selected table or query data to a file."));
	m_exportActionMenu->addAction( m_dataExportAction );

	m_printAction = new KAction(KIcon("document-print"), i18n("&Print..."), this);
	m_printAction->setObjectName("printItem");
	connect(m_printAction, SIGNAL(triggered()), 
		this, SLOT(slotPrintItem()));
	m_printAction->setWhatsThis(
		i18n("Prints data from the currently selected table or query."));
	m_pageSetupAction = new KAction(i18n("Page Setup..."), this);
	m_pageSetupAction->setObjectName("pageSetupForItem");
	connect(m_pageSetupAction, SIGNAL(triggered()), 
		this, SLOT(slotPageSetupForItem()));
	m_pageSetupAction->setWhatsThis(
		i18n("Shows page setup for printing the active table or query."));

	if (KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
		m_partMenu = 0;
	}
	else {
		m_partMenu = new KMenu(this);
		m_partMenu->setObjectName("partMenu");
		m_partMenuTitle = m_partMenu->addTitle(QString());
		m_partMenu->addAction(m_newObjectAction);
#ifdef KEXI_SHOW_UNIMPLEMENTED
		m_partMenu->addSeparator();
		plugSharedAction("edit_paste", m_partMenu);
#endif
	}

	if (m_features & ContextMenus) {
		//init menus
		m_itemMenu = new KMenu(this);
		m_itemMenu->setObjectName("itemMenu");
		m_itemMenuTitle = m_itemMenu->addTitle(QString());
		m_itemMenu->addAction(m_openAction);

		if (m_designAction) {
			m_itemMenu->addAction(m_designAction);
		}

		if (m_editTextAction) {
			m_itemMenu->addAction(m_editTextAction);
		}

		if (m_newObjectAction) {
			m_itemMenu->addAction(m_newObjectAction);
			m_itemMenu->addSeparator();
		}
#ifdef KEXI_SHOW_UNIMPLEMENTED
	//todo	plugSharedAction("edit_cut", m_itemMenu);
	//todo	plugSharedAction("edit_copy", m_itemMenu);
	//todo	m_itemMenu->addSeparator();
#endif
		m_itemMenu->addAction(m_executeAction);

		m_itemMenu->addAction(m_exportActionMenu);
		m_exportActionMenu_sep = m_itemMenu->addSeparator();

		m_itemMenu->addAction(m_printAction);

		m_itemMenu->addAction(m_pageSetupAction);
		if (m_renameAction || m_deleteAction) {
			m_pageSetupAction_sep = m_itemMenu->addSeparator();
		}
		else {
			m_pageSetupAction_sep = 0;
		}

		if (m_renameAction)
			m_itemMenu->addAction(m_renameAction);
		if (m_deleteAction)
			m_itemMenu->addAction(m_deleteAction);
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
	if (bit->item()) {
		pm = m_itemMenu;
		//update menu title
		QString title_text = bit->text(0).trimmed();
		KexiBrowserItem *par_it = static_cast<KexiBrowserItem*>(bit->parent());
		KexiPart::Part* par_part = 0;
		if (par_it && par_it->info() && ((par_part = Kexi::partManager().part(par_it->info()))) && !par_part->instanceCaption().isEmpty()) {
			//add part type name
			title_text +=  (" : " + par_part->instanceCaption());
		}
		m_itemMenuTitle->setText(title_text);
		m_itemMenuTitle->setIcon(KIcon(bit->info()->itemIcon()));
	}
	else if (m_partMenu) {
		pm = m_partMenu;
		QString title_text = bit->text(0).trimmed();
		m_itemMenuTitle->setText(title_text);
		m_itemMenuTitle->setIcon(KIcon(bit->info()->itemIcon()));
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
		m_openAction->setVisible(m_openAction->isEnabled());
		if (m_designAction)
			m_designAction->setVisible(m_designAction->isEnabled());
		if (m_editTextAction)
			m_editTextAction->setVisible(part && m_editTextAction->isEnabled());
		if (m_executeAction)
			m_executeAction->setVisible(gotitem && it->info()->isExecuteSupported());
		if (m_exportActionMenu) {
			m_exportActionMenu->setVisible(gotitem && it->info()->isDataExportSupported());
			m_exportActionMenu_sep->setVisible(gotitem && it->info()->isDataExportSupported());
		}
		if (m_printAction)
			m_printAction->setVisible(gotitem && it->info()->isPrintingSupported());
		if (m_pageSetupAction) {
			m_pageSetupAction->setVisible(gotitem && it->info()->isPrintingSupported());
			if (m_pageSetupAction_sep)
				m_pageSetupAction_sep->setVisible(gotitem && it->info()->isPrintingSupported());
		}
	}

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
	emit selectionChanged(it ? it->item() : 0);
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
		kDebug() << "resize!" << endl;
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
	bool ok = it->item()->name().toLower()!=txt.toLower(); //the new name must be different
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
	return it ? it->item() : 0;
}

bool KexiBrowser::actionEnabled(const Q3CString& actionName) const
{
	if (actionName=="project_export_data_table" && (m_features & ContextMenus))
		return m_exportActionMenu->isVisible();
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
