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

#include "Selection.h"
#include "Sheet.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoTool.h>

using namespace KSpread;

class SelectionStrategy::Private
{
public:
    Selection* selection;
    QPointF start;
};

SelectionStrategy::SelectionStrategy(KoTool* parent, KoCanvasBase* canvas, Selection* selection,
                                     const QPointF documentPos, Qt::KeyboardModifiers modifiers)
    : KoInteractionStrategy(parent, canvas)
    , d(new Private)
{
    d->selection = selection;
    d->start = documentPos;

    Q_ASSERT(canvas->shapeManager()->selection()->count() > 0);
    KoShape* shape = canvas->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - shape->position();

    // Extend selection, if control modifier is pressed.
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = d->selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = d->selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax)
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
    else if (modifiers & Qt::ControlModifier)
        d->selection->extend(QPoint(col, row), d->selection->activeSheet());
    else
        d->selection->initialize(QPoint(col, row), d->selection->activeSheet());
    m_parent->repaintDecorations();
}

SelectionStrategy::~SelectionStrategy()
{
    delete d;
}

void SelectionStrategy::handleMouseMove(const QPointF& documentPos, Qt::KeyboardModifiers modifiers)
{
    if (!modifiers & Qt::ShiftModifier)
        return;
    Q_ASSERT(m_canvas->shapeManager()->selection()->count() > 0);
    KoShape* shape = m_canvas->shapeManager()->selection()->firstSelectedShape();
    const QPointF position = documentPos - shape->position();
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = d->selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = d->selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax)
    {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
        return;
    }
    // Update the selection.
    d->selection->update(QPoint(col, row));
    m_parent->repaintDecorations();
}

QUndoCommand* SelectionStrategy::createCommand()
{
    return 0;
}

void SelectionStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    m_parent->repaintDecorations();
#if 0
    if (!modifiers & Qt::ControlModifier)
        return;
    // In which cell did the user click?
    double xpos;
    double ypos;
    int col = d->selection->activeSheet()->leftColumn(position.x(), xpos);
    int row = d->selection->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > KS_colMax || row > KS_rowMax)
    {
        kDebug(36005) << "col or row is out of range:" << "col:" << col << " row:" << row;
        return;
    }
    // Update the selection.
    d->selection->update(QPoint(col, row));
#endif
}
