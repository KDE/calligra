/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007-2010 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCanvasControllerWidgetViewport_p.h"
#include "KoCanvasBase.h"
#include "KoSelection.h"
#include "KoShape.h"
#include "KoShapeController.h"
#include "KoShapeFactoryBase.h" // for the SHAPE mimetypes
#include "KoShapeLayer.h"
#include "KoShapeManager.h"
#include "KoShapePaintingContext.h"
#include "KoShapePaste.h"
#include "KoShapeRegistry.h"
#include "KoShape_p.h"
#include "KoToolProxy.h"

#include <KoProperties.h>

#include <FlakeDebug.h>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>

#include <climits>
#include <cstdlib>

// ********** Viewport **********
Viewport::Viewport(KoCanvasControllerWidget *parent)
    : QWidget(parent)
    , m_draggedShape(nullptr)
    , m_drawShadow(false)
    , m_canvas(nullptr)
    , m_documentOffset(QPoint(0, 0))
    , m_margin(0)
{
    setAutoFillBackground(true);
    setAcceptDrops(true);
    setMouseTracking(true);
    m_parent = parent;
}

void Viewport::setCanvas(QWidget *canvas)
{
    if (m_canvas) {
        m_canvas->hide();
        delete m_canvas;
    }
    m_canvas = canvas;
    if (!canvas)
        return;
    m_canvas->setParent(this);
    m_canvas->show();
    if (!m_canvas->minimumSize().isNull()) {
        m_documentSize = m_canvas->minimumSize();
    }
    resetLayout();
}

void Viewport::setDocumentSize(const QSize &size)
{
    m_documentSize = size;
    resetLayout();
}

void Viewport::documentOffsetMoved(const QPoint &pt)
{
    m_documentOffset = pt;
    resetLayout();
}

void Viewport::setDrawShadow(bool drawShadow)
{
    m_drawShadow = drawShadow;
}

void Viewport::handleDragEnterEvent(QDragEnterEvent *event)
{
    // if not a canvas set then ignore this, makes it possible to assume
    // we have a canvas in all the support methods.
    if (!(m_parent->canvas() && m_parent->canvas()->canvasWidget()))
        return;

    // only allow dropping when active layer is editable
    KoSelection *selection = m_parent->canvas()->shapeManager()->selection();
    KoShapeLayer *activeLayer = selection->activeLayer();
    if (activeLayer && (!activeLayer->isEditable() || activeLayer->isGeometryProtected()))
        return;

    const QMimeData *data = event->mimeData();
    if (data->hasFormat(SHAPETEMPLATE_MIMETYPE) || data->hasFormat(SHAPEID_MIMETYPE)) {
        QByteArray itemData;
        bool isTemplate = true;
        if (data->hasFormat(SHAPETEMPLATE_MIMETYPE))
            itemData = data->data(SHAPETEMPLATE_MIMETYPE);
        else {
            isTemplate = false;
            itemData = data->data(SHAPEID_MIMETYPE);
        }
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString id;
        dataStream >> id;
        QString properties;
        if (isTemplate)
            dataStream >> properties;

        // and finally, there is a point.
        QPointF offset;
        dataStream >> offset;

        // The rest of this method is mostly a copy paste from the KoCreateShapeStrategy
        // So, lets remove this again when Zagge adds his new class that does this kind of thing. (KoLoadSave)
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(id);
        if (!factory) {
            warnFlake << "Application requested a shape that is not registered '" << id << "', Ignoring";
            event->ignore();
            return;
        }
        event->setDropAction(Qt::CopyAction);
        event->accept();

        if (isTemplate) {
            KoProperties props;
            props.load(properties);
            m_draggedShape = factory->createShape(&props, m_parent->canvas()->shapeController()->resourceManager());
        } else
            m_draggedShape = factory->createDefaultShape(m_parent->canvas()->shapeController()->resourceManager());

        Q_ASSERT(m_draggedShape);
        if (!m_draggedShape)
            return;

        if (m_draggedShape->shapeId().isEmpty())
            m_draggedShape->setShapeId(factory->id());
        m_draggedShape->setZIndex(KoShapePrivate::MaxZIndex);
        m_draggedShape->setAbsolutePosition(correctPosition(event->pos()));

        m_parent->canvas()->shapeManager()->addShape(m_draggedShape);
    } else if (data->hasFormat(KoOdf::mimeType(KoOdf::Text))) {
        KoShapeManager *sm = m_parent->canvas()->shapeManager();
        KoShapePaste paste(m_parent->canvas(), sm->selection()->activeLayer());
        if (paste.paste(KoOdf::Text, data)) {
            QList<KoShape *> shapes = paste.pastedShapes();
            if (shapes.count() == 1) {
                m_draggedShape = shapes.first();
                m_draggedShape->setZIndex(KoShapePrivate::MaxZIndex);
                event->setDropAction(Qt::CopyAction);
            }
            event->accept();
        }
    } else {
        event->ignore();
    }
}

