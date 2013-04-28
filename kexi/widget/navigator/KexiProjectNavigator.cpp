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
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kdialog.h>

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


class KexiProjectNavigator::Private
{
public:

    Private(Features features_)
      : features(features_)
      , prevSelectedPartInfo(0)
      , singleClick(false)
      , readOnly(false)
    {
    }

    ~Private()
    {
        delete model;
    }


    Features features;
    KexiProjectTreeView *list;
    KActionCollection *actions;

    KexiItemMenu *itemMenu;
    KexiGroupMenu *partMenu;
    KAction *deleteAction, *renameAction,
        *newObjectAction,
    // *newObjectToolbarAction,
        *openAction, *designAction, *editTextAction,
        *executeAction,
        *dataExportToClipboardAction, *dataExportToFileAction;
#ifndef KEXI_NO_QUICK_PRINTING
    KAction *printAction, *pageSetupAction;
#endif

    KActionMenu* exportActionMenu;
    QAction *itemMenuTitle, *partMenuTitle,
    *exportActionMenu_sep, *pageSetupAction_sep;

    KexiPart::Info *prevSelectedPartInfo;
    KToolBar *toolbar;
    KexiSmallToolButton /* *newObjectToolButton,*/ *deleteObjectToolButton;

    bool singleClick;
    bool readOnly;
    KexiProjectModel *model;
    QString itemsPartClass;
};

KexiProjectNavigator::KexiProjectNavigator(QWidget* parent, Features features)
        : QWidget(parent)
        , d(new Private(features))
{
    d->actions = new KActionCollection(this);

    setObjectName("KexiProjectNavigator");
    setWindowTitle(i18n("Project Navigator"));
    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(
        KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2);
    lyr->setSpacing(KDialog::marginHint() / 2);

    KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);

    d->list = new KexiProjectTreeView(this);
    d->model = new KexiProjectModel();
    d->list->setModel(d->model);
    KexiProjectItemDelegate *delegate = new KexiProjectItemDelegate(d->list);
    d->list->setItemDelegate(delegate);

    lyr->addWidget(d->list);

    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
    slotSettingsChanged(0);
    

    d->list->header()->hide();
    d->list->setAllColumnsShowFocus(true);
    d->list->setAlternatingRowColors(true);
//    d->list->renameLineEdit()->setValidator(new KexiUtils::IdentifierValidator(this));
    
    connect(d->list, SIGNAL(pressed(QModelIndex)), this,SLOT(slotSelectionChanged(QModelIndex)));

    KConfigGroup mainWindowGroup = KGlobal::config()->group("MainWindow");
    if ((d->features & SingleClickOpensItemOptionEnabled) && mainWindowGroup.readEntry("SingleClickOpensItem", false)) {
        connect(d->list, SIGNAL(activate(QModelIndex)), this, SLOT(slotExecuteItem(QModelIndex)));
    } else {
        connect(d->list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotExecuteItem(QModelIndex)));
//        d->list->enableExecuteArea = false;
    }

    // actions
    d->openAction = addAction("open_object", koIcon("document-open"), i18n("&Open"),
                             i18n("Open object"), i18n("Opens object selected in the list."),
                             SLOT(slotOpenObject()));

