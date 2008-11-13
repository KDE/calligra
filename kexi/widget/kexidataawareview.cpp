/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidataawareview.h"

#include <kexidataawareobjectiface.h>
#include <utils/kexisharedactionclient.h>
#include <core/KexiMainWindowIface.h>
#include <core/KexiStandardAction.h>
#include <KActionCollection>

#include <qlayout.h>
#include <QVBoxLayout>

#include <kmenu.h>

KexiDataAwareView::KexiDataAwareView(QWidget *parent)
        : KexiView(parent)
        , KexiSearchAndReplaceViewInterface()
        , m_internalView(0)
        , m_actionClient(0)
        , m_dataAwareObject(0)
{
}

KexiDataAwareView::~KexiDataAwareView()
{
}

void KexiDataAwareView::init(QWidget* viewWidget, KexiSharedActionClient* actionClient,
                             KexiDataAwareObjectInterface* dataAwareObject, bool noDataAware)
{
    m_internalView = viewWidget;
    m_actionClient = actionClient;
    m_dataAwareObject = dataAwareObject;
    setViewWidget(m_internalView, true);

    if (!noDataAware) {
        m_dataAwareObject->connectCellSelectedSignal(this, SLOT(slotCellSelected(int, int)));

        //! before closing - we'are accepting editing
        connect(this, SIGNAL(closing(bool&)), this, SLOT(slotClosing(bool&)));

        //! updating actions on start/stop editing
        m_dataAwareObject->connectRowEditStartedSignal(this, SLOT(slotUpdateRowActions(int)));
        m_dataAwareObject->connectRowEditTerminatedSignal(this, SLOT(slotUpdateRowActions(int)));
        m_dataAwareObject->connectReloadActionsSignal(this, SLOT(reloadActions()));
    }

//2.0 Q3VBoxLayout *box = new Q3VBoxLayout(this);
//2.0 box->addWidget(m_internalView);

    setMinimumSize(m_internalView->minimumSizeHint().width(),
                   m_internalView->minimumSizeHint().height());
    resize(preferredSizeHint(m_internalView->sizeHint()));
    setFocusProxy(m_internalView);

    if (!noDataAware) {
        initActions();
        reloadActions();
    }
}