void Viewport::handleDropEvent(QDropEvent *event)
{
    if (!m_draggedShape) {
        m_parent->canvas()->toolProxy()->dropEvent(event, correctPosition(event->pos()));
        return;
    }

    repaint(m_draggedShape);
    m_parent->canvas()->shapeManager()->remove(m_draggedShape); // remove it to not interfere with z-index calc.

    m_draggedShape->setPosition(QPointF(0, 0)); // always save position.
    QPointF newPos = correctPosition(event->pos());
    m_parent->canvas()->clipToDocument(m_draggedShape, newPos); // ensure the shape is dropped inside the document.
    m_draggedShape->setAbsolutePosition(newPos);
    KUndo2Command *cmd = m_parent->canvas()->shapeController()->addShape(m_draggedShape);
    if (cmd) {
        m_parent->canvas()->addCommand(cmd);
        KoSelection *selection = m_parent->canvas()->shapeManager()->selection();

        // repaint selection before selecting newly create shape
        foreach (KoShape *shape, selection->selectedShapes())
            shape->update();

        selection->deselectAll();
        selection->select(m_draggedShape);
    } else
        delete m_draggedShape;

    m_draggedShape = nullptr;
}

QPointF Viewport::correctPosition(const QPoint &point) const
{
    QWidget *canvasWidget = m_parent->canvas()->canvasWidget();
    Q_ASSERT(canvasWidget); // since we should not allow drag if there is not.
    QPoint correctedPos(point.x() - canvasWidget->x(), point.y() - canvasWidget->y());
    correctedPos += m_documentOffset;
    return m_parent->canvas()->viewToDocument(correctedPos);
}

void Viewport::handleDragMoveEvent(QDragMoveEvent *event)
{
    if (!m_draggedShape) {
        m_parent->canvas()->toolProxy()->dragMoveEvent(event, correctPosition(event->pos()));
        return;
    }

    m_draggedShape->update();
    repaint(m_draggedShape);
    m_draggedShape->setAbsolutePosition(correctPosition(event->pos()));
    m_draggedShape->update();
    repaint(m_draggedShape);
}

void Viewport::repaint(KoShape *shape)
{
    QRect rect = m_parent->canvas()->viewConverter()->documentToView(shape->boundingRect()).toRect();
    QWidget *canvasWidget = m_parent->canvas()->canvasWidget();
    Q_ASSERT(canvasWidget); // since we should not allow drag if there is not.
    rect.moveLeft(rect.left() + canvasWidget->x() - m_documentOffset.x());
    rect.moveTop(rect.top() + canvasWidget->y() - m_documentOffset.y());
    rect.adjust(-2, -2, 2, 2); // adjust for antialias
    update(rect);
}

void Viewport::handleDragLeaveEvent(QDragLeaveEvent *event)
{
    if (m_draggedShape) {
        repaint(m_draggedShape);
        m_parent->canvas()->shapeManager()->remove(m_draggedShape);
        delete m_draggedShape;
        m_draggedShape = nullptr;
    } else {
        m_parent->canvas()->toolProxy()->dragLeaveEvent(event);
    }
}

