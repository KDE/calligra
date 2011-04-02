/* This file is part of the KDE project
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

   Based on KexiTableView code.
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#include "kexidataawareobjectiface.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QToolTip>
#include <QStyle>
#include <QStyleOptionComplex>

#include <kmessagebox.h>

#include <kexi.h>
#include <kexi_global.h>
#include <kexiutils/validator.h>
#include <widget/utils/kexirecordnavigator.h>
#include <widget/utils/kexirecordmarker.h>
#include <kexidb/roweditbuffer.h>

//#include "kexitableviewheader.h"

#include <limits.h>

using namespace KexiUtils;

KexiDataAwareObjectInterface::KexiDataAwareObjectInterface()
{
    m_data = 0;
    m_owner = false;
    m_readOnly = -1; //don't know
    m_insertingEnabled = -1; //don't know
    m_isSortingEnabled = true;
    m_isFilteringEnabled = true;
    m_deletionPolicy = AskDelete;
    m_inside_acceptEditor = false;
    m_inside_acceptRowEdit = false;
    m_acceptsRowEditAfterCellAccepting = false;
    m_internal_acceptsRowEditAfterCellAccepting = false;
    m_contentsMousePressEvent_dblClick = false;
    m_navPanel = 0;
    m_initDataContentsOnShow = false;
    m_cursorPositionSetExplicityBeforeShow = false;
    m_verticalHeader = 0;
//    m_horizontalHeader = 0;
    m_insertItem = 0;
// m_rowEditBuffer = 0;
    m_spreadSheetMode = false;
    m_dropsAtRowEnabled = false;
    m_updateEntireRowWhenMovingToOtherRow = false;
    m_dragIndicatorLine = -1;
    m_emptyRowInsertingEnabled = false;
    m_contextMenu = 0;
    m_contextMenuEnabled = true;
    m_rowWillBeDeleted = -1;
    m_alsoUpdateNextRow = false;
    m_verticalHeaderAlreadyAdded = false;
    m_vScrollBarValueChanged_enabled = true;
    m_scrollbarToolTipsEnabled = true;
//    m_scrollBarTipTimerCnt = 0;
//    m_scrollBarTipTimer.setSingleShot(true);
//    m_scrollBarTip = 0;
    m_recentSearchDirection = KexiSearchAndReplaceViewInterface::Options::DefaultSearchDirection;

    // setup scrollbar tooltip and related members
/*replaced by QToolTip
    m_scrollBarTip = new QLabel();
    m_scrollBarTip->setObjectName("vScrollBarToolTip");
    m_scrollBarTip->setWindowFlags(
        Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WX11BypassWM
        | Qt::WStyle_StaysOnTop | Qt::WStyle_Tool);
    m_scrollBarTip->setPalette(QToolTip::palette());
    m_scrollBarTip->setContentsMargins(2, 2, 2, 2);
    m_scrollBarTip->setIndent(0);
    m_scrollBarTip->setAlignment(Qt::AlignCenter);
    m_scrollBarTip->setFrameStyle(QFrame::Plain | QFrame::Box);
    m_scrollBarTip->setLineWidth(1);
 */
    clearVariables();
}

KexiDataAwareObjectInterface::~KexiDataAwareObjectInterface()
{
    delete m_insertItem;
// delete m_rowEditBuffer;
/*replaced by QToolTip    delete m_scrollBarTip;*/
    //we cannot delete m_data here... subclasses should do this
}

void KexiDataAwareObjectInterface::clearVariables()
{
    m_editor = 0;
// m_rowEditBuffer = 0;
    m_rowEditing = false;
    m_newRowEditing = false;
    m_curRow = -1;
    m_curCol = -1;
    m_currentItem = 0;
}

void KexiDataAwareObjectInterface::setData(KexiTableViewData *data, bool owner)
{
    const bool theSameData = m_data && m_data == data;
    if (m_owner && m_data && m_data != data/*don't destroy if it's the same*/) {
        kDebug() << "destroying old data (owned)";
        delete m_data; //destroy old data
        m_data = 0;
        m_itemIterator = KexiTableViewData::Iterator();
    }
    m_owner = owner;
    m_data = data;
    if (m_data)
        m_itemIterator = m_data->constBegin();

    kDebug() << "using shared data";
    //add columns
//OK?
    clearColumnsInternal(false);
    if (m_data) {
        int i = -1;
        foreach(KexiTableViewColumn *col, m_data->columns()) {
            i++;
            KexiDB::Field *f = col->field();
            if (col->isVisible()) {
                int wid = f->width();
                if (wid == 0)
                    wid = KEXI_DEFAULT_DATA_COLUMN_WIDTH;//default col width in pixels
//! @todo add col width configuration and storage
                addHeaderColumn(col->isHeaderTextVisible()
                                ? col->captionAliasOrName() : QString(),
                                f->description(), col->icon(), wid);
            }
        }
    }
    if (m_verticalHeader) {
        m_verticalHeader->clear();
        if (m_data)
            m_verticalHeader->addLabels(m_data->count());
    }
    if (m_data && m_data->count() == 0 && m_navPanel)
        m_navPanel->setCurrentRecordNumber(0 + 1);

    if (m_data && !theSameData) {
//! @todo: store sorting settings?
        setSorting(-1);
//  connect(m_data, SIGNAL(refreshRequested()), this, SLOT(slotRefreshRequested()));
        connectToReloadDataSlot(m_data, SIGNAL(reloadRequested()));
        QObject* thisObject = dynamic_cast<QObject*>(this);
        if (thisObject) {
            QObject::connect(m_data, SIGNAL(destroying()), thisObject, SLOT(slotDataDestroying()));
            QObject::connect(m_data, SIGNAL(rowsDeleted(const QList<int> &)),
                             thisObject, SLOT(slotRowsDeleted(const QList<int> &)));
            QObject::connect(m_data, SIGNAL(aboutToDeleteRow(KexiDB::RecordData&, KexiDB::ResultInfo*, bool)),
                             thisObject, SLOT(slotAboutToDeleteRow(KexiDB::RecordData&, KexiDB::ResultInfo*, bool)));
            QObject::connect(m_data, SIGNAL(rowDeleted()), thisObject, SLOT(slotRowDeleted()));
            QObject::connect(m_data, SIGNAL(rowInserted(KexiDB::RecordData*, bool)),
                             thisObject, SLOT(slotRowInserted(KexiDB::RecordData*, bool)));
            QObject::connect(m_data, SIGNAL(rowInserted(KexiDB::RecordData*, uint, bool)),
                             thisObject, SLOT(slotRowInserted(KexiDB::RecordData*, uint, bool))); //not db-aware
            QObject::connect(m_data, SIGNAL(rowRepaintRequested(KexiDB::RecordData&)),
                             thisObject, SLOT(slotRowRepaintRequested(KexiDB::RecordData&)));
            // setup scrollbar's tooltip
//            QObject::connect(verticalScrollBar(), SIGNAL(sliderReleased()),
//                             thisObject, SLOT(vScrollBarSliderReleased()));
            QObject::connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
                             thisObject, SLOT(vScrollBarValueChanged(int)));
/*replaced by QToolTip            QObject::connect(&m_scrollBarTipTimer, SIGNAL(timeout()),
                             thisObject, SLOT(scrollBarTipTimeout()));*/
        }
    }

    if (!m_data) {
//  clearData();
        cancelRowEdit();
        //m_data->clearInternal();
        clearVariables();
    } else {
        if (!m_insertItem) {//first setData() call - add 'insert' item
            m_insertItem = m_data->createItem();
        } else {//just reinit
            m_insertItem->init(m_data->columns().count());
        }
    }

    //update gui mode
    if (m_navPanel)
        m_navPanel->setInsertingEnabled(m_data && isInsertingEnabled());
    if (m_verticalHeader)
        m_verticalHeader->showInsertRow(m_data && isInsertingEnabled());

    initDataContents();
    updateIndicesForVisibleValues();

    if (m_data)
        /*emit*/ dataSet(m_data);
}

void KexiDataAwareObjectInterface::initDataContents()
{
    m_editor = 0;
// QSize s(tableSize());
// resizeContents(s.width(),s.height());

    if (m_navPanel)
        m_navPanel->setRecordCount(rows());

    if (m_data && !m_cursorPositionSetExplicityBeforeShow) {
        //set current row:
        m_currentItem = 0;
        int curRow = -1, curCol = -1;
        if (m_data->columnsCount() > 0) {
            if (rows() > 0) {
                m_itemIterator = m_data->constBegin();
                m_currentItem = *m_itemIterator;
                curRow = 0;
                curCol = 0;
            } else {//no data
                if (isInsertingEnabled()) {
                    m_currentItem = m_insertItem;
                    curRow = 0;
                    curCol = 0;
                }
            }
        }
        setCursorPosition(curRow, curCol, true/*force*/);
    }
    ensureCellVisible(m_curRow, m_curCol);
// updateRowCountInfo();
// setNavRowCount(rows());

//OK?
// updateContents();
    updateWidgetContents();

    m_cursorPositionSetExplicityBeforeShow = false;

    /*emit*/ dataRefreshed();
}

void KexiDataAwareObjectInterface::setSortingEnabled(bool set)
{
    if (m_isSortingEnabled && !set)
        setSorting(-1);
    m_isSortingEnabled = set;
    /*emit*/ reloadActions();
}