void KexiDataAwareView::initActions()
{
    // - setup local actions
    QList<QAction*> viewActions;
    KAction* a;
    if (m_dataAwareObject->isSortingEnabled()) {
//  a = new KAction(this);
//  a->setSeparator(true);
//  viewActions << a;
        viewActions
        << KexiStandardAction::sortAscending(this, SLOT(sortAscending()), this)
        << KexiStandardAction::sortDescending(this, SLOT(sortDescending()), this);
    }
    KActionCollection *ac = KexiMainWindowIface::global()->actionCollection();
    viewActions << (a = dynamic_cast<KAction*>(ac->action("edit_find")));
    setViewActions(viewActions);

    plugSharedAction("edit_delete_row", this, SLOT(deleteCurrentRow()));
    m_actionClient->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

    plugSharedAction("edit_delete", this, SLOT(deleteAndStartEditCurrentCell()));
    m_actionClient->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

    plugSharedAction("edit_edititem", this, SLOT(startEditOrToggleValue()));
    m_actionClient->plugSharedAction(sharedAction("edit_edititem")); //for proper shortcut

    plugSharedAction("data_save_row", this, SLOT(acceptRowEdit()));
    m_actionClient->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut

    plugSharedAction("data_cancel_row_changes", this, SLOT(cancelRowEdit()));
    m_actionClient->plugSharedAction(sharedAction("data_cancel_row_changes")); //for proper shortcut

// if (m_dataAwareObject->isSortingEnabled()) {
//moved up  plugSharedAction("data_sort_az", this, SLOT(sortAscending()));
//moved up  plugSharedAction("data_sort_za", this, SLOT(sortDescending()));
// }

    m_actionClient->plugSharedAction(sharedAction("edit_insert_empty_row")); //for proper shortcut

    setAvailable("data_sort_az", m_dataAwareObject->isSortingEnabled());
    setAvailable("data_sort_za", m_dataAwareObject->isSortingEnabled());
//! \todo  plugSharedAction("data_filter", this, SLOT(???()));

    plugSharedAction("data_go_to_first_record", this, SLOT(slotGoToFirstRow()));
    plugSharedAction("data_go_to_previous_record", this, SLOT(slotGoToPreviusRow()));
    plugSharedAction("data_go_to_next_record", this, SLOT(slotGoToNextRow()));
    plugSharedAction("data_go_to_last_record", this, SLOT(slotGoToLastRow()));
    plugSharedAction("data_go_to_new_record", this, SLOT(slotGoToNewRow()));

//! \todo update availability
    setAvailable("data_go_to_first_record", true);
    setAvailable("data_go_to_previous_record", true);
    setAvailable("data_go_to_next_record", true);
    setAvailable("data_go_to_last_record", true);
    setAvailable("data_go_to_new_record", true);

    plugSharedAction("edit_copy", this, SLOT(copySelection()));
    m_actionClient->plugSharedAction(sharedAction("edit_copy")); //for proper shortcut

    plugSharedAction("edit_cut", this, SLOT(cutSelection()));
    m_actionClient->plugSharedAction(sharedAction("edit_cut")); //for proper shortcut

    plugSharedAction("edit_paste", this, SLOT(paste()));
    m_actionClient->plugSharedAction(sharedAction("edit_paste")); //for proper shortcut

// plugSharedAction("edit_find", this, SLOT(editFind()));
// m_actionClient->plugSharedAction(sharedAction("edit_find")); //for proper shortcut

// plugSharedAction("edit_findnext", this, SLOT(editFindNext()));
// m_actionClient->plugSharedAction(sharedAction("edit_findnext")); //for proper shortcut

// plugSharedAction("edit_findprevious", this, SLOT(editFindPrevious()));
// m_actionClient->plugSharedAction(sharedAction("edit_findprev")); //for proper shortcut

//! @todo plugSharedAction("edit_replace", this, SLOT(editReplace()));
//! @todo m_actionClient->plugSharedAction(sharedAction("edit_replace")); //for proper shortcut

// setAvailable("edit_find", true);
// setAvailable("edit_findnext", true);
// setAvailable("edit_findprevious", true);
//! @todo setAvailable("edit_replace", true);
}

void KexiDataAwareView::slotUpdateRowActions(int row)
{
    const bool ro = m_dataAwareObject->isReadOnly();
// const bool inserting = m_dataAwareObject->isInsertingEnabled();
    const bool deleting = m_dataAwareObject->isDeleteEnabled();
    const bool emptyInserting = m_dataAwareObject->isEmptyRowInsertingEnabled();
    const bool editing = m_dataAwareObject->rowEditing();
    const bool sorting = m_dataAwareObject->isSortingEnabled();
    const int rows = m_dataAwareObject->rows();

    setAvailable("edit_cut", !ro);
    setAvailable("edit_paste", !ro);
    setAvailable("edit_delete", !ro); // && !(inserting && row==rows));
    setAvailable("edit_delete_row", !ro && !(deleting && row == rows));
    setAvailable("edit_insert_empty_row", !ro && emptyInserting);
    setAvailable("edit_clear_table", !ro && deleting && rows > 0);
    setAvailable("data_save_row", editing);
    setAvailable("data_cancel_row_changes", editing);
    setAvailable("data_sort_az", sorting);
    setAvailable("data_sort_za", sorting);
}

QWidget* KexiDataAwareView::mainWidget() const
{
    return m_internalView;
}

QSize KexiDataAwareView::minimumSizeHint() const
{
    return m_internalView ? m_internalView->minimumSizeHint() : QSize(0, 0);//KexiView::minimumSizeHint();
}