void Viewport::handlePaintEvent(QPainter &painter, QPaintEvent *event)
{
    Q_UNUSED(event);
    painter.setBrush(palette().color(QPalette::Base));
    painter.setPen(Qt::NoPen);
    painter.drawRect(event->rect());

    painter.setBrush(Qt::NoBrush);

    // Draw the shadow around the canvas.
    if (m_parent->canvas() && m_parent->canvas()->canvasWidget() && m_drawShadow) {
        QWidget *canvas = m_parent->canvas()->canvasWidget();
        painter.setPen(QPen(Qt::black, 0));
        QRect rect(canvas->x(), canvas->y(), canvas->width(), canvas->height());
        rect.adjust(-1, -1, 0, 0);
        painter.drawRect(rect);
        painter.drawLine(rect.right() + 2, rect.top() + 2, rect.right() + 2, rect.bottom() + 2);
        painter.drawLine(rect.left() + 2, rect.bottom() + 2, rect.right() + 2, rect.bottom() + 2);
    }
    if (m_draggedShape) {
        const KoViewConverter *vc = m_parent->canvas()->viewConverter();

        painter.save();
        QWidget *canvasWidget = m_parent->canvas()->canvasWidget();
        Q_ASSERT(canvasWidget); // since we should not allow drag if there is not.
        painter.translate(canvasWidget->x() - m_documentOffset.x(), canvasWidget->y() - m_documentOffset.y());
        QPointF offset = vc->documentToView(m_draggedShape->position());
        painter.setOpacity(0.6);
        painter.translate(offset.x(), offset.y());
        painter.setRenderHint(QPainter::Antialiasing);
        KoShapePaintingContext paintContext; // FIXME
        m_draggedShape->paint(painter, *vc, paintContext);
        painter.restore();
    }
}

void Viewport::resetLayout()
{
    // Determine the area we have to show
    QRect viewRect(m_documentOffset, size());

    const int viewH = viewRect.height();
    const int viewW = viewRect.width();

    const int docH = m_documentSize.height();
    const int docW = m_documentSize.width();

    int moveX = 0;
    int moveY = 0;

    int resizeW = viewW;
    int resizeH = viewH;

    //     debugFlake <<"viewH:" << viewH << endl
    //              << "docH: " << docH << endl
    //              << "viewW: " << viewW << endl
    //              << "docW: " << docW << Qt::endl;

    if (viewH == docH && viewW == docW) {
        // Do nothing
        resizeW = docW;
        resizeH = docH;
    } else if (viewH > docH && viewW > docW) {
        // Show entire canvas centered
        moveX = (viewW - docW) / 2;
        moveY = (viewH - docH) / 2;
        resizeW = docW;
        resizeH = docH;
    } else if (viewW > docW) {
        // Center canvas horizontally
        moveX = (viewW - docW) / 2;
        resizeW = docW;

        int marginTop = m_margin - m_documentOffset.y();
        int marginBottom = viewH - (m_documentSize.height() - m_documentOffset.y());

        if (marginTop > 0)
            moveY = marginTop;
        if (marginTop > 0)
            resizeH = viewH - marginTop;
        if (marginBottom > 0)
            resizeH = viewH - marginBottom;
    } else if (viewH > docH) {
        // Center canvas vertically
        moveY = (viewH - docH) / 2;
        resizeH = docH;

        int marginLeft = m_margin - m_documentOffset.x();
        int marginRight = viewW - (m_documentSize.width() - m_documentOffset.x());

        if (marginLeft > 0)
            moveX = marginLeft;
        if (marginLeft > 0)
            resizeW = viewW - marginLeft;
        if (marginRight > 0)
            resizeW = viewW - marginRight;
    } else {
        // Take care of the margin around the canvas
        int marginTop = m_margin - m_documentOffset.y();
        int marginLeft = m_margin - m_documentOffset.x();
        int marginRight = viewW - (m_documentSize.width() - m_documentOffset.x());
        int marginBottom = viewH - (m_documentSize.height() - m_documentOffset.y());

        if (marginTop > 0)
            moveY = marginTop;
        if (marginLeft > 0)
            moveX = marginLeft;

        if (marginTop > 0)
            resizeH = viewH - marginTop;
        if (marginLeft > 0)
            resizeW = viewW - marginLeft;
        if (marginRight > 0)
            resizeW = viewW - marginRight;
        if (marginBottom > 0)
            resizeH = viewH - marginBottom;
    }
    if (m_parent->canvasMode() == KoCanvasController::AlignTop) {
        // have up to m_margin pixels at top.
        moveY = qMin(m_margin, moveY);
    }
    if (m_canvas) {
        QRect geom;
        if (m_parent->canvasMode() == KoCanvasController::Infinite)
            geom = QRect(0, 0, viewW, viewH);
        else
            geom = QRect(moveX, moveY, resizeW, resizeH);
        if (m_canvas->geometry() != geom) {
            m_canvas->setGeometry(geom);
            m_canvas->update();
        }
    }
    if (m_drawShadow) {
        update();
    }

    Q_EMIT sizeChanged();
#if 0
     debugFlake <<"View port geom:" << geometry();
     if (m_canvas)
        debugFlake <<"Canvas widget geom:" << m_canvas->geometry();
#endif
}
