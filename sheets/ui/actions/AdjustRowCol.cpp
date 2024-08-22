/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AdjustRowCol.h"
#include "Actions.h"
#include "core/ColFormatStorage.h"
#include "core/Map.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "ui/Selection.h"
#include "ui/actions/dialogs/Resize2Dialog.h"
#include "ui/actions/dialogs/ShowColRowDialog.h"
#include "ui/commands/RowColumnManipulators.h"

#include <KoCanvasBase.h>
#include <KoUnit.h>

#include <KLocalizedString>

#include <float.h>

using namespace Calligra::Sheets;

InsertRemoveRowCol::InsertRemoveRowCol(Actions *actions, bool insert, bool row)
    : CellAction(actions,
                 row ? (insert ? "insertRow" : "deleteRow") : (insert ? "insertColumn" : "deleteColumn"),
                 row ? (insert ? i18n("Insert Rows") : i18n("Remove Rows")) : (insert ? i18n("Insert Columns") : i18n("Remove Columns")),
                 row ? (insert ? koIcon("edit-table-insert-row-above") : koIcon("edit-table-delete-row"))
                     : (insert ? koIcon("edit-table-insert-column-left") : koIcon("edit-table-delete-column")),
                 QString())
    , m_insert(insert)
    , m_row(row)
{
}

InsertRemoveRowCol::~InsertRemoveRowCol() = default;

QAction *InsertRemoveRowCol::createAction()
{
    QAction *res = CellAction::createAction();
    QString tooltip;
    if (m_row) {
        if (m_insert)
            tooltip = i18n("Inserts a new row into the spreadsheet");
        else
            tooltip = i18n("Removes a row from the spreadsheet");
    } else {
        if (m_insert)
            tooltip = i18n("Inserts a new column into the spreadsheet");
        else
            tooltip = i18n("Removes the selected columns from the spreadsheet");
    }
    res->setToolTip(tooltip);
    return res;
}

void InsertRemoveRowCol::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    AbstractRegionCommand *command = nullptr;
    if (m_row) {
        InsertDeleteRowManipulator *cmd = new InsertDeleteRowManipulator();
        if (!m_insert)
            cmd->setDelete(true);
        command = cmd;
    } else {
        InsertDeleteColumnManipulator *cmd = new InsertDeleteColumnManipulator();
        if (!m_insert)
            cmd->setDelete(true);
        command = cmd;
    }

    command->add(*selection);
    command->setSheet(sheet);
    command->execute(selection->canvas());
}

bool InsertRemoveRowCol::enabledForSelection(Selection *selection, const Cell &)
{
    if (m_row && selection->isColumnSelected())
        return false;
    if ((!m_row) && selection->isRowSelected())
        return false;
    return true;
}

ShowHideRowCol::ShowHideRowCol(Actions *actions, bool show, bool row)
    : CellAction(actions,
                 row ? (show ? "showSelRows" : "hideRow") : (show ? "showSelColumns" : "hideColumn"),
                 row ? (show ? i18n("Show Rows") : i18n("Hide Rows")) : (show ? i18n("Show Columns") : i18n("Hide Columns")),
                 row ? (show ? koIcon("show_table_row") : koIcon("hide_table_row")) : (show ? koIcon("show_table_column") : koIcon("hide_table_column")),
                 QString())
    , m_show(show)
    , m_row(row)
{
}

ShowHideRowCol::~ShowHideRowCol() = default;

QAction *ShowHideRowCol::createAction()
{
    QAction *res = CellAction::createAction();
    QString tooltip;
    if (m_row) {
        if (m_show)
            tooltip = i18n("Show hidden rows in the selection");
        else
            tooltip = i18n("Hide the selected rows");
    } else {
        if (m_show)
            tooltip = i18n("Show hidden columns in the selection");
        else
            tooltip = i18n("Hide the selected columns");
    }
    res->setToolTip(tooltip);
    return res;
}

void ShowHideRowCol::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    HideShowManipulator *command = new HideShowManipulator();
    command->setSheet(sheet);
    if (m_row)
        command->setManipulateRows(true);
    else
        command->setManipulateColumns(true);
    command->setHide(!m_show);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool ShowHideRowCol::enabledForSelection(Selection *selection, const Cell &)
{
    if (m_show) {
        Region::ConstIterator endOfList = selection->constEnd();
        for (Region::ConstIterator it = selection->constBegin(); it != endOfList; ++it) {
            QRect range = (*it)->rect();
            int last;

            if (m_row) {
                const RowFormatStorage *rowFormats = selection->activeSheet()->rowFormats();
                int from = range.top();
                int to = range.bottom();
                int max = rowFormats->lastNonDefaultRow();
                if (to > max)
                    to = max;
                for (int row = from; row <= to; ++row) {
                    if (rowFormats->isHidden(row, &last))
                        return true;
                    row = last;
                }
            } else {
                const ColFormatStorage *columnFormats = selection->activeSheet()->columnFormats();
                int from = range.left();
                int to = range.right();
                int max = columnFormats->lastNonDefaultCol();
                if (to > max)
                    to = max;
                for (int col = from; col <= to; ++col) {
                    if (columnFormats->isHidden(col, &last))
                        return true;
                    col = last;
                }
            }
        }
        return false;
    }

    if (m_row && selection->isColumnSelected())
        return false;
    if ((!m_row) && selection->isRowSelected())
        return false;
    return true;
}

