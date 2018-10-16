/* This file is part of the KDE project

   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "ShapeMoveStrategy.h"
#include "SelectionDecorator.h"

#include <KoShapeAnchor.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoShapeContainer.h>
#include <KoShapeContainerModel.h>
#include <commands/KoShapeMoveCommand.h>
#include <KoSnapGuide.h>
#include <KoPointerEvent.h>
#include <KoToolBase.h>
#include <KoSelection.h>
#include <klocalizedstring.h>

ShapeMoveStrategy::ShapeMoveStrategy(KoToolBase *tool, const QPointF &clicked)
    : KoInteractionStrategy(tool)
    , m_start(clicked)
    , m_canvas(tool->canvas())
    , m_firstMove(true)
{
    // FIXME: This test is also done in DefaultTool, so it should also be responsible for setting the shapes to operate on
    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes(KoFlake::StrippedSelection);
    QRectF boundingRect;
    foreach(KoShape *shape, selectedShapes) {
        if (shape->allowedInteraction(KoShape::MoveAllowed, false)) {
            m_selectedShapes << shape;
            m_previousPositions << shape->position();
            m_newPositions << shape->position();
            boundingRect = boundingRect.united( shape->boundingRect() );
            if (shape->anchor()) {
                m_previousOffsets << shape->anchor()->offset();
                m_newOffsets << shape->anchor()->offset();
            } else {
                m_previousOffsets << QPointF();
                m_newOffsets << QPointF();
            }
        }
    }
    KoSelection * selection = m_canvas->shapeManager()->selection();
    m_initialOffset = selection->absolutePosition( SelectionDecorator::hotPosition() ) - m_start;
    m_initialSelectionPosition = selection->position();
    m_canvas->snapGuide()->setIgnoredShapes( selection->selectedShapes( KoFlake::FullSelection ) );

    tool->setStatusText(i18n("Press ALT to hold x- or y-position."));
}

void ShapeMoveStrategy::handleMouseMove(const QPointF &point, Qt::KeyboardModifiers modifiers)
{
    if(m_selectedShapes.isEmpty())
        return;

    if (m_firstMove) {
        // skip first move to avoid accidental move during mouse button press
        m_firstMove = false;
        return;
    }

    QPointF diff = point - m_start;

    if (modifiers & (Qt::AltModifier | Qt::ControlModifier)) {
        // keep x or y position unchanged
        if(qAbs(diff.x()) < qAbs(diff.y()))
            diff.setX(0);
        else
            diff.setY(0);
    } else {
        QPointF positionToSnap = point + m_initialOffset;
        tool()->canvas()->updateCanvas( tool()->canvas()->snapGuide()->boundingRect() );
        QPointF snappedPosition = tool()->canvas()->snapGuide()->snap( positionToSnap, modifiers );
        tool()->canvas()->updateCanvas( tool()->canvas()->snapGuide()->boundingRect() );
        diff = snappedPosition - m_initialOffset - m_start;
    }

    m_diff = diff;

    moveSelection();
}

void ShapeMoveStrategy::handleCustomEvent(KoPointerEvent *event)
{
    QPointF diff = tool()->canvas()->viewConverter()->viewToDocument(event->pos());

    if (event->modifiers() & (Qt::AltModifier | Qt::ControlModifier)) {
        // keep x or y position unchanged
        if(qAbs(diff.x()) < qAbs(diff.y()))
            diff.setX(0);
        else
            diff.setY(0);
    }

    m_diff += 0.1 * diff ;

    moveSelection();
}

void ShapeMoveStrategy::moveSelection()
{
    Q_ASSERT(m_newPositions.count());

    int i=0;
    foreach(KoShape *shape, m_selectedShapes) {
        QPointF delta = m_previousPositions.at(i) + m_diff - shape->position();
        if (shape->parent()) {
            shape->parent()->model()->proposeMove(shape, delta);
        }
        tool()->canvas()->clipToDocument(shape, delta);
        QPointF newPos (shape->position() + delta);
        m_newPositions[i] = newPos;
        shape->update();
        shape->setPosition(newPos);
        shape->update();
        if (shape->anchor()) {
            m_newOffsets[i] = shape->anchor()->offset();
        }
        i++;
    }
    tool()->canvas()->shapeManager()->selection()->setPosition(m_initialSelectionPosition + m_diff);
}

KUndo2Command* ShapeMoveStrategy::createCommand()
{
    if(m_diff.x() == 0 && m_diff.y() == 0) {
        return 0;
    }
    // get the shapes that has actually been moved
    QVector<QPointF> oldPositions;
    QVector<QPointF> newPositions;
    QVector<QPointF> newOffsets;
    QVector<QPointF> oldOffsets;
    QList<KoShape*> movedShapes;
    for (int i = 0; i < m_selectedShapes.count(); ++i) {
        KoShape *shape = m_selectedShapes.at(i);
        if (shape->position() != m_previousPositions.at(i)) {
            movedShapes << shape;
            oldPositions << m_previousPositions.at(i);
            newPositions << m_newPositions.at(i);
            if (shape->anchor()) {
                oldOffsets << m_previousOffsets.at(i);
                newOffsets << m_newOffsets.at(i);
            }
        }
    }
    if (movedShapes.isEmpty()) {
        return 0;
    }
    tool()->canvas()->snapGuide()->reset();
    return new KoShapeMoveCommand(movedShapes, oldPositions, newPositions, oldOffsets, newOffsets);
}

void ShapeMoveStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    tool()->canvas()->updateCanvas(tool()->canvas()->snapGuide()->boundingRect());
}

void ShapeMoveStrategy::paint( QPainter &painter, const KoViewConverter &converter)
{
    SelectionDecorator decorator (KoFlake::NoHandle, false, false);
    decorator.setSelection(tool()->canvas()->shapeManager()->selection());
    decorator.setHandleRadius(handleRadius());
    decorator.paint(painter, converter);
}
