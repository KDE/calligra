// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
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
#include "actions/CellAction.h"

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoViewConverter.h>
#include <KoColor.h>
#include <KoIcon.h>

// Qt
#include <QAction>
#include <QApplication>
#include <QPainter>

#include <KLocalizedString>


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

Calligra::Sheets::MoveTo CellToolBase::Private::directionForKey(int key) {
    Sheet * const sheet = q->selection()->activeSheet();
    if (key == Qt::Key_Down) return Bottom;
    if (key == Qt::Key_Up) return Top;
    if (key == Qt::Key_Left) {
        if (sheet->layoutDirection() == Qt::RightToLeft)
            return Right;
        return Left;
    }
    if (key == Qt::Key_Right) {
        if (sheet->layoutDirection() == Qt::RightToLeft)
            return Left;
        return Right;
    }
    if (key == Qt::Key_Tab) return Right;
    //Shift+Tab moves to the left
    if (key == Qt::Key_Backtab) return Left;
    Q_ASSERT(false);
    return NoMovement;
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

    Calligra::Sheets::MoveTo direction = directionForKey(event->key());
    bool makingSelection = event->modifiers() & Qt::ShiftModifier;
    if (event->key() == Qt::Key_Backtab) makingSelection = false;

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

// Helper for control-movement. Returns the next cell.
Cell CellToolBase::Private::nextMarginCellInDirection(const Cell &cell, Calligra::Sheets::MoveTo direction)
{
    if (direction == NoMovement) return cell;   // shouldn't happen
    if (cell.isNull()) return cell;

    QPoint cursor = cell.cellPosition();
    Sheet *sheet = cell.fullSheet();
    const CellStorage *storage = sheet->fullCellStorage();

    QPoint next = visibleCellInDirection(cursor, sheet, direction);
    if (next == cursor) return cell;  // no movement

    Cell nextCell(sheet, next);
    if (!nextCell.isEmpty()) {
        // If our cell was empty, we're now done
        if (cell.isEmpty()) return nextCell;

        // otherwise, we want to keep going until we find an empty one. Then we return the one immediately before.
        Cell res = nextCell;
        while (!nextCell.isEmpty()) {
            res = nextCell;
            QPoint nextcursor = nextCell.cellPosition();
            next = visibleCellInDirection(nextcursor, sheet, direction);
            if (next == nextcursor) break;  // we have hit the edge
            nextCell = Cell(sheet, next);
        }
        return res;
    }

    // The next cell is empty. We want to keep going until we find a non-empty one, or the end.
    // We'll advance the index past all the empty cells, to speed things up.
    while (nextCell.isEmpty()) {
        if (direction == Bottom) {
            Cell c = storage->nextInColumn(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull()) return Cell(sheet, nextCell.column(), KS_rowMax);
            nextCell = Cell(sheet, nextCell.column(), c.row() - 1);
        }
        if (direction == Top) {
            Cell c = storage->prevInColumn(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull()) return Cell(sheet, nextCell.column(), 1);
            nextCell = Cell(sheet, nextCell.column(), c.row() + 1);
        }
        if (direction == Right) {
            Cell c = storage->nextInRow(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull()) return Cell(sheet, KS_colMax, nextCell.row());
            nextCell = Cell(sheet, c.column() - 1, nextCell.row());
        }
        if (direction == Left) {
            Cell c = storage->prevInRow(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull()) return Cell(sheet, 1, nextCell.row());
            nextCell = Cell(sheet, c.column() + 1, nextCell.row());
        }

        QPoint nextcursor = nextCell.cellPosition();
        next = visibleCellInDirection(nextcursor, sheet, direction);
        if (next == nextcursor) return nextCell;   // this shouldn't happen, but just in case ...
        nextCell = Cell(sheet, next);
    }
    return nextCell;
}

bool CellToolBase::Private::processControlArrowKey(QKeyEvent *event)
{
    Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return false;

    Calligra::Sheets::MoveTo direction = directionForKey(event->key());
    bool makingSelection = event->modifiers() & Qt::ShiftModifier;

    QPoint marker = q->selection()->marker();
    Cell cell = Cell(sheet, marker.x(), marker.y());

    Cell tgcell = nextMarginCellInDirection(cell, direction);
    QPoint destination = tgcell.cellPosition();
    if (destination == marker) return false;

    if (makingSelection) {
        q->selection()->update(destination);
    } else {
        q->selection()->initialize(destination, sheet);
    }
    q->scrollToCell(destination);
    return true;
}

void CellToolBase::Private::triggerAction(const QString &name)
{
    CellAction *a = actions->cellAction(name);
    if (a) a->trigger();
}

bool CellToolBase::Private::formatKeyPress(QKeyEvent * _ev)
{
    if (!(_ev->modifiers() & Qt::ControlModifier))
        return false;

    int key = _ev->key();
    if (key == Qt::Key_Exclam)
        triggerAction("numeric");
    else if (key == Qt::Key_AsciiCircum)
        triggerAction("scientific");
    else if (key == Qt::Key_Dollar)
        triggerAction("currency");
    else if (key == Qt::Key_Percent)
        triggerAction("percent");
    else if (key == Qt::Key_At)
        triggerAction("time");
    else if (key == Qt::Key_NumberSign)
        triggerAction("date");
    else if (key == Qt::Key_Ampersand)
        triggerAction("borderOutline");
    else 
        return false;

    // Processed.
    _ev->accept();
    return true;
}

QPoint CellToolBase::Private::visibleCellInDirection(QPoint point, Sheet *sheet, Calligra::Sheets::MoveTo direction)
{
    int col = point.x();
    int row = point.y();
    Cell cell(sheet, col, row);
    cell = cell.masterCell();   // in case it's part of a merged one
    int first = 0, last = 0;
    switch (direction) {
        case NoMovement:
            return point;
        case BottomFirst:
        case Bottom:
            row += cell.mergedYCells() + 1;
            if (direction == BottomFirst) col = 1;
            while ((row <= q->maxRow()) && sheet->rowFormats()->isHiddenOrFiltered(row, &last, &first))
                row = last + 1;
            break;
        case Top:
            // Can't do row-- as we could be in the middle of a merged cell
            row = cell.row() - 1;
            while ((row >= 1) && sheet->rowFormats()->isHiddenOrFiltered(row, &last, &first))
                row = first - 1;
            break;
        case Left:
            col = cell.column() - 1;
            while ((col >= 1) && sheet->columnFormats()->isHiddenOrFiltered(col, &last, &first))
                col = first - 1;
            break;
        case Right:
            col += cell.mergedXCells() + 1;
            while ((col <= q->maxCol()) && sheet->columnFormats()->isHiddenOrFiltered(col, &last, &first))
                col = last + 1;
            break;
    }
    if ((row < 1) || (col < 1) || (row > q->maxRow()) || (col > q->maxCol())) return point;
    return QPoint(col, row);
}

QRect CellToolBase::Private::moveDirection(Calligra::Sheets::MoveTo direction, bool extendSelection)
{
    debugSheetsUI << "Canvas::moveDirection";

     Sheet * const sheet = q->selection()->activeSheet();
    if (!sheet)
        return QRect();

    QPoint cursor = q->selection()->cursor();
    QPoint destination = CellToolBase::Private::visibleCellInDirection(cursor, sheet, direction);

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
        popupActions.append(actions->action("clearAll"));
        popupActions.append(actions->action("adjust"));
        popupActions.append(actions->action("setDefaultStyle"));
        popupActions.append(actions->action("setAreaName"));

        if (!q->selection()->isColumnOrRowSelected()) {
            popupActions.append(popupMenuActions["separator3"]);
            popupActions.append(actions->action("insertCell"));
            popupActions.append(actions->action("deleteCell"));
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
        popupActions.append(actions->action("comment"));
        if (!cell.comment().isEmpty()) {
            popupActions.append(actions->action("clearComment"));
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

    action = new QAction(i18n("Selection List..."), q);
    connect(action, &QAction::triggered, q, &CellToolBase::listChoosePopupMenu);
    popupMenuActions.insert("listChoose", action);
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
