/* This file is part of the KDE project
   Copyright 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or(at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "CellToolBase_p.h"
#include "CellToolBase.h"

// KSpread
#include "ApplicationSettings.h"
#include "CalculationSettings.h"
#include "CellStorage.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"

// commands
#include "commands/DataManipulators.h"
#include "commands/StyleCommand.h"

// ui
#include "ui/CellEditor.h"
#include "ui/CellToolOptionWidget.h"
#include "ui/ExternalEditor.h"
#include "ui/SheetView.h"

// KOffice
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoResourceManager.h>
#include <KoViewConverter.h>

// KDE
#include <KFontAction>
#include <KFontChooser>
#include <KFontSizeAction>

// Qt
#include <QApplication>
#include <QGridLayout>
#include <QPainter>
#include <QToolButton>

using namespace KSpread;

void CellToolBase::Private::updateEditor(const Cell& cell)
{
    const Cell& theCell = cell.isPartOfMerged() ? cell.masterCell() : cell;
    const Style style = theCell.style();
    if (q->selection()->activeSheet()->isProtected() && style.hideFormula()) {
        optionWidget->editor()->setPlainText(theCell.displayText());
    } else if (q->selection()->activeSheet()->isProtected() && style.hideAll()) {
        optionWidget->editor()->clear();
    } else {
        optionWidget->editor()->setPlainText(theCell.userInput());
    }
}

#define ACTION_EXEC( name, command ) { \
        QAction *a = q->action(name); \
        const bool blocked = a->blockSignals(true); \
        a->command; \
        a->blockSignals(blocked); \
    }

void CellToolBase::Private::updateActions(const Cell& cell)
{
    const Style style = cell.style();

    // -- font actions --
    ACTION_EXEC("bold", setChecked(style.bold()));
    ACTION_EXEC("italic", setChecked(style.italic()));
    ACTION_EXEC("underline", setChecked(style.underline()));
    ACTION_EXEC("strikeOut", setChecked(style.strikeOut()));

    static_cast<KFontAction*>(q->action("font"))->setFont(style.fontFamily());
    static_cast<KFontSizeAction*>(q->action("fontSize"))->setFontSize(style.fontSize());
    // -- horizontal alignment actions --
    ACTION_EXEC("alignLeft", setChecked(style.halign() == Style::Left));
    ACTION_EXEC("alignCenter", setChecked(style.halign() == Style::Center));
    ACTION_EXEC("alignRight", setChecked(style.halign() == Style::Right));
    // -- vertical alignment actions --
    ACTION_EXEC("alignTop", setChecked(style.valign() == Style::Top));
    ACTION_EXEC("alignMiddle", setChecked(style.valign() == Style::Middle));
    ACTION_EXEC("alignBottom", setChecked(style.valign() == Style::Bottom));

    ACTION_EXEC("verticalText", setChecked(style.verticalText()));
    ACTION_EXEC("wrapText", setChecked(style.wrapText()));

    Format::Type ft = style.formatType();
    ACTION_EXEC("percent", setChecked(ft == Format::Percentage));
    ACTION_EXEC("currency", setChecked(ft == Format::Money));

    const bool showFormulas = q->selection()->activeSheet()->getShowFormula();
    q->action("alignLeft")->setEnabled(!showFormulas);
    q->action("alignCenter")->setEnabled(!showFormulas);
    q->action("alignRight")->setEnabled(!showFormulas);

    if (!q->selection()->activeSheet()->isProtected() || style.notProtected()) {
        q->action("clearComment")->setEnabled(!cell.comment().isEmpty());
        q->action("decreaseIndentation")->setEnabled(style.indentation() > 0.0);
    }

    // Now, activate/deactivate some actions depending on what is selected.
    if (!q->selection()->activeSheet()->isProtected()) {
        const bool colSelected = q->selection()->isColumnSelected();
        const bool rowSelected = q->selection()->isRowSelected();
        // -- column & row actions --
        q->action("resizeCol")->setEnabled(!rowSelected);
        q->action("insertColumn")->setEnabled(!rowSelected);
        q->action("deleteColumn")->setEnabled(!rowSelected);
        q->action("hideColumn")->setEnabled(!rowSelected);
        q->action("equalizeCol")->setEnabled(!rowSelected);
        q->action("resizeRow")->setEnabled(!colSelected);
        q->action("deleteRow")->setEnabled(!colSelected);
        q->action("insertRow")->setEnabled(!colSelected);
        q->action("hideRow")->setEnabled(!colSelected);
        q->action("equalizeRow")->setEnabled(!colSelected);
        // -- data insert actions --
        q->action("textToColumns")->setEnabled(!rowSelected);

        const bool simpleSelection = q->selection()->isSingular() || colSelected || rowSelected;
        q->action("sheetFormat")->setEnabled(!simpleSelection);
        q->action("sort")->setEnabled(!simpleSelection);
        q->action("sortDec")->setEnabled(!simpleSelection);
        q->action("sortInc")->setEnabled(!simpleSelection);
        q->action("mergeCells")->setEnabled(!simpleSelection);
        q->action("mergeCellsHorizontal")->setEnabled(!simpleSelection);
        q->action("mergeCellsVertical")->setEnabled(!simpleSelection);
        q->action("fillRight")->setEnabled(!simpleSelection);
        q->action("fillUp")->setEnabled(!simpleSelection);
        q->action("fillDown")->setEnabled(!simpleSelection);
        q->action("fillLeft")->setEnabled(!simpleSelection);
        q->action("createStyleFromCell")->setEnabled(simpleSelection); // just from one cell

        const bool contiguousSelection = q->selection()->isContiguous();
        q->action("subtotals")->setEnabled(contiguousSelection);
    }
}

void CellToolBase::Private::setProtectedActionsEnabled(bool enable)
{
    // Enable/disable actions.
    const QList<KAction*> actions = q->actions().values();
    for (int i = 0; i < actions.count(); ++i)
        actions[i]->setEnabled(enable);
    optionWidget->formulaButton()->setEnabled(enable);
    optionWidget->editor()->setEnabled(enable);

    // These actions are always enabled.
    q->action("copy")->setEnabled(true);
    q->action("gotoCell")->setEnabled(true);
    q->action("edit_find")->setEnabled(true);
    q->action("edit_find_next")->setEnabled(true);
    q->action("edit_find_last")->setEnabled(true);
}

void CellToolBase::Private::processEnterKey(QKeyEvent* event)
{
// array is true, if ctrl+alt are pressed
    bool array = (event->modifiers() & Qt::AltModifier) &&
                 (event->modifiers() & Qt::ControlModifier);

    /* save changes to the current editor */
    q->deleteEditor(true, array);

    /* use the configuration setting to see which direction we're supposed to move
        when enter is pressed.
    */
    KSpread::MoveTo direction = q->selection()->activeSheet()->map()->settings()->moveToValue();

