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

#include "KexiProjectListView.h"
#include "KexiProjectListView_p.h"
#include "KexiProjectListViewItem.h"

#include <q3header.h>
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
#include <k3listview.h>
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
#include "KexiProjectNavigator.h"

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

KexiProjectListView::KexiProjectListView(QWidget* parent, Features features)
        : QWidget(parent)
        , m_features(features)
        , m_actions(new KActionCollection(this))
        , m_prevSelectedPart(0)
        , m_singleClick(false)
// , m_nameEndsWithAsterisk(false)
        , m_readOnly(false)
// , m_enableExecuteArea(true)
{
    setObjectName("KexiBrowser");
    setWindowTitle(i18n("Project Navigator"));
    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    QVBoxLayout *lyr = new QVBoxLayout(this);
    lyr->setContentsMargins(
        KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2, KDialog::marginHint() / 2);
    lyr->setSpacing(KDialog::marginHint() / 2);

    KexiFlowLayout *buttons_flyr = new KexiFlowLayout(lyr);

    m_list = new KexiBrowserListView(this);
    lyr->addWidget(m_list);
    m_list->installEventFilter(this);
    m_list->renameLineEdit()->installEventFilter(this);
    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
    slotSettingsChanged(0);

    m_list->header()->hide();
    m_list->addColumn(QString());
    m_list->setShowToolTips(true);
    m_list->setSorting(0);
    m_list->sort();
    m_list->setAllColumnsShowFocus(true);
    m_list->setTooltipColumn(0);
    m_list->renameLineEdit()->setValidator(new KexiUtils::IdentifierValidator(this));
    m_list->setResizeMode(Q3ListView::LastColumn);
    connect(m_list, SIGNAL(selectionChanged(Q3ListViewItem*)), this,
            SLOT(slotSelectionChanged(Q3ListViewItem*)));
    connect(m_list, SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
            this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem *, const QPoint&)));

    KConfigGroup mainWindowGroup = KGlobal::config()->group("MainWindow");
    if ((m_features & SingleClickOpensItemOptionEnabled)
            && mainWindowGroup.readEntry("SingleClickOpensItem", false)) {
        connect(m_list, SIGNAL(executed(Q3ListViewItem*)), this,
                SLOT(slotExecuteItem(Q3ListViewItem*)));
    } else {
        connect(m_list, SIGNAL(doubleClicked(Q3ListViewItem*)), this,
                SLOT(slotExecuteItem(Q3ListViewItem*)));
        m_list->enableExecuteArea = false;
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

    if (KexiMainWindowIface::global()->userMode()) {
//! @todo some of these actions can be supported once we deliver ACLs...
        m_deleteAction = 0;
        m_renameAction = 0;
        m_designAction = 0;
        m_editTextAction = 0;
        m_newObjectAction = 0;
//(new action removed)  m_newObjectMenu = 0;
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

        m_newObjectAction = addAction("new_object", KIcon("document-new"), QString(),
              QString(), QString(),
              SLOT(slotNewObject()));
        if (m_features & Toolbar) {
            /*(new action removed)   m_newObjectToolButton = new KexiSmallToolButton(this);
                  m_newObjectMenu = new KMenu(this);
                  m_newObjectMenu->setObjectName("newObjectMenu");
                  connect(m_newObjectMenu, SIGNAL(aboutToShow()),
                    this, SLOT(slotNewObjectMenuAboutToShow()));
                  m_newObjectToolButton->setMenu(m_newObjectMenu);
            //   m_newObjectToolButton->setPopupDelay(QApplication::startDragTime());
                  connect(m_newObjectToolButton, SIGNAL(clicked()), this, SLOT(slotNewObject()));
                  buttons_flyr->add(m_newObjectToolButton);*/

            m_deleteObjectToolButton = new KexiSmallToolButton(m_deleteAction, this);
            //m_deleteObjectToolButton->setText(QString());
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

#ifndef KEXI_NO_QUICK_PRINTING
    m_printAction = addAction("print_object", KIcon("document-print"), i18n("&Print..."),
                              i18n("Print data"),
                              i18n("Prints data from the currently selected table or query."),
                              SLOT(slotPrintObject()));

    m_pageSetupAction = addAction("pageSetupForObject", KIcon(/*"document-page-setup" not yet in Oxygen */), i18n("Page Setup..."),
                                  i18n("Page setup for data"),
                                  i18n("Shows page setup for printing the active table or query."),
                                  SLOT(slotPageSetupForObject()));
#endif

    if (KexiMainWindowIface::global()->userMode()) {
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
}

KexiProjectListView::~KexiProjectListView()
{
}

void KexiProjectListView::setProject(KexiProject* prj, const QString& itemsPartClass,
                             QString* partManagerErrorMessages)
{
    clear();
    m_itemsPartClass = itemsPartClass;
    m_list->setRootIsDecorated(m_itemsPartClass.isEmpty());

    KexiPart::PartInfoList* plist = Kexi::partManager().infoList();
    if (!plist)
        return;
    foreach(KexiPart::Info *info, *plist) {
        if (!info->isVisibleInNavigator())
            continue;
        if (!m_itemsPartClass.isEmpty() && info->partClass() != m_itemsPartClass)
            continue;

//  kDebug() << "KexiMainWindowImpl::initNavigator(): adding " << it->groupName();

        /*   KexiPart::Part *p=Kexi::partManager().part(it);
            if (!p) {
              //TODO: js - OPTIONALLY: show error
              continue;
            }
            p->createGUIClient(this);*/

        //load part - we need this to have GUI merged with part's actions
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
        kDebug() << info->partClass() << info->objectName();
        KexiPart::Part *p = Kexi::partManager().part(info);
        if (p) {
            KexiProjectListViewItem *groupItem = 0;
            if (m_itemsPartClass.isEmpty()) {
                groupItem = addGroup(*info);
                if (!groupItem)
                    continue;
            }
            //lookup project's objects (part items)
//! @todo FUTURE - don't do that when DESIGN MODE is OFF
            KexiPart::ItemDict *item_dict = prj->items(info);
            if (!item_dict)
                continue;
            foreach(KexiPart::Item *item, *item_dict)
                addItem(*item, groupItem, info);
            if (!m_itemsPartClass.isEmpty())
                break; //the only group added, so our work is completed
        } else {
            //add this error to the list that will be displayed later
            QString msg, details;
            KexiDB::getHTMLErrorMesage(&Kexi::partManager(), msg, details);
            if (!msg.isEmpty() && partManagerErrorMessages) {
                if (partManagerErrorMessages->isEmpty()) {
                    *partManagerErrorMessages = QString("<qt><p>")
                                                + i18n("Errors encountered during loading plugins:") + "<ul>";
                }
                partManagerErrorMessages->append(QString("<li>") + msg);
                if (!details.isEmpty())
                    partManagerErrorMessages->append(QString("<br>") + details);
                partManagerErrorMessages->append("</li>");
            }
        }
    }
    if (partManagerErrorMessages && !partManagerErrorMessages->isEmpty())
        partManagerErrorMessages->append("</ul></p>");
}

KAction* KexiProjectListView::addAction(const QString& name, const KIcon& icon, const QString& text,
                                const QString& toolTip, const QString& whatsThis, const char* slot)
{
    KAction *action = new KAction(icon, text, this);
    m_actions->addAction(name, action);
    action->setToolTip(toolTip);
    action->setWhatsThis(whatsThis);
    connect(action, SIGNAL(triggered()), this, slot);
    return action;
}

QString KexiProjectListView::itemsPartClass() const
{
    return m_itemsPartClass;
}

KexiProjectListViewItem *KexiProjectListView::addGroup(KexiPart::Info& info)
{
    if (!info.isVisibleInNavigator())
        return 0;

    KexiProjectListViewItem *item = new KexiProjectListViewItem(m_list, &info);
    m_baseItems.insert(info.partClass().toLower(), item);
    return item;
// kDebug() << "KexiBrowser::addGroup()";
}

KexiProjectListViewItem* KexiProjectListView::addItem(KexiPart::Item& item)
{
    //part object for this item
    KexiProjectListViewItem *parent = item.partClass().isEmpty()
                              ? 0 : m_baseItems.value(item.partClass().toLower());
    return addItem(item, parent, parent->partInfo());
}

KexiProjectListViewItem* KexiProjectListView::addItem(KexiPart::Item& item, KexiProjectListViewItem *parent, KexiPart::Info* info)
{
// if (!parent) //TODO: add "Other" part group for that
    // return 0;
// kDebug() << "KexiBrowser::addItem() found parent:" << parent;
    KexiProjectListViewItem *bitem;
    if (parent)
        bitem = new KexiProjectListViewItem(parent, info, &item);
    else
        bitem = new KexiProjectListViewItem(m_list, info, &item);
    m_normalItems.insert(item.identifier(), bitem);
    return bitem;
}

void
KexiProjectListView::slotRemoveItem(const KexiPart::Item &item)
{
    KexiProjectListViewItem *to_remove = m_normalItems.take(item.identifier());
    if (!to_remove)
        return;

    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());

    Q3ListViewItem *new_item_to_select = 0;
    if (it == to_remove) {//compute item to select if current one will be removed
        new_item_to_select = it->itemBelow();//nearest item to select
        if (!new_item_to_select || new_item_to_select->parent() != it->parent()) {
            new_item_to_select = it->itemAbove();
        }
    }
    delete to_remove;

    if (new_item_to_select)
        m_list->setSelected(new_item_to_select, true);
}

void
KexiProjectListView::slotContextMenu(K3ListView* /*list*/, Q3ListViewItem *item, const QPoint &pos)
{
    if (!item || !(m_features & ContextMenus))
        return;
    KexiProjectListViewItem *bit = static_cast<KexiProjectListViewItem*>(item);
    KMenu *pm = 0;
    if (bit->partItem()) {
        pm = m_itemMenu;
        KexiProjectListViewItem *par_it = static_cast<KexiProjectListViewItem*>(bit->parent());
        m_itemMenu->update(par_it->partInfo(), bit->partItem());
    } else if (m_partMenu) {
        pm = m_partMenu;
//        m_newObjectAction->setIcon( KIcon(bit->partInfo()->itemIcon()) );
        m_partMenu->update(bit->partInfo());
        m_list->setCurrentItem(item);
        m_list->repaintItem(item);
    }
    if (pm)
        pm->exec(pos);
}

void
KexiProjectListView::slotExecuteItem(Q3ListViewItem *vitem)
{
// kDebug() << "KexiBrowser::slotExecuteItem()";
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(vitem);
    if (!it)
        return;
    if (!it->partItem() && !m_singleClick /*annoying when in single click mode*/) {
        m_list->setOpen(vitem, !vitem->isOpen());
        return;
    }
    if (it->partInfo()->isExecuteSupported())
        emit executeItem(it->partItem());
    else
        emit openOrActivateItem(it->partItem(), Kexi::DataViewMode);
}

void
KexiProjectListView::slotSelectionChanged(Q3ListViewItem* i)
{
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(i);
    if (!it)
        return;
    KexiPart::Part* part = Kexi::partManager().part(it->partInfo());
    if (!part) {
        it = static_cast<KexiProjectListViewItem*>(it->parent());
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

    m_openAction->setEnabled(gotitem && part && (part->info()->supportedViewModes() & Kexi::DataViewMode));
    if (m_designAction) {
//  m_designAction->setVisible(gotitem && part && (part->supportedViewModes() & Kexi::DesignViewMode));
        m_designAction->setEnabled(gotitem && part && (part->info()->supportedViewModes() & Kexi::DesignViewMode));
    }
    if (m_editTextAction)
        m_editTextAction->setEnabled(gotitem && part && (part->info()->supportedViewModes() & Kexi::TextViewMode));

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
              i18n("&Create Object: %1...", part->info()->instanceCaption() ));
            m_newObjectAction->setIcon( KIcon(part->info()->createItemIcon()) );
            if (m_features & Toolbar) {
/*              m_newObjectToolButton->setIcon( KIcon(part->info()->createItemIcon()) );
              m_newObjectToolButton->setToolTip(
                i18n("Create object: %1", part->info()->instanceCaption().toLower() ));
              m_newObjectToolButton->setWhatsThis(
                i18n("Creates a new object: %1", part->info()->instanceCaption().toLower() ));*/
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

void KexiProjectListView::installEventFilter(QObject * filterObj)
{
    if (!filterObj)
        return;
    m_list->installEventFilter(filterObj);
    QWidget::installEventFilter(filterObj);
}

bool KexiProjectListView::eventFilter(QObject *o, QEvent * e)
{
    /* if (o==m_list && e->type()==QEvent::Resize) {
        kDebug() << "resize!";
      }*/
    if (o == m_list->renameLineEdit()) {
        if (e->type() == QEvent::Hide)
            itemRenameDone();
    } else if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            if (0 == (ke->modifiers() & (Qt::ShiftButton | Qt::ControlButton | Qt::AltButton))) {
                Q3ListViewItem *it = m_list->selectedItem();
                if (it)
                    slotExecuteItem(it);
            } else if (Qt::ControlButton == (ke->modifiers() & (Qt::ShiftButton | Qt::ControlButton | Qt::AltButton))) {
                slotDesignObject();
            }
        }
    } else if (e->type() == QEvent::ShortcutOverride) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        //override delete action
        if (ke->key() == Qt::Key_Delete && ke->modifiers() == Qt::NoButton) {
            slotRemove();
            ke->accept();
            return true;
        }
        //override rename action
        if (ke->key() == Qt::Key_F2 && ke->modifiers() == Qt::NoButton) {
            slotRename();
            ke->accept();
            return true;
        }
        /*  else if (ke->key()==Qt::Key_Enter || ke->key()==Qt::Key_Return) {
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

void KexiProjectListView::slotRemove()
{
    if (!m_deleteAction || !m_deleteAction->isEnabled() || !(m_features & Writable))
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it || !it->partItem())
        return;
    emit removeItem(it->partItem());
}

void KexiProjectListView::slotNewObject()
{
    if (!m_newObjectAction || !(m_features & Writable))
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it || !it->partInfo())
        return;
    emit newItem(it->partInfo());
}

void KexiProjectListView::slotOpenObject()
{
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DataViewMode);
}

void KexiProjectListView::slotDesignObject()
{
    if (!m_designAction)
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::DesignViewMode);
}

void KexiProjectListView::slotEditTextObject()
{
    if (!m_editTextAction)
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it || !it->partItem())
        return;
    emit openItem(it->partItem(), Kexi::TextViewMode);
}

void KexiProjectListView::slotCut()
{
    if (!(m_features & Writable))
        return;
// KEXI_UNFINISHED_SHARED_ACTION("edit_cut");
    //TODO
}

void KexiProjectListView::slotCopy()
{
// KEXI_UNFINISHED_SHARED_ACTION("edit_copy");
    //TODO
}

void KexiProjectListView::slotPaste()
{
    if (!(m_features & Writable))
        return;
// KEXI_UNFINISHED_SHARED_ACTION("edit_paste");
    //TODO
}

void KexiProjectListView::slotRename()
{
    if (!m_renameAction || !(m_features & Writable))
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (it)
        m_list->rename(it, 0);
}

void KexiProjectListView::itemRenameDone()
{
    if (!(m_features & Writable))
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    if (!it)
        return;
    QString txt = it->text(0).trimmed();
    bool ok = it->partItem()->name().toLower() != txt.toLower(); //the new name must be different
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

void KexiProjectListView::setFocus()
{
    if (!m_list->selectedItem() && m_list->firstChild())//select first
        m_list->setSelected(m_list->firstChild(), true);
    m_list->setFocus();
}

void KexiProjectListView::updateItemName(KexiPart::Item& item, bool dirty)
{
    if (!(m_features & Writable))
        return;
    KexiProjectListViewItem *bitem = m_normalItems.value(item.identifier());
    if (!bitem)
        return;
    bitem->setText(0, item.name() + (dirty ? "*" : ""));
}

void KexiProjectListView::slotSettingsChanged(int)
{
    m_singleClick = KGlobalSettings::singleClick();
}

void KexiProjectListView::selectItem(KexiPart::Item& item)
{
    KexiProjectListViewItem *bitem = m_normalItems.value(item.identifier());
    if (!bitem)
        return;
    m_list->setSelected(bitem, true);
    m_list->ensureItemVisible(bitem);
    m_list->setCurrentItem(bitem);
}

void KexiProjectListView::clearSelection()
{
    m_list->clearSelection();
    Q3ListViewItem *item = m_list->firstChild();
    if (item) {
        m_list->ensureItemVisible(item);
    }
}

/*(new action removed)
void KexiBrowser::slotNewObjectMenuAboutToShow()
{
// kDebug() << "KexiBrowser::slotNewObjectMenuAboutToShow()";
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
}*/

void KexiProjectListView::slotExecuteObject()
{
    if (!m_executeAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit executeItem(item);
}

void KexiProjectListView::slotExportToClipboardAsDataTable()
{
    if (!m_dataExportToClipboardAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToClipboardAsDataTable(item);
}

void KexiProjectListView::slotExportToFileAsDataTable()
{
    if (!m_dataExportToFileAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit exportItemToFileAsDataTable(item);
}

KexiPart::Item* KexiProjectListView::selectedPartItem() const
{
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(m_list->selectedItem());
    return it ? it->partItem() : 0;
}

bool KexiProjectListView::actionEnabled(const QString& actionName) const
{
    if (actionName == "project_export_data_table" && (m_features & ContextMenus))
        return m_exportActionMenu->isVisible();
    kWarning() << "KexiBrowser::actionEnabled() no such action: " << actionName;
    return false;
}

void KexiProjectListView::slotPrintObject()
{
#ifndef KEXI_NO_QUICK_PRINTING
    if (!m_printAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit printItem(item);
#endif
}

void KexiProjectListView::slotPageSetupForObject()
{
#ifndef KEXI_NO_QUICK_PRINTING
    if (!m_pageSetupAction)
        return;
    KexiPart::Item* item = selectedPartItem();
    if (item)
        emit pageSetupForItem(item);
#endif
}


void KexiProjectListView::setReadOnly(bool set)
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

bool KexiProjectListView::isReadOnly() const
{
    return m_readOnly;
}

void KexiProjectListView::clear()
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
    setAlternateBackground(Qt::red);
}

KexiBrowserListView::~KexiBrowserListView()
{
}

void KexiBrowserListView::rename(Q3ListViewItem *item, int c)
{
    if (renameLineEdit()->isVisible())
        return;
    KexiProjectListViewItem *it = static_cast<KexiProjectListViewItem*>(item);
    if (it->partItem() && c == 0) {
        //only edit 1st column for items, not item groups
//TODO: also check it this item is not read-only
//  item->setText(0, item->text(0).mid(1,item->text(0).length()-2));
        //remove "modified" flag for editing
        nameEndsWithAsterisk = item->text(0).endsWith("*");
        if (nameEndsWithAsterisk)
            item->setText(0, item->text(0).left(item->text(0).length() - 1));
        K3ListView::rename(item, c);
        adjustColumn(0);
    }
}

bool KexiBrowserListView::isExecuteArea(const QPoint& point)
{
    return enableExecuteArea && K3ListView::isExecuteArea(point);
}



#include "KexiProjectListView.moc"
#include "KexiProjectListView_p.moc"