// d->openAction->plug(d->toolbar);
    KexiSmallToolButton *btn;
    if (d->features & Toolbar) {
        btn = new KexiSmallToolButton(d->openAction, this);
        buttons_flyr->addWidget(btn);
    }

    if (KexiMainWindowIface::global() && KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
        d->deleteAction = 0;
        d->renameAction = 0;
        d->designAction = 0;
        d->editTextAction = 0;
        d->newObjectAction = 0;
    } else {
        d->deleteAction = addAction("edit_delete", koIcon("edit-delete"), i18n("&Delete..."),
                                   i18n("Delete object"),
                                   i18n("Deletes the object selected in the list."),
                                   SLOT(slotRemove()));

        d->renameAction = addAction("edit_rename", koIcon("edit-rename"), i18n("&Rename..."),
                                   i18n("Rename object"),
                                   i18n("Renames the object selected in the list."),
                                   SLOT(slotRename()));
//! @todo enable, doesn't work now: d->renameAction->setShortcut(KShortcut(Qt::Key_F2));
#ifdef KEXI_SHOW_UNIMPLEMENTED
        //todo plugSharedAction("edit_cut",SLOT(slotCut()));
        //todo plugSharedAction("edit_copy",SLOT(slotCopy()));
        //todo plugSharedAction("edit_paste",SLOT(slotPaste()));
#endif

        d->designAction = addAction("design_object", koIcon("document-properties"), i18n("&Design"),
                                   i18n("Design object"),
                                   i18n("Starts designing of the object selected in the list."),
                                   SLOT(slotDesignObject()));
        if (d->features & Toolbar) {
            btn = new KexiSmallToolButton(d->designAction, this);
//   d->designAction->setEnabled(false);
            buttons_flyr->addWidget(btn);
        }

        d->editTextAction = addAction("editText_object", KIcon(), i18n("Design in &Text View"),
                                     i18n("Design object in text view"),
                                     i18n("Starts designing of the object in the list in text view."),
                                     SLOT(slotEditTextObject()));

        d->newObjectAction = addAction("new_object", koIcon("document-new"), QString(),QString(), QString(), SLOT(slotNewObject()));

        if (d->features & Toolbar) {
            d->deleteObjectToolButton = new KexiSmallToolButton(d->deleteAction, this);
            buttons_flyr->addWidget(d->deleteObjectToolButton);
        }
    }

    d->executeAction = addAction("data_execute", koIcon("system-run"), i18n("Execute"),
//! @todo tooltip, what's this
                                QString(), QString(),
                                SLOT(slotExecuteObject()));

    d->actions->addAction("export_object",
                         d->exportActionMenu = new KActionMenu(i18n("Export"), this));
    d->dataExportToClipboardAction = addAction("exportToClipboardAsDataTable", koIcon("edit-copy"),
                                   i18nc("Export->To Clipboard as Data... ", "To &Clipboard..."),
                                   i18n("Export data to clipboard"),
                                   i18n("Exports data from the currently selected table or query to clipboard."),
                                   SLOT(slotExportToClipboardAsDataTable()));
    d->exportActionMenu->addAction(d->dataExportToClipboardAction);

    d->dataExportToFileAction = addAction("exportToFileAsDataTable", koIcon("table"),
                                   i18nc("Export->To File As Data &Table... ", "To &File As Data Table..."),
                                   i18n("Export data to a file"),
                                   i18n("Exports data from the currently selected table or query to a file."),
                                   SLOT(slotExportToFileAsDataTable()));
    d->exportActionMenu->addAction(d->dataExportToFileAction);

#ifndef KEXI_NO_QUICK_PRINTING
    d->printAction = addAction("print_object", koIcon("document-print"), i18n("&Print..."),
                              i18n("Print data"),
                              i18n("Prints data from the currently selected table or query."),
                              SLOT(slotPrintObject()));

    d->pageSetupAction = addAction("pageSetupForObject", koIconWanted("not yet in Oxygen 4.3", "document-page-setup"),
                                  i18n("Page Setup..."),
                                  i18n("Page setup for data"),
                                  i18n("Shows page setup for printing the active table or query."),
                                  SLOT(slotPageSetupForObject()));
#endif

    if (KexiMainWindowIface::global() && KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
        d->partMenu = 0;
    } else {
        d->partMenu = new KexiGroupMenu(this, d->actions);
    }

    if (d->features & ContextMenus) {
        d->itemMenu = new KexiItemMenu(this, d->actions);
    } else {
        d->itemMenu = 0;
    }
    if (!(d->features & Writable)) {
        setReadOnly(true);
    }
    slotSelectionChanged(QModelIndex());
}

void KexiProjectNavigator::setProject(KexiProject* prj, const QString& itemsPartClass, QString* partManagerErrorMessages, bool addAsSearchableModel)
{
    d->itemsPartClass = itemsPartClass;

    d->model->setProject(prj, itemsPartClass, partManagerErrorMessages);
    
    if (addAsSearchableModel) {
      KexiMainWindowIface::global()->addSearchableModel(d->model);
    }
    
    d->list->expandAll();
    if (itemsPartClass.isEmpty()) {
        d->list->setRootIsDecorated(true);
    } else {
        d->list->setRootIsDecorated(false);
    }
}

QString KexiProjectNavigator::itemsPartClass() const
{
  return d->itemsPartClass;
}

KexiProjectNavigator::~KexiProjectNavigator()
{
    delete d;
}

KAction* KexiProjectNavigator::addAction(const QString& name, const KIcon& icon, const QString& text,
                                const QString& toolTip, const QString& whatsThis, const char* slot)
{
    KAction *action = new KAction(icon, text, this);
    d->actions->addAction(name, action);
    action->setToolTip(toolTip);
    action->setWhatsThis(whatsThis);
    connect(action, SIGNAL(triggered()), this, slot);
    return action;
}