//if shift Button clicked inverse move direction
    if (event->modifiers() & Qt::ShiftModifier) {
        switch (direction) {
        case Bottom:
            direction = Top;
            break;
        case Top:
            direction = Bottom;
            break;
        case Left:
            direction = Right;
            break;
        case Right:
            direction = Left;
            break;
        case BottomFirst:
            direction = BottomFirst;
            break;
        case NoMovement:
            direction = NoMovement;
            break;
        }
    }

    /* never extend a selection with the enter key -- the shift key reverses
        direction, not extends the selection
    */
    QRect r(moveDirection(direction, false));
    event->accept(); // QKeyEvent
}

void CellToolBase::Private::processArrowKey(QKeyEvent *event)
{
    /* NOTE:  hitting the tab key also calls this function.  Don't forget
        to account for it
    */
    register Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return;

    /* save changes to the current editor */
    q->selection()->emitCloseEditor(true);

    KSpread::MoveTo direction = Bottom;
    bool makingSelection = event->modifiers() & Qt::ShiftModifier;

    switch (event->key()) {
    case Qt::Key_Down:
        direction = Bottom;
        break;
    case Qt::Key_Up:
        direction = Top;
        break;
    case Qt::Key_Left:
        if (sheet->layoutDirection() == Qt::RightToLeft)
            direction = Right;
        else
            direction = Left;
        break;
    case Qt::Key_Right:
        if (sheet->layoutDirection() == Qt::RightToLeft)
            direction = Left;
        else
            direction = Right;
        break;
    case Qt::Key_Tab:
        direction = Right;
        break;
    case Qt::Key_Backtab:
        //Shift+Tab moves to the left
        direction = Left;
        makingSelection = false;
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    QRect r(moveDirection(direction, makingSelection));
    event->accept(); // QKeyEvent
}

void CellToolBase::Private::processEscapeKey(QKeyEvent * event)
{
    q->selection()->emitCloseEditor(false); // discard changes
    event->accept(); // QKeyEvent
}

bool CellToolBase::Private::processHomeKey(QKeyEvent* event)
{
    register Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return false;

    bool makingSelection = event->modifiers() & Qt::ShiftModifier;

    if (q->editor()) {
        // We are in edit mode -> go beginning of line
        QApplication::sendEvent(q->editor()->widget(), event);
        return false;
    } else {
        QPoint destination;
        /* start at the first used cell in the row and cycle through the right until
            we find a cell that has some output text.  But don't look past the current
            marker.
            The end result we want is to move to the left to the first cell with text,
            or just to the first column if there is no more text to the left.

            But why?  In excel, home key sends you to the first column always.
            We might want to change to that behavior.
        */

        if (event->modifiers() & Qt::ControlModifier) {
            /* ctrl + Home will always just send us to location (1,1) */
            destination = QPoint(1, 1);
        } else {
            QPoint marker = q->selection()->marker();

            Cell cell = sheet->cellStorage()->firstInRow(marker.y(), CellStorage::VisitContent);
            while (!cell.isNull() && cell.column() < marker.x() && cell.isEmpty()) {
                cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
            }

            int col = (!cell.isNull() ? cell.column() : 1);
            if (col == marker.x())
                col = 1;
            destination = QPoint(col, marker.y());
        }

        if (q->selection()->marker() == destination)
            return false;

        if (makingSelection) {
            q->selection()->update(destination);
        } else {
            q->selection()->initialize(destination, sheet);
        }
        q->scrollToCell(destination);
        event->accept(); // QKeyEvent
    }
    return true;
}

bool CellToolBase::Private::processEndKey(QKeyEvent *event)
{
    register Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return false;

    bool makingSelection = event->modifiers() & Qt::ShiftModifier;
    Cell cell;
    QPoint marker = q->selection()->marker();

    if (q->editor()) {
        // We are in edit mode -> go end of line
        QApplication::sendEvent(q->editor()->widget(), event);
        return false;
    } else {
        // move to the last used cell in the row
        int col = 1;

        cell = sheet->cellStorage()->lastInRow(marker.y(), CellStorage::VisitContent);
        while (!cell.isNull() && cell.column() > q->selection()->marker().x() && cell.isEmpty()) {
            cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
        }

        col = (cell.isNull()) ? q->maxCol() : cell.column();

        QPoint destination(col, marker.y());
        if (destination == marker)
            return false;

        if (makingSelection) {
            q->selection()->update(destination);
        } else {
            q->selection()->initialize(destination, sheet);
        }
        q->scrollToCell(destination);
        event->accept(); // QKeyEvent
    }
    return true;
}

bool CellToolBase::Private::processPriorKey(QKeyEvent *event)
{
    bool makingSelection = event->modifiers() & Qt::ShiftModifier;
    q->selection()->emitCloseEditor(true); // save changes

    QPoint marker = q->selection()->marker();

    QPoint destination(marker.x(), qMax(1, marker.y() - 10));
    if (destination == marker)
        return false;

    if (makingSelection) {
        q->selection()->update(destination);
    } else {
        q->selection()->initialize(destination, q->selection()->activeSheet());
    }
    q->scrollToCell(destination);
    event->accept(); // QKeyEvent
    return true;
}

bool CellToolBase::Private::processNextKey(QKeyEvent *event)
{
    bool makingSelection = event->modifiers() & Qt::ShiftModifier;

    q->selection()->emitCloseEditor(true); // save changes

    QPoint marker = q->selection()->marker();
    QPoint destination(marker.x(), qMax(1, marker.y() + 10));

    if (marker == destination)
        return false;

    if (makingSelection) {
        q->selection()->update(destination);
    } else {
        q->selection()->initialize(destination, q->selection()->activeSheet());
    }
    q->scrollToCell(destination);
    event->accept(); // QKeyEvent
    return true;
}

void CellToolBase::Private::processOtherKey(QKeyEvent *event)
{
    register Sheet * const sheet = q->selection()->activeSheet();

    // No null character ...
    if (event->text().isEmpty() || !q->selection()->activeSheet()->map()->isReadWrite() ||
            !sheet || sheet->isProtected()) {
        event->accept(); // QKeyEvent
    } else {
        if (!q->editor()) {
            // Switch to editing mode
            q->createEditor();
        }
        // Send it to the embedded editor.
        QApplication::sendEvent(q->editor()->widget(), event);
    }
}

bool CellToolBase::Private::processControlArrowKey(QKeyEvent *event)
{
    register Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return false;

    bool makingSelection = event->modifiers() & Qt::ShiftModifier;

    Cell cell;
    Cell lastCell;
    QPoint destination;
    bool searchThroughEmpty = true;
    int row;
    int col;

    QPoint marker = q->selection()->marker();

    /* here, we want to move to the first or last cell in the given direction that is
        actually being used.  Ignore empty cells and cells on hidden rows/columns */
    switch (event->key()) {
        //Ctrl+Qt::Key_Up
    case Qt::Key_Up:

        cell = Cell(sheet, marker.x(), marker.y());
        if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.y() != 1)) {
            lastCell = cell;
            row = marker.y() - 1;
            cell = Cell(sheet, cell.column(), row);
            while ((!cell.isNull()) && (row > 0) && (!cell.isEmpty())) {
                if (!sheet->rowFormat(cell.row())->isHiddenOrFiltered()) {
                    lastCell = cell;
                    searchThroughEmpty = false;
                }
                row--;
                if (row > 0)
                    cell = Cell(sheet, cell.column(), row);
            }
            cell = lastCell;
        }
        if (searchThroughEmpty) {
            cell = sheet->cellStorage()->prevInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

            while ((!cell.isNull()) &&
                    (cell.isEmpty() || (sheet->rowFormat(cell.row())->isHiddenOrFiltered()))) {
                cell = sheet->cellStorage()->prevInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
            }
        }

        if (cell.isNull())
            row = 1;
        else
            row = cell.row();

        while (sheet->rowFormat(row)->isHiddenOrFiltered()) {
            row++;
        }

        destination.setX(qBound(1, marker.x(), q->maxCol()));
        destination.setY(qBound(1, row, q->maxRow()));
        break;

        //Ctrl+Qt::Key_Down
    case Qt::Key_Down:

        cell = Cell(sheet, marker.x(), marker.y());
        if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.y() != q->maxRow())) {
            lastCell = cell;
            row = marker.y() + 1;
            cell = Cell(sheet, cell.column(), row);
            while ((!cell.isNull()) && (row < q->maxRow()) && (!cell.isEmpty())) {
                if (!(sheet->rowFormat(cell.row())->isHiddenOrFiltered())) {
                    lastCell = cell;
                    searchThroughEmpty = false;
                }
                row++;
                cell = Cell(sheet, cell.column(), row);
            }
            cell = lastCell;
        }
        if (searchThroughEmpty) {
            cell = sheet->cellStorage()->nextInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

            while ((!cell.isNull()) &&
                    (cell.isEmpty() || (sheet->rowFormat(cell.row())->isHiddenOrFiltered()))) {
                cell = sheet->cellStorage()->nextInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
            }
        }

        if (cell.isNull())
            row = marker.y();
        else
            row = cell.row();

        while (sheet->rowFormat(row)->isHiddenOrFiltered()) {
            row--;
        }

        destination.setX(qBound(1, marker.x(), q->maxCol()));
        destination.setY(qBound(1, row, q->maxRow()));
        break;