void KexiDataAwareObjectInterface::setSorting(int col, bool ascending)
{
    if (!m_data || !m_isSortingEnabled)
        return;
// d->pTopHeader->setSortIndicator(col, ascending ? Ascending : Descending);
    setLocalSortingOrder(col, ascending ? 1 : -1);
    m_data->setSorting(col, ascending);
}

int KexiDataAwareObjectInterface::dataSortedColumn() const
{
    if (m_data && m_isSortingEnabled)
        return m_data->sortedColumn();
    return -1;
}

int KexiDataAwareObjectInterface::dataSortingOrder() const
{
    return m_data ? m_data->sortingOrder() : 0;
}

bool KexiDataAwareObjectInterface::sort()
{
    if (!m_data || !m_isSortingEnabled)
        return false;

    if (rows() < 2)
        return true;

    if (!acceptRowEdit())
        return false;

    const int oldRow = m_curRow;
    if (m_data->sortedColumn() != -1)
        m_data->sort();

    //locate current record
    if (!m_currentItem) {
        m_itemIterator = m_data->constBegin();
        m_currentItem = *m_itemIterator;
        m_curRow = 0;
        if (!m_currentItem)
            return true;
    }
    if (m_currentItem != m_insertItem) {
        m_curRow = m_data->indexOf(m_currentItem);
        int jump = m_curRow - oldRow;
        if (jump < 0)
            m_itemIterator -= -jump;
        else
            m_itemIterator += jump;
    }

    updateGUIAfterSorting();
    editorShowFocus(m_curRow, m_curCol);
    if (m_verticalHeader)
        m_verticalHeader->setCurrentRow(m_curRow);
//!TEMP HACK
//    if (m_horizontalHeader)
//        m_horizontalHeader->setSelectedSection(m_curCol);
    if (m_navPanel)
        m_navPanel->setCurrentRecordNumber(m_curRow + 1);
    return true;
}

void KexiDataAwareObjectInterface::sortAscending()
{
    if (currentColumn() < 0)
        return;
    sortColumnInternal(currentColumn(), 1);
}

void KexiDataAwareObjectInterface::sortDescending()
{
    if (currentColumn() < 0)
        return;
    sortColumnInternal(currentColumn(), -1);
}

void KexiDataAwareObjectInterface::sortColumnInternal(int col, int order)
{
    //-select sorting
    bool asc;
    if (order == 0) {// invert
        if (col == dataSortedColumn() && dataSortingOrder() == 1)
            asc = dataSortingOrder() == -1; //inverse sorting for this column -> descending order
        else
            asc = true;
    } else
        asc = (order == 1);

    int prevSortOrder = currentLocalSortingOrder();
    const int prevSortColumn = currentLocalSortingOrder();
    setSorting(col, asc);
    //-perform sorting
    if (!sort())
        setLocalSortingOrder(prevSortColumn, prevSortOrder); //this will also remove indicator
    //if prevSortColumn==-1
    if (col != prevSortColumn)
        /*emit*/ sortedColumnChanged(col);
}

bool KexiDataAwareObjectInterface::isInsertingEnabled() const
{
    if (isReadOnly())
        return false;
    if (m_insertingEnabled == 1 || m_insertingEnabled == 0)
        return (bool)m_insertingEnabled;
    if (!hasData())
        return true;
    return m_data->isInsertingEnabled();
}

void KexiDataAwareObjectInterface::setFilteringEnabled(bool set)
{
    m_isFilteringEnabled = set;
}

bool KexiDataAwareObjectInterface::isDeleteEnabled() const
{
    return (m_deletionPolicy != NoDelete) && !isReadOnly();
}

void KexiDataAwareObjectInterface::setDeletionPolicy(DeletionPolicy policy)
{
    m_deletionPolicy = policy;
// updateContextMenu();
}

void KexiDataAwareObjectInterface::setReadOnly(bool set)
{
    if (isReadOnly() == set || (m_data && m_data->isReadOnly() && !set))
        return; //not allowed!
    m_readOnly = (set ? 1 : 0);
    if (set)
        setInsertingEnabled(false);
    updateWidgetContents();
    /*emit*/ reloadActions();
}

bool KexiDataAwareObjectInterface::isReadOnly() const
{
    if (!hasData())
        return true;
    if (m_readOnly == 1 || m_readOnly == 0)
        return (bool)m_readOnly;
    if (!hasData())
        return true;
    return m_data->isReadOnly();
}

void KexiDataAwareObjectInterface::setInsertingEnabled(bool set)
{
    if (isInsertingEnabled() == set || (m_data && !m_data->isInsertingEnabled() && set))
        return; //not allowed!
    m_insertingEnabled = (set ? 1 : 0);
    if (m_navPanel)
        m_navPanel->setInsertingEnabled(set);
    if (m_verticalHeader)
        m_verticalHeader->showInsertRow(set);
    if (set)
        setReadOnly(false);
// update();
    updateWidgetContents();
    /*emit*/ reloadActions();
}

void KexiDataAwareObjectInterface::setSpreadSheetMode()
{
    m_spreadSheetMode = true;
    setSortingEnabled(false);
    setInsertingEnabled(false);
    setAcceptsRowEditAfterCellAccepting(true);
    setFilteringEnabled(false);
    setEmptyRowInsertingEnabled(true);
    m_navPanelEnabled = false;
}

void KexiDataAwareObjectInterface::selectNextRow()
{
    selectRow(qMin(rows() - 1 + (isInsertingEnabled() ? 1 : 0), m_curRow + 1));
}

void KexiDataAwareObjectInterface::selectPrevPage()
{
    selectRow(
        qMax(0, m_curRow - rowsPerPage())
    );
}

void KexiDataAwareObjectInterface::selectNextPage()
{
    selectRow(
        qMin(
            rows() - 1 + (isInsertingEnabled() ? 1 : 0),
            m_curRow + rowsPerPage()
        )
    );
}

void KexiDataAwareObjectInterface::selectFirstRow()
{
    selectRow(0);
}

void KexiDataAwareObjectInterface::selectLastRow()
{
// selectRow(rows() - 1 + (isInsertingEnabled()?1:0));
    selectRow(rows() - 1);
}

void KexiDataAwareObjectInterface::selectRow(int row)
{
    m_vScrollBarValueChanged_enabled = false; //disable tooltip
    setCursorPosition(row, -1);
    m_vScrollBarValueChanged_enabled = true;
}

void KexiDataAwareObjectInterface::selectPrevRow()
{
    selectRow(qMax(0, m_curRow - 1));
}

void KexiDataAwareObjectInterface::clearSelection()
{
// selectRow( -1 );
    int oldRow = m_curRow;
// int oldCol = m_curCol;
    m_curRow = -1;
    m_curCol = -1;
    m_currentItem = 0;
    updateRow(oldRow);
    if (m_navPanel)
        m_navPanel->setCurrentRecordNumber(0);
// setNavRowNumber(-1);
}

