/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "KexiProjectNavigator.h"
#include "KexiProjectTreeView.h"
#include "KexiProjectModel.h"
#include "KexiProjectModelItem.h"

#include "KexiProjectListView_p.h" //TODO temp for Menus

#include <QHeaderView>
#include <qpoint.h>
#include <qpixmapcache.h>
#include <qtoolbutton.h>
#include <QKeyEvent>
#include <QEvent>

#include <kglobalsettings.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kglobal.h>
#include <KActionCollection>
#include <KActionMenu>
#include <KDialog>

#include <kexi.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/identifier.h>
#include <kexiutils/FlowLayout.h>
#include <kexiutils/SmallToolButton.h>
#include <kexidb/utils.h>
#include <kexi_global.h>


KexiProjectNavigator::KexiProjectNavigator(QWidget* parent, Features features)
        : QWidget(parent)
        , m_features(features)
        , m_actions(new KActionCollection(this))
        , m_prevSelectedPart(0)
        , m_singleClick(false)
        , m_readOnly(false)
{
    kDebug();
    setObjectName("KexiProjectNavigator");
    setWindowTitle(i18n("Project Navigator"));
    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    m_model = new KexiProjectModel();
    
    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(
        KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2);
    lyr->setSpacing(KDialog::marginHint() / 2);

    KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);

    m_list = new KexiProjectTreeView(this);
    m_list->setModel(m_model);
    lyr->addWidget(m_list);

    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
    slotSettingsChanged(0);
    

    m_list->header()->hide();
    m_list->setAllColumnsShowFocus(true);
    m_list->setAlternatingRowColors(true);
//    m_list->renameLineEdit()->setValidator(new KexiUtils::IdentifierValidator(this));
    
    connect(m_list, SIGNAL(pressed(const QModelIndex&)), this,SLOT(slotSelectionChanged(const QModelIndex&)));
    connect(m_list, SIGNAL(contextMenu(QTreeView *, const QModelIndex&, const QPoint &)), this, SLOT(slotContextMenu(QTreeView*, const QModelIndex &, const QPoint&)));

    KConfigGroup mainWindowGroup = KGlobal::config()->group("MainWindow");
    if ((m_features & SingleClickOpensItemOptionEnabled) && mainWindowGroup.readEntry("SingleClickOpensItem", false)) {
        connect(m_list, SIGNAL(activate(const QModelIndex&)), this, SLOT(slotExecuteItem(const QModelIndex&)));
    } else {
        connect(m_list, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(slotExecuteItem(const QModelIndex&)));
//        m_list->enableExecuteArea = false;
    }

    // actions
    m_openAction = addAction("open_object", KIcon("document-open"), i18n("&Open"),
                             i18n("Open object"), i18n("Opens object selected in the list."),
                             SLOT(slotOpenObject()));