//Ctrl+Qt::Key_Left
    case Qt::Key_Left:

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            cell = Cell(sheet, marker.x(), marker.y());
            if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != q->maxCol())) {
                lastCell = cell;
                col = marker.x() + 1;
                cell = Cell(sheet, col, cell.row());
                while ((!cell.isNull()) && (col < q->maxCol()) && (!cell.isEmpty())) {
                    if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered())) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col++;
                    cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = sheet->cellStorage()->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered()))) {
                    cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = marker.x();
            else
                col = cell.column();

            while (sheet->columnFormat(col)->isHiddenOrFiltered()) {
                col--;
            }

            destination.setX(qBound(1, col, q->maxCol()));
            destination.setY(qBound(1, marker.y(), q->maxRow()));
        } else {
            cell = Cell(sheet, marker.x(), marker.y());
            if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != 1)) {
                lastCell = cell;
                col = marker.x() - 1;
                cell = Cell(sheet, col, cell.row());
                while ((!cell.isNull()) && (col > 0) && (!cell.isEmpty())) {
                    if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered())) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col--;
                    if (col > 0)
                        cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = sheet->cellStorage()->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered()))) {
                    cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = 1;
            else
                col = cell.column();

            while (sheet->columnFormat(col)->isHiddenOrFiltered()) {
                col++;
            }

            destination.setX(qBound(1, col, q->maxCol()));
            destination.setY(qBound(1, marker.y(), q->maxRow()));
        }
        break;