void KexiDataAwareObjectInterface::setCursorPosition(int row, int col/*=-1*/, bool forceSet)
{
    int newrow = row;
    int newcol = col;

    if (rows() <= 0) {
        if (m_verticalHeader)
            m_verticalHeader->setCurrentRow(-1);
//!TEMP HACK
//        if (m_horizontalHeader)
//           m_horizontalHeader->setSelectedSection(-1);
        if (isInsertingEnabled()) {
            m_currentItem = m_insertItem;
            newrow = 0;
            if (col >= 0)
                newcol = col;
            else
                newcol = 0;
        } else {
            m_currentItem = 0;
            m_curRow = -1;
            m_curCol = -1;
            return;
        }
    }

    if (col >= 0) {
        newcol = qMax(0, col);
        newcol = qMin(columns() - 1, newcol);
    } else {
        newcol = m_curCol; //no changes
        newcol = qMax(0, newcol); //may not be < 0 !
    }
    newrow = qMax(0, row);
    newrow = qMin(rows() - 1 + (isInsertingEnabled() ? 1 : 0), newrow);

// d->pCurrentItem = itemAt(d->curRow);
// kDebug() << "setCursorPosition(): d->curRow=" << d->curRow << " oldRow=" << oldRow << " d->curCol=" << d->curCol << " oldCol=" << oldCol;

    if (forceSet || m_curRow != newrow || m_curCol != newcol) {
        kDebug() << QString("old:%1,%2 new:%3,%4").arg(m_curCol)
            .arg(m_curRow).arg(newcol).arg(newrow);

        // cursor moved: get rid of editor
        if (m_editor) {
            if (!m_contentsMousePressEvent_dblClick) {
                if (!acceptEditor()) {
                    return;
                }
                //update row num. again
                newrow = qMin(rows() - 1 + (isInsertingEnabled() ? 1 : 0), newrow);
            }
        }
        if (m_errorMessagePopup) {
            m_errorMessagePopup->close();
        }

        if ((m_curRow != newrow || forceSet) && m_navPanel)  {//update current row info
            m_navPanel->setCurrentRecordNumber(newrow + 1);
//   setNavRowNumber(newrow);
//   d->navBtnPrev->setEnabled(newrow>0);
//   d->navBtnFirst->setEnabled(newrow>0);
//   d->navBtnNext->setEnabled(newrow<(rows()-1+(isInsertingEnabled()?1:0)));
//   d->navBtnLast->setEnabled(newrow!=(rows()-1));
        }

        // cursor moved to other row: end of row editing
        bool newRowInserted = false;
        if (m_rowEditing && m_curRow != newrow) {
            newRowInserted = m_newRowEditing;
            if (!acceptRowEdit()) {
                //accepting failed: cancel setting the cursor
                return;
            }
            //update row number, because number of rows changed
            newrow = qMin(rows() - 1 + (isInsertingEnabled() ? 1 : 0), newrow);

            if (m_navPanel)
                m_navPanel->setCurrentRecordNumber(newrow + 1); //refresh
        }

        //change position
        int oldRow = m_curRow;
        int oldCol = m_curCol;
        m_curRow = newrow;
        m_curCol = newcol;

//  int cw = columnWidth( d->curCol );
//  int rh = rowHeight();
//  ensureVisible( columnPos( d->curCol ) + cw / 2, rowPos( d->curRow ) + rh / 2, cw / 2, rh / 2 );
//  center(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + rh / 2, cw / 2, rh / 2);
// kDebug() << " contentsY() = "<< contentsY();

//js  if (oldRow > d->curRow)
//js   ensureVisible(columnPos(d->curCol), rowPos(d->curRow) + rh, columnWidth(d->curCol), rh);
//js  else// if (oldRow <= d->curRow)
//js  ensureVisible(columnPos(d->curCol), rowPos(d->curRow), columnWidth(d->curCol), rh);


        //show editor-dependent focus, if we're changing the current column
        if (oldCol >= 0 && oldCol < columns() && m_curCol != oldCol) {
            //find the editor for this column
            KexiDataItemInterface *edit = editor(oldCol);
            if (edit) {
                edit->hideFocus();
            }
        }

        // position changed, so subsequent searching should be started from scratch
        // (e.g. from the current cell or the top-left cell)
        m_positionOfRecentlyFoundValue.exists = false;

        //show editor-dependent focus, if needed
        editorShowFocus(m_curRow, m_curCol);

        if (m_updateEntireRowWhenMovingToOtherRow)
            updateRow(oldRow);
        else
            updateCell(oldRow, oldCol);

//  //quite clever: ensure the cell is visible:
//  ensureCellVisible(m_curRow, m_curCol);

//  QPoint pcenter = QRect( columnPos(d->curCol), rowPos(d->curRow), columnWidth(d->curCol), rh).center();
//  ensureVisible(pcenter.x(), pcenter.y(), columnWidth(d->curCol)/2, rh/2);

//  ensureVisible(columnPos(d->curCol), rowPos(d->curRow) - contentsY(), columnWidth(d->curCol), rh);
        if (m_verticalHeader && (oldRow != m_curRow || forceSet))
            m_verticalHeader->setCurrentRow(m_curRow);

        if (m_updateEntireRowWhenMovingToOtherRow)
            updateRow(m_curRow);
        else
            updateCell(m_curRow, m_curCol);

        if (m_curCol != oldCol || m_curRow != oldRow || forceSet) {//ensure this is also refreshed
            if (!m_updateEntireRowWhenMovingToOtherRow) //only if entire row has not been updated
                updateCell(oldRow, m_curCol);
        }
        //update row
        if (forceSet || m_curRow != oldRow) {
            if (isInsertingEnabled() && m_curRow == rows()) {
                kDebug() << "NOW insert item is current";
                m_currentItem = m_insertItem;
                m_itemIterator = KexiTableViewData::Iterator();
            } else {
                kDebug() << QString("NOW item at %1 (%2) is current")
                    .arg(m_curRow).arg((ulong)itemAt(m_curRow));
                if (   !newRowInserted && isInsertingEnabled() && m_currentItem == m_insertItem
                    && m_curRow == (rows() - 1))
                {
                    //moving from the 'insert item' to the last item
                    m_itemIterator = m_data->constBegin();
                    m_itemIterator += (m_data->count() - 1);
                }
                else if (!newRowInserted && !forceSet && m_currentItem != m_insertItem && 0 == m_curRow) {
                    m_itemIterator = m_data->constBegin();
                }
                else if (   !newRowInserted && !forceSet && m_currentItem != m_insertItem
                         && oldRow >= 0 && (oldRow + 1) == m_curRow)
                {
                    ++m_itemIterator; // just move next
                }
                else if (   !newRowInserted && !forceSet && m_currentItem != m_insertItem
                         && oldRow >= 0 && (oldRow - 1) == m_curRow)
                {
                    --m_itemIterator; // just move back
                }
                else { //move at:
                    m_itemIterator = m_data->constBegin();
                    m_itemIterator += m_curRow;
                }
                if (!*m_itemIterator) { //sanity
                    m_itemIterator = m_data->constBegin();
                    m_itemIterator += m_curRow;
                }
                m_currentItem = *m_itemIterator;
            }
        }

        //quite clever: ensure the cell is visible:
        ensureCellVisible(m_curRow, m_curCol);

        //!TEMP HACK
//        if (m_horizontalHeader && (oldCol != m_curCol || forceSet))
//            m_horizontalHeader->setSelectedSection(m_curCol);

        /*emit*/ itemSelected(m_currentItem);
        /*emit*/ cellSelected(m_curCol, m_curRow);
        /* only needed for forms */
        selectCellInternal();
    } else {
        kDebug() << "NO CHANGE";
    }

    if (m_initDataContentsOnShow) {
        m_cursorPositionSetExplicityBeforeShow = true;
    }
}

bool KexiDataAwareObjectInterface::acceptRowEdit()
{
    if (!m_rowEditing || /*sanity*/!m_data->rowEditBuffer() || m_inside_acceptRowEdit)
        return true;
    if (m_inside_acceptEditor) {
        m_internal_acceptsRowEditAfterCellAccepting = true;
        return true;
    }
    m_inside_acceptRowEdit = true; // avoid recursion
    KexiUtils::Setter<bool> acceptRowEditSetter(&m_inside_acceptRowEdit, false);

    m_internal_acceptsRowEditAfterCellAccepting = false;

    const int columnEditedBeforeAccepting = m_editor ? currentColumn() : -1;
    if (!acceptEditor()) {
        return false;
    }
    kDebug() << "EDIT ROW ACCEPTING...";

    bool success = true;
// bool allow = true;
// int faultyColumn = -1; // will be !=-1 if cursor has to be moved to that column
    const bool inserting = m_newRowEditing;
// QString msg, desc;
// bool inserting = d->pInsertItem && d->pInsertItem==d->pCurrentItem;

    if (m_data->rowEditBuffer()->isEmpty() && !m_newRowEditing) {
        /*  if (d->newRowEditing) {
              cancelRowEdit();
              kDebug() << "-- NOTHING TO INSERT!!!";
              return true;
            }
            else {*/
        kDebug() << "-- NOTHING TO ACCEPT!!!";
//  }
    } else {//not empty edit buffer or new row to insert:
        if (m_newRowEditing) {
//   emit aboutToInsertRow(d->pCurrentItem, m_data->rowEditBuffer(), success, &faultyColumn);
//   if (success) {
            kDebug() << "-- INSERTING: ";
            m_data->rowEditBuffer()->debug();
            success = m_data->saveNewRow(*m_currentItem);
//    if (!success) {
//    }
//   }
        } else {
//   emit aboutToUpdateRow(d->pCurrentItem, m_data->rowEditBuffer(), success, &faultyColumn);
            if (success) {
                //accept changes for this row:
                kDebug() << "-- UPDATING: ";
                m_data->rowEditBuffer()->debug();
                kDebug() << "-- BEFORE: ";
                m_currentItem->debug();
                success = m_data->saveRowChanges(*m_currentItem);//, &msg, &desc, &faultyColumn);
                kDebug() << "-- AFTER: ";
                m_currentItem->debug();

//    if (!success) {
//    }
            }
        }
    }

    if (success) {
        //editing is finished:
        if (m_newRowEditing) {
            //update current-item-iterator
            setCursorPosition(-1, -1, true /*forceSet*/);
//2.0            m_itemIterator += (m_data->count() - 1);
//2.0            m_currentItem = *m_itemIterator;
        }
        m_rowEditing = false;
        m_newRowEditing = false;
        //indicate on the vheader that we are not editing
        if (m_verticalHeader)
            m_verticalHeader->setEditRow(-1);

        updateAfterAcceptRowEdit();

        kDebug() << "EDIT ROW ACCEPTED:";
//  /*debug*/itemAt(m_curRow);

        if (inserting) {
//   emit rowInserted(d->pCurrentItem);
            //update navigator's data
            if (m_navPanel)
                m_navPanel->setRecordCount(rows());
        } else {
//   emit rowUpdated(d->pCurrentItem);
        }

        /*emit*/ rowEditTerminated(m_curRow);
    } else {
//  if (!allow) {
//   kDebug() << "INSERT/EDIT ROW - DISALLOWED by signal!";
//  }
//  else {
//   kDebug() << "EDIT ROW - ERROR!";
//  }
        int faultyColumn = -1;
        if (m_data->result().column >= 0 && m_data->result().column < columns())
            faultyColumn = m_data->result().column;
        else if (columnEditedBeforeAccepting >= 0)
            faultyColumn = columnEditedBeforeAccepting;
        if (faultyColumn >= 0) {
            setCursorPosition(m_curRow, faultyColumn);
        }

        const int button = showErrorMessageForResult(m_data->result());
        if (KMessageBox::No == button) {
            //discard changes
            cancelRowEdit();
        } else {
            if (faultyColumn >= 0) {
                //edit this cell
                startEditCurrentCell();
            }
        }
    }
    return success;
}