// m_openAction->plug(m_toolbar);
    KexiSmallToolButton *btn;
    if (m_features & Toolbar) {
        btn = new KexiSmallToolButton(m_openAction, this);
        buttons_flyr->addWidget(btn);
    }

    if (KexiMainWindowIface::global() && KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
        m_deleteAction = 0;
        m_renameAction = 0;
        m_designAction = 0;
        m_editTextAction = 0;
        m_newObjectAction = 0;
    } else {
        m_deleteAction = addAction("edit_delete", KIcon("edit-delete"), i18n("&Delete"),
                                   i18n("Delete object"), i18n("Deletes the object selected in the list."),
                                   SLOT(slotRemove()));

        m_renameAction = addAction("edit_rename", KIcon("edit-rename"), i18n("&Rename"),
                                   i18n("Rename object"), i18n("Renames the object selected in the list."),
                                   SLOT(slotRename()));
#ifdef KEXI_SHOW_UNIMPLEMENTED
        //todo plugSharedAction("edit_cut",SLOT(slotCut()));
        //todo plugSharedAction("edit_copy",SLOT(slotCopy()));
        //todo plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

        m_designAction = addAction("design_object", KIcon("document-properties"), i18n("&Design"),
                                   i18n("Design object"), i18n("Starts designing of the object selected in the list."),
                                   SLOT(slotDesignObject()));
        if (m_features & Toolbar) {
            btn = new KexiSmallToolButton(m_designAction, this);
//   m_designAction->setEnabled(false);
            buttons_flyr->addWidget(btn);
        }

        m_editTextAction = addAction("editText_object", KIcon(), i18n("Open in &Text View"),
                                     i18n("Open object in text view"), i18n("Opens selected object in the list in text view."),
                                     SLOT(slotEditTextObject()));

        m_newObjectAction = addAction("new_object", KIcon("document-new"), QString(),QString(), QString(), SLOT(slotNewObject()));

        if (m_features & Toolbar) {
            m_deleteObjectToolButton = new KexiSmallToolButton(m_deleteAction, this);
            buttons_flyr->addWidget(m_deleteObjectToolButton);
        }
    }

    m_executeAction = addAction("data_execute", KIcon("media-playback-start"), i18n("Execute"),
//! @todo tooltip, what's this
                                QString(), QString(),
                                SLOT(slotExecuteObject()));

    m_actions->addAction("export_object",
                         m_exportActionMenu = new KActionMenu(i18n("Export"), this));
    m_dataExportToClipboardAction = addAction("exportToClipboardAsDataTable", KIcon("edit-copy"),
                                   i18nc("Export->To Clipboard as Data... ", "To &Clipboard..."),
                                   i18n("Export data to clipboard"),
                                   i18n("Exports data from the currently selected table or query to clipboard."),
                                   SLOT(slotExportToClipboardAsDataTable()));
    m_exportActionMenu->addAction(m_dataExportToClipboardAction);

    m_dataExportToFileAction = addAction("exportToFileAsDataTable", KIcon("table"),
                                   i18nc("Export->To File As Data &Table... ", "To &File As Data Table..."),
                                   i18n("Export data to a file"),
                                   i18n("Exports data from the currently selected table or query to a file."),
                                   SLOT(slotExportToFileAsDataTable()));
    m_exportActionMenu->addAction(m_dataExportToFileAction);

    m_printAction = addAction("print_object", KIcon("document-print"), i18n("&Print..."),
                              i18n("Print data"),
                              i18n("Prints data from the currently selected table or query."),
                              SLOT(slotPrintObject()));

    m_pageSetupAction = addAction("pageSetupForObject", KIcon(/*"document-page-setup" not yet in Oxygen */), i18n("Page Setup..."),
                                  i18n("Page setup for data"),
                                  i18n("Shows page setup for printing the active table or query."),
                                  SLOT(slotPageSetupForObject()));


    if (KexiMainWindowIface::global() && KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
        m_partMenu = 0;
    } else {
        m_partMenu = new KexiGroupMenu(this, m_actions);
    }

    if (m_features & ContextMenus) {
        m_itemMenu = new KexiItemMenu(this, m_actions);
    } else {
        m_itemMenu = 0;
    }
    if (!(m_features & Writable)) {
        setReadOnly(true);
    }
    slotSelectionChanged(QModelIndex());
}

void KexiProjectNavigator::setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages)
{
    kDebug() << itemsPartClass << ".";
    m_model->setProject(prj, itemsPartClass, partManagerErrorMessages);
    m_list->expandAll();
}

KexiProjectNavigator::~KexiProjectNavigator()
{
}



KAction* KexiProjectNavigator::addAction(const QString& name, const KIcon& icon, const QString& text,
                                const QString& toolTip, const QString& whatsThis, const char* slot)
{
    KAction *action = new KAction(icon, text, this);
    m_actions->addAction(name, action);
    action->setToolTip(toolTip);
    action->setWhatsThis(whatsThis);
    connect(action, SIGNAL(triggered()), this, slot);
    return action;
}

void KexiProjectNavigator::contextMenuEvent(QContextMenuEvent* event)
{
    if (!m_list->currentIndex().isValid() || !(m_features & ContextMenus))
        return;
    
    KexiProjectModelItem *bit = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    KMenu *pm = 0;
    if (bit->partItem()) {
        pm = m_itemMenu;
        KexiProjectModelItem *par_it = static_cast<KexiProjectModelItem*>(bit->parent());
        m_itemMenu->update(par_it->partInfo(), bit->partItem());
    } else if (m_partMenu) {
        pm = m_partMenu;
        m_partMenu->update(bit->partInfo());
    }
    if (pm)
        pm->exec(event->globalPos());

    event->setAccepted(true);
}

void KexiProjectNavigator::slotExecuteItem(const QModelIndex& vitem)
{
    kDebug();
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(vitem.internalPointer());
    if (!it) {
        kDebug() << "No internal pointer";
        return;
    }
//TODO is this needed?
//    if (!it->partItem() && !m_singleClick /*annoying when in single click mode*/) {
//        vitem.
//        m_list->setOpen(vitem, !vitem->isOpen());
//        return;
//    }

    if (it->partInfo()->isExecuteSupported())
        emit executeItem(it->partItem());
    else
        emit openOrActivateItem(it->partItem(), Kexi::DataViewMode);
}