//Ctrl+Qt::Key_Right
    case Qt::Key_Right:

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            cell = Cell(sheet, marker.x(), marker.y());
            if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != 1)) {
                lastCell = cell;
                col = marker.x() - 1;
                cell = Cell(sheet, col, cell.row());
                while ((!cell.isNull()) && (col > 0) && (!cell.isEmpty())) {
                    if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered())) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col--;
                    if (col > 0)
                        cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = sheet->cellStorage()->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered()))) {
                    cell = sheet->cellStorage()->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = 1;
            else
                col = cell.column();

            while (sheet->columnFormat(col)->isHiddenOrFiltered()) {
                col++;
            }

            destination.setX(qBound(1, col, q->maxCol()));
            destination.setY(qBound(1, marker.y(), q->maxRow()));
        } else {
            cell = Cell(sheet, marker.x(), marker.y());
            if ((!cell.isNull()) && (!cell.isEmpty()) && (marker.x() != q->maxCol())) {
                lastCell = cell;
                col = marker.x() + 1;
                cell = Cell(sheet, col, cell.row());
                while ((!cell.isNull()) && (col < q->maxCol()) && (!cell.isEmpty())) {
                    if (!(sheet->columnFormat(cell.column())->isHiddenOrFiltered())) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col++;
                    cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = sheet->cellStorage()->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormat(cell.column())->isHiddenOrFiltered()))) {
                    cell = sheet->cellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = marker.x();
            else
                col = cell.column();

            while (sheet->columnFormat(col)->isHiddenOrFiltered()) {
                col--;
            }

            destination.setX(qBound(1, col, q->maxCol()));
            destination.setY(qBound(1, marker.y(), q->maxRow()));
        }
        break;

    }

    if (marker == destination)
        return false;

    if (makingSelection) {
        q->selection()->update(destination);
    } else {
        q->selection()->initialize(destination, sheet);
    }
    q->scrollToCell(destination);
    return true;
}