bool KexiDataAwareObjectInterface::cancelRowEdit()
{
    if (!hasData())
        return false;
    if (!m_rowEditing)
        return false;
    cancelEditor();
    m_rowEditing = false;
    //indicate on the vheader that we are not editing
    if (m_verticalHeader)
        m_verticalHeader->setEditRow(-1);
    m_alsoUpdateNextRow = m_newRowEditing;
    if (m_newRowEditing) {
        m_newRowEditing = false;
        //remove current edited row (it is @ the end of list)
        m_data->removeLast();
        //current item is now empty, last row
        m_currentItem = m_insertItem;
        //update visibility
        if (m_verticalHeader)
            m_verticalHeader->removeLabel(false); //-1 label
//  updateContents(columnPos(0), rowPos(rows()),
//   viewport()->width(), d->rowHeight*3 + (m_navPanel ? m_navPanel->height() : 0)*3 );
//  updateContents(); //js: above did not work well so we do that dirty
        updateWidgetContents();
//TODO: still doesn't repaint properly!!
//  QSize s(tableSize());
//  resizeContents(s.width(), s.height());
        updateWidgetContentsSize();
//  m_verticalHeader->update();
        //--no cancel action is needed for datasource,
        //  because the row was not yet stored.
    }

    m_data->clearRowEditBuffer();
    updateAfterCancelRowEdit();

//! \todo (js): cancel changes for this row!
    kDebug() << "EDIT ROW CANCELLED.";

    /*emit*/ rowEditTerminated(m_curRow);
    return true;
}

void KexiDataAwareObjectInterface::updateAfterCancelRowEdit()
{
    updateRow(m_curRow);
    if (m_alsoUpdateNextRow)
        updateRow(m_curRow + 1);
    m_alsoUpdateNextRow = false;
}

void KexiDataAwareObjectInterface::updateAfterAcceptRowEdit()
{
    updateRow(m_curRow);
}

void KexiDataAwareObjectInterface::removeEditor()
{
    if (!m_editor)
        return;
    m_editor->hideWidget();
    m_editor = 0;
}

bool KexiDataAwareObjectInterface::cancelEditor()
{
    if (m_errorMessagePopup) {
        m_errorMessagePopup->close();
    }
    if (!m_editor)
        return false;
    removeEditor();
    return true;
}

bool KexiDataAwareObjectInterface::acceptEditor()
{
    if (!hasData())
        return true;
    if (!m_editor || m_inside_acceptEditor)
        return true;

    m_inside_acceptEditor = true; // avoid recursion
    KexiUtils::Setter<bool> acceptRowEditSetter(&m_inside_acceptEditor, false);

    QVariant newval;
    Validator::Result res = Validator::Ok;
    QString msg, desc;
    bool setNull = false;
// bool allow = true;
// static const QString msg_NOT_NULL = i18n("\"%1\" column requires a value to be entered.");

    //autoincremented field can be omitted (left as null or empty) if we're inserting a new row
    const bool autoIncColumnCanBeOmitted = m_newRowEditing && m_editor->field()->isAutoIncrement();
// const bool autoIncColumnCanBeOmitted = m_newRowEditing && m_editor->columnInfo()->field->isAutoIncrement();

    bool valueChanged = m_editor->valueChanged();
    bool editCurrentCellAgain = false;

    if (valueChanged) {
        if (!m_editor->valueIsValid()) {
            //used e.g. for date or time values - the value can be null but not necessary invalid
            res = Validator::Error;
            editCurrentCellAgain = true;
            QWidget *par = dynamic_cast<QScrollArea*>(this) ? dynamic_cast<QScrollArea*>(this)->widget() :
                           dynamic_cast<QWidget*>(this);
            QWidget *edit = dynamic_cast<QWidget*>(m_editor);
            if (par && edit) {
//! @todo allow displaying user-defined warning
//! @todo also use for other error messages
                if (!m_errorMessagePopup) {
//     m_errorMessagePopup->close();
                    m_errorMessagePopup = new KexiArrowTip(
                        i18nc("Question", "Error: %1?", m_editor->columnInfo()->field->typeName()),
                        dynamic_cast<QWidget*>(this));
                    m_errorMessagePopup->move(
                        par->mapToGlobal(edit->pos()) + QPoint(6, edit->height() + 0));
                    m_errorMessagePopup->show();
                }
                m_editor->setFocus();
            }
        } else if (m_editor->valueIsNull()) {//null value entered
//   if (m_editor->columnInfo()->field->isNotNull() && !autoIncColumnCanBeOmitted) {
            if (m_editor->field()->isNotNull() && !autoIncColumnCanBeOmitted) {
                kDebug() << "NULL NOT ALLOWED!";
                res = Validator::Error;
//    msg = Validator::msgColumnNotEmpty().arg(m_editor->columnInfo()->field->captionOrName())
                msg = Validator::msgColumnNotEmpty().arg(m_editor->field()->captionOrName())
                      + "\n\n" + Kexi::msgYouCanImproveData();
                desc = i18n("The column's constraint is declared as NOT NULL.");
                editCurrentCellAgain = true;
                //   allow = false;
                //   removeEditor();
                //   return true;
            } else {
                kDebug() << "NULL VALUE WILL BE SET";
                //ok, just leave newval as NULL
                setNull = true;
            }
        } else if (m_editor->valueIsEmpty()) {//empty value entered
//   if (m_editor->columnInfo()->field->hasEmptyProperty()) {
            if (m_editor->field()->hasEmptyProperty()) {
//    if (m_editor->columnInfo()->field->isNotEmpty() && !autoIncColumnCanBeOmitted) {
                if (m_editor->field()->isNotEmpty() && !autoIncColumnCanBeOmitted) {
                    kDebug() << "EMPTY NOT ALLOWED!";
                    res = Validator::Error;
//     msg = Validator::msgColumnNotEmpty().arg(m_editor->columnInfo()->field->captionOrName())
                    msg = Validator::msgColumnNotEmpty().arg(m_editor->field()->captionOrName())
                          + "\n\n" + Kexi::msgYouCanImproveData();
                    desc = i18n("The column's constraint is declared as NOT EMPTY.");
                    editCurrentCellAgain = true;
                    //    allow = false;
                    //    removeEditor();
                    //    return true;
                } else {
                    kDebug() << "EMPTY VALUE WILL BE SET";
                }
            } else {
//    if (m_editor->columnInfo()->field->isNotNull() && !autoIncColumnCanBeOmitted) {
                if (m_editor->field()->isNotNull() && !autoIncColumnCanBeOmitted) {
                    kDebug() << "NEITHER NULL NOR EMPTY VALUE CAN BE SET!";
                    res = Validator::Error;
//     msg = Validator::msgColumnNotEmpty().arg(m_editor->columnInfo()->field->captionOrName())
                    msg = Validator::msgColumnNotEmpty().arg(m_editor->field()->captionOrName())
                          + "\n\n" + Kexi::msgYouCanImproveData();
                    desc = i18n("The column's constraint is declared as NOT EMPTY and NOT NULL.");
                    editCurrentCellAgain = true;
//    allow = false;
                    //    removeEditor();
                    //    return true;
                } else {
                    kDebug() << "NULL VALUE WILL BE SET BECAUSE EMPTY IS NOT ALLOWED";
                    //ok, just leave newval as NULL
                    setNull = true;
                }
            }
        }
    }//changed

    const int realFieldNumber = fieldNumberForColumn(m_curCol);
    if (realFieldNumber < 0) {
        kWarning() << "fieldNumberForColumn(m_curCol) < 0";
        return false;
    }

    KexiTableViewColumn *currentTVColumn = column(m_curCol);

    //try to get the value entered:
    if (res == Validator::Ok) {
        if (   (!setNull && !valueChanged)
            || (m_editor->field()->type() != KexiDB::Field::Boolean && setNull && m_currentItem->at(realFieldNumber).isNull()))
        {
            kDebug() << "VALUE NOT CHANGED.";
            removeEditor();
            if (m_acceptsRowEditAfterCellAccepting || m_internal_acceptsRowEditAfterCellAccepting)
                acceptRowEdit();
            return true;
        }
        if (!setNull) {//get the new value
//   bool ok;
            newval = m_editor->value();
//! @todo validation rules for this value?
            /*
                  if (!ok) {
                    kDebug() << "INVALID VALUE - NOT CHANGED.";
                    res = KexiValidator::Error;
            //js: TODO get detailed info on why m_editor->value() failed
                    msg = i18n("Entered value is invalid.")
                      + "\n\n" + KexiValidator::msgYouCanImproveData();
                    editCurrentCellAgain = true;
            //    removeEditor();
            //    return true;
                  }*/
        }

        //Check other validation rules:
        //1. check using validator
//  KexiValidator *validator = m_data->column(m_curCol)->validator();
        Validator *validator = currentTVColumn->validator();
        if (validator) {
//   res = validator->check(m_data->column(m_curCol)->field()->captionOrName(),
            res = validator->check(currentTVColumn->field()->captionOrName(),
                                   newval, msg, desc);
        }
    }

    //show the validation result if not OK:
    if (res == Validator::Error) {
        if (!msg.isEmpty()) {
            if (desc.isEmpty())
                KMessageBox::sorry(dynamic_cast<QWidget*>(this), msg);
            else
                KMessageBox::detailedSorry(dynamic_cast<QWidget*>(this), msg, desc);
        }
        editCurrentCellAgain = true;
//  allow = false;
    } else if (res == Validator::Warning) {
        //js: todo: message!!!
        KMessageBox::messageBox(dynamic_cast<QWidget*>(this), KMessageBox::Sorry, msg + "\n" + desc);
        editCurrentCellAgain = true;
    }

    if (res == Validator::Ok) {
        //2. check using signal
        //bool allow = true;
//  emit aboutToChangeCell(d->pCurrentItem, newval, allow);
//  if (allow) {
        //send changes to the backend
        QVariant visibleValue;
        if (   !newval.isNull()/* visible value should be null if value is null */
            && currentTVColumn->visibleLookupColumnInfo())
        {
            visibleValue = m_editor->visibleValue(); //visible value for lookup field
        }
        //should be also added to the buffer
        if (m_data->updateRowEditBufferRef(m_currentItem, m_curCol, currentTVColumn,
                                           newval, /*allowSignals*/true, 
                                           currentTVColumn->visibleLookupColumnInfo() ? &visibleValue : 0))
        {
            kDebug() << "------ EDIT BUFFER CHANGED TO:";
            m_data->rowEditBuffer()->debug();
        } else {
            kDebug() << "------ CHANGE FAILED in KexiDataAwareObjectInterface::updateRowEditBuffer()";
            res = Validator::Error;

            //now: there might be called cancelEditor() in updateRowEditBuffer() handler,
            //if this is true, d->pEditor is NULL.

            if (m_editor && m_data->result().column >= 0 && m_data->result().column < columns()) {
                //move to faulty column (if m_editor is not cleared)
                setCursorPosition(m_curRow, m_data->result().column);
            }
            if (!m_data->result().msg.isEmpty()) {
                const int button = showErrorMessageForResult(m_data->result());
                if (KMessageBox::No == button) {
                    //discard changes
                    cancelEditor();
                    if (m_acceptsRowEditAfterCellAccepting)
                        cancelRowEdit();
                    return false;
                }
            }
        }
    }

    if (res == Validator::Ok) {
        removeEditor();
        /*emit*/ itemChanged(m_currentItem, m_curRow, m_curCol,
                             m_currentItem->at(realFieldNumber));
        /*emit*/ itemChanged(m_currentItem, m_curRow, m_curCol);
    }
    if (res == Validator::Ok) {
        if (m_acceptsRowEditAfterCellAccepting || m_internal_acceptsRowEditAfterCellAccepting)
            acceptRowEdit();
        return true;
    }
    if (m_editor) {
        //allow to edit the cell again, (if m_pEditor is not cleared)

        if (m_editor->hasFocusableWidget()) {
            m_editor->showWidget();
            m_editor->setFocus();
        }
//  startEditCurrentCell(newval.type()==QVariant::String ? newval.toString() : QString());
//  m_editor->setFocus();
    }
    return false;
}

