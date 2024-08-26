/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "WebTool.h"

#include <QPainter>

#include <KLocalizedString>
#include <kundo2command.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "WebShape.h"
#include "WebToolWidget.h"

class ChangeScroll : public KUndo2Command
{
public:
    ChangeScroll(WebShape *shape, const QPointF &oldScroll)
        : m_shape(shape)
        , m_newScroll(shape->scroll())
        , m_oldScroll(oldScroll)
    {
    }

    void undo() override
    {
        m_shape->setScroll(m_oldScroll);
        m_shape->update();
    }

    void redo() override
    {
        m_shape->setScroll(m_newScroll);
        m_shape->update();
    }

private:
    WebShape *m_shape;
    QPointF m_newScroll;
    QPointF m_oldScroll;
};

class ChangeZoom : public KUndo2Command
{
public:
    ChangeZoom(WebShape *shape, qreal oldZoom)
        : m_shape(shape)
        , m_newZoom(shape->zoom())
        , m_oldZoom(oldZoom)
    {
    }
    void undo() override
    {
        m_shape->setZoom(m_oldZoom);
        m_shape->update();
    }
    void redo() override
    {
        m_shape->setZoom(m_newZoom);
        m_shape->update();
    }

private:
    WebShape *m_shape;
    qreal m_newZoom;
    qreal m_oldZoom;
};

WebTool::WebTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_tmpShape(nullptr)
    , m_dragMode(NO_DRAG)
{
}

WebTool::~WebTool() = default;

void WebTool::activate(ToolActivation /*toolActivation*/, const QSet<KoShape *> & /*shapes*/)
{
    Q_ASSERT(m_dragMode == NO_DRAG);
    KoSelection *selection = canvas()->shapeManager()->selection();
    const auto selectionShapes = selection->selectedShapes();
    for (KoShape *shape : selectionShapes) {
        m_currentShape = dynamic_cast<WebShape *>(shape);
        if (m_currentShape) {
            break;
        }
    }
    Q_EMIT shapeChanged(m_currentShape);
    if (m_currentShape == nullptr) {
        // none found
        Q_EMIT done();
        return;
    }
}

void WebTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void WebTool::mousePressEvent(KoPointerEvent *event)
{
    WebShape *hit = nullptr;
    QRectF roi(event->point, QSizeF(1, 1));
    const QList<KoShape *> shapes = canvas()->shapeManager()->shapesAt(roi);
    KoSelection *selection = canvas()->shapeManager()->selection();
    for (KoShape *shape : shapes) {
        hit = dynamic_cast<WebShape *>(shape);
        if (hit) {
            if (hit == m_currentShape) {
                m_scrollPoint = event->point;
                Q_ASSERT(m_dragMode == NO_DRAG);
                if (event->modifiers() & Qt::ShiftModifier) {
                    m_oldZoom = m_currentShape->zoom();
                    m_dragMode = ZOOM_DRAG;
                } else {
                    m_oldScroll = m_currentShape->scroll();
                    m_dragMode = SCROLL_DRAG;
                }
            } else {
                selection->deselectAll();
                m_currentShape = hit;
                selection->select(m_currentShape);
                Q_EMIT shapeChanged(m_currentShape);
            }
        }
    }
}

void WebTool::mouseMoveEvent(KoPointerEvent *event)
{
    switch (m_dragMode) {
    case NO_DRAG:
        break;
    case SCROLL_DRAG: {
        m_currentShape->scrollOf(m_scrollPoint - event->point);
        m_scrollPoint = event->point;
        m_currentShape->update();
        break;
    }
    case ZOOM_DRAG: {
        m_currentShape->zoomOf(1.0 - (event->point.y() - m_scrollPoint.y()) / 100.0);
        m_scrollPoint = event->point;
        m_currentShape->update();
    }
    }
}

void WebTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
    switch (m_dragMode) {
    case NO_DRAG:
        break;
    case SCROLL_DRAG:
        canvas()->addCommand(new ChangeScroll(m_currentShape, m_oldScroll));
        break;
    case ZOOM_DRAG:
        canvas()->addCommand(new ChangeZoom(m_currentShape, m_oldZoom));
        break;
    }
    m_dragMode = NO_DRAG;
}

QList<QPointer<QWidget>> WebTool::createOptionWidgets()
{
    auto widget = new WebToolWidget(this);
    widget->open(m_currentShape);
    return {widget};
}