void KexiProjectNavigator::slotSelectionChanged(const QModelIndex& i)
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(i.internalPointer());
    if (!it) {
        m_openAction->setEnabled(false);
        m_designAction->setEnabled(false);
        m_deleteAction->setEnabled(false);
        return;
    }
    KexiPart::Part* part = Kexi::partManager().part(it->partInfo());
    if (!part) {
        it = static_cast<KexiProjectModelItem*>(it->parent());
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
//todo setAvailable("edit_cut",gotitem);
//todo setAvailable("edit_copy",gotitem);
//todo setAvailable("edit_edititem",gotitem);
#endif

    m_openAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::DataViewMode));
    if (m_designAction) {
//  m_designAction->setVisible(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
        m_designAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
    }
    if (m_editTextAction)
        m_editTextAction->setEnabled(gotitem && part && (part->supportedViewModes() & Kexi::TextViewMode));

// if (m_features & ContextMenus) {
//  m_openAction->setVisible(m_openAction->isEnabled());
//  if (m_designAction)
    //  m_designAction->setVisible(m_designAction->isEnabled());
//  if (m_editTextAction)
//   m_editTextAction->setVisible(part && m_editTextAction->isEnabled());
//  if (m_executeAction)
//   m_executeAction->setVisible(gotitem && it->partInfo()->isExecuteSupported());
//  if (m_exportActionMenu) {
    //m_exportActionMenu->setVisible(gotitem && it->partInfo()->isDataExportSupported());
//  }
//  if (m_printAction)
//   m_printAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
//  if (m_pageSetupAction) {
//   m_pageSetupAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
//  }
// }

    if (m_prevSelectedPart != part) {
        m_prevSelectedPart = part;
        if (part) {
          if (m_newObjectAction) {
            m_newObjectAction->setText(
              i18n("&Create Object: %1...", part->instanceCaption() ));
            m_newObjectAction->setIcon( KIcon(part->info()->createItemIcon()) );
            if (m_features & Toolbar) {
/*              m_newObjectToolButton->setIcon( KIcon(part->info()->createItemIcon()) );
              m_newObjectToolButton->setToolTip(
                i18n("Create object: %1", part->instanceCaption().toLower() ));
              m_newObjectToolButton->setWhatsThis(
                i18n("Creates a new object: %1", part->instanceCaption().toLower() ));*/
            }
          }
        } else {
          if (m_newObjectAction) {
            m_newObjectAction->setText(i18n("&Create Object..."));
      //   m_newObjectToolbarAction->setIcon( KIcon("document-new") );
      //   m_newObjectToolbarAction->setText(m_newObjectAction->text());
            if (m_features & Toolbar) {
/*              m_newObjectToolButton->setIcon( KIcon("document-new") );
              m_newObjectToolButton->setToolTip(i18n("Create object"));
              m_newObjectToolButton->setWhatsThis(i18n("Creates a new object"));*/
            }
          }
        }
    }
    emit selectionChanged(it ? it->partItem() : 0);
}

void KexiProjectNavigator::slotRemove()
{
    if (!m_deleteAction || !m_deleteAction->isEnabled() || !(m_features & Writable))
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit removeItem(it->partItem());
}

void KexiProjectNavigator::slotNewObject()
{
    if (!m_newObjectAction || !(m_features & Writable))
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    if (!it || !it->partInfo())
        return;
    emit newItem(it->partInfo());
}

void KexiProjectNavigator::slotOpenObject()
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DataViewMode);
}

void KexiProjectNavigator::slotDesignObject()
{
    if (!m_designAction)
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DesignViewMode);
}

void KexiProjectNavigator::slotEditTextObject()
{
    if (!m_editTextAction)
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::TextViewMode);
}

void KexiProjectNavigator::slotCut()
{
    if (!(m_features & Writable))
        return;
// KEXI_UNFINISHED_SHARED_ACTION("edit_cut");
    //TODO
}

void KexiProjectNavigator::slotCopy()
{
// KEXI_UNFINISHED_SHARED_ACTION("edit_copy");
    //TODO
}

void KexiProjectNavigator::slotPaste()
{
    if (!(m_features & Writable))
        return;
// KEXI_UNFINISHED_SHARED_ACTION("edit_paste");
    //TODO
}

void KexiProjectNavigator::slotRename()
{
    if (!m_renameAction || !(m_features & Writable))
        return;
    m_list->edit(m_list->currentIndex());
}

void KexiProjectNavigator::setFocus()
{
#if 0
    if (!m_list->currentIndex().isValid() && m_list->firstChild())//select first
        m_list->setCurrentIndex(m_list->firstChild(), true);
    m_list->setFocus();
#endif
}

