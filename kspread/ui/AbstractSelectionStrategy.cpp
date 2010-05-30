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

#include "Limits.h"
#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoToolBase.h>

using namespace KSpread;

class AbstractSelectionStrategy::Private
{
public:
    Selection* selection;
    QPointF start;
};

AbstractSelectionStrategy::AbstractSelectionStrategy(KoToolBase *parent, Selection *selection,
        const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : KoInteractionStrategy(parent)
        , d(new Private)
{
    Q_UNUSED(modifiers)
    d->selection = selection;
    d->start = documentPos;
}

AbstractSelectionStrategy::~AbstractSelectionStrategy()
{
    delete d;
}

void AbstractSelectionStrategy::handleMouseMove(const QPointF& documentPos, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    const KoShape *shape = tool()->canvas()->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - (shape ? shape->position() : QPointF(0.0, 0.0));
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = d->selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = d->selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
        return;
    }
    // Test whether mouse is over the Selection.handle
    const QRectF selectionHandle = d->selection->selectionHandleArea(tool()->canvas()->viewConverter());
    if (selectionHandle.contains(position)) {
        // If the cursor is over the handle, than it might be already on the next cell.
        // Recalculate the cell position!
        col = d->selection->activeSheet()->leftColumn(position.x() - tool()->canvas()->viewConverter()->viewToDocumentX(2.0), xpos);
        row = d->selection->activeSheet()->topRow(position.y() - tool()->canvas()->viewConverter()->viewToDocumentY(2.0), ypos);
    }
    // Update the selection.
    d->selection->update(QPoint(col, row));
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

Selection* AbstractSelectionStrategy::selection() const
{
    return d->selection;
}

const QPointF& AbstractSelectionStrategy::startPosition() const
{
    return d->start;
}