void KexiDataAwareObjectInterface::startEditCurrentCell(const QString &setText)
{
    kDebug() << "setText:" << setText;
// if (columnType(d->curCol) == KexiDB::Field::Boolean)
//  return;
    if (isReadOnly() || !columnEditable(m_curCol))
        return;
    if (m_editor) {
        if (m_editor->hasFocusableWidget()) {
            m_editor->showWidget();
            m_editor->setFocus();
        }
    }
// ensureVisible(columnPos(m_curCol), rowPos(m_curRow)+rowHeight(),
//  columnWidth(m_curCol), rowHeight());
//OK?
    //ensureCellVisible(m_curRow+1, m_curCol);
    if (!m_editor)
        createEditor(m_curRow, m_curCol, setText, !setText.isEmpty());
}

void KexiDataAwareObjectInterface::deleteAndStartEditCurrentCell()
{
    if (isReadOnly() || !columnEditable(m_curCol))
        return;
    if (m_editor) {//if we've editor - just clear it
        m_editor->clear();
        return;
    }
//js if (columnType(m_curCol) == KexiDB::Field::Boolean)
//js  return;
// ensureVisible(columnPos(m_curCol), rowPos(m_curRow) + rowHeight(),
//  columnWidth(m_curCol), rowHeight());
//OK?
    ensureCellVisible(m_curRow + 1, m_curCol);
    createEditor(m_curRow, m_curCol, QString(), false/*removeOld*/);
    if (!m_editor)
        return;
    m_editor->clear();
    if (m_editor->acceptEditorAfterDeleteContents())
        acceptEditor();
    if (!m_editor || !m_editor->hasFocusableWidget())
        updateCell(m_curRow, m_curCol);
}

void KexiDataAwareObjectInterface::deleteCurrentRow()
{
    if (m_newRowEditing) {//we're editing fresh new row: just cancel this!
        cancelRowEdit();
        return;
    }

    if (!acceptRowEdit())
        return;

    if (!isDeleteEnabled() || !m_currentItem || m_currentItem == m_insertItem) {
        return;
    }
    switch (m_deletionPolicy) {
    case NoDelete:
        return;
    case ImmediateDelete:
        break;
    case AskDelete:
        if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(
                    dynamic_cast<QWidget*>(this),
                    i18n("Do you want to delete selected row?"), QString(),
                    KGuiItem(i18n("&Delete Row"), "edit-delete"), KStandardGuiItem::cancel(),
                    "dontAskBeforeDeleteRow"/*config entry*/,
                    KMessageBox::Notify | KMessageBox::Dangerous))
        {
            return;
        }
        break;
    case SignalDelete:
        /*emit*/ itemDeleteRequest(m_currentItem, m_curRow, m_curCol);
        /*emit*/ currentItemDeleteRequest();
        return;
    default:
        return;
    }

    if (!deleteItem(m_currentItem)) {//nothing
    }
}

KexiDB::RecordData* KexiDataAwareObjectInterface::insertEmptyRow(int pos)
{
    if (!acceptRowEdit() || !isEmptyRowInsertingEnabled()
            || (pos != -1 && pos >= ((int)rows() + (isInsertingEnabled() ? 1 : 0))))
        return 0;

    KexiDB::RecordData *newRecord = m_data->createItem();
    insertItem(newRecord, pos);
    return newRecord;
}

void KexiDataAwareObjectInterface::insertItem(KexiDB::RecordData *newRecord, int pos)
{
    const bool changeCurrentRecord = pos == -1 || pos == m_curRow;
    if (changeCurrentRecord) {
        //change current record
        pos = (m_curRow >= 0 ? m_curRow : 0);
        m_currentItem = newRecord;
        m_curRow = pos;
    } else if (m_curRow >= pos) {
        m_curRow++;
    }

    m_data->insertRow(*newRecord, pos, true /*repaint*/);

    if (changeCurrentRecord) {
        //update iter...
        m_itemIterator = m_data->constBegin();
        m_itemIterator += m_curRow;
    }
    /*
      QSize s(tableSize());
      resizeContents(s.width(),s.height());

      //redraw only this row and below:
      int leftcol = d->pTopHeader->sectionAt( d->pTopHeader->offset() );
    // updateContents( columnPos( leftcol ), rowPos(d->curRow),
    //  clipper()->width(), clipper()->height() - (rowPos(d->curRow) - contentsY()) );
      updateContents( columnPos( leftcol ), rowPos(row),
        clipper()->width(), clipper()->height() - (rowPos(row) - contentsY()) );

      m_verticalHeader->addLabel();

      //update navigator's data
      setNavRowCount(rows());

      if (d->curRow >= row) {
        //update
        editorShowFocus( d->curRow, d->curCol );
      }
      */
}

void KexiDataAwareObjectInterface::slotRowInserted(KexiDB::RecordData* record, bool repaint)
{
    slotRowInserted(record, m_data->indexOf(record), repaint);
}

void KexiDataAwareObjectInterface::slotRowInserted(KexiDB::RecordData * /*record*/, uint pos, bool repaint)
{
    if (repaint && (int)pos < rows()) {
        updateWidgetContentsSize();

        /* updateAllVisibleRowsBelow() used instead
            //redraw only this row and below:
            int leftcol = d->pTopHeader->sectionAt( d->pTopHeader->offset() );
            updateContents( columnPos( leftcol ), rowPos(row),
              clipper()->width(), clipper()->height() - (rowPos(row) - contentsY()) );
        */
        updateAllVisibleRowsBelow(pos);

        if (!m_verticalHeaderAlreadyAdded) {
            if (m_verticalHeader)
                m_verticalHeader->addLabel();
        } else //it was added because this inserting was interactive
            m_verticalHeaderAlreadyAdded = false;

        //update navigator's data
        if (m_navPanel)
            m_navPanel->setRecordCount(rows());

        if (m_curRow >= (int)pos) {
            //update
            editorShowFocus(m_curRow, m_curCol);
        }
    }
}

tristate KexiDataAwareObjectInterface::deleteAllRows(bool ask, bool repaint)
{
    if (!hasData())
        return true;
    if (m_data->count() < 1)
        return true;

    if (ask) {
        QString tableName = m_data->dbTableName();
        if (!tableName.isEmpty()) {
            tableName.prepend(" \"");
            tableName.append("\"");
        }
        if (KMessageBox::Cancel == KMessageBox::warningContinueCancel(dynamic_cast<QWidget*>(this),
                i18n("Do you want to clear the contents of table %1?", tableName),
                0, KGuiItem(i18n("&Clear Contents"))))
        {
            return cancelled;
        }
    }

    cancelRowEdit();
// acceptRowEdit();
// m_verticalHeader->clear();
    const bool repaintLater = repaint && m_spreadSheetMode;
    const int oldRows = rows();

    bool res = m_data->deleteAllRows(repaint && !repaintLater);

    if (res) {
        if (m_spreadSheetMode) {
//   const uint columns = m_data->columns.count();
            for (int i = 0; i < oldRows; i++) {
                m_data->append(m_data->createItem());
            }
        }
    }
    if (repaintLater)
        m_data->reload();

// d->clearVariables();
// m_verticalHeader->setCurrentRow(-1);

// d->pUpdateTimer->start(1,true);
// if (repaint)
//  viewport()->repaint();
    return res;
}