EqualizeRowCol::EqualizeRowCol(Actions *actions, bool row)
    : CellAction(actions,
                 row ? "equalizeRow" : "equalizeCol",
                 row ? i18n("Equalize Row") : i18n("Equalize Column"),
                 row ? koIcon("adjustrow") : koIcon("adjustcol"),
                 QString())
    , m_row(row)
{
}

EqualizeRowCol::~EqualizeRowCol() = default;

QAction *EqualizeRowCol::createAction()
{
    QAction *res = CellAction::createAction();
    QString tooltip;
    if (m_row)
        tooltip = i18n("Resizes selected rows to be the same size");
    else
        tooltip = i18n("Resizes selected columns to be the same size");
    res->setToolTip(tooltip);
    return res;
}

void EqualizeRowCol::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect range = selection->lastRange();
    int from = 0, to = 0, last = 0;
    double size = -1;

    if (m_row) {
        const RowFormatStorage *rowFormats = sheet->rowFormats();
        from = range.top();
        to = range.bottom();
        for (int row = from; row <= to; ++row) {
            size = qMax(rowFormats->rowHeight(row, &last), size);
            row = last;
        }

        ResizeRowManipulator *command = new ResizeRowManipulator();
        command->setSize(qMax(2.0, size));
        command->setSheet(sheet);
        command->add(*selection);
        command->execute(selection->canvas());
    } else {
        const ColFormatStorage *columnFormats = sheet->columnFormats();
        from = range.left();
        to = range.right();
        for (int col = from; col <= to; ++col) {
            size = qMax(columnFormats->colWidth(col, &last), size);
            col = last;
        }

        ResizeColumnManipulator *command = new ResizeColumnManipulator();
        command->setSize(qMax(2.0, size));
        command->setSheet(sheet);
        command->add(*selection);
        command->execute(selection->canvas());
    }
}

bool EqualizeRowCol::enabledForSelection(Selection *selection, const Cell &)
{
    if (m_row && selection->isColumnSelected())
        return false;
    if ((!m_row) && selection->isRowSelected())
        return false;
    return true;
}

AdjustRowCol::AdjustRowCol(Actions *actions, bool row, bool col)
    : CellAction(actions,
                 row ? (col ? "adjust" : "adjustRow") : "adjustColumn",
                 row ? (col ? i18n("Adjust Row && Column") : i18n("Adjust Row")) : i18n("Adjust Column"),
                 row ? (col ? QIcon() : koIcon("adjustrow")) : koIcon("adjustcol"),
                 QString())
    , m_row(row)
    , m_col(col)
{
}

AdjustRowCol::~AdjustRowCol() = default;

QAction *AdjustRowCol::createAction()
{
    QAction *res = CellAction::createAction();
    QString tooltip;
    if (m_row) {
        if (m_col)
            tooltip = i18n("Adjusts row/column size so that the contents will fit");
        else
            tooltip = i18n("Adjusts row size so that the contents will fit");
    } else
        tooltip = i18n("Adjusts column size so that the contents will fit");
    res->setToolTip(tooltip);
    return res;
}

void AdjustRowCol::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    AdjustColumnRowManipulator *command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    if (m_col)
        command->setAdjustColumn(true);
    if (m_row)
        command->setAdjustRow(true);
    command->add(*selection);
    command->execute(selection->canvas());
}

ShowRowColQuery::ShowRowColQuery(Actions *actions, bool row)
    : CellAction(actions,
                 row ? "showRow" : "showColumn",
                 row ? i18n("Show Rows...") : i18n("Show Columns..."),
                 row ? koIcon("show_table_row") : koIcon("show_table_column"),
                 row ? i18n("Show hidden rows") : i18n("Show hidden columns"))
    , m_row(row)
    , m_dlg(nullptr)
{
}

ShowRowColQuery::~ShowRowColQuery()
{
    if (m_dlg)
        delete m_dlg;
}

