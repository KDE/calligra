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
#include "core/ApplicationSettings.h"
#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/DocBase.h"
#include "core/Map.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"

// ui
#include "CellEditor.h"
#include "ExternalEditor.h"
#include "actions/Actions.h"
#include "actions/CellAction.h"

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoColor.h>
#include <KoIcon.h>
#include <KoViewConverter.h>

// Qt
#include <QAction>
#include <QApplication>
#include <QPainter>

#include <KLocalizedString>

using namespace Calligra::Sheets;

void CellToolBase::Private::updateEditor(const Cell &cell)
{
    if (!externalEditor)
        return;
    const Cell &theCell = cell.isPartOfMerged() ? cell.masterCell() : cell;
    const Style style = theCell.style();
    if (q->selection()->activeSheet()->isProtected() && style.hideFormula()) {
        externalEditor->setPlainText(theCell.displayText());
    } else if (q->selection()->activeSheet()->isProtected() && style.hideAll()) {
        externalEditor->clear();
    } else {
        externalEditor->setPlainText(theCell.userInput());
    }
}

QPoint CellToolBase::Private::moveBy(const QPoint &point, int dx, int dy)
{
    int newx = point.x() + dx;
    int newy = point.y() + dy;
    if (newx < 1)
        newx = 1;
    if (newy < 1)
        newy = 1;
    if (newx > KS_colMax)
        newx = KS_colMax;
    if (newy > KS_rowMax)
        newy = KS_rowMax;
    return QPoint(newx, newy);
}

QPoint CellToolBase::Private::destinationForKey(QKeyEvent *event)
{
    int key = event->key();
    bool ctrl = (event->modifiers() & Qt::ControlModifier);

    Selection *sel = q->selection();
    Sheet *sheet = sel->activeSheet();
    if (!sheet)
        return QPoint(0, 0); // This shouldn't happen ...

    QPoint cursor = sel->cursor();
    Cell cell = Cell(sheet, cursor.x(), cursor.y());

    // Cursor keys + Tab + Enter
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        if (key == Qt::Key_Left)
            key = Qt::Key_Right;
        else if (key == Qt::Key_Right)
            key = Qt::Key_Left;
    }

    if (ctrl) {
        if ((key == Qt::Key_Return) || (key == Qt::Key_Enter))
            return QPoint(0, 0);
        if ((key == Qt::Key_Tab) || (key == Qt::Key_Backtab))
            return QPoint(0, 0);

        if (key == Qt::Key_Down)
            return nextMarginCellInDirection(cell, Bottom).cellPosition();
        if (key == Qt::Key_Up)
            return nextMarginCellInDirection(cell, Top).cellPosition();
        if (key == Qt::Key_Left)
            return nextMarginCellInDirection(cell, Left).cellPosition();
        if (key == Qt::Key_Right)
            return nextMarginCellInDirection(cell, Right).cellPosition();
    } else {
        if ((key == Qt::Key_Return) || (key == Qt::Key_Enter)) {
            Calligra::Sheets::MoveTo direction = q->selection()->activeSheet()->fullMap()->applicationSettings()->moveToValue();
            // If Shift is pressed, reverse move direction
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
                default:
                    break;
                }
            }
            if (direction == Top)
                key = Qt::Key_Up;
            else if (direction == Bottom)
                key = Qt::Key_Down;
            else if (direction == Left)
                key = Qt::Key_Left;
            else if (direction == Right)
                key = Qt::Key_Right;

            if (direction == NoMovement)
                return cursor; // Nothing to do.
            if (direction == BottomFirst)
                return moveBy(QPoint(1, cursor.y()), 0, 1); // using moveBy in case we're on the bottom-most row
        }

        if (key == Qt::Key_Down)
            return visibleCellInDirection(cursor, sheet, Bottom);
        if (key == Qt::Key_Up)
            return visibleCellInDirection(cursor, sheet, Top);
        if (key == Qt::Key_Left)
            return visibleCellInDirection(cursor, sheet, Left);
        if (key == Qt::Key_Right)
            return visibleCellInDirection(cursor, sheet, Right);
        if (key == Qt::Key_Tab)
            return visibleCellInDirection(cursor, sheet, Right);
        if (key == Qt::Key_Backtab)
            return visibleCellInDirection(cursor, sheet, Left);
    }

    // Home + End
    if (key == Qt::Key_Home) {
        // ctrl + Home will always just send us to location (1,1)
        return QPoint(1, ctrl ? 1 : cursor.y());
    }
    if (key == Qt::Key_End) {
        QRect used = sheet->usedArea();
        if (!used.right())
            return QPoint(1, 1);
        // ctrl + Home will always just send us to the last used cell
        return QPoint(used.right(), ctrl ? used.bottom() : cursor.y());
    }
    if (key == Qt::Key_PageUp) {
        // TODO - actually determine this length
        return moveBy(cursor, 0, -25);
    }
    if (key == Qt::Key_PageDown) {
        // TODO - actually determine this length
        return moveBy(cursor, 0, 25);
    }

    return QPoint(0, 0);
}