void KexiDataAwareObjectInterface::clearColumns(bool repaint)
{
    cancelRowEdit();
    m_data->clearInternal();

    clearColumnsInternal(repaint);
    updateIndicesForVisibleValues();

    if (repaint)
//  viewport()->repaint();
//OK?
        updateWidgetContents();

    /* for(int i=0; i < rows(); i++)
      {
        m_verticalHeader->removeLabel();
      }

      editorCancel();
      m_contents->clear();

      d->clearVariables();
      d->numCols = 0;

      while(d->pTopHeader->count()>0)
        d->pTopHeader->removeLabel(0);

      m_verticalHeader->setCurrentRow(-1);

      viewport()->repaint();

    // d->pColumnTypes.resize(0);
    // d->pColumnModes.resize(0);
    // d->pColumnDefaults.clear();*/
}

void KexiDataAwareObjectInterface::reloadData()
{
// cancelRowEdit();
    acceptRowEdit();
    if (m_verticalHeader)
        m_verticalHeader->clear();

    if (m_curCol >= 0 && m_curCol < columns()) {
        //find the editor for this column
        KexiDataItemInterface *edit = editor(m_curCol);
        if (edit) {
            edit->hideFocus();
        }
    }
// setCursorPosition(-1, -1, true);
    clearVariables();
    if (m_verticalHeader)
        m_verticalHeader->setCurrentRow(-1);

    if (dynamic_cast<QWidget*>(this) && dynamic_cast<QWidget*>(this)->isVisible())
        initDataContents();
    else
        m_initDataContentsOnShow = true;

    if (m_verticalHeader)
        m_verticalHeader->addLabels(m_data->count());

    updateWidgetScrollBars();
}

int KexiDataAwareObjectInterface::columnType(int col)
{
    KexiTableViewColumn* c = m_data ? column(col) : 0;
    return c ? c->field()->type() : KexiDB::Field::InvalidType;
}

bool KexiDataAwareObjectInterface::columnEditable(int col)
{
    KexiTableViewColumn* c = m_data ? column(col) : 0;
    return c ? (! c->isReadOnly()) : false;
}

int KexiDataAwareObjectInterface::rows() const
{
    if (!hasData())
        return 0;
    return m_data->count();
}

int KexiDataAwareObjectInterface::dataColumns() const
{
    if (!hasData())
        return 0;
    return m_data->columns().count();
}

QVariant KexiDataAwareObjectInterface::columnDefaultValue(int /*col*/) const
{
    return QVariant(0);
//TODO(js)
// return m_data->columns[col].defaultValue;
}

void KexiDataAwareObjectInterface::setAcceptsRowEditAfterCellAccepting(bool set)
{
    m_acceptsRowEditAfterCellAccepting = set;
}

void KexiDataAwareObjectInterface::setDropsAtRowEnabled(bool set)
{
// const bool old = d->dropsAtRowEnabled;
    if (!set)
        m_dragIndicatorLine = -1;
    if (m_dropsAtRowEnabled && !set) {
        m_dropsAtRowEnabled = false;
//  update();
        updateWidgetContents();
    } else {
        m_dropsAtRowEnabled = set;
    }
}

void KexiDataAwareObjectInterface::setEmptyRowInsertingEnabled(bool set)
{
    m_emptyRowInsertingEnabled = set;
    /*emit*/ reloadActions();
}

void KexiDataAwareObjectInterface::slotAboutToDeleteRow(KexiDB::RecordData& record,
        KexiDB::ResultInfo* /*result*/, bool repaint)
{
    if (repaint) {
        m_rowWillBeDeleted = m_data->indexOf(&record);
    }
}

void KexiDataAwareObjectInterface::slotRowDeleted()
{
    if (m_rowWillBeDeleted >= 0) {
        if (m_rowWillBeDeleted > 0 && m_rowWillBeDeleted >= (rows() - 1) && !m_spreadSheetMode)
            m_rowWillBeDeleted = rows() - 1; //move up if it's the last row
        updateWidgetContentsSize();

        if (!(m_spreadSheetMode && m_rowWillBeDeleted >= (rows() - 1)))
            setCursorPosition(m_rowWillBeDeleted, m_curCol, true/*forceSet*/);
        if (m_verticalHeader)
            m_verticalHeader->removeLabel();

        updateAllVisibleRowsBelow(m_curRow); //needed for KexiTableView

        //update navigator's data
        if (m_navPanel)
            m_navPanel->setRecordCount(rows());

        m_rowWillBeDeleted = -1;
    }
}

bool KexiDataAwareObjectInterface::beforeDeleteItem(KexiDB::RecordData*)
{
    //always return
    return true;
}

bool KexiDataAwareObjectInterface::deleteItem(KexiDB::RecordData* record)
{
    if (!record || !beforeDeleteItem(record))
        return false;

    QString msg, desc;
// bool current = (record == d->pCurrentItem);
    //we need to know this so we
    //can return to the last row
    //after reinserting it
    const bool lastRowDeleted = m_spreadSheetMode && m_data->last() == record; 
    if (!m_data->deleteRow(*record, true /*repaint*/)) {
        /*const int button =*/
        showErrorMessageForResult(m_data->result());
//  if (KMessageBox::No == button) {
        //discard changes
        // }
        return false;
    } else {
//setCursorPosition() wil lset this!  if (current)
        //d->pCurrentItem = m_data->current();
    }

// repaintAfterDelete();
    if (m_spreadSheetMode) { //append empty row for spreadsheet mode
        m_data->append(m_data->createItem());
        if (m_verticalHeader)
            m_verticalHeader->addLabels(1);
        if (lastRowDeleted) //back to the last row
            setCursorPosition(rows() - 1, m_curCol, true/*forceSet*/);
        /*emit*/ newItemAppendedForAfterDeletingInSpreadSheetMode();
    }
    return true;
}

KexiTableViewColumn* KexiDataAwareObjectInterface::column(int col)
{
    return m_data->column(col);
}

bool KexiDataAwareObjectInterface::hasDefaultValueAt(const KexiTableViewColumn& tvcol)
{
    if (m_rowEditing && m_data->rowEditBuffer() && m_data->rowEditBuffer()->isDBAware()) {
        return m_data->rowEditBuffer()->hasDefaultValueAt(*tvcol.columnInfo());
    }
    return false;
}

const QVariant* KexiDataAwareObjectInterface::bufferedValueAt(int col, bool useDefaultValueIfPossible)
{
    if (m_rowEditing && m_data->rowEditBuffer()) {
        KexiTableViewColumn* tvcol = column(col);
        if (tvcol->isDBAware()) {
            //get the stored value
            const int realFieldNumber = fieldNumberForColumn(col);
            if (realFieldNumber < 0) {
                kWarning() << "fieldNumberForColumn(m_curCol) < 0";
                return 0;
            }
            const QVariant *storedValue = &m_currentItem->at(realFieldNumber);

            //db-aware data: now, try to find a buffered value (or default one)
            const QVariant *cv = m_data->rowEditBuffer()->at(*tvcol->columnInfo(),
                                 storedValue->isNull() && useDefaultValueIfPossible);
            if (cv)
                return cv;
            return storedValue;
        }
        //not db-aware data:
        const QVariant *cv = m_data->rowEditBuffer()->at(tvcol->field()->name());
        if (cv)
            return cv;
    }
    //not db-aware data:
    const int realFieldNumber = fieldNumberForColumn(col);
    if (realFieldNumber < 0) {
        kWarning() << "fieldNumberForColumn(m_curCol) < 0";
        return 0;
    }
    return &m_currentItem->at(realFieldNumber);
}

void KexiDataAwareObjectInterface::startEditOrToggleValue()
{
    if (!isReadOnly() && columnEditable(m_curCol)) {
        if (columnType(m_curCol) == KexiDB::Field::Boolean) {
            boolToggled();
        } else {
            startEditCurrentCell();
            return;
        }
    }
}

void KexiDataAwareObjectInterface::boolToggled()
{
    startEditCurrentCell();
    if (m_editor) {
        m_editor->clickedOnContents();
    }
    acceptEditor();
    updateCell(m_curRow, m_curCol);

    /* int s = m_currentItem->at(m_curCol).toInt();
      QVariant oldValue=m_currentItem->at(m_curCol);
      (*m_currentItem)[m_curCol] = QVariant(s ? 0 : 1);
      updateCell(m_curRow, m_curCol);
    // emit itemChanged(m_currentItem, m_curRow, m_curCol, oldValue);
    // emit itemChanged(m_currentItem, m_curRow, m_curCol);*/
}

void KexiDataAwareObjectInterface::slotDataDestroying()
{
    m_data = 0;
    m_itemIterator = KexiTableViewData::Iterator();
}

void KexiDataAwareObjectInterface::addNewRecordRequested()
{
    if (!isInsertingEnabled())
        return;
    if (m_rowEditing) {
        if (!acceptRowEdit())
            return;
    }
// setFocus();
    selectRow(rows());
    startEditCurrentCell();
    if (m_editor)
        m_editor->setFocus();
}

