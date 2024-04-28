/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SelectionStrategy.h"

#include "../CellEditorBase.h"
#include "../CellToolBase.h"
#include "../Selection.h"
#include "core/Sheet.h"
#include "engine/calligra_sheets_limits.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SelectionStrategy::Private
{
public:
    Cell startCell;
};

SelectionStrategy::SelectionStrategy(CellToolBase *cellTool, const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
    : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
    , d(new Private)
{
    d->startCell = Cell();

    const QPointF position = documentPos;
    Sheet *const sheet = this->selection()->activeSheet();
    Selection *const selection = this->selection();

#if 0 // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION
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
#endif // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    const int col = sheet->leftColumn(position.x(), xpos);
    const int row = sheet->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        debugSheetsUI << "col or row is out of range:"
                      << "col:" << col << " row:" << row;
    } else {
        d->startCell = Cell(sheet, col, row);
        if (selection->referenceSelectionMode()) {
            selection->emitRequestFocusEditor();
            const bool sizeGripHit = hitTestReferenceSizeGrip(tool()->canvas(), selection, position);
            const bool shiftPressed = modifiers & Qt::ShiftModifier;
            if (sizeGripHit) {
                // FIXME The size grip is partly located in the adjacent cells.
                // Activate the selection's location/range.
                const int index = selection->setActiveElement(d->startCell);
                // If successful, activate the editor's location/range.
                if (index >= 0 && cellTool->editor()) {
                    cellTool->editor()->setActiveSubRegion(index);
                }
                selection->update(QPoint(col, row));
            } else if (shiftPressed) {
                selection->update(QPoint(col, row));
            } else if (modifiers & Qt::ControlModifier) {
                // Extend selection, if control modifier is pressed.
                selection->extend(QPoint(col, row), sheet);
#if 0 // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION
            } else if (hitSelection) {
                // start cell is already set above
                // No change; the range will be moved
#endif // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION
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

void SelectionStrategy::handleMouseMove(const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
{
#if 0 // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION
    Q_UNUSED(modifiers);
    //const KoShape* shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos /*- (shape ? shape->position() : QPointF(0.0, 0.0))*/;
    Sheet *const sheet = selection()->activeSheet();

    if (selection()->referenceSelectionMode()) {
        // In which cell did the user move?
        double xpos;
        double ypos;
        const int col = sheet->leftColumn(position.x(), xpos);
        const int row = sheet->topRow(position.y(), ypos);
        // Check boundaries.
        if (col > KS_colMax || row > KS_rowMax) {
            debugSheetsUI << "col or row is out of range:" << "col:" << col << " row:" << row;
        } else if (!(d->startCell == Cell(sheet, col, row))) {
            const QRect range = selection()->activeElement()->rect();
            const QPoint offset = d->startCell.cellPosition() - range.topLeft();
            const QPoint topLeft = QPoint(col, row) + offset;
            selection()->initialize(QRect(topLeft, range.size()), sheet);
            return;
        }
    }
#endif // CALLIGRA_SHEETS_WIP_DRAG_REFERENCE_SELECTION
    AbstractSelectionStrategy::handleMouseMove(documentPos, modifiers);
}