void CellToolBase::Private::moveToDestination(QPoint destination, bool extendSelection)
{
    Selection *sel = q->selection();
    Sheet *sheet = sel->activeSheet();

    if (extendSelection)
        sel->update(destination);
    else
        sel->initialize(destination, sheet);

    q->scrollToCell(destination);
    updateEditor(Cell(sheet, destination));
}

bool CellToolBase::Private::handleMovementKeys(QKeyEvent *event)
{
    QPoint target = destinationForKey(event);
    if (target.x() <= 0)
        return false; // Not a movement key.

    Selection *sel = q->selection();
    // If we are not editing a formula, close the current editor. Otherwise we want to fill in the cell coordinates.
    if (!sel->referenceSelectionMode())
        sel->emitCloseEditor(true);

    QPoint cursor = sel->cursor();
    // If the cursor already is where it should (e.g. pressing Up while on the top row), there is nothing more to do.
    if (cursor == target) {
        event->accept();
        return true;
    }

    bool makingSelection = event->modifiers() & Qt::ShiftModifier;
    if (event->key() == Qt::Key_Backtab)
        makingSelection = false;

    moveToDestination(target, makingSelection);

    event->accept();
    return true;
}

// Helper for control-movement. Returns the next cell.
Cell CellToolBase::Private::nextMarginCellInDirection(const Cell &cell, Calligra::Sheets::MoveTo direction)
{
    if (direction == NoMovement)
        return cell; // shouldn't happen
    if (cell.isNull())
        return cell;

    QPoint cursor = cell.cellPosition();
    Sheet *sheet = cell.fullSheet();
    const CellStorage *storage = sheet->fullCellStorage();

    QPoint next = visibleCellInDirection(cursor, sheet, direction);
    if (next == cursor)
        return cell; // no movement

    Cell nextCell(sheet, next);
    if (!nextCell.isEmpty()) {
        // If our cell was empty, we're now done
        if (cell.isEmpty())
            return nextCell;

        // otherwise, we want to keep going until we find an empty one. Then we return the one immediately before.
        Cell res = nextCell;
        while (!nextCell.isEmpty()) {
            res = nextCell;
            QPoint nextcursor = nextCell.cellPosition();
            next = visibleCellInDirection(nextcursor, sheet, direction);
            if (next == nextcursor)
                break; // we have hit the edge
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
            if (c.isNull())
                return Cell(sheet, nextCell.column(), KS_rowMax);
            nextCell = Cell(sheet, nextCell.column(), c.row() - 1);
        }
        if (direction == Top) {
            Cell c = storage->prevInColumn(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull())
                return Cell(sheet, nextCell.column(), 1);
            nextCell = Cell(sheet, nextCell.column(), c.row() + 1);
        }
        if (direction == Right) {
            Cell c = storage->nextInRow(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull())
                return Cell(sheet, KS_colMax, nextCell.row());
            nextCell = Cell(sheet, c.column() - 1, nextCell.row());
        }
        if (direction == Left) {
            Cell c = storage->prevInRow(nextCell.column(), nextCell.row(), CellStorage::VisitContent);
            // hit the end?
            if (c.isNull())
                return Cell(sheet, 1, nextCell.row());
            nextCell = Cell(sheet, c.column() + 1, nextCell.row());
        }

        QPoint nextcursor = nextCell.cellPosition();
        next = visibleCellInDirection(nextcursor, sheet, direction);
        if (next == nextcursor)
            return nextCell; // this shouldn't happen, but just in case ...
        nextCell = Cell(sheet, next);
    }
    return nextCell;
}

void CellToolBase::Private::triggerAction(const QString &name)
{
    CellAction *a = actions->cellAction(name);
    if (a)
        a->trigger();
}

bool CellToolBase::Private::formatKeyPress(QKeyEvent *_ev)
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
    cell = cell.masterCell(); // in case it's part of a merged one
    int first = 0, last = 0;
    switch (direction) {
    case NoMovement:
        return point;
    case BottomFirst:
    case Bottom:
        row += cell.mergedYCells() + 1;
        if (direction == BottomFirst)
            col = 1;
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
    if ((row < 1) || (col < 1) || (row > q->maxRow()) || (col > q->maxCol()))
        return point;
    return QPoint(col, row);
}