bool KexiDataAwareObjectInterface::handleKeyPress(QKeyEvent *e, int &curRow, int &curCol,
        bool fullRowSelection, bool *moveToFirstField, bool *moveToLastField)
{
    if (moveToFirstField)
        *moveToFirstField = false;
    if (moveToLastField)
        *moveToLastField = false;

    const bool nobtn = e->modifiers() == Qt::NoModifier;
    const int k = e->key();

    if (   (k == Qt::Key_Up && nobtn)
        || (k == Qt::Key_PageUp && e->modifiers() == Qt::ControlModifier))
    {
        selectPrevRow();
        e->accept();
    }
    else if (   (k == Qt::Key_Down && nobtn)
               || (k == Qt::Key_PageDown && e->modifiers() == Qt::ControlModifier))
    {
        selectNextRow();
        e->accept();
    } else if (k == Qt::Key_PageUp && nobtn) {
        selectPrevPage();
        e->accept();
    } else if (k == Qt::Key_PageDown && nobtn) {
        selectNextPage();
        e->accept();
    } else if (k == Qt::Key_Home) {
        if (fullRowSelection) {
            //we're in row-selection mode: home key always moves to 1st row
            curRow = 0;//to 1st row
        } else {//cell selection mode: different actions depending on ctrl and shift keys mods
            if (nobtn) {
                curCol = 0;//to 1st col
            } else if (e->modifiers() == Qt::ControlModifier) {
                curRow = 0;//to 1st row and col
                curCol = 0;
            } else
                return false;
        }
        if (moveToFirstField)
            *moveToFirstField = true;
        //do not accept yet
        e->ignore();
    } else if (k == Qt::Key_End) {
        if (fullRowSelection) {
            //we're in row-selection mode: home key always moves to last row
            curRow = m_data->count() - 1 + (isInsertingEnabled() ? 1 : 0);//to last row
        } else {//cell selection mode: different actions depending on ctrl and shift keys mods
            if (nobtn) {
                curCol = columns() - 1;//to last col
            } else if (e->modifiers() == Qt::ControlModifier) {
                curRow = m_data->count() - 1 /*+(isInsertingEnabled()?1:0)*/; //to last row and col
                curCol = columns() - 1;//to last col
            } else
                return false;
        }
        if (moveToLastField)
            *moveToLastField = true;
        //do not accept yet
        e->ignore();
    }
    else if (isInsertingEnabled()
               && (   (e->modifiers() == Qt::ControlModifier && k == Qt::Key_Equal)
                   || (e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && k == Qt::Key_Equal)
                  )
            )
    {
        curRow = m_data->count(); //to the new row
        curCol = 0;//to first col
        if (moveToFirstField)
            *moveToFirstField = true;
        //do not accept yet
        e->ignore();
    } else {
        return false;
    }

    return true;
}

void KexiDataAwareObjectInterface::vScrollBarValueChanged(int v)
{
    Q_UNUSED(v);
    if (!m_vScrollBarValueChanged_enabled)
        return;

    if (m_scrollbarToolTipsEnabled && verticalScrollBar()->isSliderDown()) {
        QWidget* thisWidget = dynamic_cast<QWidget*>(this);
//        const QRect r( verticalScrollBar()->sliderRect() );
        const int row = lastVisibleRow() + 1;
        const QString toolTipText( i18n("Row: %1", row) );
        QToolTip::showText(
            QPoint(
                thisWidget->mapToGlobal(verticalScrollBar()->pos()).x()
                    - thisWidget->fontMetrics().width(toolTipText+"    "),
                QCursor::pos().y() ),
//                +QPoint(0,verticalScrollBar()->height() * verticalScrollBar()->value()/verticalScrollBar()->maximum()),
//                +QPoint(0,verticalScrollBar()->sliderPosition()), 
//                + QPoint(- m_scrollBarTip->width() - 5,
//                 r.height() / 2 - m_scrollBarTip->height() / 2),
            toolTipText,
            0, 
            QRect()
        );
//  const QRect r( verticalScrollBar()->sliderRect() );
/*replaced by QToolTip
        QStyleOptionComplex styleOption;
        styleOption.initFrom(verticalScrollBar());
#ifdef __GNUC__
#warning ported but not tested KexiDataAwareObjectInterface::vScrollBarValueChanged()
#else
#pragma WARNING( ported but not tested KexiDataAwareObjectInterface::vScrollBarValueChanged() )
#endif
        QRect r;
        //TODO  const QRect r( verticalScrollBar()->style()->subControlRect(
        //      QStyle::CC_ScrollBar, &styleOption,
        //      QStyle::SC_ScrollBarSlider, verticalScrollBar()) ); 

        const int row = lastVisibleRow() + 1;
        if (row <= 0) {
            m_scrollBarTipTimer.stop();
            m_scrollBarTip->hide();
            return;
        }
        m_scrollBarTip->setText(i18n("Row: ") + QString::number(row));
        m_scrollBarTip->adjustSize();
        QWidget* thisWidget = dynamic_cast<QWidget*>(this);
        m_scrollBarTip->move(
            thisWidget->mapToGlobal(r.topLeft() + verticalScrollBar()->pos())
            + QPoint(- m_scrollBarTip->width() - 5,
                     r.height() / 2 - m_scrollBarTip->height() / 2));
        if (verticalScrollBar()->isSliderDown()) {
            kDebug() << "  isSliderDown()  ";
            m_scrollBarTipTimer.stop();
            //m_scrollBarTip->show();
            //m_scrollBarTip->raise();
        } else {
            m_scrollBarTipTimerCnt++;
            if (m_scrollBarTipTimerCnt > 4) {
                m_scrollBarTipTimerCnt = 0;
                m_scrollBarTip->show();
                m_scrollBarTip->raise();
                m_scrollBarTipTimer.start(500);
            }
        }*/
    }
    //update bottom view region
    /* if (m_navPanel && (contentsHeight() - contentsY() - clipper()->height()) <= qMax(d->rowHeight,m_navPanel->height())) {
        slotUpdate();
        triggerUpdate();
      }*/
}

void KexiDataAwareObjectInterface::setContextMenuTitle(const QIcon &icon, const QString &text)
{
    m_contextMenuTitleIcon = icon;
    m_contextMenuTitleText = text;
    /*emit*/ reloadActions();
}

bool KexiDataAwareObjectInterface::scrollbarToolTipsEnabled() const
{
    return m_scrollbarToolTipsEnabled;
}

void KexiDataAwareObjectInterface::setScrollbarToolTipsEnabled(bool set)
{
    m_scrollbarToolTipsEnabled = set;
}

#if 0
void KexiDataAwareObjectInterface::vScrollBarSliderReleased()
{
    kDebug() << "vScrollBarSliderReleased()";
/*replaced by QToolTip    m_scrollBarTip->hide();*/
}
#endif

/*replaced by QToolTip
void KexiDataAwareObjectInterface::scrollBarTipTimeout()
{
    if (m_scrollBarTip->isVisible()) {
//  kDebug() << "TIMEOUT! - hide";
        if (m_scrollBarTipTimerCnt > 0) {
            m_scrollBarTipTimerCnt = 0;
            m_scrollBarTipTimer.start(500);
            return;
        }
        m_scrollBarTip->hide();
    }
    m_scrollBarTipTimerCnt = 0;
}*/

void KexiDataAwareObjectInterface::focusOutEvent(QFocusEvent* e)
{
    Q_UNUSED(e);
/*replaced by QToolTip    m_scrollBarTipTimer.stop();
    m_scrollBarTip->hide();*/

    updateCell(m_curRow, m_curCol);
}

int KexiDataAwareObjectInterface::showErrorMessageForResult(const KexiDB::ResultInfo& resultInfo)
{
    QWidget *thisWidget = dynamic_cast<QWidget*>(this);
    if (resultInfo.allowToDiscardChanges) {
        return KMessageBox::questionYesNo(thisWidget, resultInfo.msg
                                          + (resultInfo.desc.isEmpty() ? QString() : ("\n" + resultInfo.desc)),
                                          QString(),
                                          KGuiItem(i18nc("Correct Changes", "Correct"),
                                                   QString(),
                                                   i18n("Correct changes")),
                                          KGuiItem(i18n("Discard Changes")));
    }

    if (resultInfo.desc.isEmpty())
        KMessageBox::sorry(thisWidget, resultInfo.msg);
    else
        KMessageBox::detailedSorry(thisWidget, resultInfo.msg, resultInfo.desc);

    return KMessageBox::Ok;
}

void KexiDataAwareObjectInterface::updateIndicesForVisibleValues()
{
    m_indicesForVisibleValues.resize(m_data ? m_data->columnsCount() : 0);
    if (!m_data)
        return;
    for (uint i = 0; i < m_data->columnsCount(); i++) {
        KexiTableViewColumn* tvCol = m_data->column(i);
        if (tvCol->columnInfo() && tvCol->columnInfo()->indexForVisibleLookupValue() != -1)
            // retrieve visible value from lookup field
            m_indicesForVisibleValues[ i ] = tvCol->columnInfo()->indexForVisibleLookupValue();
        else
            m_indicesForVisibleValues[ i ] = i;
    }
}

/*! Performs searching \a stringValue in \a where string.
 \a matchAnyPartOfField, \a matchWholeField, \a wholeWordsOnly options are used to control how to search.

 If \a matchWholeField is true, \a wholeWordsOnly is not checked.
 \a firstCharacter is in/out parameter. If \a matchAnyPartOfField is true and \a matchWholeField is false,
 \a firstCharacter >= 0, the search will be performed after skipping first \a firstCharacter characters.

 If \a forward is false, we are searching backward from \a firstCharacter position. \a firstCharacter == -1
 means then the last character. \a firstCharacter == INT_MAX means "before first" place, so searching fails
 immediately.
 On success, true is returned and \a firstCharacter is set to position of the matched string. */
