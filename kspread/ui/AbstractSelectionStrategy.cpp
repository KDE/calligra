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

#include "AbstractSelectionStrategy.h"

#include "CellToolBase.h"
#include "kspread_limits.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

using namespace KSpread;

class AbstractSelectionStrategy::Private
{
public:
    CellToolBase *cellTool;
    QPointF start;
};

AbstractSelectionStrategy::AbstractSelectionStrategy(CellToolBase *cellTool,
        const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : KoInteractionStrategy(cellTool)
        , d(new Private)
{
    Q_UNUSED(modifiers)
    d->cellTool = cellTool;
    d->start = documentPos;
}

AbstractSelectionStrategy::~AbstractSelectionStrategy()
{
    delete d;
}

void AbstractSelectionStrategy::handleMouseMove(const QPointF& documentPos, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    Selection *const selection = d->cellTool->selection();
    const KoShape* shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
        return;
    }
    // Test whether mouse is over the Selection.handle
    if (hitTestSelectionSizeGrip(tool()->canvas(), selection, position)) {
        // If the cursor is over the handle, than it might be already on the next cell.
        // Recalculate the cell position!
        col = selection->activeSheet()->leftColumn(position.x() - tool()->canvas()->viewConverter()->viewToDocumentX(2.0), xpos);
        row = selection->activeSheet()->topRow(position.y() - tool()->canvas()->viewConverter()->viewToDocumentY(2.0), ypos);
    }
    // Update the selection.
    selection->update(QPoint(col, row));
    tool()->repaintDecorations();
}

QUndoCommand* AbstractSelectionStrategy::createCommand()
{
    return 0;
}

void AbstractSelectionStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    tool()->repaintDecorations();
}

// static
bool AbstractSelectionStrategy::hitTestSelectionSizeGrip(KoCanvasBase *canvas,
                                                         Selection *selection,
                                                         const QPointF &position)
{
    if (selection->referenceSelectionMode() || !selection->isValid()) {
        return false;
    }

    // Define the (normal) selection size grip.
    const qreal pixelX = canvas->viewConverter()->viewToDocumentX(1);
    const qreal pixelY = canvas->viewConverter()->viewToDocumentY(1);
    const QRectF gripArea(-2 * pixelX, -2 * pixelY, 5 * pixelX, 5 * pixelY);

    Sheet *const sheet = selection->activeSheet();

    int column, row;
    if (selection->isColumnOrRowSelected()) {
        // complete rows/columns are selected, use the marker.
        const QPoint marker = selection->marker();
        column = marker.x();
        row = marker.y();
    } else {
        const QRect range = selection->lastRange();
        column = range.right();
        row = range.bottom();
    }

    const double xpos = sheet->columnPosition(column);
    const double ypos = sheet->rowPosition(row);
    const double width = sheet->columnFormat(column)->width();
    const double height = sheet->rowFormat(row)->height();
    return gripArea.translated(xpos + width, ypos + height).contains(position);
}

// static
bool AbstractSelectionStrategy::hitTestReferenceSizeGrip(KoCanvasBase *canvas,
                                                         Selection *selection,
                                                         const QPointF &position)
{
    if (!selection->referenceSelectionMode() || !selection->isValid()) {
        return false;
    }

    // Define the reference selection size grip.
    const qreal pixelX = canvas->viewConverter()->viewToDocumentX(1);
    const qreal pixelY = canvas->viewConverter()->viewToDocumentY(1);
    const QRectF gripArea(-3 * pixelX, -3 * pixelY, 6 * pixelX, 6 * pixelY);

    // Iterate over the referenced ranges.
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        // Only check the ranges on the active sheet.
        if (sheet != selection->activeSheet()) {
            continue;
        }
        const QRect range = (*it)->rect();
        const QRectF area = sheet->cellCoordinatesToDocument(range);
        const QPointF corner(area.bottomRight());
        if (gripArea.translated(corner).contains(position)) {
            return true;
        }
    }
    return false;
}

CellToolBase *AbstractSelectionStrategy::cellTool() const
{
    return d->cellTool;
}

Selection* AbstractSelectionStrategy::selection() const
{
    return d->cellTool->selection();
}

const QPointF& AbstractSelectionStrategy::startPosition() const
{
    return d->start;
}
