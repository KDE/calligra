/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2012 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

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
#include "KexiProjectItemDelegate.h"
#include <widget/KexiNameDialog.h>
#include <widget/KexiNameWidget.h>

#include <QHeaderView>
#include <QPoint>
#include <QPixmapCache>
#include <QToolButton>
#include <QKeyEvent>
#include <QEvent>

#include <kglobalsettings.h>
#include <kapplication.h>
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

#include <KoIcon.h>
#include <kexi.h>
#include <kexipart.h>
#include <kexipartinfo.h>
#include <kexipartitem.h>
#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <kexiutils/identifier.h>
#include <kexiutils/FlowLayout.h>
#include <kexiutils/SmallToolButton.h>
#include <db/utils.h>
#include <kexidb/dbobjectnamevalidator.h>
#include <kexi_global.h>


KexiProjectNavigator::KexiProjectNavigator(QWidget* parent, Features features)
        : QWidget(parent)
        , m_features(features)
        , m_actions(new KActionCollection(this))
        , m_prevSelectedPartInfo(0)
        , m_singleClick(false)
        , m_readOnly(false)
{
    setObjectName("KexiProjectNavigator");
    setWindowTitle(i18n("Project Navigator"));
    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(
        KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2);
    lyr->setSpacing(KDialog::marginHint() / 2);

    KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);

    m_list = new KexiProjectTreeView(this);
    m_model = new KexiProjectModel();
    m_list->setModel(m_model);
    KexiProjectItemDelegate *delegate = new KexiProjectItemDelegate(m_list);
    m_list->setItemDelegate(delegate);

    lyr->addWidget(m_list);

    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
    slotSettingsChanged(0);
    

    m_list->header()->hide();
    m_list->setAllColumnsShowFocus(true);
    m_list->setAlternatingRowColors(true);
//    m_list->renameLineEdit()->setValidator(new KexiUtils::IdentifierValidator(this));
    
    connect(m_list, SIGNAL(pressed(const QModelIndex&)), this,SLOT(slotSelectionChanged(const QModelIndex&)));

    KConfigGroup mainWindowGroup = KGlobal::config()->group("MainWindow");
    if ((m_features & SingleClickOpensItemOptionEnabled) && mainWindowGroup.readEntry("SingleClickOpensItem", false)) {
        connect(m_list, SIGNAL(activate(const QModelIndex&)), this, SLOT(slotExecuteItem(const QModelIndex&)));
    } else {
        connect(m_list, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(slotExecuteItem(const QModelIndex&)));
//        m_list->enableExecuteArea = false;
    }

    // actions
    m_openAction = addAction("open_object", koIcon("document-open"), i18n("&Open"),
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
        m_deleteAction = addAction("edit_delete", koIcon("edit-delete"), i18n("&Delete..."),
                                   i18n("Delete object"),
                                   i18n("Deletes the object selected in the list."),
                                   SLOT(slotRemove()));

        m_renameAction = addAction("edit_rename", koIcon("edit-rename"), i18n("&Rename..."),
                                   i18n("Rename object"),
                                   i18n("Renames the object selected in the list."),
                                   SLOT(slotRename()));
//! @todo enable, doesn't work now: m_renameAction->setShortcut(KShortcut(Qt::Key_F2));
#ifdef KEXI_SHOW_UNIMPLEMENTED
        //todo plugSharedAction("edit_cut",SLOT(slotCut()));
        //todo plugSharedAction("edit_copy",SLOT(slotCopy()));
        //todo plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

        m_designAction = addAction("design_object", koIcon("document-properties"), i18n("&Design"),
                                   i18n("Design object"),
                                   i18n("Starts designing of the object selected in the list."),
                                   SLOT(slotDesignObject()));
        if (m_features & Toolbar) {
            btn = new KexiSmallToolButton(m_designAction, this);
//   m_designAction->setEnabled(false);
            buttons_flyr->addWidget(btn);
        }

        m_editTextAction = addAction("editText_object", KIcon(), i18n("Design in &Text View"),
                                     i18n("Design object in text view"),
                                     i18n("Starts designing of the object in the list in text view."),
                                     SLOT(slotEditTextObject()));

        m_newObjectAction = addAction("new_object", koIcon("document-new"), QString(),QString(), QString(), SLOT(slotNewObject()));

        if (m_features & Toolbar) {
            m_deleteObjectToolButton = new KexiSmallToolButton(m_deleteAction, this);
            buttons_flyr->addWidget(m_deleteObjectToolButton);
        }
    }

    m_executeAction = addAction("data_execute", koIcon("system-run"), i18n("Execute"),
//! @todo tooltip, what's this
                                QString(), QString(),
                                SLOT(slotExecuteObject()));

    m_actions->addAction("export_object",
                         m_exportActionMenu = new KActionMenu(i18n("Export"), this));
    m_dataExportToClipboardAction = addAction("exportToClipboardAsDataTable", koIcon("edit-copy"),
                                   i18nc("Export->To Clipboard as Data... ", "To &Clipboard..."),
                                   i18n("Export data to clipboard"),
                                   i18n("Exports data from the currently selected table or query to clipboard."),
                                   SLOT(slotExportToClipboardAsDataTable()));
    m_exportActionMenu->addAction(m_dataExportToClipboardAction);

    m_dataExportToFileAction = addAction("exportToFileAsDataTable", koIcon("table"),
                                   i18nc("Export->To File As Data &Table... ", "To &File As Data Table..."),
                                   i18n("Export data to a file"),
                                   i18n("Exports data from the currently selected table or query to a file."),
                                   SLOT(slotExportToFileAsDataTable()));
    m_exportActionMenu->addAction(m_dataExportToFileAction);

