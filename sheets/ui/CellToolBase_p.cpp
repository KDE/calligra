// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
// SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 1999-2004 David Faure <faure@kde.org>
// SPDX-FileCopyrightText: 2004-2005 Meni Livne <livne@kde.org>
// SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
// SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 2003 Hamish Rodda <rodda@kde.org>
// SPDX-FileCopyrightText: 2003 Joseph Wenninger <jowenn@kde.org>
// SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
// SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
// SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
// SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
// SPDX-FileCopyrightText: 2002 Daniel Naber <daniel.naber@t-online.de>
// SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>
// SPDX-FileCopyrightText: 1999-2000 Stephan Kulow <coolo@kde.org>
// SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
// SPDX-FileCopyrightText: 2000 Wilco Greven <greven@kde.org>
// SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
// SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
// SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
// SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "CellToolBase_p.h"
#include "CellToolBase.h"

// Sheets
#include "engine/CalculationSettings.h"
#include "core/ApplicationSettings.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"

// ui
#include "CellEditor.h"
#include "ExternalEditor.h"
#include "actions/Actions.h"
#include "commands/StyleCommand.h"

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoViewConverter.h>
#include <KoColor.h>
#include <KoIcon.h>

// KF5
#include <kfontaction.h>
#include <kfontsizeaction.h>

// Qt
#include <QApplication>
#include <QPainter>

using namespace Calligra::Sheets;