void KexiProjectNavigator::contextMenuEvent(QContextMenuEvent* event)
{
    if (!d->list->currentIndex().isValid() || !(d->features & ContextMenus))
        return;
    
    KexiProjectModelItem *bit = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    KMenu *pm = 0;
    if (bit->partItem()) {
        pm = d->itemMenu;
        KexiProjectModelItem *par_it = static_cast<KexiProjectModelItem*>(bit->parent());
        d->itemMenu->update(par_it->partInfo(), bit->partItem());
    } else if (d->partMenu) {
        pm = d->partMenu;
        d->partMenu->update(bit->partInfo());
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
//    if (!it->partItem() && !d->singleClick /*annoying when in single click mode*/) {
//        vitem.
//        d->list->setOpen(vitem, !vitem->isOpen());
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
            d->openAction->setEnabled(false);
            d->designAction->setEnabled(false);
            d->deleteAction->setEnabled(false);
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
    if (d->deleteAction) {
        d->deleteAction->setEnabled(gotitem && !d->readOnly);
        if (d->features & Toolbar) {
            d->deleteObjectToolButton->setEnabled(gotitem && !d->readOnly);
        }
    }
#ifdef KEXI_SHOW_UNIMPLEMENTED
//todo setAvailable("edit_cut",gotitem);
//todo setAvailable("edit_copy",gotitem);
//todo setAvailable("edit_edititem",gotitem);
#endif

    if ( KexiMainWindowIface::global() && !KexiMainWindowIface::global()->userMode() ) {
        d->openAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::DataViewMode));
        if (d->designAction) {
    //  d->designAction->setVisible(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
            d->designAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::DesignViewMode));
        }
        if (d->editTextAction)
            d->editTextAction->setEnabled(gotitem && (it->partInfo()->supportedViewModes() & Kexi::TextViewMode));

    // if (d->features & ContextMenus) {
    //  d->openAction->setVisible(d->openAction->isEnabled());
    //  if (d->designAction)
        //  d->designAction->setVisible(d->designAction->isEnabled());
    //  if (d->editTextAction)
    //   d->editTextAction->setVisible(part && d->editTextAction->isEnabled());
    //  if (d->executeAction)
    //   d->executeAction->setVisible(gotitem && it->partInfo()->isExecuteSupported());
    //  if (d->exportActionMenu) {
        //d->exportActionMenu->setVisible(gotitem && it->partInfo()->isDataExportSupported());
    //  }
    //  if (d->printAction)
    //   d->printAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
    //  if (d->pageSetupAction) {
    //   d->pageSetupAction->setVisible(gotitem && it->partInfo()->isPrintingSupported());
    //  }
    // }

        if (d->prevSelectedPartInfo != it->partInfo()) {
            d->prevSelectedPartInfo = it->partInfo();
            //if (part) {
            if (d->newObjectAction) {
                d->newObjectAction->setText(
                i18n("&Create Object: %1...", it->partInfo()->instanceCaption() ));
                d->newObjectAction->setIcon(KIcon(it->partInfo()->createItemIconName()));
                if (d->features & Toolbar) {
    /*              d->newObjectToolButton->setIcon(KIcon(part->info()->createItemIconName()));
                d->newObjectToolButton->setToolTip(
                    i18n("Create object: %1", part->info()->instanceCaption().toLower() ));
                d->newObjectToolButton->setWhatsThis(
                    i18n("Creates a new object: %1", part->info()->instanceCaption().toLower() ));*/
                }
            }
        #if 0 
             } else {
            if (d->newObjectAction) {
                d->newObjectAction->setText(i18n("&Create Object..."));
        //   d->newObjectToolbarAction->setIcon( koIcon("document-new") );
        //   d->newObjectToolbarAction->setText(d->newObjectAction->text());
                if (d->features & Toolbar) {
    /*              d->newObjectToolButton->setIcon( koIcon("document-new") );
                d->newObjectToolButton->setToolTip(i18n("Create object"));
                d->newObjectToolButton->setWhatsThis(i18n("Creates a new object"));*/
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
    if (!d->deleteAction || !d->deleteAction->isEnabled() || !(d->features & Writable))
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit removeItem(it->partItem());
}

void KexiProjectNavigator::slotNewObject()
{
    if (!d->newObjectAction || !(d->features & Writable))
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    if (!it || !it->partInfo())
        return;
    emit newItem(it->partInfo());
}

void KexiProjectNavigator::slotOpenObject()
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DataViewMode);
}

void KexiProjectNavigator::slotDesignObject()
{
    if (!d->designAction)
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DesignViewMode);
}

void KexiProjectNavigator::slotEditTextObject()
{
    if (!d->editTextAction)
        return;
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::TextViewMode);
}

void KexiProjectNavigator::slotCut()
{
    if (!(d->features & Writable))
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
    if (!(d->features & Writable))
        return;
// KEXI_UNFINISHED_SHARED_ACTION("edit_paste");
    //TODO
}