void CellToolBase::Private::paintSelection(QPainter &painter, const QRectF &viewRect)
{
    if (q->selection()->referenceSelection() || q->editor()) {
        return;
    }
    Sheet *sheet = q->selection()->activeSheet();

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

    const Calligra::Sheets::Selection *selection = q->selection();
    QRect selected{selection->lastRange()};

    Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        Sheet *esheet = dynamic_cast<Sheet *>((*it)->sheet());
        if (sheet != esheet)
            continue;
        QRect range = (*it)->rect();
        // Only the active element (the one with the anchor) will be drawn with a border
        const bool current = (range == selected);

        range = selection->extendToMergedAreas(range, esheet);

        double positions[4];
        bool paintSides[4];
        retrieveMarkerInfo(range, viewRect, positions, paintSides);

        double left = positions[0];
        double top = positions[1];
        double right = positions[2];
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

        bool paintLeft = paintSides[0];
        bool paintTop = paintSides[1];
        bool paintRight = paintSides[2];
        bool paintBottom = paintSides[3];
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            paintLeft = paintSides[2];
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
            const QRect extCursor = selection->extendToMergedAreas(cursor, selection->activeSheet());
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
                line = QLineF(left + 4 * unzoomedPixelX, bottom, right + unzoomedPixelY, bottom);
                painter.drawLine(line);
                painter.fillRect(QRectF(left - 2 * unzoomedPixelX, bottom - 2 * unzoomedPixelY, 5 * unzoomedPixelX, 5 * unzoomedPixelY), painter.pen().color());
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
                painter.fillRect(QRectF(right - 2 * unzoomedPixelX, bottom - 2 * unzoomedPixelX, 5 * unzoomedPixelX, 5 * unzoomedPixelY),
                                 painter.pen().color());
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

        const QRect range = q->selection()->extendToMergedAreas((*it)->rect(), sheet);
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

void CellToolBase::Private::retrieveMarkerInfo(const QRect &cellRange, const QRectF &viewRect, double positions[], bool paintSides[])
{
    // Everything is in document coordinates here.
    // The layout direction, which is view dependent, is applied afterwards.

    const Sheet *sheet = q->selection()->activeSheet();
    const QRectF visibleRect = sheet->cellCoordinatesToDocument(cellRange);

    /* these vars are used for clarity, the array for simpler function arguments  */
    qreal left = visibleRect.left();
    qreal top = visibleRect.top();
    qreal right = visibleRect.right();
    qreal bottom = visibleRect.bottom();

    /* left, top, right, bottom */
    paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) && (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom()) && (right >= viewRect.left()) && (left <= viewRect.right());
    paintSides[2] = (viewRect.left() <= right) && (right <= viewRect.right()) && (bottom >= viewRect.top()) && (top <= viewRect.bottom());
    paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom()) && (right >= viewRect.left()) && (left <= viewRect.right());

    positions[0] = qMax(left, viewRect.left());
    positions[1] = qMax(top, viewRect.top());
    positions[2] = qMin(right, viewRect.right());
    positions[3] = qMin(bottom, viewRect.bottom());
}

QList<QAction *> CellToolBase::Private::popupActionList() const
{
    Selection *sel = q->selection();
    QList<QAction *> popupActions;
    const Cell cell = Cell(sel->activeSheet(), sel->cursor());
    const bool isProtected = (!sel->activeSheet()->fullMap()->isReadWrite()) || sel->isProtected();
    if (!isProtected) {
        popupActions.append(actions->action("cellStyle"));
        popupActions.append(popupMenuActions["separator1"]);
        popupActions.append(actions->action("cut"));
    }
    popupActions.append(actions->action("copy"));
    if (!isProtected) {
        popupActions.append(actions->action("paste"));
        popupActions.append(actions->action("specialPaste"));
        popupActions.append(actions->action("pasteWithInsertion"));
        popupActions.append(popupMenuActions["separator2"]);
        popupActions.append(actions->action("clearAll"));
        popupActions.append(actions->action("adjust"));
        popupActions.append(actions->action("setDefaultStyle"));
        popupActions.append(actions->action("setAreaName"));

        if (!sel->isColumnOrRowSelected()) {
            popupActions.append(popupMenuActions["separator3"]);
            popupActions.append(actions->action("insertCell"));
            popupActions.append(actions->action("deleteCell"));
        } else if (sel->isColumnSelected()) {
            popupActions.append(actions->action("resizeCol"));
            popupActions.append(actions->action("adjustColumn"));
            popupActions.append(popupMenuActions["separator4"]);
            popupActions.append(actions->action("insertColumn"));
            popupActions.append(actions->action("deleteColumn"));
            popupActions.append(actions->action("hideColumn"));
            popupActions.append(actions->action("showSelColumns"));
        } else if (sel->isRowSelected()) {
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

        bool readWrite = sel->activeSheet()->fullMap()->doc()->isReadWrite();
        if (actions->cellAction("listChoose")->shouldBeEnabled(readWrite, sel, cell)) {
            popupActions.append(popupMenuActions["separator7"]);
            popupActions.append(actions->action("listChoose"));
        }
    }
    return popupActions;
}

void CellToolBase::Private::createPopupMenuActions()
{
    QAction *action = nullptr;

    for (int i = 1; i <= 7; ++i) {
        action = new QAction(q);
        action->setSeparator(true);
        popupMenuActions.insert(QString("separator%1").arg(i), action);
    }
}