void CellToolBase::Private::updateEditor(const Cell& cell)
{
    if (!externalEditor) return;
    const Cell& theCell = cell.isPartOfMerged() ? cell.masterCell() : cell;
    const Style style = theCell.style();
    if (q->selection()->activeSheet()->isProtected() && style.hideFormula()) {
        externalEditor->setPlainText(theCell.displayText());
    } else if (q->selection()->activeSheet()->isProtected() && style.hideAll()) {
        externalEditor->clear();
    } else {
        externalEditor->setPlainText(theCell.userInput());
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
    // TODO - this should eventually be removed, all the checks are done in the CellAction classes instead

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
        // -- data insert actions --
        q->action("textToColumns")->setEnabled(!rowSelected);

        const bool simpleSelection = q->selection()->isSingular() || colSelected || rowSelected;
        q->action("sheetFormat")->setEnabled(!simpleSelection);
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
    // TODO - this should eventually be removed, all the checks are done in the CellAction classes instead
    // Enable/disable actions.
    const QList<QAction*> actions = q->actions().values();
    for (int i = 0; i < actions.count(); ++i)
        actions[i]->setEnabled(enable);
    q->action("insertFormula")->setEnabled(enable);
    if (externalEditor) externalEditor->setEnabled(enable);

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
    Calligra::Sheets::MoveTo direction = q->selection()->activeSheet()->fullMap()->applicationSettings()->moveToValue();

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
    moveDirection(direction, false);
    event->accept(); // QKeyEvent
}

void CellToolBase::Private::processArrowKey(QKeyEvent *event)
{
    /* NOTE:  hitting the tab key also calls this function.  Don't forget
        to account for it
    */
     Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return;

    /* If we are not editing a formula, close the current editor. Otherwise we want to fill in the cell coordinates. */
    if (!q->selection()->referenceSelectionMode())
      q->selection()->emitCloseEditor(true);

    Calligra::Sheets::MoveTo direction = Bottom;
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

    moveDirection(direction, makingSelection);
    event->accept(); // QKeyEvent
}

void CellToolBase::Private::processEscapeKey(QKeyEvent * event)
{
    q->selection()->emitCloseEditor(false); // discard changes
    event->accept(); // QKeyEvent
}

bool CellToolBase::Private::processHomeKey(QKeyEvent* event)
{
     Sheet * const sheet = q->selection()->activeSheet();
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

            Cell cell = sheet->fullCellStorage()->firstInRow(marker.y(), CellStorage::VisitContent);
            while (!cell.isNull() && cell.column() < marker.x() && cell.isEmpty()) {
                cell = sheet->fullCellStorage()->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
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
     Sheet * const sheet = q->selection()->activeSheet();
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

        CellStorage *cells = sheet->fullCellStorage();
        cell = cells->lastInRow(marker.y(), CellStorage::VisitContent);
        while (!cell.isNull() && cell.column() > q->selection()->marker().x() && cell.isEmpty()) {
            cell = cells->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
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
     Sheet * const sheet = q->selection()->activeSheet();

    // No null character ...
    if (event->text().isEmpty() || !q->selection()->activeSheet()->fullMap()->isReadWrite() ||
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
     Sheet * const sheet = q->selection()->activeSheet();
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
    CellStorage *cells = sheet->fullCellStorage();

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
                if (!sheet->rowFormats()->isHiddenOrFiltered(cell.row())) {
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
            cell = cells->prevInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

            while ((!cell.isNull()) &&
                    (cell.isEmpty() || (sheet->rowFormats()->isHiddenOrFiltered(cell.row())))) {
                cell = cells->prevInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
            }
        }

        if (cell.isNull())
            row = 1;
        else
            row = cell.row();

        int lastHiddenOrFiltered;
        while (sheet->rowFormats()->isHiddenOrFiltered(row, &lastHiddenOrFiltered)) {
            row = lastHiddenOrFiltered + 1;
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
                if (!(sheet->rowFormats()->isHiddenOrFiltered(cell.row()))) {
                    lastCell = cell;
                    searchThroughEmpty = false;
                }
                row++;
                cell = Cell(sheet, cell.column(), row);
            }
            cell = lastCell;
        }
        if (searchThroughEmpty) {
            cell = cells->nextInColumn(marker.x(), marker.y(), CellStorage::VisitContent);

            while ((!cell.isNull()) &&
                    (cell.isEmpty() || (sheet->rowFormats()->isHiddenOrFiltered(cell.row())))) {
                cell = cells->nextInColumn(cell.column(), cell.row(), CellStorage::VisitContent);
            }
        }

        if (cell.isNull())
            row = marker.y();
        else
            row = cell.row();

        int firstHiddenOrFiltered;
        while (row >= 1 && sheet->rowFormats()->isHiddenOrFiltered(row, 0, &firstHiddenOrFiltered)) {
            row = firstHiddenOrFiltered - 1;
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
                    if (!(sheet->columnFormats()->isHiddenOrFiltered(cell.column()))) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col++;
                    cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = cells->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormats()->isHiddenOrFiltered(cell.column())))) {
                    cell = cells->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = marker.x();
            else
                col = cell.column();

            while (sheet->columnFormats()->isHiddenOrFiltered(col)) {
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
                    if (!(sheet->columnFormats()->isHiddenOrFiltered(cell.column()))) {
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
                cell = cells->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormats()->isHiddenOrFiltered(cell.column())))) {
                    cell = cells->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = 1;
            else
                col = cell.column();

            while (sheet->columnFormats()->isHiddenOrFiltered(col)) {
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
                    if (!(sheet->columnFormats()->isHiddenOrFiltered(cell.column()))) {
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
                cell = cells->prevInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormats()->isHiddenOrFiltered(cell.column())))) {
                    cell = cells->prevInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = 1;
            else
                col = cell.column();

            while (sheet->columnFormats()->isHiddenOrFiltered(col)) {
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
                    if (!(sheet->columnFormats()->isHiddenOrFiltered(cell.column()))) {
                        lastCell = cell;
                        searchThroughEmpty = false;
                    }
                    col++;
                    cell = Cell(sheet, col, cell.row());
                }
                cell = lastCell;
            }
            if (searchThroughEmpty) {
                cell = cells->nextInRow(marker.x(), marker.y(), CellStorage::VisitContent);

                while ((!cell.isNull()) &&
                        (cell.isEmpty() || (sheet->columnFormats()->isHiddenOrFiltered(cell.column())))) {
                    cell = cells->nextInRow(cell.column(), cell.row(), CellStorage::VisitContent);
                }
            }

            if (cell.isNull())
                col = marker.x();
            else
                col = cell.column();

            while (sheet->columnFormats()->isHiddenOrFiltered(col)) {
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
        command->setText(kundo2_i18n("Number Format"));
        command->setFormatType(Format::Number);
        command->setPrecision(2);
        break;

    case Qt::Key_Dollar:
        command->setText(kundo2_i18n("Currency Format"));
        command->setFormatType(Format::Money);
        break;

    case Qt::Key_Percent:
        command->setText(kundo2_i18n("Percentage Format"));
        command->setFormatType(Format::Percentage);
        break;

    case Qt::Key_At:
        command->setText(kundo2_i18n("Time Format"));
        command->setFormatType(Format::SecondeTime);
        break;

    case Qt::Key_NumberSign:
        command->setText(kundo2_i18n("Date Format"));
        command->setFormatType(Format::ShortDate);
        break;

    case Qt::Key_AsciiCircum:
        command->setText(kundo2_i18n("Scientific Format"));
        command->setFormatType(Format::Scientific);
        break;

    case Qt::Key_Ampersand:
        command->setText(kundo2_i18n("Change Border"));
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

QRect CellToolBase::Private::moveDirection(Calligra::Sheets::MoveTo direction, bool extendSelection)
{
    debugSheetsUI << "Canvas::moveDirection";

     Sheet * const sheet = q->selection()->activeSheet();
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
    switch (direction)
        /* for each case, figure out how far away the next cell is and then keep
            going one row/col at a time after that until a visible row/col is found

            NEVER use cell.column() or cell.row() -- it might be a default cell
        */
    {
    case Bottom:
        offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
        while (((cursor.y() + offset) <= q->maxRow()) && sheet->rowFormats()->isHiddenOrFiltered(cursor.y() + offset)) {
            offset++;
        }

        destination = QPoint(cursor.x(), qMin(cursor.y() + offset, q->maxRow()));
        break;
    case Top:
        offset = (cellCorner.y() - cursor.y()) - 1;
        while (((cursor.y() + offset) >= 1) && sheet->rowFormats()->isHiddenOrFiltered(cursor.y() + offset)) {
            offset--;
        }
        destination = QPoint(cursor.x(), qMax(cursor.y() + offset, 1));
        break;
    case Left:
        offset = (cellCorner.x() - cursor.x()) - 1;
        while (((cursor.x() + offset) >= 1) && sheet->columnFormats()->isHiddenOrFiltered(cursor.x() + offset)) {
            offset--;
        }
        destination = QPoint(qMax(cursor.x() + offset, 1), cursor.y());
        break;
    case Right:
        offset = cell.mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
        while (((cursor.x() + offset) <= q->maxCol()) && sheet->columnFormats()->isHiddenOrFiltered(cursor.x() + offset)) {
            offset++;
        }
        destination = QPoint(qMin(cursor.x() + offset, q->maxCol()), cursor.y());
        break;
    case BottomFirst:
        offset = cell.mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
        while (((cursor.y() + offset) <= q->maxRow()) && sheet->rowFormats()->isHiddenOrFiltered(cursor.y() + offset)) {
            ++offset;
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

    const Calligra::Sheets::Selection* selection = q->selection();
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
        Sheet *const sheet = dynamic_cast<Sheet *>((*it)->sheet());
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
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            // See comment in paintSelection().
            const qreal offset = /*2 * viewRect.left() +*/ viewRect.width();
            const qreal left = offset - area.right();
            const qreal right = offset - area.left();
            area.setLeft(left);
            area.setRight(right);
        }

        painter.setBrush(QBrush());
        painter.setPen(QPen(color, 0));
        painter.drawRect(area);

        // Now draw the size grip (the little rectangle on the bottom right-hand corner of
        // the range which the user can click and drag to resize the region)
        painter.setPen(QPen(Qt::white, 0));
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
    QList<QAction*> popupActions;
    const Cell cell = Cell(q->selection()->activeSheet(), q->selection()->marker());
    const bool isProtected = !q->selection()->activeSheet()->fullMap()->isReadWrite() ||
                             (q->selection()->activeSheet()->isProtected() &&
                              !(cell.style().notProtected() && q->selection()->isSingular()));
    if (!isProtected) {
        popupActions.append(q->action("cellStyle"));
        popupActions.append(popupMenuActions["separator1"]);
        popupActions.append(q->action("cut"));
    }
    popupActions.append(q->action("copy"));
    if (!isProtected) {
        popupActions.append(q->action("paste"));
        popupActions.append(q->action("specialPaste"));
        popupActions.append(q->action("pasteWithInsertion"));
        popupActions.append(popupMenuActions["separator2"]);
        popupActions.append(q->action("clearAll"));
        popupActions.append(q->action("adjust"));
        popupActions.append(q->action("setDefaultStyle"));
        popupActions.append(q->action("setAreaName"));

        if (!q->selection()->isColumnOrRowSelected()) {
            popupActions.append(popupMenuActions["separator3"]);
            popupActions.append(popupMenuActions["insertCell"]);
            popupActions.append(popupMenuActions["deleteCell"]);
        } else if (q->selection()->isColumnSelected()) {
            popupActions.append(actions->action("resizeCol"));
            popupActions.append(actions->action("adjustColumn"));
            popupActions.append(popupMenuActions["separator4"]);
            popupActions.append(actions->action("insertColumn"));
            popupActions.append(actions->action("deleteColumn"));
            popupActions.append(actions->action("hideColumn"));
            popupActions.append(actions->action("showSelColumns"));
        } else if (q->selection()->isRowSelected()) {
            popupActions.append(actions->action("resizeRow"));
            popupActions.append(actions->action("adjustRow"));
            popupActions.append(popupMenuActions["separator5"]);
            popupActions.append(actions->action("insertRow"));
            popupActions.append(actions->action("deleteRow"));
            popupActions.append(actions->action("hideRow"));
            popupActions.append(actions->action("showSelRows"));
        }
        popupActions.append(popupMenuActions["separator6"]);
        popupActions.append(popupMenuActions["comment"]);
        if (!cell.comment().isEmpty()) {
            popupActions.append(popupMenuActions["clearComment"]);
        }

        if (testListChoose(q->selection())) {
            popupActions.append(popupMenuActions["separator7"]);
            popupActions.append(popupMenuActions["listChoose"]);
        }
    }
    return popupActions;
}

void CellToolBase::Private::createPopupMenuActions()
{
    QAction* action = 0;

    for (int i = 1; i <= 7; ++i) {
        action = new QAction(q);
        action->setSeparator(true);
        popupMenuActions.insert(QString("separator%1").arg(i), action);
    }

    action = new QAction(koIcon("insertcell"), i18n("Insert Cells..."), q);
    connect(action, &QAction::triggered, q, &CellToolBase::insertCells);
    popupMenuActions.insert("insertCell", action);

    action = new QAction(koIcon("removecell"), i18n("Delete Cells..."), q);
    connect(action, &QAction::triggered, q, &CellToolBase::deleteCells);
    popupMenuActions.insert("deleteCell", action);

    action = new QAction(i18n("Selection List..."), q);
    connect(action, &QAction::triggered, q, &CellToolBase::listChoosePopupMenu);
    popupMenuActions.insert("listChoose", action);

    action = new QAction(koIcon("edit-comment"), i18n("Comment"), q);
    connect(action, &QAction::triggered, q, &CellToolBase::comment);
    popupMenuActions.insert("comment", action);

    action = new QAction(koIcon("delete-comment"),i18n("Clear Comment"), q);
    connect(action, &QAction::triggered, q, &CellToolBase::clearComment);
    popupMenuActions.insert("clearComment", action);

}

bool CellToolBase::Private::testListChoose(Selection *selection) const
{
    Sheet *sheet = selection->activeSheet();
    const Cell cursorCell(sheet, selection->cursor());
    const CellStorage *const storage = sheet->fullCellStorage();

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