static inline bool findInString(const QString& stringValue, int stringLength,
                                const QString& where,
                                int& firstCharacter, bool matchAnyPartOfField, bool matchWholeField,
                                Qt::CaseSensitivity caseSensitivity, bool wholeWordsOnly, bool forward)
{
    if (where.isEmpty()) {
        firstCharacter = -1;
        return false;
    }

    if (matchAnyPartOfField) {
        if (forward) {
            int pos = firstCharacter == -1 ? 0 : firstCharacter;
            if (wholeWordsOnly) {
                const int whereLength = where.length();
                while (true) {
                    pos = where.indexOf(stringValue, pos, caseSensitivity);
                    if (pos == -1)
                        break;
                    if (   (pos > 0 && where.at(pos - 1).isLetterOrNumber())
                        || ((pos + stringLength - 1) < (whereLength - 1) && where.at(pos + stringLength - 1 + 1).isLetterOrNumber()))
                    {
                        pos++; // invalid match because before or after the string there is non-white space
                    } else
                        break;
                }//while
                firstCharacter = pos;
            } else {// !wholeWordsOnly
                firstCharacter = where.indexOf(stringValue, pos, caseSensitivity);
            }
            return firstCharacter != -1;
        } else { // !matchAnyPartOfField
            if (firstCharacter == INT_MAX) {
                firstCharacter = -1; //next time we'll be looking at different cell
                return false;
            }
            int pos = firstCharacter;
            if (wholeWordsOnly) {
                const int whereLength = where.length();
                while (true) {
                    pos = where.lastIndexOf(stringValue, pos, caseSensitivity);
                    if (pos == -1)
                        break;
                    if (   (pos > 0 && where.at(pos - 1).isLetterOrNumber())
                        || ((pos + stringLength - 1) < (whereLength - 1) && where.at(pos + stringLength - 1 + 1).isLetterOrNumber()))
                    {
                        // invalid match because before or after the string there is non-white space
                        pos--;
                        if (pos < 0) // it can make pos < 0
                            break;
                    } else
                        break;
                }//while
                firstCharacter = pos;
            } else {// !wholeWordsOnly
                firstCharacter = where.lastIndexOf(stringValue, pos, caseSensitivity);
            }
            return firstCharacter != -1;
        }
    } else if (matchWholeField) {
        if (firstCharacter != -1 && firstCharacter != 0) { //we're not at 0-th char
            firstCharacter = -1;
        } else if ((caseSensitivity == Qt::CaseSensitive ? where : where.toLower()) == stringValue) {
            firstCharacter = 0;
            return true;
        }
    } else {// matchStartOfField
        if (firstCharacter != -1 && firstCharacter != 0) { //we're not at 0-th char
            firstCharacter = -1;
        } else if (where.startsWith(stringValue, caseSensitivity)) {
            if (wholeWordsOnly) {
                // If where.length() < stringValue.length(), true will be returned too - fine.
                return !where.at(stringValue.length()).isLetterOrNumber();
            }
            firstCharacter = 0;
            return true;
        }
    }
    return false;
}

tristate KexiDataAwareObjectInterface::find(const QVariant& valueToFind,
        const KexiSearchAndReplaceViewInterface::Options& options, bool next)
{
    if (!hasData())
        return cancelled;
    const QVariant prevSearchedValue(m_recentlySearchedValue);
    m_recentlySearchedValue = valueToFind;
    const KexiSearchAndReplaceViewInterface::Options::SearchDirection prevSearchDirection = m_recentSearchDirection;
    m_recentSearchDirection = options.searchDirection;
    if (valueToFind.isNull() || valueToFind.toString().isEmpty())
        return cancelled;

    const bool forward = (options.searchDirection == KexiSearchAndReplaceViewInterface::Options::SearchUp)
                         ? !next : next; //direction can be reversed

    if ((!prevSearchedValue.isNull() && prevSearchedValue != valueToFind)
            || (prevSearchDirection != options.searchDirection && options.searchDirection == KexiSearchAndReplaceViewInterface::Options::SearchAllRows))
    {
        // restart searching when value has been changed or new direction is SearchAllRows
        m_positionOfRecentlyFoundValue.exists = false;
    }

    const bool startFrom1stRowAndCol = !m_positionOfRecentlyFoundValue.exists && next
                                       && options.searchDirection == KexiSearchAndReplaceViewInterface::Options::SearchAllRows;
    const bool startFromLastRowAndCol =
           (   !m_positionOfRecentlyFoundValue.exists && !next
            && options.searchDirection == KexiSearchAndReplaceViewInterface::Options::SearchAllRows)
        || (m_curRow >= rows() && !forward); //we're at "insert" row, and searching backwards: move to the last cell

    if (!startFrom1stRowAndCol && !startFromLastRowAndCol && m_curRow >= rows()) {
        //we're at "insert" row, and searching forward: no chances to find something
        return false;
    }
    KexiTableViewData::Iterator it((startFrom1stRowAndCol || startFromLastRowAndCol)
                                   ? m_data->constBegin() : m_itemIterator /*start from the current cell*/);
    if (startFromLastRowAndCol)
        it += (m_data->columnsCount() - 1);
    int firstCharacter;
    if (m_positionOfRecentlyFoundValue.exists) {// start after the next/prev char position
        if (forward)
            firstCharacter = m_positionOfRecentlyFoundValue.lastCharacter + 1;
        else {
            firstCharacter = (m_positionOfRecentlyFoundValue.firstCharacter > 0) ?
                             (m_positionOfRecentlyFoundValue.firstCharacter - 1) : INT_MAX /* this means 'before first'*/;
        }
    } else {
        firstCharacter = -1; //forward ? -1 : INT_MAX;
    }

    const int columnsCount = m_data->columnsCount();
    int row, col;
    if (startFrom1stRowAndCol) {
        row = 0;
        col = 0;
    } else if (startFromLastRowAndCol) {
        row = rows() - 1;
        col = columnsCount - 1;
    } else {
        row = m_curRow;
        col = m_curCol;
    }

    //sache some flags for efficiency
    const bool matchAnyPartOfField
        = options.textMatching == KexiSearchAndReplaceViewInterface::Options::MatchAnyPartOfField;
    const bool matchWholeField
        = options.textMatching == KexiSearchAndReplaceViewInterface::Options::MatchWholeField;
    const Qt::CaseSensitivity caseSensitivity
        = options.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    const bool wholeWordsOnly = options.wholeWordsOnly;
//unused const bool promptOnReplace = options.promptOnReplace;
    int columnNumber = (options.columnNumber == KexiSearchAndReplaceViewInterface::Options::CurrentColumn)
                       ? m_curCol : options.columnNumber;
    if (columnNumber >= 0)
        col = columnNumber;
    const bool lookInAllColumns = columnNumber == KexiSearchAndReplaceViewInterface::Options::AllColumns;
    int firstColumn; // real number of the first column, can be smaller than lastColumn if forward==true
    int lastColumn; // real number of the last column
    if (lookInAllColumns) {
        firstColumn = forward ? 0 : columnsCount - 1;
        lastColumn = forward ? columnsCount - 1 : 0;
    } else {
        firstColumn = columnNumber;
        lastColumn = columnNumber;
    }
    const QString stringValue(
        caseSensitivity == Qt::CaseSensitive
        ? valueToFind.toString() : valueToFind.toString().toLower());
    const int stringLength = stringValue.length();

    // search
    const int prevRow = m_curRow;
    KexiDB::RecordData *record = 0;
    while ((it != m_data->constEnd() && (record = *it))) {
        for (; forward ? col <= lastColumn : col >= lastColumn;
                col = forward ? (col + 1) : (col - 1)) {
            const QVariant cell(record->at(m_indicesForVisibleValues[ col ]));
            if (findInString(stringValue, stringLength, cell.toString(), firstCharacter,
                             matchAnyPartOfField, matchWholeField, caseSensitivity, 
                             wholeWordsOnly, forward))
            {
                //*m_itemIterator = it;
                //m_currentItem = *it;
                //m_curRow = row;
                //m_curCol = col;
                setCursorPosition(row, col, true/*forceSet*/);
                if (prevRow != m_curRow)
                    updateRow(prevRow);
                // remember the exact position for the found value
                m_positionOfRecentlyFoundValue.exists = true;
                m_positionOfRecentlyFoundValue.firstCharacter = firstCharacter;
//! @todo for regexp lastCharacter should be computed
                m_positionOfRecentlyFoundValue.lastCharacter = firstCharacter + stringLength - 1;
                return true;
            }
        }//for
        if (forward) {
            ++it;
            ++row;
        } else {
            --it;
            --row;
        }
        col = firstColumn;
    }//while
    return false;
}

tristate KexiDataAwareObjectInterface::findNextAndReplace(
    const QVariant& valueToFind, const QVariant& replacement,
    const KexiSearchAndReplaceViewInterface::Options& options, bool replaceAll)
{
    Q_UNUSED(replacement);
    Q_UNUSED(options);
    Q_UNUSED(replaceAll);

    if (isReadOnly())
        return cancelled;
    if (valueToFind.isNull() || valueToFind.toString().isEmpty())
        return cancelled;
    //! @todo implement KexiDataAwareObjectInterface::findAndReplace()
    return false;
}
