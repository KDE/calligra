/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCreateShapeStrategy.h"
#include "KoCanvasBase.h"
#include "KoCreateShapesTool.h"
#include "KoSelection.h"
#include "KoShape.h"
#include "KoShapeController.h"
#include "KoShapeFactoryBase.h"
#include "KoShapeManager.h"
#include "KoShapeRegistry.h"
#include "KoShapeRubberSelectStrategy_p.h"
#include "KoViewConverter.h"

#include <QPainter>

#include <FlakeDebug.h>

KoCreateShapeStrategy::KoCreateShapeStrategy(KoCreateShapesTool *tool, const QPointF &clicked)
    : KoShapeRubberSelectStrategy(tool, clicked, tool->canvas()->snapToGrid())
{
    KoCreateShapesTool *parent = static_cast<KoCreateShapesTool *>(d_ptr->tool);
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(parent->shapeId());
    if (factory) {
        const KoProperties *props = parent->shapeProperties();
        KoShape *shape;
        if (props) {
            shape = factory->createShape(props);
        } else {
            shape = factory->createDefaultShape();
        }

        m_outline = shape->outline();
        m_outlineBoundingRect = m_outline.boundingRect();
        delete shape;
    }
}

KUndo2Command *KoCreateShapeStrategy::createCommand()
{
    Q_D(KoShapeRubberSelectStrategy);
    KoCreateShapesTool *parent = static_cast<KoCreateShapesTool *>(d_ptr->tool);
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(parent->shapeId());
    if (!factory) {
        warnFlake << "Application requested a shape that is not registered" << parent->shapeId();
        return nullptr;
    }

    const KoProperties *props = parent->shapeProperties();
    KoShape *shape;
    if (props)
        shape = factory->createShape(props, parent->canvas()->shapeController()->resourceManager());
    else
        shape = factory->createDefaultShape(parent->canvas()->shapeController()->resourceManager());
    if (shape->shapeId().isEmpty())
        shape->setShapeId(factory->id());
    QRectF rect = d->selectedRect();
    shape->setPosition(rect.topLeft());
    QSizeF newSize = rect.size();
    // if the user has dragged when creating the shape,
    // resize the shape to the dragged size
    if (newSize.width() > 1.0 && newSize.height() > 1.0)
        shape->setSize(newSize);

    KUndo2Command *cmd = parent->canvas()->shapeController()->addShape(shape);
    if (cmd) {
        KoSelection *selection = parent->canvas()->shapeManager()->selection();
        selection->deselectAll();
        selection->select(shape);
    }
    return cmd;
}

void KoCreateShapeStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    Q_D(KoShapeRubberSelectStrategy);
    d->tool->canvas()->updateCanvas(d->selectedRect());
}

void KoCreateShapeStrategy::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_D(KoShapeRubberSelectStrategy);
    if (m_outline.isEmpty())
        KoShapeRubberSelectStrategy::paint(painter, converter);
    else {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);

        QColor selectColor(Qt::blue); // TODO make configurable
        selectColor.setAlphaF(0.5);
        QBrush sb(selectColor, Qt::SolidPattern);
        painter.setPen(QPen(sb, 0));
        painter.setBrush(sb);
        QRectF paintRect = converter.documentToView(d->selectedRect());

        qreal xscale = paintRect.width() / m_outlineBoundingRect.width();
        qreal yscale = paintRect.height() / m_outlineBoundingRect.height();
        QTransform matrix;
        matrix.translate(-m_outlineBoundingRect.left(), -m_outlineBoundingRect.top());
        matrix.scale(xscale, yscale);
        painter.translate(paintRect.left(), paintRect.top());

        if (painter.hasClipping())
            paintRect = paintRect.intersected(painter.clipRegion().boundingRect());

        painter.setTransform(matrix, true);
        painter.drawPath(m_outline);
        painter.restore();
    }
}

void KoCreateShapeStrategy::handleMouseMove(const QPointF &point, Qt::KeyboardModifiers modifiers)
{
    Q_D(KoShapeRubberSelectStrategy);
    KoShapeRubberSelectStrategy::handleMouseMove(point, modifiers);
    if (!m_outline.isEmpty())
        d->tool->canvas()->updateCanvas(d->selectedRect());
}