bool CellToolBase::Private::formatKeyPress(QKeyEvent * _ev)
{
    if (!(_ev->modifiers() & Qt::ControlModifier))
        return false;

    int key = _ev->key();
    if (key != Qt::Key_Exclam && key != Qt::Key_At &&
            key != Qt::Key_Ampersand && key != Qt::Key_Dollar &&
            key != Qt::Key_Percent && key != Qt::Key_AsciiCircum &&
            key != Qt::Key_NumberSign)
        return false;

    StyleCommand* command = new StyleCommand();
    command->setSheet(q->selection()->activeSheet());

    switch (_ev->key()) {
    case Qt::Key_Exclam:
        command->setText(i18n("Number Format"));
        command->setFormatType(Format::Number);
        command->setPrecision(2);
        break;

    case Qt::Key_Dollar:
        command->setText(i18n("Currency Format"));
        command->setFormatType(Format::Money);
        command->setPrecision(q->selection()->activeSheet()->map()->calculationSettings()->locale()->fracDigits());
        break;

    case Qt::Key_Percent:
        command->setText(i18n("Percentage Format"));
        command->setFormatType(Format::Percentage);
        break;

    case Qt::Key_At:
        command->setText(i18n("Time Format"));
        command->setFormatType(Format::SecondeTime);
        break;

    case Qt::Key_NumberSign:
        command->setText(i18n("Date Format"));
        command->setFormatType(Format::ShortDate);
        break;

    case Qt::Key_AsciiCircum:
        command->setText(i18n("Scientific Format"));
        command->setFormatType(Format::Scientific);
        break;

    case Qt::Key_Ampersand:
        command->setText(i18n("Change Border"));
        command->setTopBorderPen(QPen(q->canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
        command->setBottomBorderPen(QPen(q->canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
        command->setLeftBorderPen(QPen(q->canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
        command->setRightBorderPen(QPen(q->canvas()->resourceManager()->foregroundColor().toQColor(), 1, Qt::SolidLine));
        break;

    default:
        delete command;
        return false;
    }

    command->add(*q->selection());
    command->execute();
    _ev->accept(); // QKeyEvent

    return true;
}

QRect CellToolBase::Private::moveDirection(KSpread::MoveTo direction, bool extendSelection)
{
    kDebug(36005) << "Canvas::moveDirection";

    register Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return QRect();

    QPoint destination;
    QPoint cursor = q->selection()->cursor();

    QPoint cellCorner = cursor;
    Cell cell(sheet, cursor.x(), cursor.y());

    /* cell is either the same as the marker, or the cell that is forced obscuring
        the marker cell
    */
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        cellCorner = QPoint(cell.column(), cell.row());
    }

    /* how many cells must we move to get to the next cell? */
    int offset = 0;
    const RowFormat *rl = 0;
    const ColumnFormat *cl = 0;
    switch (direction)
        /* for each case, figure out how far away the next cell is and then keep
            going one row/col at a time after that until a visible row/col is found

            NEVER use cell.column() or cell.row() -- it might be a default cell
        */
    {
    case Bottom:
        offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
        rl = sheet->rowFormat(cursor.y() + offset);
        while (((cursor.y() + offset) <= q->maxRow()) && rl->isHiddenOrFiltered()) {
            offset++;
            rl = sheet->rowFormat(cursor.y() + offset);
        }

        destination = QPoint(cursor.x(), qMin(cursor.y() + offset, q->maxRow()));
        break;
    case Top:
        offset = (cellCorner.y() - cursor.y()) - 1;
        rl = sheet->rowFormat(cursor.y() + offset);
        while (((cursor.y() + offset) >= 1) && rl->isHiddenOrFiltered()) {
            offset--;
            rl = sheet->rowFormat(cursor.y() + offset);
        }
        destination = QPoint(cursor.x(), qMax(cursor.y() + offset, 1));
        break;
    case Left:
        offset = (cellCorner.x() - cursor.x()) - 1;
        cl = sheet->columnFormat(cursor.x() + offset);
        while (((cursor.x() + offset) >= 1) && cl->isHiddenOrFiltered()) {
            offset--;
            cl = sheet->columnFormat(cursor.x() + offset);
        }
        destination = QPoint(qMax(cursor.x() + offset, 1), cursor.y());
        break;
    case Right:
        offset = cell.mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
        cl = sheet->columnFormat(cursor.x() + offset);
        while (((cursor.x() + offset) <= q->maxCol()) && cl->isHiddenOrFiltered()) {
            offset++;
            cl = sheet->columnFormat(cursor.x() + offset);
        }
        destination = QPoint(qMin(cursor.x() + offset, q->maxCol()), cursor.y());
        break;
    case BottomFirst:
        offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
        rl = sheet->rowFormat(cursor.y() + offset);
        while (((cursor.y() + offset) <= q->maxRow()) && rl->isHiddenOrFiltered()) {
            ++offset;
            rl = sheet->rowFormat(cursor.y() + offset);
        }

        destination = QPoint(1, qMin(cursor.y() + offset, q->maxRow()));
        break;
    case NoMovement:
        destination = cursor;
        break;
    }

    if (extendSelection) {
        q->selection()->update(destination);
    } else {
        q->selection()->initialize(destination, sheet);
    }
    q->scrollToCell(destination);
    updateEditor(Cell(q->selection()->activeSheet(), q->selection()->cursor()));

    return QRect(cursor, destination);
}

void CellToolBase::Private::paintSelection(QPainter &painter, const QRectF &viewRect)
{
    if (q->selection()->referenceSelection() || q->editor()) {
        return;
    }
    Sheet *const sheet = q->selection()->activeSheet();

    // save the painter state
    painter.save();
    // disable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, false);
    // Extend the clip rect by one in each direction to avoid artefacts caused by rounding errors.
    // TODO Stefan: This unites the region's rects. May be bad. Check!
    painter.setClipRegion(painter.clipRegion().boundingRect().adjusted(-1, -1, 1, 1));

    QLineF line;
    QPen pen(QApplication::palette().text().color(), q->canvas()->viewConverter()->viewToDocumentX(2.0));
    painter.setPen(pen);

    const KSpread::Selection* selection = q->selection();
    const QRect currentRange = selection->extendToMergedAreas(QRect(selection->anchor(), selection->marker()));
    Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        const QRect range = (*it)->isAll() ? (*it)->rect() : selection->extendToMergedAreas((*it)->rect());

        // Only the active element (the one with the anchor) will be drawn with a border
        const bool current = (currentRange == range);

        double positions[4];
        bool paintSides[4];
        retrieveMarkerInfo(range, viewRect, positions, paintSides);

        double left =   positions[0];
        double top =    positions[1];
        double right =  positions[2];
        double bottom = positions[3];
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            // The painter's origin is translated by the negative canvas offset.
            // viewRect.left() is the canvas offset. Add it once to the
            // coordinates. Then, the upper left corner of the canvas has to
            // match the correct document position, which is the scrolling
            // offset (viewRect.left()) plus the width of the visible area
            // (viewRect.width()); that's the right border (left+width).
            const qreal offset = /*2 * viewRect.left() +*/ viewRect.width();
            left = offset - positions[2];
            right = offset - positions[0];
        }

        bool paintLeft =   paintSides[0];
        bool paintTop =    paintSides[1];
        bool paintRight =  paintSides[2];
        bool paintBottom = paintSides[3];
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            paintLeft  = paintSides[2];
            paintRight = paintSides[0];
        }

        const double unzoomedPixelX = q->canvas()->viewConverter()->viewToDocumentX(1.0);
        const double unzoomedPixelY = q->canvas()->viewConverter()->viewToDocumentY(1.0);
        // get the transparent selection color
        QColor selectionColor(QApplication::palette().highlight().color());
        selectionColor.setAlpha(127);
        if (current) {
            // save old clip region
            const QRegion clipRegion = painter.clipRegion();
            // clip out the cursor region
            const QRect cursor = QRect(selection->cursor(), selection->cursor());
            const QRect extCursor = selection->extendToMergedAreas(cursor);
            QRectF cursorRect = sheet->cellCoordinatesToDocument(extCursor);
            if (sheet->layoutDirection() == Qt::RightToLeft) {
                // See comment above.
                const qreal offset = /*2 * viewRect.left() +*/ viewRect.width();
                const qreal left = offset - cursorRect.right();
                const qreal right = offset - cursorRect.left();
                cursorRect.setLeft(left);
                cursorRect.setRight(right);
            }
            cursorRect.adjust(unzoomedPixelX, unzoomedPixelY, unzoomedPixelX, unzoomedPixelY);
            painter.setClipRegion(clipRegion.subtracted(cursorRect.toRect()));
            // draw the transparent selection background
            painter.fillRect(QRectF(left, top, right - left, bottom - top), selectionColor);
            // restore clip region
            painter.setClipRegion(clipRegion);
        } else {
            // draw the transparent selection background
            painter.fillRect(QRectF(left, top, right - left, bottom - top), selectionColor);
        }

        if (paintTop) {
            line = QLineF(left, top, right, top);
            painter.drawLine(line);
        }
        if (selection->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            if (paintRight) {
                line = QLineF(right, top, right, bottom);
                painter.drawLine(line);
            }
            if (paintLeft && paintBottom && current) {
                /* then the 'handle' in the bottom left corner is visible. */
                line = QLineF(left, top, left, bottom - 3 * unzoomedPixelY);
                painter.drawLine(line);
                line = QLineF(left + 4 * unzoomedPixelX,  bottom, right + unzoomedPixelY, bottom);
                painter.drawLine(line);
                painter.fillRect(QRectF(left - 2 * unzoomedPixelX, bottom - 2 * unzoomedPixelY,
                                        5 * unzoomedPixelX, 5 * unzoomedPixelY), painter.pen().color());
            } else {
                if (paintLeft) {
                    line = QLineF(left, top, left, bottom);
                    painter.drawLine(line);
                }
                if (paintBottom) {
                    line = QLineF(left, bottom, right, bottom);
                    painter.drawLine(line);
                }
            }
        } else { // activeSheet()->layoutDirection() == Qt::LeftToRight
            if (paintLeft) {
                line = QLineF(left, top, left, bottom);
                painter.drawLine(line);
            }
            if (paintRight && paintBottom && current) {
                /* then the 'handle' in the bottom right corner is visible. */
                line = QLineF(right, top, right, bottom - 3 * unzoomedPixelY);
                painter.drawLine(line);
                line = QLineF(left, bottom, right - 3 * unzoomedPixelX, bottom);
                painter.drawLine(line);
                painter.fillRect(QRectF(right - 2 * unzoomedPixelX, bottom - 2 * unzoomedPixelX,
                                        5 * unzoomedPixelX, 5 * unzoomedPixelY), painter.pen().color());
            } else {
                if (paintRight) {
                    line = QLineF(right, top, right, bottom);
                    painter.drawLine(line);
                }
                if (paintBottom) {
                    line = QLineF(left, bottom, right, bottom);
                    painter.drawLine(line);
                }
            }
        }
    }
    // restore painter state
    painter.restore();
}

void CellToolBase::Private::paintReferenceSelection(QPainter &painter, const QRectF &viewRect)
{
    Q_UNUSED(viewRect);
    if (!q->selection()->referenceSelection()) {
        return;
    }
    // save painter state
    painter.save();

    // Define the reference selection handle.
    const qreal pixelX = q->canvas()->viewConverter()->viewToDocumentX(1);
    const qreal pixelY = q->canvas()->viewConverter()->viewToDocumentY(1);
    const QRectF handleArea(-3 * pixelX, -3 * pixelY, 6 * pixelX, 6 * pixelY);

    // A list of already found regions to color the same region with the same color.
    QSet<QString> alreadyFoundRegions;
    // The colors for the referenced ranges and the color index.
    const QList<QColor> colors = q->selection()->colors();
    int index = 0;

    // Iterate over the referenced ranges.
    const Region::ConstIterator end(q->selection()->constEnd());
    for (Region::ConstIterator it(q->selection()->constBegin()); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        // Only paint ranges or cells on the current sheet
        if (sheet != q->selection()->activeSheet()) {
            index++;
            continue;
        }
        // Only paint a reference once.
        if (alreadyFoundRegions.contains((*it)->name())) {
            continue;
        }
        alreadyFoundRegions.insert((*it)->name());

        const QRect range = q->selection()->extendToMergedAreas((*it)->rect());
        QRectF area = sheet->cellCoordinatesToDocument(range);

        // Convert region from sheet coordinates to canvas coordinates for use with the painter
        // retrieveMarkerInfo(region,viewRect,positions,paintSides);

        // Now adjust the highlight rectangle is slightly inside the cell borders (this means
        // that multiple highlighted cells look nicer together as the borders do not clash)
        area.adjust(pixelX, pixelY, -pixelX, -pixelY);

        // The current color.
        const QColor color = colors[index++ % colors.size()];

        // Paint the reference range's outline.
        if ((*it)->sheet()->layoutDirection() == Qt::RightToLeft) {
            // See comment in paintSelection().
            const qreal offset = /*2 * viewRect.left() +*/ viewRect.width();
            const qreal left = offset - area.right();
            const qreal right = offset - area.left();
            area.setLeft(left);
            area.setRight(right);
        }

        painter.setBrush(QBrush());
        painter.setPen(color);
        painter.drawRect(area);

        // Now draw the size grip (the little rectangle on the bottom right-hand corner of
        // the range which the user can click and drag to resize the region)
        painter.setPen(Qt::white);
        painter.setBrush(color);
        const bool rtl = sheet->layoutDirection() == Qt::RightToLeft;
        const QPointF corner(rtl ? area.bottomLeft() : area.bottomRight());
        painter.drawRect(handleArea.translated(corner));
    }

    // restore painter state
    painter.restore();
}

void CellToolBase::Private::retrieveMarkerInfo(const QRect &cellRange, const QRectF &viewRect,
        double positions[], bool paintSides[])
{
    // Everything is in document coordinates here.
    // The layout direction, which is view dependent, is applied afterwards.

    const Sheet* sheet = q->selection()->activeSheet();
    const QRectF visibleRect = sheet->cellCoordinatesToDocument(cellRange);

    /* these vars are used for clarity, the array for simpler function arguments  */
    qreal left = visibleRect.left();
    qreal top = visibleRect.top();
    qreal right = visibleRect.right();
    qreal bottom = visibleRect.bottom();

    /* left, top, right, bottom */
    paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom()) &&
                    (right >= viewRect.left()) && (left <= viewRect.right());
    paintSides[2] = (viewRect.left() <= right) && (right <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom()) &&
                    (right >= viewRect.left()) && (left <= viewRect.right());

    positions[0] = qMax(left,   viewRect.left());
    positions[1] = qMax(top,    viewRect.top());
    positions[2] = qMin(right,  viewRect.right());
    positions[3] = qMin(bottom, viewRect.bottom());
}

QList<QAction*> CellToolBase::Private::popupActionList() const
{
    QList<QAction*> actions;
    const Cell cell = Cell(q->selection()->activeSheet(), q->selection()->marker());
    const bool isProtected = !q->selection()->activeSheet()->map()->isReadWrite() ||
                             (q->selection()->activeSheet()->isProtected() &&
                              !(cell.style().notProtected() && q->selection()->isSingular()));
    if (!isProtected) {
        actions.append(q->action("cellStyle"));
        actions.append(popupMenuActions["separator1"]);
        actions.append(q->action("cut"));
    }
    actions.append(q->action("copy"));
    if (!isProtected) {
        actions.append(q->action("paste"));
        actions.append(q->action("specialPaste"));
        actions.append(q->action("pasteWithInsertion"));
        actions.append(popupMenuActions["separator2"]);
        actions.append(q->action("clearAll"));
        actions.append(q->action("adjust"));
        actions.append(q->action("setDefaultStyle"));
        actions.append(q->action("setAreaName"));

        if (!q->selection()->isColumnOrRowSelected()) {
            actions.append(popupMenuActions["separator3"]);
            actions.append(popupMenuActions["insertCell"]);
            actions.append(popupMenuActions["deleteCell"]);
        } else if (q->selection()->isColumnSelected()) {
            actions.append(q->action("resizeCol"));
            actions.append(popupMenuActions["adjustColumn"]);
            actions.append(popupMenuActions["separator4"]);
            actions.append(popupMenuActions["insertColumn"]);
            actions.append(popupMenuActions["deleteColumn"]);
            actions.append(q->action("hideColumn"));

            q->action("showSelColumns")->setEnabled(false);
            const ColumnFormat* columnFormat;
            Region::ConstIterator endOfList = q->selection()->constEnd();
            for (Region::ConstIterator it = q->selection()->constBegin(); it != endOfList; ++it) {
                QRect range = (*it)->rect();
                int col;
                for (col = range.left(); col < range.right(); ++col) {
                    columnFormat = q->selection()->activeSheet()->columnFormat(col);
                    if (columnFormat->isHidden()) {
                        q->action("showSelColumns")->setEnabled(true);
                        actions.append(q->action("showSelColumns"));
                        break;
                    }
                }
                if (range.left() > 1 && col == range.right()) {
                    bool allHidden = true;
                    for (col = 1; col < range.left(); ++col) {
                        columnFormat = q->selection()->activeSheet()->columnFormat(col);
                        allHidden &= columnFormat->isHidden();
                    }
                    if (allHidden) {
                        q->action("showSelColumns")->setEnabled(true);
                        actions.append(q->action("showSelColumns"));
                        break;
                    }
                } else {
                    break;
                }
            }
        } else if (q->selection()->isRowSelected()) {
            actions.append(q->action("resizeRow"));
            actions.append(popupMenuActions["adjustRow"]);
            actions.append(popupMenuActions["separator5"]);
            actions.append(popupMenuActions["insertRow"]);
            actions.append(popupMenuActions["deleteRow"]);
            actions.append(q->action("hideRow"));

            q->action("showSelRows")->setEnabled(false);
            const RowFormat* rowFormat;
            Region::ConstIterator endOfList = q->selection()->constEnd();
            for (Region::ConstIterator it = q->selection()->constBegin(); it != endOfList; ++it) {
                QRect range = (*it)->rect();
                int row;
                for (row = range.top(); row < range.bottom(); ++row) {
                    rowFormat = q->selection()->activeSheet()->rowFormat(row);
                    if (rowFormat->isHidden()) {
                        q->action("showSelRows")->setEnabled(true);
                        actions.append(q->action("showSelRows"));
                        break;
                    }
                }
                if (range.top() > 1 && row == range.bottom()) {
                    bool allHidden = true;
                    for (row = 1; row < range.top(); ++row) {
                        rowFormat = q->selection()->activeSheet()->rowFormat(row);
                        allHidden &= rowFormat->isHidden();
                    }
                    if (allHidden) {
                        q->action("showSelRows")->setEnabled(true);
                        actions.append(q->action("showSelRows"));
                        break;
                    }
                } else {
                    break;
                }
            }
        }
        actions.append(popupMenuActions["separator6"]);
        actions.append(q->action("comment"));
        if (!cell.comment().isEmpty()) {
            actions.append(q->action("clearComment"));
        }

        if (testListChoose(q->selection())) {
            actions.append(popupMenuActions["separator7"]);
            actions.append(popupMenuActions["listChoose"]);
        }
    }
    return actions;
}

void CellToolBase::Private::createPopupMenuActions()
{
    QAction* action = 0;

    for (int i = 1; i <= 7; ++i) {
        action = new QAction(q);
        action->setSeparator(true);
        popupMenuActions.insert(QString("separator%1").arg(i), action);
    }

    action = new KAction(KIcon("insertcell"), i18n("Insert Cells..."), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(insertCells()));
    popupMenuActions.insert("insertCell", action);

    action = new KAction(KIcon("removecell"), i18n("Delete Cells..."), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(deleteCells()));
    popupMenuActions.insert("deleteCell", action);

    action = new KAction(KIcon("adjustcol"), i18n("Adjust Column"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(adjustColumn()));
    popupMenuActions.insert("adjustColumn", action);

    action = new KAction(KIcon("insert_table_col"), i18n("Insert Columns"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(insertColumn()));
    popupMenuActions.insert("insertColumn", action);

    action = new KAction(KIcon("delete_table_col"), i18n("Delete Columns"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(deleteColumn()));
    popupMenuActions.insert("deleteColumn", action);

    action = new KAction(KIcon("adjustrow"), i18n("Adjust Row"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(adjustRow()));
    popupMenuActions.insert("adjustRow", action);

    action = new KAction(KIcon("insert_table_row"), i18n("Insert Rows"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(insertRow()));
    popupMenuActions.insert("insertRow", action);

    action = new KAction(KIcon("delete_table_row"), i18n("Delete Rows"), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(deleteRow()));
    popupMenuActions.insert("deleteRow", action);

    action = new KAction(i18n("Selection List..."), q);
    connect(action, SIGNAL(triggered(bool)), q, SLOT(listChoosePopupMenu()));
    popupMenuActions.insert("listChoose", action);
}

bool CellToolBase::Private::testListChoose(Selection *selection) const
{
    const Sheet *const sheet = selection->activeSheet();
    const Cell cursorCell(sheet, selection->cursor());
    const CellStorage *const storage = sheet->cellStorage();

    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (cursorCell.column() < range.left() || cursorCell.column() > range.right()) {
            continue; // next range
        }
        Cell cell;
        if (range.top() == 1) {
            cell = storage->firstInColumn(cursorCell.column(), CellStorage::Values);
        } else {
            cell = storage->nextInColumn(cursorCell.column(), range.top() - 1, CellStorage::Values);
        }
        while (!cell.isNull() && cell.row() <= range.bottom()) {
            if (cell.isDefault() || cell.isPartOfMerged()
                    || cell.isFormula() || cell.isTime() || cell.isDate()
                    || cell.value().isNumber() || cell.value().asString().isEmpty()
                    || (cell == cursorCell)) {
                cell = storage->nextInColumn(cell.column(), cell.row(), CellStorage::Values);
                continue;
            }
            if (cell.userInput() != cursorCell.userInput()) {
                return true;
            }
            cell = storage->nextInColumn(cell.column(), cell.row(), CellStorage::Values);
        }
    }
    return false;
}