QSize KexiDataAwareView::sizeHint() const
{
    return m_internalView ? m_internalView->sizeHint() : QSize(0, 0);//KexiView::sizeHint();
}

void KexiDataAwareView::updateActions(bool activated)
{
    setAvailable("data_sort_az", m_dataAwareObject->isSortingEnabled());
    setAvailable("data_sort_za", m_dataAwareObject->isSortingEnabled());
    KexiView::updateActions(activated);
}

void KexiDataAwareView::reloadActions()
{
// m_view->initActions(guiClient()->actionCollection());
//warning FIXME Move this to the table part
    /*
      kDebug()<<"INIT ACTIONS***********************************************************************";
      new KAction(i18n("Filter"), "view-filter", 0, this, SLOT(filter()), actionCollection(), "tablepart_filter");
      setXMLFile("kexidatatableui.rc");
    */
    m_dataAwareObject->contextMenu()->clear();
    if (!m_dataAwareObject->contextMenuTitleText().isEmpty()) {
        m_dataAwareObject->contextMenu()->addTitle(
            m_dataAwareObject->contextMenuTitleIcon(),
            m_dataAwareObject->contextMenuTitleText());
    }

    plugSharedAction("edit_cut", m_dataAwareObject->contextMenu());
    plugSharedAction("edit_copy", m_dataAwareObject->contextMenu());
    plugSharedAction("edit_paste", m_dataAwareObject->contextMenu());

    bool separatorNeeded = true;

    unplugSharedAction("edit_clear_table");
    plugSharedAction("edit_clear_table", this, SLOT(deleteAllRows()));

    if (m_dataAwareObject->isEmptyRowInsertingEnabled()) {
        unplugSharedAction("edit_insert_empty_row");
        plugSharedAction("edit_insert_empty_row", m_internalView, SLOT(insertEmptyRow()));
        if (separatorNeeded)
            m_dataAwareObject->contextMenu()->addSeparator();
        plugSharedAction("edit_insert_empty_row", m_dataAwareObject->contextMenu());
    } else {
        unplugSharedAction("edit_insert_empty_row");
        unplugSharedAction("edit_insert_empty_row", m_dataAwareObject->contextMenu());
    }

    if (m_dataAwareObject->isDeleteEnabled()) {
        if (separatorNeeded)
            m_dataAwareObject->contextMenu()->addSeparator();
        plugSharedAction("edit_delete", m_dataAwareObject->contextMenu());
        plugSharedAction("edit_delete_row", m_dataAwareObject->contextMenu());
    } else {
        unplugSharedAction("edit_delete_row", m_dataAwareObject->contextMenu());
        unplugSharedAction("edit_delete_row", m_dataAwareObject->contextMenu());
    }
    //if (!m_view->isSortingEnabled()) {
//  unplugSharedAction("data_sort_az");
//  unplugSharedAction("data_sort_za");
    //}
    setAvailable("data_sort_az", m_dataAwareObject->isSortingEnabled());
    setAvailable("data_sort_za", m_dataAwareObject->isSortingEnabled());

    slotCellSelected(m_dataAwareObject->currentColumn(), m_dataAwareObject->currentRow());
}

void KexiDataAwareView::slotCellSelected(int /*col*/, int row)
{
    slotUpdateRowActions(row);
}

void KexiDataAwareView::deleteAllRows()
{
    m_dataAwareObject->deleteAllRows(true/*ask*/, true/*repaint*/);
}

void KexiDataAwareView::deleteCurrentRow()
{
    m_dataAwareObject->deleteCurrentRow();
}

void KexiDataAwareView::deleteAndStartEditCurrentCell()
{
    m_dataAwareObject->deleteAndStartEditCurrentCell();
}

void KexiDataAwareView::startEditOrToggleValue()
{
    m_dataAwareObject->startEditOrToggleValue();
}