void KexiProjectNavigator::slotRename()
{
    if (!d->renameAction || !(d->features & Writable))
        return;

    KexiPart::Item* partItem = selectedPartItem();
    if (!partItem) {
        return;
    }
    KexiProjectModelItem *partModelItem = d->model->modelItemFromItem(*partItem);
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
    if (!d->model->project()) {
        kWarning() << "No KexiProject assigned!";
        return;
    }
    dialog.widget()->addNameSubvalidator( //check if new name is allowed
        new KexiDB::ObjectNameValidator(d->model->project()->dbConnection()->driver()));
    dialog.widget()->setCaptionText(partItem->caption());
    dialog.widget()->setNameText(partItem->name());
    dialog.setWindowTitle(
        i18nc("@title:window Rename Object %1.", "Rename <resource>%1</resource>").arg(partItem->name()));
    dialog.setDialogIcon(info->itemIconName());
    dialog.setAllowOverwriting(true);

    bool overwriteNeeded;
    if (dialog.execAndCheckIfObjectExists(*d->model->project(), *part, &overwriteNeeded)
        != QDialog::Accepted)
    {
        return;
    }
    if (dialog.widget()->nameText() != dialog.widget()->originalNameText()
        && !d->model->renameItem(partItem, dialog.widget()->nameText()))
    {
        return;
    }
    d->model->setItemCaption(partItem, dialog.widget()->captionText());
}

void KexiProjectNavigator::setFocus()
{
    if (!d->list->currentIndex().isValid()) { // select first
        QModelIndex first = d->model->firstPartItem();
        d->list->setCurrentIndex(first);
    }
    d->list->setFocus();
}

void KexiProjectNavigator::updateItemName(KexiPart::Item& item, bool dirty)
{
    if (!(d->features & Writable))
        return;

    d->model->updateItemName(item, dirty);
}

void KexiProjectNavigator::slotSettingsChanged(int)
{
    d->singleClick = KGlobalSettings::singleClick();
}

void KexiProjectNavigator::selectItem(KexiPart::Item& item)
{
    KexiProjectModelItem *bitem = d->model->modelItemFromItem(item);
    if (!bitem)
        return;

    QModelIndex idx = d->model->indexFromItem(bitem);
    d->list->setCurrentIndex(idx);
    d->list->scrollTo(idx);
}

void KexiProjectNavigator::clearSelection()
{
    d->list->clearSelection();
    d->list->scrollToTop();
}

void KexiProjectNavigator::slotExecuteObject()
{
    if (!d->executeAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit executeItem(item);
}

void KexiProjectNavigator::slotExportToClipboardAsDataTable()
{
    if (!d->dataExportToClipboardAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToClipboardAsDataTable(item);
}

void KexiProjectNavigator::slotExportToFileAsDataTable()
{
    if (!d->dataExportToFileAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToFileAsDataTable(item);
}

KexiPart::Item* KexiProjectNavigator::selectedPartItem() const
{
    KexiProjectModelItem *it = static_cast<KexiProjectModelItem*>(d->list->currentIndex().internalPointer());
    return it ? it->partItem() : 0;
}

bool KexiProjectNavigator::actionEnabled(const QString& actionName) const
{
    if (actionName == "project_export_data_table" && (d->features & ContextMenus))
        return d->exportActionMenu->isVisible();
    kWarning() << "no such action: " << actionName;
    return false;
}

void KexiProjectNavigator::slotPrintObject()
{
#ifndef KEXI_NO_QUICK_PRINTING
    if (!d->printAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit printItem(item);
#endif
}

void KexiProjectNavigator::slotPageSetupForObject()
{
#ifndef KEXI_NO_QUICK_PRINTING
    if (!d->pageSetupAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit pageSetupForItem(item);
#endif
}


void KexiProjectNavigator::setReadOnly(bool set)
{
    d->readOnly = set;
    if (d->deleteAction)
        d->deleteAction->setEnabled(!d->readOnly);
    if (d->renameAction)
        d->renameAction->setEnabled(!d->readOnly);
      if (d->newObjectAction) {
        d->newObjectAction->setEnabled(!d->readOnly);
        if (d->features & Toolbar) {
//removed          d->newObjectMenu->setEnabled(!d->readOnly);
//removed          d->newObjectToolButton->setEnabled(!d->readOnly);
        }
      }
}

bool KexiProjectNavigator::isReadOnly() const
{
    return d->readOnly;
}

void KexiProjectNavigator::clear()
{
    d->model->clear();
}

KexiProjectModel* KexiProjectNavigator::model() const
{
    return d->model;
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
