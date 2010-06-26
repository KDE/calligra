/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "SelectionStrategy.h"

#include "CellToolBase.h"
#include "kspread_limits.h"
#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

using namespace KSpread;

class SelectionStrategy::Private
{
public:
    Cell startCell;
};

SelectionStrategy::SelectionStrategy(CellToolBase *cellTool,
                                     const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
        , d(new Private)
{
    d->startCell = Cell();

    const KoShape* shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));
    Sheet *const sheet = this->selection()->activeSheet();
    Selection *const selection = this->selection();

#if 0 // KSPREAD_WIP_DRAG_REFERENCE_SELECTION
    // Check, if the selected area was hit.
    bool hitSelection = false;
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        // Only process ranges on the active sheet.
        if (sheet != (*it)->sheet()) {
            continue;
        }
        const QRect range = (*it)->rect();
        if (sheet->cellCoordinatesToDocument(range).contains(position)) {
            hitSelection = true;
            break;
        }
    }
#endif // KSPREAD_WIP_DRAG_REFERENCE_SELECTION

    // In which cell did the user click?
    double xpos;
    double ypos;
    const int col = sheet->leftColumn(position.x(), xpos);
    const int row = sheet->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        d->startCell = Cell(sheet, col, row);
        if (selection->referenceSelectionMode()) {
            selection->emitRequestFocusEditor();
            const bool sizeGripHit = hitTestReferenceSizeGrip(tool()->canvas(), selection, position);
            const bool shiftPressed = modifiers & Qt::ShiftModifier;
            if (sizeGripHit || shiftPressed) {
                selection->update(QPoint(col, row));
            } else if (modifiers & Qt::ControlModifier) {
                // Extend selection, if control modifier is pressed.
                selection->extend(QPoint(col, row), sheet);
#if 0 // KSPREAD_WIP_DRAG_REFERENCE_SELECTION
            } else if (hitSelection) {
                // start cell is already set above
                // No change; the range will be moved
#endif // KSPREAD_WIP_DRAG_REFERENCE_SELECTION
            } else {
                selection->initialize(QPoint(col, row), sheet);
            }
        } else {
            selection->emitCloseEditor(true);
            if (modifiers & Qt::ControlModifier) {
                // Extend selection, if control modifier is pressed.
                selection->extend(QPoint(col, row), sheet);
            } else if (modifiers & Qt::ShiftModifier) {
                selection->update(QPoint(col, row));
            } else {
                selection->initialize(QPoint(col, row), sheet);
            }
        }
    }
    tool()->repaintDecorations();
}

SelectionStrategy::~SelectionStrategy()
{
    delete d;
}

void SelectionStrategy::handleMouseMove(const QPointF &documentPos,
                                        Qt::KeyboardModifiers modifiers)
{
#if 0 // KSPREAD_WIP_DRAG_REFERENCE_SELECTION
    Q_UNUSED(modifiers);
    const KoShape* shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));
    Sheet *const sheet = selection()->activeSheet();

    if (selection()->referenceSelectionMode()) {
        // In which cell did the user move?
        double xpos;
        double ypos;
        const int col = sheet->leftColumn(position.x(), xpos);
        const int row = sheet->topRow(position.y(), ypos);
        // Check boundaries.
        if (col > KS_colMax || row > KS_rowMax) {
            kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
        } else if (!(d->startCell == Cell(sheet, col, row))) {
            const QRect range = selection()->activeElement()->rect();
            const QPoint offset = d->startCell.cellPosition() - range.topLeft();
            const QPoint topLeft = QPoint(col, row) + offset;
            selection()->initialize(QRect(topLeft, range.size()), sheet);
            return;
        }
    }
#endif // KSPREAD_WIP_DRAG_REFERENCE_SELECTION
    AbstractSelectionStrategy::handleMouseMove(documentPos, modifiers);
}