#ifndef KEXI_NO_QUICK_PRINTING
    m_printAction = addAction("print_object", koIcon("document-print"), i18n("&Print..."),
                              i18n("Print data"),
                              i18n("Prints data from the currently selected table or query."),
                              SLOT(slotPrintObject()));

    m_pageSetupAction = addAction("pageSetupForObject", koIconWanted("not yet in Oxygen 4.3", "document-page-setup"),
                                  i18n("Page Setup..."),
                                  i18n("Page setup for data"),
                                  i18n("Shows page setup for printing the active table or query."),
                                  SLOT(slotPageSetupForObject()));
#endif

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

void KexiProjectNavigator::setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages, bool addAsSearchableModel)
{
    m_itemsPartClass = itemsPartClass;

    m_model->setProject(prj, itemsPartClass, partManagerErrorMessages);
    
    if (addAsSearchableModel) {
      KexiMainWindowIface::global()->addSearchableModel(m_model);
    }
    
    m_list->expandAll();
    if (itemsPartClass.isEmpty()) {
        m_list->setRootIsDecorated(true);
    } else {
        m_list->setRootIsDecorated(false);
    }
}

QString KexiProjectNavigator::itemsPartClass() const
{
  return m_itemsPartClass;
}

KexiProjectNavigator::~KexiProjectNavigator()
{
    delete m_model;
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
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(vitem.internalPointer());
    if (!it) {
        kWarning() << "No internal pointer";
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
        if (KexiMainWindowIface::global() && !KexiMainWindowIface::global()->userMode()) {
            m_openAction->setEnabled(false);
            m_designAction->setEnabled(false);
            m_deleteAction->setEnabled(false);
        }
        return;
    }
    /* no need to load part so early:
    KexiPart::Part* part = Kexi::partManager().part(it->partInfo());
    if (!part) {
        it = static_cast<KexiProjectModelItem*>(it->parent());
        if (it) {
            part = Kexi::partManager().part(it->partInfo());
        }
    }*/

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

    if ( KexiMainWindowIface::global() && !KexiMainWindowIface::global()->userMode() ) {
        m_openAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::DataViewMode));
        if (m_designAction) {
    //  m_designAction->setVisible(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
            m_designAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::DesignViewMode));
        }
        if (m_editTextAction)
            m_editTextAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::TextViewMode));

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

        if (m_prevSelectedPartInfo != it->partInfo()) {
            m_prevSelectedPartInfo = it->partInfo();
            //if (part) {
            if (m_newObjectAction) {
                m_newObjectAction->setText(
                i18n("&Create Object: %1...", it->partInfo()->instanceCaption() ));
                m_newObjectAction->setIcon(KIcon(it->partInfo()->createItemIconName()));
                if (m_features & Toolbar) {
    /*              m_newObjectToolButton->setIcon(KIcon(part->info()->createItemIconName()));
                m_newObjectToolButton->setToolTip(
                    i18n("Create object: %1", part->info()->instanceCaption().toLower() ));
                m_newObjectToolButton->setWhatsThis(
                    i18n("Creates a new object: %1", part->info()->instanceCaption().toLower() ));*/
                }
            }
        #if 0 
             } else {
            if (m_newObjectAction) {
                m_newObjectAction->setText(i18n("&Create Object..."));
        //   m_newObjectToolbarAction->setIcon( koIcon("document-new") );
        //   m_newObjectToolbarAction->setText(m_newObjectAction->text());
                if (m_features & Toolbar) {
    /*              m_newObjectToolButton->setIcon( koIcon("document-new") );
                m_newObjectToolButton->setToolTip(i18n("Create object"));
                m_newObjectToolButton->setWhatsThis(i18n("Creates a new object"));*/
                }
            }
            }
        #endif
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

    KexiPart::Item* partItem = selectedPartItem();
    if (!partItem) {
        return;
    }
    KexiProjectModelItem *partModelItem = m_model->modelItemFromItem(*partItem);
    if (!partModelItem) {
        return;
    }
    KexiPart::Info *info = partModelItem->partInfo();
    KexiPart::Part *part = Kexi::partManager().partForClass(partItem->partClass());
    if (!info || !part) {
        return;
    }
    KexiNameDialog dialog(
        i18nc("@info Rename object %1:", "Rename <resource>%1</resource>:").arg(partItem->name()),
        this);
    if (!m_model->project()) {
        kWarning() << "No KexiProject assigned!";
        return;
    }
    dialog.widget()->addNameSubvalidator( //check if new name is allowed
        new KexiDB::ObjectNameValidator(m_model->project()->dbConnection()->driver()));
    dialog.widget()->setCaptionText(partItem->caption());
    dialog.widget()->setNameText(partItem->name());
    dialog.setWindowTitle(
        i18nc("@title:window Rename Object %1.", "Rename <resource>%1</resource>").arg(partItem->name()));
    dialog.setDialogIcon(info->itemIconName());
    dialog.setAllowOverwriting(true);

    bool overwriteNeeded;
    if (dialog.execAndCheckIfObjectExists(*m_model->project(), *part, &overwriteNeeded)
        != QDialog::Accepted)
    {
        return;
    }
    if (dialog.widget()->nameText() != dialog.widget()->originalNameText()
        && !m_model->renameItem(partItem, dialog.widget()->nameText()))
    {
        return;
    }
    m_model->setItemCaption(partItem, dialog.widget()->captionText());
}