void ShowRowColQuery::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_dlg = new ShowColRowDialog(canvasWidget, m_row);

    QVector<int> indexes;
    QVector<QString> captions;

    bool showColNumber = sheet->getShowColumnNumber();
    QString text;
    if (!m_row) {
        int lastCol, col = 1;
        while (col <= KS_colMax) {
            if (sheet->columnFormats()->isHidden(col, &lastCol)) {
                for (int i = col; i <= lastCol; ++i)
                    indexes.append(i);
            }
            col = lastCol + 1;
        }

        for (int idx : indexes) {
            if (!showColNumber)
                captions.append(i18n("Column: %1", CellBase::columnName(idx)));
            else
                captions.append(i18n("Column: %1", text.setNum(idx)));
        }
    } else {
        int lastRow, row = 1;
        while (row <= KS_rowMax) {
            if (sheet->rowFormats()->isHidden(row, &lastRow)) {
                for (int i = row; i <= lastRow; ++i)
                    indexes.append(i);
            }
            row = lastRow + 1;
        }
        for (int idx : indexes)
            captions.append(i18n("Row: %1", text.setNum(idx)));
    }

    m_dlg->setList(captions);

    if (m_dlg->exec()) {
        QVector<int> selected = m_dlg->selectedIndexes();
        Region region;
        for (int idx : selected) {
            int rowcol = indexes[idx];
            if (m_row)
                region.add(QRect(1, rowcol, KS_colMax, 1));
            else
                region.add(QRect(rowcol, 1, 1, KS_rowMax));
        }

        HideShowManipulator *manipulator = new HideShowManipulator();
        manipulator->setSheet(sheet);
        if (m_row)
            manipulator->setManipulateRows(true);
        else
            manipulator->setManipulateColumns(true);
        manipulator->setHide(false);
        manipulator->add(region);
        manipulator->execute(selection->canvas());
    }
    delete m_dlg;
    m_dlg = nullptr;
}

ResizeRowCol::ResizeRowCol(Actions *actions, bool row)
    : CellAction(actions,
                 row ? "resizeRow" : "resizeCol",
                 row ? i18n("Resize Row...") : i18n("Resize Column..."),
                 row ? koIcon("resizerow") : koIcon("resizecol"),
                 row ? i18n("Change the height of a row") : i18n("Change the width of a column"))
    , m_row(row)
    , m_dlgCol(nullptr)
    , m_dlgRow(nullptr)
{
}

ResizeRowCol::~ResizeRowCol()
{
    if (m_dlgCol)
        delete m_dlgCol;
    if (m_dlgRow)
        delete m_dlgRow;
}

void ResizeRowCol::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    KoUnit unit = selection->canvas()->unit();

    if (m_row) {
        int row = selection->lastRange().top();
        RowFormatStorage *rf = sheet->rowFormats();
        double rowHeight = rf->rowHeight(row);
        double defaultHeight = sheet->fullMap()->defaultRowFormat().height;

        m_dlgRow = new ResizeRowDialog(canvasWidget, unit);
        m_dlgRow->setRowHeight(rowHeight);
        m_dlgRow->setDefault(defaultHeight);

        if (m_dlgRow->exec()) {
            double newHeight = m_dlgRow->rowHeight();
            // Don't generate a resize, when there isn't a change or the change is only a rounding issue
            if (fabs(newHeight - rowHeight) > DBL_EPSILON) {
                ResizeRowManipulator *manipulator = new ResizeRowManipulator();
                manipulator->setSheet(sheet);
                manipulator->setSize(newHeight);
                manipulator->add(*selection);
                manipulator->execute(selection->canvas());
            }
        }
        delete m_dlgRow;
        m_dlgRow = nullptr;
    } else {
        int col = selection->lastRange().left();
        ColFormatStorage *cf = sheet->columnFormats();
        double colWidth = cf->colWidth(col);
        double defaultWidth = sheet->fullMap()->defaultColumnFormat().width;

        m_dlgCol = new ResizeColumnDialog(canvasWidget, unit);
        m_dlgCol->setColWidth(colWidth);
        m_dlgCol->setDefault(defaultWidth);

        if (m_dlgCol->exec()) {
            double newWidth = m_dlgCol->colWidth();
            // Don't generate a resize, when there isn't a change or the change is only a rounding issue
            if (fabs(newWidth - colWidth) > DBL_EPSILON) {
                ResizeColumnManipulator *manipulator = new ResizeColumnManipulator();
                manipulator->setSheet(sheet);
                manipulator->setSize(newWidth);
                manipulator->add(*selection);
                manipulator->execute(selection->canvas());
            }
        }
        delete m_dlgCol;
        m_dlgCol = nullptr;
    }
}

bool ResizeRowCol::enabledForSelection(Selection *selection, const Cell &)
{
    if (m_row && selection->isColumnSelected())
        return false;
    if ((!m_row) && selection->isRowSelected())
        return false;
    return true;
}