void KexiProjectNavigator::updateItemName(KexiPart::Item& item, bool dirty)
{
    if (!(m_features & Writable))
        return;

    m_model->updateItemName(item, dirty);
}

void KexiProjectNavigator::slotSettingsChanged(int)
{
    m_singleClick = KGlobalSettings::singleClick();
}

void KexiProjectNavigator::selectItem(KexiPart::Item& item)
{
    KexiProjectModelItem *bitem = m_model->modelItemFromItem(item);
    if (!bitem)
        return;

    QModelIndex idx = m_model->indexFromItem(bitem);
    m_list->setCurrentIndex(idx);
    m_list->scrollTo(idx);
}

void KexiProjectNavigator::clearSelection()
{
    m_list->clearSelection();
    m_list->scrollToTop();
}

void KexiProjectNavigator::slotExecuteObject()
{
    if (!m_executeAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit executeItem(item);
}

void KexiProjectNavigator::slotExportToClipboardAsDataTable()
{
    if (!m_dataExportToClipboardAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToClipboardAsDataTable(item);
}

void KexiProjectNavigator::slotExportToFileAsDataTable()
{
    if (!m_dataExportToFileAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToFileAsDataTable(item);
}

KexiPart::Item* KexiProjectNavigator::selectedPartItem() const
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(m_list->currentIndex().internalPointer());
    return it ? it->partItem() : 0;
}

bool KexiProjectNavigator::actionEnabled(const QString& actionName) const
{
    if (actionName == "project_export_data_table" && (m_features & ContextMenus))
        return m_exportActionMenu->isVisible();
    kWarning() << "no such action: " << actionName;
    return false;
}

void KexiProjectNavigator::slotPrintObject()
{
    if (!m_printAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit printItem(item);
}

void KexiProjectNavigator::slotPageSetupForObject()
{
    if (!m_pageSetupAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit pageSetupForItem(item);
}


void KexiProjectNavigator::setReadOnly(bool set)
{
    m_readOnly = set;
    if (m_deleteAction)
        m_deleteAction->setEnabled(!m_readOnly);
    if (m_renameAction)
        m_renameAction->setEnabled(!m_readOnly);
      if (m_newObjectAction) {
        m_newObjectAction->setEnabled(!m_readOnly);
        if (m_features & Toolbar) {
//removed          m_newObjectMenu->setEnabled(!m_readOnly);
//removed          m_newObjectToolButton->setEnabled(!m_readOnly);
        }
      }
}

bool KexiProjectNavigator::isReadOnly() const
{
    return m_readOnly;
}

void KexiProjectNavigator::clear()
{
m_list->setModel(0);
}

KexiProjectModel* KexiProjectNavigator::model() const
{
return m_model;
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
    QString title_text(partItem->name());
    KexiPart::Part *part = partInfo ? Kexi::partManager().part(partInfo) : 0;
    if (part && !part->instanceCaption().isEmpty()) {
        //+ type name
        title_text += (" : " + part->instanceCaption());
    }
    addTitle(KIcon(partInfo->itemIcon()), title_text);

    if (m_actionCollection->action("open_object")
            && m_actionCollection->action("open_object")->isEnabled()
            && partItem && part && (part->supportedViewModes() & Kexi::DataViewMode)) {
        addAction("open_object");
    }
    if (m_actionCollection->action("design_object")
            && m_actionCollection->action("design_object")->isEnabled()
            && partItem && part && (part->supportedViewModes() & Kexi::DesignViewMode)) {
        addAction("design_object");
    }
    if (m_actionCollection->action("editText_object")
            && m_actionCollection->action("editText_object")->isEnabled()
            && partItem && part && (part->supportedViewModes() & Kexi::TextViewMode)) {
        addAction("editText_object");
    }
//    if (addAction("new_object"))
//        addSeparator();

#ifdef KEXI_SHOW_UNIMPLEMENTED
    //todo plugSharedAction("edit_cut", m_itemMenu);
    //todo plugSharedAction("edit_copy", m_itemMenu);
    //todo addSeparator();
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

//#if 0 //unused
void KexiGroupMenu::update(KexiPart::Info* partInfo)
{
    Q_UNUSED(partInfo);
    clear();
//not needed    addTitle(KIcon(partInfo->itemIcon()), partInfo->groupName());
    addAction("new_object");
#ifdef KEXI_SHOW_UNIMPLEMENTED
// addSeparator();
// qobject_cast<KexiBrowser*>(parent())->plugSharedAction("edit_paste", this);
#endif
}