void KexiProjectNavigator::setFocus()
{
    if (!m_list->currentIndex().isValid()) { // select first
        QModelIndex first = m_model->firstPartItem();
        m_list->setCurrentIndex(first);
    }
    m_list->setFocus();
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
#ifndef KEXI_NO_QUICK_PRINTING
    if (!m_printAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit printItem(item);
#endif
}

void KexiProjectNavigator::slotPageSetupForObject()
{
#ifndef KEXI_NO_QUICK_PRINTING
    if (!m_pageSetupAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit pageSetupForItem(item);
#endif
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
    m_model->clear();
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
    //KexiPart::Part *part = partInfo ? Kexi::partManager().part(partInfo) : 0;
    if (partInfo && !partInfo->instanceCaption().isEmpty()) {
        //+ type name
        title_text += (" : " + partInfo->instanceCaption());
    }
    addTitle(KIcon(partInfo->itemIconName()), title_text);

    if (m_actionCollection->action("open_object")
            && m_actionCollection->action("open_object")->isEnabled()
            && partItem && (partInfo->supportedViewModes() & Kexi::DataViewMode)) {
        addAction("open_object");
    }
    if (m_actionCollection->action("design_object")
            && m_actionCollection->action("design_object")->isEnabled()
            && partItem && (partInfo->supportedViewModes() & Kexi::DesignViewMode)) {
        addAction("design_object");
    }
    if (m_actionCollection->action("editText_object")
            && m_actionCollection->action("editText_object")->isEnabled()
            && partItem && (partInfo->supportedViewModes() & Kexi::TextViewMode)) {
        addAction("editText_object");
    }
    addSeparator();
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

#ifndef KEXI_NO_QUICK_PRINTING
    if (partItem && partInfo->isPrintingSupported())
        addAction("print_object");
    if (partItem && partInfo->isPrintingSupported())
        addAction("pageSetupForObject");
    if (m_actionCollection->action("edit_rename") || m_actionCollection->action("edit_delete"))
        addSeparator();
#endif
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
//not needed    addTitle(KIcon(partInfo->itemIconName()), partInfo->groupName());
    addAction("new_object");
#ifdef KEXI_SHOW_UNIMPLEMENTED
// addSeparator();
// qobject_cast<KexiBrowser*>(parent())->plugSharedAction("edit_paste", this);
#endif
}