bool KexiDataAwareView::acceptRowEdit()
{
    return m_dataAwareObject->acceptRowEdit();
}

void KexiDataAwareView::slotClosing(bool& cancel)
{
    if (!acceptRowEdit())
        cancel = true;
}

void KexiDataAwareView::cancelRowEdit()
{
    m_dataAwareObject->cancelRowEdit();
}

void KexiDataAwareView::sortAscending()
{
    m_dataAwareObject->sortAscending();
}

void KexiDataAwareView::sortDescending()
{
    m_dataAwareObject->sortDescending();
}

void KexiDataAwareView::copySelection()
{
    m_dataAwareObject->copySelection();
}

void KexiDataAwareView::cutSelection()
{
    m_dataAwareObject->cutSelection();
}

void KexiDataAwareView::paste()
{
    m_dataAwareObject->paste();
}

void KexiDataAwareView::slotGoToFirstRow()
{
    m_dataAwareObject->selectFirstRow();
}
void KexiDataAwareView::slotGoToPreviusRow()
{
    m_dataAwareObject->selectPrevRow();
}
void KexiDataAwareView::slotGoToNextRow()
{
    m_dataAwareObject->selectNextRow();
}
void KexiDataAwareView::slotGoToLastRow()
{
    m_dataAwareObject->selectLastRow();
}
void KexiDataAwareView::slotGoToNewRow()
{
    m_dataAwareObject->addNewRecordRequested();
}

bool KexiDataAwareView::setupFindAndReplace(QStringList& columnNames, QStringList& columnCaptions,
        QString& currentColumnName)
{
    if (!dataAwareObject() || !dataAwareObject()->data())
        return false;
    const KexiTableViewColumn::List columns(dataAwareObject()->data()->columns());
    foreach(KexiTableViewColumn *col, columns) {
        if (!col->isVisible())
            continue;
        columnNames.append(col->field()->name());
        columnCaptions.append(col->captionAliasOrName());
    }

    //update "look in" selection if there was any
    const int currentColumnNumber = dataAwareObject()->currentColumn();
    if (currentColumnNumber >= 0 && currentColumnNumber < columns.count()) {
        KexiTableViewColumn *col = columns.at(currentColumnNumber);
        if (col && col->field())
            currentColumnName = col->field()->name();
    }
    return true;
}

tristate KexiDataAwareView::find(const QVariant& valueToFind,
                                 const KexiSearchAndReplaceViewInterface::Options& options, bool next)
{
    if (!dataAwareObject() || !dataAwareObject()->data())
        return cancelled;

// const KexiDataAwareObjectInterface::FindAndReplaceOptions options(dlg->options());
    /* if (res == KexiFindDialog::Find) {*/
//  QVariant valueToFind(dlg->valueToFind());
    return dataAwareObject()->find(valueToFind, options, next);
    /*
    //! @todo result...

      }
      else if (res == KexiFindDialog::Replace) {
    //! @todo
      }
      else if (res == KexiFindDialog::ReplaceAll) {
    //! @todo
      }
      */
}

tristate KexiDataAwareView::findNextAndReplace(const QVariant& valueToFind,
        const QVariant& replacement,
        const KexiSearchAndReplaceViewInterface::Options& options, bool replaceAll)
{
    if (!dataAwareObject() || !dataAwareObject()->data())
        return cancelled;

    return dataAwareObject()->findNextAndReplace(valueToFind, replacement, options, replaceAll);
}

/*
void KexiDataAwareView::editFindNext()
{
  //! @todo reuse code from editFind()
}

void KexiDataAwareView::editFindPrevious()
{
  //! @todo reuse code from editFind()
}

void KexiDataAwareView::editReplace()
{
  //! @todo editReplace()
  //! @todo reuse code from editFind()
  // When ready, update KexiDataAwareView::initActions() and KexiMainWindowImpl
}*/

#include "kexidataawareview.moc"
